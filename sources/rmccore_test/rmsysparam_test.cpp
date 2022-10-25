#include "stdafx.h"

#include "rmccore/restful/rmsyspara.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;



BOOST_AUTO_TEST_SUITE(RmcCoreRMSystemParaTest)

const std::string sysParamJsonString = R"foo(
{
	"ClientGUID": "E7FE96EA8A000960F59BECDC4D6841EB",
	"DeviceName": "Kundy.nextlabs.com",
	"PlatformID": 0,
	"ProcessorArch": 1,
	"ProductInfo": {
		"AppName": "SkyDRM Desktop for Windows",
		"MajorVer": 10,
		"MinorVer": 0,
		"BuildNo": 0,
		"AppPath": "",
		"Publisher": "",
		"NewVersion": "",
		"DownloadURL": "",
		"Checksum": ""
	}
}
)foo";


BOOST_AUTO_TEST_CASE(TestRMSystemParaImportFromString)
{
    RetValue result = RETSUCCEED();

    RMSystemPara sysParam, sysParam2;

    //Test ImportFromString
    result = sysParam.ImportFromString(sysParamJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);

    std::string strClientId = "E7FE96EA8A000960F59BECDC4D6841EB";
    BOOST_CHECK_EQUAL(sysParam.GetClientID(), strClientId);

    std::string strDeviceName = "Kundy.nextlabs.com";
    BOOST_CHECK_EQUAL(sysParam.GetDeviceID(), strDeviceName);

    std::string strDeviceType = "Windows Desktop";
    BOOST_CHECK_EQUAL(sysParam.GetDeviceType(), strDeviceType);

    //Test ExportToString
    std::string str1 = sysParam.ExportToString();
    
    auto ret = sysParam2.ImportFromString(str1);
    BOOST_CHECK_EQUAL(ret.GetCode(), RMCCORE_ERROR_SUCCESS);
    
    std::string str2 = sysParam2.ExportToString();
    BOOST_CHECK_EQUAL(str1, str2);

}

BOOST_AUTO_TEST_SUITE_END()
