#include "client/client.h"
#include "iostream"
#include "vector"
#include "set"

struct ReadItem {
    const std::string key;
    const std::string id;
    const std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> item;
    unsigned int new_size;
    uint8_t* copy_mem;
    ReadItem(
        const std::string& key,
        const std::string& id,
        const std::shared_ptr<RikerIO::Response::v1::DataList::DataListItem> item):
        key(key),
        id(id),
        item(item) {

        int new_size = item->get_size() / 8;

        if ((item->get_size() % 8) > 0) {
            new_size += 1;
        }

        copy_mem = (uint8_t*) calloc(1, new_size);

    }

    ~ReadItem() {
        free(copy_mem);
    }

};

int cmd_read(std::vector<std::string>& patterns, RikerIO::Client& client) {

    std::vector<std::shared_ptr<ReadItem>> items;
    std::set<int> semaphore_ids;
    std::set<std::shared_ptr<RikerIO::Semaphore>> semaphores;

    /* read links */

    for (auto pattern : patterns) {

        /* get links */
        {
            RikerIO::Request::v1::LinkList req(pattern);

            auto res = client.link_list(req);

            for (auto item : res->get_items()) {

                if (!item->get_data()) {
                    continue;
                }

                std::shared_ptr<ReadItem> read_item =
                    std::make_shared<ReadItem>(res->get_key(), res->get_id(), item->get_data());

                items.push_back(read_item);

            }
        }

        /* get data */
        {

            RikerIO::Request::v1::DataList req(pattern);

            auto res = client.data_list(req);

            for (auto item : res->get_items()) {

                std::shared_ptr<ReadItem> read_item =
                    std::make_shared<ReadItem>("", item->get_id(), item);

                items.push_back(read_item);

            }

        }
    }

    /* collect semaphores */
    for (auto read_item : items) {

        std::shared_ptr<RikerIO::Semaphore> sem = read_item->item->get_semaphore();

        if (semaphore_ids.find(sem->get_id()) == semaphore_ids.end()) {
            continue;
        }

        semaphore_ids.insert(sem->get_id());
        semaphores.insert(sem);

    }

    /* set real time scheduler attributes */

    struct sched_param sched;

    sched.sched_priority = 90;

    if (sched_setscheduler(0, SCHED_RR, &sched) != 0) {
        std::cout << "Error setting scheduler policy/priority (" << strerror(errno) << ")" << std::endl;
    }

    /* read per semaphore from memory */

    for (auto sem : semaphores) {

        sem->lock();

        for (auto read_item : items) {

            std::shared_ptr<RikerIO::Semaphore> item_sem = read_item->item->get_semaphore();

            if (item_sem->get_id() != sem->get_id()) {
                continue;
            }

            memcpy(read_item->copy_mem, read_item->item->get_data_ptr(), read_item->new_size);

        }

        sem->unlock();

    }

    /* output different types */

    for (auto read_item : items) {

        std::cout << read_item->key << " " << read_item->id;

        if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::BIT) {

            uint8_t* value = read_item->copy_mem;

            std::bitset<8> bs(*value);

            std::cout << bs[read_item->item->get_index()] << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::UINT8) {

            uint8_t* value = read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::INT8) {

            int8_t* value = (int8_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::UINT16) {

            uint16_t* value = (uint16_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::INT16) {

            int16_t* value = (int16_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::INT32) {

            uint32_t* value = (uint32_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::UINT32) {

            int32_t* value = (int32_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::INT64) {

            uint64_t* value = (uint64_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::UINT64) {

            int64_t* value = (int64_t*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::FLOAT) {

            float* value = (float*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else if (read_item->item->get_datatype() == RikerIO::Utils::Datatype::DOUBLE) {

            double* value = (double*) read_item->copy_mem;

            std::cout << *value << std::endl;

        } else {

            uint8_t* value = read_item->copy_mem;

            std::bitset<8> bs(*value);

            for (unsigned int i = read_item->item->get_index(); i < read_item->item->get_size(); i += 1) {
                std::cout << bs[i];
            }

            std::cout << std::endl;


        }

    }

    return EXIT_SUCCESS;

}
