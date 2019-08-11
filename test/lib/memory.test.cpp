#include "lib/mockstubclient.h"
#include "rikerio.h"
#include "gtest/gtest.h"

#include "common/error.h"

using ::testing::Throw;
using ::testing::Return;

TEST(LibraryMemory, AllocateMemoryFailed) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Throw(RikerIO::OutOfSpaceError()));

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName");

    try {

        task.alloc(100);
        FAIL();

    } catch (RikerIO::OutOfSpaceError &e) {
        return;
    } catch (...) {
        FAIL();
    }

    FAIL();

}


TEST(LibraryMemory, AllocateMemory) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Return(123));

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName", -1, false);


    try {
        RikerIO::Client::Allocation& a = task.alloc(100);
        ASSERT_EQ(a.getOffset(), 123);
        ASSERT_EQ(a.getSize(), 100);
    } catch (...) {
        FAIL();
    }

}


/* The purpose is, that we fake a allocate call and imitate a
 * no such permission response, on the paper the API calls are all good
 * and valid */
TEST(LibraryMemory, DeallocateMemoryFromAnotherTask) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Return(100));

    EXPECT_CALL(mClient, memory_dealloc(100, "someToken"))
    .Times(1)
    .WillOnce(Throw(RikerIO::PermissionError()));

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName", -1, false);
    RikerIO::Client::Allocation& a = task.alloc(100);

    try {
        task.dealloc(a);
        FAIL();
    } catch (RikerIO::PermissionError& e) {
        return;
    } catch (...) {
        FAIL();
    }

}


TEST(LibraryMemory, DeallocateMemory) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Return(0));

    EXPECT_CALL(mClient, memory_dealloc(0, "someToken"))
    .Times(1);

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName", -1, false);
    RikerIO::Client::Allocation& alloc = task.alloc(100);

    try {
        task.dealloc(alloc);
    } catch (...) {
        FAIL();
    }

}

#if 0
TEST(LibraryMemory, Disabled_ListLocalAllocations) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("someToken"));

    EXPECT_CALL(mClient, memory_alloc(100, "someToken"))
    .Times(1)
    .WillOnce(Return(0));

    EXPECT_CALL(mClient, memory_alloc(123, "someToken"))
    .Times(1)
    .WillOnce(Return(875));


    RikerIO::Profile p(mClient, "default");
    p.registerTask("taskName", -1, false);

    auto resA = p.getLocalAlloc();

    ASSERT_EQ(0, resA.size());

    p.alloc(100);

    auto resB = p.getLocalAlloc();

    ASSERT_EQ(1, resB.size());
    ASSERT_EQ(0, resB[0]->getOffset());
    ASSERT_EQ(100, resB[0]->getSize());

    p.alloc(123);

    auto resC = p.getLocalAlloc();

    ASSERT_EQ(2, resC.size());
    ASSERT_EQ(0, resC[0]->getOffset());
    ASSERT_EQ(100, resC[0]->getSize());
    ASSERT_EQ(875, resC[1]->getOffset());
    ASSERT_EQ(123, resC[1]->getSize());




}

#endif
