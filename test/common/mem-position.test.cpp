#include "gtest/gtest.h"
#include "common/mem-position.h"

TEST(MemoryPosition, ConvertFromString) {

    RikerIO::MemoryPosition mp_a("0.0");

    ASSERT_EQ(0, mp_a.get_byte_offset());
    ASSERT_EQ(0, mp_a.get_bit_index());
    ASSERT_STREQ("0.0", mp_a.to_string().c_str());

    RikerIO::MemoryPosition mp_b("10.3");

    ASSERT_EQ(10, mp_b.get_byte_offset());
    ASSERT_EQ(3, mp_b.get_bit_index());
    ASSERT_STREQ("10.3", mp_b.to_string().c_str());

    RikerIO::MemoryPosition mp_c("33");

    ASSERT_EQ(33, mp_c.get_byte_offset());
    ASSERT_EQ(0, mp_c.get_bit_index());
    ASSERT_STREQ("33.0", mp_c.to_string().c_str());

    try {
        RikerIO::MemoryPosition mp_d("abc");
        ASSERT_TRUE(false);
    } catch (RikerIO::MemoryPosition::MemoryPositionError& e) {

    }

}

TEST(MemoryPosition, AddValues) {

    RikerIO::MemoryPosition mp_a("11.3");

    ASSERT_EQ(11, mp_a.get_byte_offset());
    ASSERT_EQ(3, mp_a.get_bit_index());

    mp_a.add_byte_offset(5);

    ASSERT_EQ(16, mp_a.get_byte_offset());
    ASSERT_EQ(3, mp_a.get_bit_index());

    mp_a.add_bit_offset(17);

    ASSERT_EQ(18, mp_a.get_byte_offset());
    ASSERT_EQ(4, mp_a.get_bit_index());


}
