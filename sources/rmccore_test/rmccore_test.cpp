#include "stdafx.h"

#define BOOST_TEST_MODULE RMCCORE
#include <boost/test/unit_test.hpp>
#include "rmccore/base/coreinit.h"

using namespace RMCCORE;

struct RmcCoreTestGlobalFixture {
    RmcCoreTestGlobalFixture() {
        BOOST_TEST_MESSAGE( "ctor fixture");
    }
    void setup() {
        BOOST_TEST_MESSAGE( "setup fixture");
        CoreInit("DummyAppId", "http://DummyRouter.com");
    }
    void teardown() {
        BOOST_TEST_MESSAGE( "teardown fixture");
        CoreCleanup();
    }
    ~RmcCoreTestGlobalFixture() {
        BOOST_TEST_MESSAGE( "dtor fixture");
    }
};

BOOST_TEST_GLOBAL_FIXTURE( RmcCoreTestGlobalFixture );
