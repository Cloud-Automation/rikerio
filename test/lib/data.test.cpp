#include "lib/mockstubclient.h"
#include "rikerio.h"
#include "gtest/gtest.h"

#include "common/error.h"
#include "common/datatypes.h"

using ::testing::Throw;
using ::testing::Return;
using ::testing::_;
using ::testing::Eq;

TEST(LibraryMemory, CreateData) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Return(123));

    RikerIO::Data dA(RikerIO::Datatype::UINT8, 129, 0, 8);
    RikerIO::Data dB(RikerIO::Datatype::UINT16, 123, 0, 16);
    RikerIO::Data dC(RikerIO::Datatype::FLOAT, 125, 0, 32);

    EXPECT_CALL(mClient, data_create(Eq(dA), "testDataAId", "someToken")).Times(1);
    EXPECT_CALL(mClient, data_create(Eq(dB), "testDataBId", "someToken")).Times(1);
    EXPECT_CALL(mClient, data_create(Eq(dC), "testDataCId", "someToken")).Times(1);

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName");
    RikerIO::Client::Allocation& alloc = task.alloc(100);

    RikerIO::Client::Data<uint8_t> dataA = alloc.createData<uint8_t>(6,0);
    RikerIO::Client::Data<uint16_t> dataB = alloc.createData<uint16_t>();
    RikerIO::Client::Data<float> dataC = alloc.createData<float>();

    try {

        task.registerData(dataA, "testDataAId");
        task.registerData(dataB, "testDataBId");
        task.registerData(dataC, "testDataCId");

    } catch (...) {
        FAIL();
    }

}
