#include "stdafx.h"

#include "rmccore/restful/rmheartbeat.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMHeartbeatTest)


const std::string policyConfigRMSResponse = R"foo(
    {
        "tokenGroupName": "skydrm.com_a_1",
        "configurationModifiedTimeStamp": 1519891154798,
        "watermarkConfig": {},
        "classificationCategories": [
            {
                "name": "Sensitivity",
                "multiSelect": false,
                "mandatory": false,
                "labels": [
                    {
                        "name": "Sensitivity",
                        "default": false
                    }
                ]
            }
        ]
    }
)foo";

BOOST_AUTO_TEST_CASE(TestPolicyConfigImportExport)
{
    RetValue result = RETSUCCEED();

    RMPolicyConfig policyConfig1, policyConfig2, policyConfig3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = policyConfig1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = policyConfig1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = policyConfig1.ImportFromRMSResponse(policyConfigRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(policyConfig1.GetPolicyLastModify(), 0);
    BOOST_CHECK_GT(policyConfig1.GetClassificationCategories().size(), 0U);

    // Test ExportToString()
    s1 = policyConfig1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = policyConfig2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(policyConfig2.GetPolicyLastModify(), 0);
    BOOST_CHECK_GT(policyConfig2.GetClassificationCategories().size(), 0U);
    s2 = policyConfig2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = policyConfig1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = policyConfig3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(policyConfig3.GetPolicyLastModify(), 0);
    BOOST_CHECK_GT(policyConfig3.GetClassificationCategories().size(), 0U);
    s2 = policyConfig3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

const std::string heartbeatRMSResponse = R"foo(
    {
        "statusCode": 200,
        "message": "OK",
        "serverTime": 1520401503023,
        "results": {
            "heartbeatFrequency": 1000,
            "policyConfigData": [
                {
                    "tokenGroupName": "skydrm.com",
                    "configurationModifiedTimeStamp": 1517998870135,
                    "watermarkConfig": {},
                    "classificationCategories": []
                },
                {
                    "tokenGroupName": "skydrm.com_a_1",
                    "configurationModifiedTimeStamp": 1519891154798,
                    "watermarkConfig": {},
                    "classificationCategories": [
                        {
                            "name": "Sensitivity",
                            "multiSelect": false,
                            "mandatory": false,
                            "labels": [
                                {
                                    "name": "Sensitivity",
                                    "default": false
                                }
                            ]
                        }
                    ]
                }
            ],
            "tokenGroupResourceTypeMapping": {
                "skydrm.com_a_1": "t_18bdb2fb_89cf_4437_92e9_ef752a07014c"
            },
            "icenetUrl": "https://pf-manjaro:8443",
            "watermarkConfig": {
                "serialNumber": "f8cf145d",
                "content": "{\r\n\"text\": \"$(User)\\n $(Date) $(Time)\",\r\n\"transparentRatio\": 70,\r\n\"fontName\": \"Arial Unicode MS\",\r\n\"fontSize\": 26,\r\n\"fontColor\": \"#008000\",\r\n\"rotation\": \"Anticlockwise\",\r\n\"repeat\": true,\r\n\"density\": \"Dense\"\r\n}"
            }
        }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestHeartbeatImportExport)
{
    RetValue result = RETSUCCEED();

    RMHeartbeat heartbeat1, heartbeat2, heartbeat3;
    std::string s1, s2;
    const std::string tokenGroupName("skydrm.com_a_1");
    const std::string resourceType("t_18bdb2fb_89cf_4437_92e9_ef752a07014c");
    std::string tmp;

    // Test exporting empty object
    s1 = heartbeat1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = heartbeat1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = heartbeat1.ImportFromRMSResponse(heartbeatRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(heartbeat1.GetFrequency(), 0U);
    BOOST_CHECK_GT(heartbeat1.GetPolicyConfigCount(), 0U);
    BOOST_CHECK(heartbeat1.GetTokenGroupResourceType(tokenGroupName, tmp));
    BOOST_CHECK_EQUAL(tmp, resourceType);
    BOOST_CHECK(!heartbeat1.GetIcenetUrl().empty());

    // Test ExportToString()
    s1 = heartbeat1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = heartbeat2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(heartbeat2.GetFrequency(), 0U);
    BOOST_CHECK_GT(heartbeat2.GetPolicyConfigCount(), 0U);
    BOOST_CHECK(heartbeat2.GetTokenGroupResourceType(tokenGroupName, tmp));
    BOOST_CHECK_EQUAL(tmp, resourceType);
    BOOST_CHECK(!heartbeat2.GetIcenetUrl().empty());
    s2 = heartbeat2.ExportToString();

    // Test ExportToJson()
    nlohmann::json root2 = heartbeat1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = heartbeat3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(heartbeat3.GetFrequency(), 0U);
    BOOST_CHECK_GT(heartbeat3.GetPolicyConfigCount(), 0U);
    BOOST_CHECK(heartbeat3.GetTokenGroupResourceType(tokenGroupName, tmp));
    BOOST_CHECK_EQUAL(tmp, resourceType);
    BOOST_CHECK(!heartbeat3.GetIcenetUrl().empty());
    s2 = heartbeat3.ExportToString();
}


BOOST_AUTO_TEST_SUITE_END()
