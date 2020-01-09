#include "gtest/gtest.h"
#include "common/type.h"

TEST(Type, ConvertFromString) {

    RikerIO::Type t_a("bit");

    ASSERT_EQ(RikerIO::Type::Types::BIT, t_a.get_type());
    ASSERT_EQ(1, t_a.get_bit_size());
    ASSERT_STREQ("bit", t_a.to_string().c_str());

    RikerIO::Type t_b("int8");

    ASSERT_EQ(RikerIO::Type::Types::INT8, t_b.get_type());
    ASSERT_EQ(8, t_b.get_bit_size());
    ASSERT_STREQ("int8", t_b.to_string().c_str());

    RikerIO::Type t_c("uint8");

    ASSERT_EQ(RikerIO::Type::Types::UINT8, t_c.get_type());
    ASSERT_EQ(8, t_c.get_bit_size());
    ASSERT_STREQ("uint8", t_c.to_string().c_str());

    RikerIO::Type t_d("int16");

    ASSERT_EQ(RikerIO::Type::Types::INT16, t_d.get_type());
    ASSERT_EQ(16, t_d.get_bit_size());
    ASSERT_STREQ("int16", t_d.to_string().c_str());

    RikerIO::Type t_e("uint16");

    ASSERT_EQ(RikerIO::Type::Types::UINT16, t_e.get_type());
    ASSERT_EQ(16, t_e.get_bit_size());
    ASSERT_STREQ("uint16", t_e.to_string().c_str());

    RikerIO::Type t_f("int32");

    ASSERT_EQ(RikerIO::Type::Types::INT32, t_f.get_type());
    ASSERT_EQ(32, t_f.get_bit_size());
    ASSERT_STREQ("int32", t_f.to_string().c_str());

    RikerIO::Type t_g("uint32");

    ASSERT_EQ(RikerIO::Type::Types::UINT32, t_g.get_type());
    ASSERT_EQ(32, t_g.get_bit_size());
    ASSERT_STREQ("uint32", t_g.to_string().c_str());

    RikerIO::Type t_h("int64");

    ASSERT_EQ(RikerIO::Type::Types::INT64, t_h.get_type());
    ASSERT_EQ(64, t_h.get_bit_size());
    ASSERT_STREQ("int64", t_h.to_string().c_str());

    RikerIO::Type t_i("uint64");

    ASSERT_EQ(RikerIO::Type::Types::UINT64, t_i.get_type());
    ASSERT_EQ(64, t_i.get_bit_size());
    ASSERT_STREQ("uint64", t_i.to_string().c_str());

    RikerIO::Type t_j("float");

    ASSERT_EQ(RikerIO::Type::Types::FLOAT, t_j.get_type());
    ASSERT_EQ(32, t_j.get_bit_size());
    ASSERT_STREQ("float", t_j.to_string().c_str());

    RikerIO::Type t_k("double");

    ASSERT_EQ(RikerIO::Type::Types::DOUBLE, t_k.get_type());
    ASSERT_EQ(64, t_k.get_bit_size());
    ASSERT_STREQ("double", t_k.to_string().c_str());

    RikerIO::Type t_l("1bit");

    ASSERT_EQ(RikerIO::Type::Types::UNDEFINED, t_l.get_type());
    ASSERT_EQ(1, t_l.get_bit_size());
    ASSERT_STREQ("1bit", t_l.to_string().c_str());

    RikerIO::Type t_m("3byte");

    ASSERT_EQ(RikerIO::Type::Types::UNDEFINED, t_m.get_type());
    ASSERT_EQ(24, t_m.get_bit_size());
    ASSERT_STREQ("3byte", t_m.to_string().c_str());

    RikerIO::Type t_n("4kilobyte");

    ASSERT_EQ(RikerIO::Type::Types::UNDEFINED, t_n.get_type());
    ASSERT_EQ(4*8*1024, t_n.get_bit_size());
    ASSERT_STREQ("4kilobyte", t_n.to_string().c_str());

    try {
        RikerIO::Type t_o("rubbish");
        ASSERT_TRUE(false);
    } catch (RikerIO::Type::TypeError& e) {

    } catch (...) {
        ASSERT_TRUE(false);
    }

    RikerIO::Type t_p(22);

    ASSERT_EQ(RikerIO::Type::Types::UNDEFINED, t_p.get_type());
    ASSERT_EQ(22, t_p.get_bit_size());
    ASSERT_STREQ("22bit", t_p.to_string().c_str());

    try {
        RikerIO::Type t_q(0);
        ASSERT_TRUE(false);
    } catch (RikerIO::Type::TypeError& e) {

    } catch (...) {
        ASSERT_TRUE(false);
    }

    try {
        RikerIO::Type t_r(RikerIO::Type::UNDEFINED);
        ASSERT_TRUE(false);
    } catch (RikerIO::Type::TypeError& e) {

    } catch (...) {
        ASSERT_TRUE(false);
    }


}

TEST(Type, Compare) {

    RikerIO::Type a("1bit");
    RikerIO::Type b("uint32");
    RikerIO::Type c("bit");
    RikerIO::Type d("1bit");

    ASSERT_FALSE(a==b);
    ASSERT_TRUE(a==d);
    ASSERT_FALSE(b==c);


}
