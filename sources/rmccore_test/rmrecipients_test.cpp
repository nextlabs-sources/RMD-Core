#include "stdafx.h"

#include "rmccore/restful/rmrecipients.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMrecipientsTest)

const std::string recipientsJsonString = R"foo(
{
	"statusCode": 200,
	"message": "OK",
	"serverTime": 1585018837311,
	"results": {
		"alreadySharedList": [],
		"newRecipients": [
			"john.tyler@qapf1.qalab01.nextlabs.com"
		]
	}
}
)foo";

BOOST_AUTO_TEST_CASE(TestRMRecipientsImportExport)
{
    //
    // NOTE: We are not testing RMTenant::ImportFromRMSResponse() here,
    // because its code doesn't seem to match any REST API on the RMS side.
    //

    RetValue result = RETSUCCEED();

    RMRecipients recipients1, recipients2, recipients3;
    std::string s1, s2;

    std::vector<std::string> vecAddRecipients;
    vecAddRecipients.push_back("john.tyler@qapf1.qalab01.nextlabs.com");
    recipients1.AddRecipients(vecAddRecipients);
    // Test ImportFromString()
    result = recipients1.ImportFromRMSResponse(recipientsJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!recipients1.GetRecipients().empty());

    // Test exporting empty object
    s1 = recipients1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = recipients1.ExportToJson();
    BOOST_CHECK(!root.empty());


    // Test ExportToString()
    s1 = recipients1.ExportToString();
    BOOST_CHECK(!s1.empty());
    result = recipients2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!recipients2.GetRecipients().empty());
    s2 = recipients2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = recipients1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = recipients3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!recipients3.GetRecipients().empty());
    s2 = recipients3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_SUITE_END()
