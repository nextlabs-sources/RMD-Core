#include "stdafx.h"

#include "rmccore/restful/rmtoken.h"
//#include "common/celog2/celog.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMTokenTest)



const std::string tokenRMSResponse = R"foo(
    {  
       "statusCode":200,
       "message":"OK",
       "serverTime":1552032187550,
       "results":{  
          "token":"F26D12A99E81CA1C03B530E1F165065FC6DE50280A1EF395714062EA2CA10C10"
       }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestTokenImportExport)
{
    RetValue result = RETSUCCEED();

    RMToken token1, token2, token3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = token1.ExportToString();
    BOOST_CHECK(!s1.empty());


    // Test ImportFromRMSResponse()
    result = token1.ImportFromRMSResponse(tokenRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!token1.GetKey().empty());

    // Test ExportToString()
    s1 = token1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = token2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!token2.GetKey().empty());
    s2 = token2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root = token1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromJson()
    result = token3.ImportFromJson(root);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!token3.GetKey().empty());
    s2 = token3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}



const std::string tokenPoolRMSResponse = R"foo({
        "statusCode": 200,
        "message": "OK",
        "serverTime": 1552290524371,
        "results": {
            "tokens": {
                "6840F701C5A19215CC5892B7CD82803F": {
                    "otp": "914E4953059B5B9162F2B16D87DE20A2",
                    "token": "086D797D9E9D319304EF10AD9B11B26F1E23DCB7ACAB8DADA02173D3236B8672"
                },
                "744F3702CC161E08E779D04E495D5164": {
                    "otp": "C4BDEED41E90B06E2F412DFF08A84F7E",
                    "token": "66CC0D02E6F63B34B6E543533397E2ADC313378D033C428D3CEFC3CA9FF1A9B5"
                }
            },
            "ml": "0"
        }
})foo";

BOOST_AUTO_TEST_CASE(TestTokenPoolImportExport)
{
    RetValue result = RETSUCCEED();

    RMTokenPool tokenPool1, tokenPool2, tokenPool3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = tokenPool1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = tokenPool1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = tokenPool1.ImportFromRMSResponse(tokenPoolRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(tokenPool1.size(), 0U);

    // Test ExportToString()
    s1 = tokenPool1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = tokenPool2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(tokenPool2.size(), 0U);
    s2 = tokenPool2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = tokenPool1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = tokenPool3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(tokenPool3.size(), 0U);
    s2 = tokenPool3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}



BOOST_AUTO_TEST_SUITE_END()
