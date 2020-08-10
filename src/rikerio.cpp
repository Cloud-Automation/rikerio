
#include "rikerio.h"

#include "functional"
#include "iostream"
#include "algorithm"
#include "set"
#include "unistd.h"
#include "string.h"
#include "dirent.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/file.h"
#include "sys/mman.h"
#include "sys/sem.h"

#define RIO_MEMORY_SEM_LOCK -1
#define RIO_MEMORY_SEM_UNLOCK 1

using namespace RikerIO;

static const int result_error = -1;
static const int result_ok = 0;

RikerIO::BitSize RikerIO::get_bitsizeof(RikerIO::Type& type) {

    switch (type) {
    case (Type::UNDEF):
        return 0;
    case (Type::BIT):
        return 1;
    case (Type::BOOL):
        return 8;
    case (Type::UINT8):
        return 8;
    case (Type::INT8):
        return 8;
    case (Type::UINT16):
        return 16;
    case (Type::INT16):
        return 16;
    case (Type::UINT32):
        return 32;
    case (Type::INT32):
        return 32;
    case (Type::UINT64):
        return 64;
    case (Type::INT64):
        return 64;
    case (Type::FLOAT):
        return 32;
    case (Type::DOUBLE):
        return 64;
    case (Type::STRING):
        return 0; // undefined
    }

    return 0;

}

RikerIO::ByteSize RikerIO::calc_bytesize(BitSize bit_size) {

    unsigned int rest = bit_size % 8;
    unsigned int bit_size_without_rest = bit_size - rest;
    unsigned int full_bytes = bit_size_without_rest / 8;

    return rest > 0 ? (full_bytes + 1) : full_bytes;

}

static int _rio_get_memory_pointer(const char* filename, uint8_t** ptr, size_t size) {

    if (!filename || !ptr) {
        return -1;
    }

    FILE* fp = fopen(filename, "r+");

    if (!fp) {
        return -1;
    }

    int fd = fileno(fp);

    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (p == MAP_FAILED) {
        return -1;
    }

    *ptr = (uint8_t*) p;

    fclose(fp);

    return 0;
}

int _rio_lock_and_handle(const std::string& filename, int flags, std::function<int(int)> cb) {

    int fd = open(filename.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    if (fd == -1) {
//        error_code = RIO_ERROR_FILE_NOT_FOUND;
//        std::cout << filename << " " << strerror(errno) << std::endl;
        return RikerIO::result_error;
    }

    if (flock(fd, LOCK_EX) == -1) {
//        error_code = RIO_ERROR_FILE_LOCK;
        return RikerIO::result_error;
    }

    int ret_val = cb(fd);

    if (flock(fd, LOCK_UN) == -1) {
//        error_code = RIO_ERROR_FILE_UNLOCK;
        return -1;
    }

    close(fd);

    return ret_val;

}

int _read_allocations(int fd, std::vector<RikerIO::Allocation>& alloc_list) {

    do {

        RikerIO::Allocation tmp_entry;
        int ret = read(fd, &tmp_entry, sizeof(RikerIO::Allocation));

        if (ret == 0) { // eof
            break;
        }

        alloc_list.push_back(tmp_entry);

    } while (1);

    return RikerIO::result_ok;

}

int _write_allocations(int fd, std::vector<RikerIO::Allocation>& alloc_list) {

    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);

    for (auto it = alloc_list.begin(); it != alloc_list.end(); ++it) {
        write(fd, &*it, sizeof(RikerIO::Allocation));
    }

    return RikerIO::result_ok;

}

int _read_links(int fd, std::vector<std::string>& data_point_list) {

    do {

        RikerIO::ID tmp;
        int res = read(fd, &tmp, sizeof(tmp));

        if (res == 0) {
            return RikerIO::result_ok;
        } else if (res < 0) {
            return RikerIO::result_error;
        }

        data_point_list.push_back(std::string(tmp));

    } while (1);

    return RikerIO::result_ok;

}

int _write_links(int fd, std::vector<std::string>& data_point_list) {

    ftruncate(fd, 0);
    lseek(fd, 0, SEEK_SET);

    for (auto id : data_point_list) {
        RikerIO::ID tmp = {};
        memset(&tmp, 0, sizeof(tmp));
        strcpy(tmp, id.c_str());
        if (write(fd, &tmp, sizeof(tmp)) < 0) {
            return RikerIO::result_error;
        };
    }

    return RikerIO::result_ok;

}

bool _fit_allocation(
    const RikerIO::Profile& profile,
    std::vector<RikerIO::Allocation>& alloc_list,
    const std::string& alloc_id,
    uint32_t size,
    RikerIO::Allocation& entry) {

    uint32_t start = 0;
    uint32_t end = alloc_list.size() == 0 ? profile.byte_size : alloc_list[0].offset;
    bool inserted = false;

    auto it = alloc_list.begin();

    do {

        if (it == alloc_list.end()) {
            end = profile.byte_size;
        } else {
            auto& alloc_item = *it;
            end = alloc_item.offset;
        }

        long diff = end - start;

        if (diff >= size) {

            //std::cout << size << " fits in between " << start << " and " << end << std::endl;

            inserted = true;

            entry.offset = start;
            entry.byte_size = size;
            entry.ptr = profile.ptr + start;
            strcpy(entry.id, alloc_id.c_str());

            if (it == alloc_list.begin()) {
                alloc_list.insert(alloc_list.begin(), entry);
            } else if (it == alloc_list.end()) {
                alloc_list.insert(alloc_list.end(), entry);
            } else {
                alloc_list.insert(it, entry);
            }

            break;
        } else {
            if (it != alloc_list.end()) {
                start = end + (*it).byte_size;
            } else {
                start = end;
                end = profile.byte_size;
            }
        }

        it++;

    } while (it <= alloc_list.end());

    return inserted;

}

int RikerIO::init(const std::string& profile_id, Profile& ret_profile) {

    /* create link folder */

    const std::string sys_folder = RikerIO::root_path + "/" + profile_id;
    const std::string profile_info = sys_folder + "/info";
    const std::string shm_file = sys_folder + "/" + shared_memory_filename;

    /* open dir */

    DIR *dir;
    if ((dir = opendir (sys_folder.c_str())) == NULL) {
        return -1;
    }
    closedir (dir);

    /* 1. get profile data */

    int fd = open(profile_info.c_str(), O_RDONLY, S_IRUSR);

    if (fd < 0) {
        return result_error;
    }

    read(fd, &ret_profile, sizeof(RikerIO::Profile));

    close(fd);

    /* 2. get memory pointer */

    if (_rio_get_memory_pointer(shm_file.c_str(), &ret_profile.ptr, ret_profile.byte_size) == -1) {
        return result_error;
    }

    ret_profile.sem_id = semget(ret_profile.sem_key, 0, 0);

    return result_ok;

}

int RikerIO::lock(const RikerIO::Profile& profile) {

    struct sembuf semaphore = { };

    semaphore.sem_op = RIO_MEMORY_SEM_LOCK;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(profile.sem_id, &semaphore, 1) == -1) {
        return -1;
    }

    return 1;
}

int RikerIO::unlock(const RikerIO::Profile& profile) {

    struct sembuf semaphore = { };

    semaphore.sem_op = RIO_MEMORY_SEM_UNLOCK;
    semaphore.sem_flg = SEM_UNDO;
    if (semop(profile.sem_id, &semaphore, 1) == -1) {
        return -1;
    }

    return 1;

}


int RikerIO::alloc(const Profile& profile, uint32_t size, const std::string& alloc_id, RikerIO::Allocation& entry) {

    /* 1. get filesize of shm */

    const std::string alloc_file = RikerIO::root_path + "/" + profile.id + "/" + RikerIO::allocation_filename;

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    return _rio_lock_and_handle(alloc_file, O_RDWR, [&](int fd) {

        /* 1. read allocations */

        std::vector<RikerIO::Allocation> alloc_list;

        _read_allocations(fd, alloc_list);

        /* 2. look for an existing allocation */

        for (auto it = alloc_list.begin() ; it != alloc_list.end(); ++it) {
            auto& alloc_item = *it;
            bool id_match = alloc_id != "" && strcmp(alloc_item.id, alloc_id.c_str()) == 0;
            if (id_match) {
                return RikerIO::result_error;
            }
        }

        bool inserted = _fit_allocation(profile, alloc_list, alloc_id, size, entry);

        /* 3. write back */

        _write_allocations(fd, alloc_list);

        return !inserted ? RikerIO::result_error : RikerIO::result_ok;

    });

}

int RikerIO::realloc(const Profile& profile, uint32_t size, const std::string& alloc_id, RikerIO::Allocation& entry) {

    /* 1. get filesize of shm */

    const std::string alloc_file = RikerIO::root_path + "/" + profile.id + "/" + RikerIO::allocation_filename;

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    return _rio_lock_and_handle(alloc_file, O_RDWR, [&](int fd) {

        /* 1. read allocations */

        std::vector<RikerIO::Allocation> alloc_list;

        _read_allocations(fd, alloc_list);

        /* 2. look for an existing allocation */


        for (auto it = alloc_list.begin() ; it != alloc_list.end(); ++it) {
            auto& alloc_item = *it;
            bool id_match = "" && strcmp(alloc_item.id, alloc_id.c_str()) == 0;
            bool size_match = alloc_item.byte_size == size;
            /* if there is a allocation with said id and size then return this
             * and do not create a new entry */

            if (id_match && size_match) {
                entry = alloc_item;
                entry.ptr = profile.ptr + alloc_item.offset;
                return RikerIO::result_ok;
            } else if (id_match && !size_match) {
                //std::cout << "size does not match" << std::endl;
                alloc_list.erase(it);
                break;
            }

        }

        bool inserted = _fit_allocation(profile, alloc_list, alloc_id, size, entry);

        /* 5. write back */

        _write_allocations(fd, alloc_list);

        return !inserted ? RikerIO::result_error : RikerIO::result_ok;

    });

}


int RikerIO::dealloc(const Profile& profile, const std::string& alloc_id) {

    /* 1. get filesize of shm */

    const std::string alloc_file = RikerIO::root_path + "/" + profile.id + "/" + RikerIO::allocation_filename;

    /* 2. open file located on /var/www/rikerio/{id}/alloc */

    return _rio_lock_and_handle(alloc_file, O_RDWR, [&](int fd) {

        /* 1. read allocations */

        std::vector<RikerIO::Allocation> alloc_list;

        _read_allocations(fd, alloc_list);

        bool erased = false;

        /* 2. look for an existing allocation */

        for (auto it = alloc_list.begin() ; it != alloc_list.end(); ++it) {

            auto& alloc_item = *it;
            bool id_match = "" && strcmp(alloc_item.id, alloc_id.c_str()) == 0;
            /* if there is a allocation with said id and size then return this
             * and do not create a new entry */

            if (id_match) {
                alloc_list.erase(it);
                erased = true;
                break;
            }
        }

        /* 5. write back */

        if (erased) {
            _write_allocations(fd, alloc_list);
        }

        return !erased ? RikerIO::result_error : RikerIO::result_ok;

    });

}

int RikerIO::list(const Profile& profile, std::vector<Allocation>& alloc_list) {

    const std::string alloc_file = root_path + "/" + std::string(profile.id) + "/" + allocation_filename;

    return _rio_lock_and_handle(alloc_file, O_RDONLY, [&](int fd) {

        return _read_allocations(fd, alloc_list);

    });


}

int RikerIO::Data::set(const Profile& profile, Allocation& alloc, const std::string& id, DataPoint& dp) {

    const std::string dp_file = root_path + "/" + std::string(profile.id) + "/data/" + id;

    /* 1. check available space on alloc with auto offset */

    auto bytesize = calc_bytesize(dp.bit_size);

    if ((alloc.local_byte_offset + bytesize) <= alloc.byte_size) {
        dp.byte_offset = alloc.offset + alloc.local_byte_offset;
        dp.bit_index = 0;
        alloc.local_byte_offset += bytesize;
        dp.ptr = (uint8_t*) profile.ptr + dp.byte_offset;
    } else {
        return result_error;
    }

    strcpy(dp.id, id.c_str());

    /* 1. create or open file id */

    return _rio_lock_and_handle(dp_file, O_WRONLY | O_CREAT, [&](int fd) {

        int rret = write(fd, &dp, sizeof(dp));

        if (rret != sizeof(dp)) {
            return result_error;
        }

        return result_ok;

    });

}

int RikerIO::Data::set(
    const Profile& profile,
    const Allocation& alloc,
    const std::string& id,
    ByteOffset byte_offset,
    BitIndex bit_index,
    DataPoint& dp) {

    const std::string dp_file = root_path + "/" + std::string(profile.id) + "/data/" + id;

    /* 1. check available space on alloc with auto offset */

    auto bytesize = calc_bytesize(dp.bit_size);

    if ((byte_offset + bytesize) <= alloc.byte_size) {
        dp.byte_offset = alloc.offset + byte_offset;
        dp.bit_index = bit_index;
        dp.ptr = (uint8_t*) profile.ptr + dp.byte_offset;
    } else {
        return result_error;
    }

    strcpy(dp.id, id.c_str());

    /* 1. create or open file id */

    return _rio_lock_and_handle(dp_file, O_WRONLY | O_CREAT, [&](int fd) {

        int rret = write(fd, &dp, sizeof(dp));

        if (rret != sizeof(dp)) {
            return result_error;
        }

        return result_ok;

    });

}

int RikerIO::Data::set(
    const Profile& profile,
    const std::string& id,
    ByteOffset byte_offset,
    BitIndex bit_index,
    DataPoint& dp) {

    const std::string dp_file = root_path + "/" + std::string(profile.id) + "/data/" + id;

    /* 1. check available space on alloc with auto offset */

    auto bytesize = calc_bytesize(dp.bit_size);

    if ((byte_offset + bytesize) <= profile.byte_size) {
        dp.byte_offset = byte_offset;
        dp.bit_index = bit_index;
        dp.ptr = (uint8_t*) profile.ptr + dp.byte_offset;
    } else {
        return result_error;
    }

    strcpy(dp.id, id.c_str());

    /* 1. create or open file id */

    return _rio_lock_and_handle(dp_file, O_WRONLY | O_CREAT, [&](int fd) {

        int rret = write(fd, &dp, sizeof(dp));

        if (rret != sizeof(dp)) {
            return result_error;
        }

        return result_ok;

    });

}

int RikerIO::Data::remove(const RikerIO::Profile& profile, const std::string& id) {

    const std::string dp_file = root_path + "/" + std::string(profile.id) + "/data/" + id;

    if (unlink(dp_file.c_str()) == -1) {
        return RikerIO::result_error;
    };

    return RikerIO::result_ok;

}

int RikerIO::Data::get(const RikerIO::Profile& profile, const std::string& id, RikerIO::DataPoint& dp) {

    const std::string dp_file = root_path + "/" + std::string(profile.id) + "/data/" + id;

    return _rio_lock_and_handle(dp_file, O_RDONLY, [&](int fd) {

        if (read(fd, &dp, sizeof(dp)) != sizeof(dp)) {
            return RikerIO::result_error;
        }

        dp.ptr = (uint8_t*) profile.ptr + dp.byte_offset;

        return RikerIO::result_ok;

    });

}

int RikerIO::Data::list(const RikerIO::Profile& profile, std::vector<std::string>& data_list) {

    const std::string data_folder = root_path + "/" + std::string(profile.id) + "/data";

    DIR* d;
    struct dirent* dir;

    d = opendir(data_folder.c_str());

    if (!d) {
        return RikerIO::result_error;
    }

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_REG) {
            continue;
        }
        data_list.push_back(dir->d_name);
    }

    std::sort(data_list.begin(), data_list.end());

    closedir(d);

    return RikerIO::result_ok;

}

int RikerIO::Link::set(const RikerIO::Profile& profile, const std::string& link_key, const std::string& data_id) {

    const std::string key_file = root_path + "/" + std::string(profile.id) + "/links/" + link_key;

    std::vector<std::string> data_points;

    return _rio_lock_and_handle(key_file, O_RDWR | O_CREAT, [&](int fd) {

        _read_links(fd, data_points);

        auto it = std::find(data_points.begin(), data_points.end(), data_id);

        if (it != data_points.end()) {
            return RikerIO::result_ok;
        }

        data_points.push_back(data_id);

        _write_links(fd, data_points);

        return RikerIO::result_ok;

    });

}

int RikerIO::Link::remove(const RikerIO::Profile& profile, const std::string& link_key, const std::string& data_id) {

    if (data_id == "") {
        return RikerIO::result_error;
    }

    const std::string key_file = root_path + "/" + std::string(profile.id) + "/links/" + link_key;

    std::vector<std::string> data_points;

    return _rio_lock_and_handle(key_file, O_RDWR | O_CREAT, [&](int fd) {

        _read_links(fd, data_points);

        auto it = std::find(data_points.begin(), data_points.end(), data_id);
        if (it == data_points.end()) {
            return RikerIO::result_ok;
        }
        data_points.erase(it);

        _write_links(fd, data_points);

        return RikerIO::result_ok;

    });

}

int RikerIO::Link::remove(const RikerIO::Profile& profile, const std::string& key) {

    const std::string link_file = root_path + "/" + std::string(profile.id) + "/links/" + key;

    if (unlink(link_file.c_str()) == -1) {
        return RikerIO::result_error;
    };

    return RikerIO::result_ok;

}


int RikerIO::Link::get(const Profile& profile, const std::string& link_key, std::vector<std::string>& ids) {

    const std::string key_file = root_path + "/" + std::string(profile.id) + "/links/" + link_key;

    return _rio_lock_and_handle(key_file, O_RDONLY, [&](int fd) {
        return _read_links(fd, ids);
    });

}

int RikerIO::Link::list(const RikerIO::Profile& profile, std::vector<std::string>& link_list) {

    const std::string data_folder = root_path + "/" + std::string(profile.id) + "/links";

    DIR* d;
    struct dirent* dir;

    d = opendir(data_folder.c_str());

    if (!d) {
        return RikerIO::result_error;
    }

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_REG) {
            continue;
        }
        link_list.push_back(dir->d_name);
    }

    std::sort(link_list.begin(), link_list.end());

    closedir(d);

    return RikerIO::result_ok;

}
