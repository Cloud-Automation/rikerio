#include "rikerio.h"
#include "iostream"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "cassert"

#define PROFILE "default"

int main() {

    RikerIO::Profile profile;

    int ret_init = RikerIO::init("default", profile);

    assert(ret_init == RikerIO::result_ok);
    assert(strcmp(profile.id, "default") == 0);
    assert(profile.byte_size == 4096);
    assert(profile.base_cycle == 10000);

    RikerIO::Allocation alloc_a;
    RikerIO::Allocation alloc_b;

    int ret_a = RikerIO::alloc(profile, 100, "test-a", alloc_a);

    assert(ret_a == RikerIO::result_ok);
    assert(alloc_a.offset == 0);

    int ret_b = RikerIO::alloc(profile, 150, "test-b", alloc_b);

    assert(ret_b == RikerIO::result_ok);
    assert(alloc_b.offset == 100);

    ret_a = RikerIO::dealloc(profile, "test-a");

    assert(ret_a == RikerIO::result_ok);

    RikerIO::Allocation alloc_c, alloc_d;

    int ret_c = RikerIO::alloc(profile, 75, "test-c", alloc_c);

    assert(ret_c == RikerIO::result_ok);
    assert(alloc_c.offset == 0);

    int ret_d = RikerIO::alloc(profile, 10, "test-d", alloc_d);

    assert(ret_d == RikerIO::result_ok);
    assert(alloc_d.offset == 75);

    RikerIO::Allocation alloc_cc;

    int ret_cc = RikerIO::realloc(profile, 176, "test-c", alloc_cc);

    assert(ret_cc == RikerIO::result_ok);
    assert(alloc_cc.offset == 250);

    ret_cc = RikerIO::realloc(profile, 75, "test-c", alloc_cc);

    assert(ret_cc == RikerIO::result_ok);
    assert(alloc_cc.offset == 0);

    /* list allocations */

    std::vector<RikerIO::Allocation> alloc_list;

    int ret_alloc_list = RikerIO::list(profile, alloc_list);

    assert(ret_alloc_list == RikerIO::result_ok);
    assert(alloc_list.size() == 3);
    assert(strcmp(alloc_list[0].id, "test-c") == 0);
    assert(strcmp(alloc_list[1].id, "test-d") == 0);
    assert(strcmp(alloc_list[2].id, "test-b") == 0);

    /* create data */

    RikerIO::DataPoint dp_a(RikerIO::Type::FLOAT),
            dp_b(RikerIO::Type::BIT),
            dp_c(RikerIO::Type::UNDEF, 10),
            dp_d(RikerIO::Type::UNDEF, 10),
            dp_e(RikerIO::Type::STRING, 200);

    assert(dp_a.bit_size == 32);

    /* auto assign */
    int ret_dp_a = RikerIO::Data::set(profile, alloc_b, "in.test.value.a", dp_a);

    assert(ret_dp_a == RikerIO::result_ok);
    assert(strcmp(dp_a.id, "in.test.value.a") == 0);
    assert(dp_a.byte_offset == 100);
    assert(alloc_b.local_byte_offset == 4);

    /* manual placement in alloc */
    int ret_dp_b = RikerIO::Data::set(profile, alloc_b, "in.test.value.b", 10, 5, dp_b);

    assert(ret_dp_b == RikerIO::result_ok);
    assert(dp_b.byte_offset == 110);
    assert(dp_b.bit_index = 5);
    assert(alloc_b.local_byte_offset == 4);

    int ret_dp_c = RikerIO::Data::set(profile, alloc_b, "in.test.value.c", dp_c);

    assert(ret_dp_c == RikerIO::result_ok);
    assert(dp_c.byte_offset == 104);
    assert(dp_c.bit_index == 0);
    assert(alloc_b.local_byte_offset == 14);

    int ret_db_b = RikerIO::Data::set(profile, alloc_b, "in.test.value.d", 145, 0, dp_d);

    assert(ret_db_b == RikerIO::result_error);

    int ret_db_e = RikerIO::Data::set(profile, "in.test.value.e", 100, 0, dp_e);

    assert(ret_db_e == RikerIO::result_ok);
    assert(dp_e.byte_offset == 100);
    assert(dp_e.bit_index == 0);

    int ret_rmdata_a = RikerIO::Data::remove(profile, "in.test.value.c");

    assert(ret_rmdata_a == RikerIO::result_ok);

    int ret_rmdata_b = RikerIO::Data::remove(profile, "unknown.data");

    assert(ret_rmdata_b == RikerIO::result_error);

    RikerIO::DataPoint dp_f;

    int ret_getdata_b = RikerIO::Data::get(profile, "in.test.value.b", dp_f);

    assert(ret_getdata_b == RikerIO::result_ok);
    assert(memcmp(&dp_f, &dp_b, sizeof(dp_b)) == 0);

    int ret_getdata_unknown = RikerIO::Data::get(profile, "unknown.data", dp_f);

    assert(ret_getdata_unknown == RikerIO::result_error);

    std::vector<std::string> data_list;
    int ret_listdata = RikerIO::Data::list(profile, data_list);

    assert(ret_listdata == RikerIO::result_ok);

    for (auto d : data_list) {
        std::cout << d << std::endl;
    }

    assert(data_list[0] == "in.test.value.a");
    assert(data_list[1] == "in.test.value.b");
    assert(data_list[2] == "in.test.value.e");

    // create links

    int ret_addlink_a = RikerIO::Link::set(profile, "in.link.a", "in.test.value.a");

    assert(ret_addlink_a == RikerIO::result_ok);

    int ret_addlink_b = RikerIO::Link::set(profile, "in.link.a", "undefined.data.point");

    assert(ret_addlink_b == RikerIO::result_ok);

    int ret_removelink_a = RikerIO::Link::remove(profile, "in.link.a", "in.test.value.a");

    assert(ret_removelink_a == RikerIO::result_ok);

    int ret_addlink_c = RikerIO::Link::set(profile, "in.link.a", "in.test.value.b");

    assert(ret_addlink_c == RikerIO::result_ok);

    int ret_addlink_d = RikerIO::Link::set(profile, "in.link.b", "in.test.value.a");

    assert(ret_addlink_d == RikerIO::result_ok);

    std::vector<std::string> data_list_a;

    int ret_getlink_a = RikerIO::Link::get(profile, "in.link.a", data_list_a);

    assert(ret_getlink_a == RikerIO::result_ok);
    assert(data_list_a.size() == 2);
    assert(data_list_a[0] == "undefined.data.point");
    assert(data_list_a[1] == "in.test.value.b");

    std::vector<std::string> link_list_a;

    int ret_listlink_a = RikerIO::Link::list(profile, link_list_a);

    assert(ret_listlink_a == RikerIO::result_ok);
    assert(link_list_a.size() == 2);
    assert(link_list_a[0] == "in.link.a");
    assert(link_list_a[1] == "in.link.b");

    // clean up

    // 1. remove all links
    for (auto l : link_list_a) {
        RikerIO::Link::remove(profile, l);
    }

    // 2. remove all data

    for (auto dp : data_list) {
        RikerIO::Data::remove(profile, dp);
    }

    // 3. remove all alocations

    for (auto a : alloc_list) {
        std::string id = std::string(a.id);
        RikerIO::dealloc(profile, id);
    }

    return EXIT_SUCCESS;

}
