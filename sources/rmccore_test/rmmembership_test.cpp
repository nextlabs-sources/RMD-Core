#include "stdafx.h"

#include "rmccore/restful/rmmembership.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMMembershipTest)

const std::string membershipRMSResponse = R"foo(
    {
        "id": "m24@skydrm.com",
        "type": 0,
        "tenantId": "48726a20-493e-4258-8d3d-25ae75793138",
        "projectId": 3
    }
)foo";

BOOST_AUTO_TEST_CASE(TestMembershipImportExport)
{
    RetValue result = RETSUCCEED();

    RMMembership membership1, membership2, membership3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = membership1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = membership1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = membership1.ImportFromRMSResponse(membershipRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!membership1.GetID().empty());
    BOOST_CHECK(!membership1.GetTenantID().empty());
    BOOST_CHECK(membership1.GetTokenGroupName().empty());

    // Test ExportToString()
    s1 = membership1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = membership2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!membership2.GetID().empty());
    BOOST_CHECK(!membership2.GetTenantID().empty());
    BOOST_CHECK(membership2.GetTokenGroupName().empty());
    s2 = membership2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = membership1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = membership3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!membership3.GetID().empty());
    BOOST_CHECK(!membership3.GetTenantID().empty());
    BOOST_CHECK(membership3.GetTokenGroupName().empty());
    s2 = membership3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_SUITE_END()
