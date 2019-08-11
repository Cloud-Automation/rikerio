#include "lib/mockstubclient.h"
#include "rikerio.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "common/error.h"

using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

TEST(LibraryTask, RegisterTaskFailed) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Throw(RikerIO::ConnectionError()));

    RikerIO::Client::Profile p(mClient, "default");

    try {

        p.registerTask("taskName", -1, false);

        FAIL();
    } catch (RikerIO::ConnectionError &e) {
        return;
    } catch (...) {
        FAIL();
    }

    FAIL();

}

TEST(LibraryTask, UnregisterTask) {

    MockClient mClient;

    EXPECT_CALL(mClient, task_register("taskName", -1, false))
    .Times(1)
    .WillOnce(Return("abc123"));

    EXPECT_CALL(mClient, task_unregister("abc123"))
    .Times(1);

    RikerIO::Client::Profile p(mClient, "default");
    RikerIO::Client::Task& task = p.registerTask("taskName", -1, false);

    ASSERT_EQ("abc123", task.getToken());

    try {
        p.unregisterTask(task);
    } catch (...) {
        FAIL();
    }

}
