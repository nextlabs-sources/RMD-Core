#include "stdafx.h"

#include "rmccore/restful/rmactivitylog.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;



BOOST_AUTO_TEST_SUITE(RmcCoreRMNXLFileLogsTest)

const std::string getFileLogRequestResponse = R"foo(
{
	"statusCode": 200,
	"message": "Success",
	"serverTime": 1484193294457,
	"results": {
		"data": {
			"name": "AdhocSharing2-2016-12-30-03-47-02.pptx.nxl",
			"duid": "63369AB6FBCAC06EDD007E65DC178F9F",
			"logRecords": [
				{
					"email": "naresh.nallagatla@nextlabs.com",
					"operation": "Share",
					"deviceType": "WebApp",
					"deviceId": "118.189.77.114",
					"accessTime": 1484115333911,
					"accessResult": "Allow"
				},
				{
					"email": "naresh.nallagatla@nextlabs.com",
					"operation": "View",
					"deviceType": "WebApp",
					"deviceId": "118.189.77.114",
					"accessTime": 1484115080584,
					"accessResult": "Allow"
				},
				{
					"email": "naresh.nallagatla@nextlabs.com",
					"operation": "Protect",
					"deviceType": "WebApp",
					"deviceId": "118.189.77.114",
					"accessTime": 1483069622214,
					"accessResult": "Allow"
				}
			]
		},
		"totalCount": 3
	}
}
)foo";


BOOST_AUTO_TEST_CASE(TestRMNXLFileLogsImportFromRMSResponse)
{
    RetValue result = RETSUCCEED();

    RMNXLFileLogs fileLog, fileLog2;

    //Test ImportFromString
    result = fileLog.ImportFromRMSResponse(getFileLogRequestResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);

    BOOST_CHECK_EQUAL(fileLog.GetTotalLogNum(), 3);

    std::string strFileName = "AdhocSharing2-2016-12-30-03-47-02.pptx.nxl";
    BOOST_CHECK_EQUAL(fileLog.GetFileName(), strFileName);

    std::string strDuid = "63369AB6FBCAC06EDD007E65DC178F9F";
    BOOST_CHECK_EQUAL(fileLog.GetDUID(), strDuid);

    //Test ExportToString
    std::string str1 = fileLog.ExportToString();

    //Test ImportFromString
    auto ret = fileLog2.ImportFromString(str1);
    BOOST_CHECK_EQUAL(ret.GetCode(), RMCCORE_ERROR_SUCCESS);

    std::string str2 = fileLog2.ExportToString();
    BOOST_CHECK_EQUAL(str1, str2);

}

BOOST_AUTO_TEST_SUITE_END()
