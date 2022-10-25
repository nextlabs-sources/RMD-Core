#include "stdafx.h"

#include "rmccore/restful/rmproduct.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMProductTest)

const std::string productJsonString = R"foo(
{
	"statusCode": 200,
	"message": "OK",
	"serverTime": 1585018837311,
	"results": {
		"newVersion": "1.1",
		"downloadURL": "www.abc.com",
		"sha1Checksum": ""
	}
}
)foo";

BOOST_AUTO_TEST_CASE(TestRMRMProductImportExport)
{
    RetValue result = RETSUCCEED();

    RMProduct product1("", 0, 0, 0), product2("", 0, 0, 0), product3("", 0, 0, 0);
    std::string s1, s2;

    // Test ImportFromString()
    result = product1.ImportFromRMSResponse(productJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!product1.GetNewVersionString().empty());

    // Test exporting empty object
    s1 = product1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = product1.ExportToJson();
    BOOST_CHECK(!root.empty());


    // Test ExportToString()
    s1 = product1.ExportToString();
    BOOST_CHECK(!s1.empty());
    result = product2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!product2.GetNewVersionString().empty());
    s2 = product2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = product1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = product3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!product3.GetNewVersionString().empty());
    s2 = product3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_SUITE_END()
