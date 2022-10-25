#include "stdafx.h"

#include "rmccore/restful/rmtenant.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMTenantTest)

const std::string tenantJsonString = R"foo(
    {"Tenant":"skydrm.com","RouterURL":"https://www.skydrm.com","ServerURL":"https://www.skydrm.com/rms"}
)foo";

BOOST_AUTO_TEST_CASE(TestTenantImportExport)
{
    //
    // NOTE: We are not testing RMTenant::ImportFromRMSResponse() here,
    // because its code doesn't seem to match any REST API on the RMS side.
    //

    RetValue result = RETSUCCEED();

    RMTenant tenant1, tenant2, tenant3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = tenant1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = tenant1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromString()
    result = tenant1.ImportFromString(tenantJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!tenant1.GetTenant().empty());
    BOOST_CHECK(!tenant1.GetRouterURL().empty());
    BOOST_CHECK(!tenant1.GetRMSURL().empty());

    // Test ExportToString()
    s1 = tenant1.ExportToString();
    BOOST_CHECK(!s1.empty());
    result = tenant2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!tenant2.GetTenant().empty());
    BOOST_CHECK(!tenant2.GetRouterURL().empty());
    BOOST_CHECK(!tenant2.GetRMSURL().empty());
    s2 = tenant2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = tenant1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = tenant3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!tenant3.GetTenant().empty());
    BOOST_CHECK(!tenant3.GetRouterURL().empty());
    BOOST_CHECK(!tenant3.GetRMSURL().empty());
    s2 = tenant3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_SUITE_END()
