#include "stdafx.h"

#include "rmccore/restful/rmuser.h"
#include <boost/test/unit_test.hpp>

#include "common.h"

using namespace RMCCORE;

const ExeDir<char> kExeDir;
const ExeDir<wchar_t> kExeDirW;

BOOST_AUTO_TEST_SUITE(RmcCoreRMUserTest)

const std::string userRMSResponse = R"foo(
    {
        "statusCode": 200,
        "message": "Authorized",
        "serverTime": 1526278702322,
        "extra": {
            "userId": 21,
            "ticket": "55F23B75BA953CAAB8B9754F54CBE8F1",
            "tenantId": "48726a20-493e-4258-8d3d-25ae75793138",
            "ttl": 1526365087606,
            "name": "Abraham Lincoln",
            "email": "abraham.lincoln@qapf1.qalab01.nextlabs.com",
            "memberships": [
                {
                    "id": "m24@skydrm.com",
                    "type": 0,
                    "tenantId": "48726a20-493e-4258-8d3d-25ae75793138",
                    "projectId": 3
                },
                {
                    "id": "m65@t-d0fb1eabcfc94b20984c5816071bc004",
                    "type": 0,
                    "tenantId": "5994bdab-880c-4704-8a84-2f90d884d572",
                    "projectId": 5
                },
                {
                    "id": "m67@t-fc00458990ec4975833bc4f8e2157e44",
                    "type": 0,
                    "tenantId": "ae54a444-cab5-481c-9d03-198e9cbee267",
                    "projectId": 39
                },
                {
                    "id":"user22@ed8b24a9-40ba-482f-8d0a-4ff1781317e7_system",
                    "type":0,
                    "tenantId":"ed8b24a9-40ba-482f-8d0a-4ff1781317e7_system"
                }
            ],
            "defaultTenant": "skydrm.com",
            "defaultTenantUrl": "https://sudong.nextlabs.com/rms",
            "attributes": {
                "aduser": [
                    "abraham.lincoln"
                ],
                "Company": [
                    "biflex"
                ],
                "Department": [
                    "HR"
                ],
                "addomain": [
                    "qapf1.qalab01.nextlabs.com"
                ],
                "displayName": [
                    "Abraham Lincoln"
                ],
                "objectSid": [
                    "S-1-5-21-2018228179-1005617703-974104760-1200"
                ],
                "memberOf": [
                    "CN=Prop Class 1 - Source Code,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=SAP EM Admin,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Tes,OU=License Groups,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=ReportingGroup {6d07c1f1-3416-4077-b5b8-08029cf31da1},DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Embraer Admin,OU=Embraer,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RIGHTSWATCH_SECURITY_ROLE_2,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RIGHTSWATCH_SECURITY_ROLE_1,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_AUDITING,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_MONIT,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_USERS,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_CLASSIFICATIONS,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_CONFIGURATION,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_LICENSING,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_ROLES,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_RULES,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RightsWATCH_BLACKLIST,OU=RIGHTSWATCH,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=UK GSC 2014 O,OU=License Groups,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Ametek_DSP 050056205,OU=License Groups,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=LMT Administrators,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=RMT Administrators,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=PrivReportingGroup {773f1cc1-7b9a-4963-bd9a-f5db42ca1731},DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=PrivUserGroup {773f1cc1-7b9a-4963-bd9a-f5db42ca1731},DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Export Control Access Rights,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=BAE Systems Intellectual Property Access Rights,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Government Security Access Rights,OU=bae-group,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Board Members,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Presidential Soccer Team,OU=Groups,OU=Presidents,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Administrators,CN=Builtin,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Domain Admins,CN=Users,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Schema Admins,CN=Users,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com",
                    "CN=Enterprise Admins,CN=Users,DC=qapf1,DC=qalab01,DC=nextlabs,DC=com"
                ],
                "email": [
                    "abraham.lincoln@qapf1.qalab01.nextlabs.com"
                ]
            }
        }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestUserImportExport)
{
    RetValue result = RETSUCCEED();

    RMUser user1, user2, user3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = user1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = user1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = user1.ImportFromRMSResponse(userRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!user1.GetName().empty());
    BOOST_CHECK(!user1.GetEmail().empty());
    BOOST_CHECK(!user1.GetDefaultMembership().GetID().empty());
    BOOST_CHECK(!user1.GetDefaultMembership().GetTenantID().empty());
    BOOST_CHECK(user1.GetDefaultMembership().GetTokenGroupName().empty());

    // Test ExportToString()
    s1 = user1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = user2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!user2.GetName().empty());
    BOOST_CHECK(!user2.GetEmail().empty());
    BOOST_CHECK(!user2.GetDefaultMembership().GetID().empty());
    BOOST_CHECK(!user2.GetDefaultMembership().GetTenantID().empty());
    BOOST_CHECK(user2.GetDefaultMembership().GetTokenGroupName().empty());
    s2 = user2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = user1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = user3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!user3.GetName().empty());
    BOOST_CHECK(!user3.GetEmail().empty());
    BOOST_CHECK(!user3.GetDefaultMembership().GetID().empty());
    BOOST_CHECK(!user3.GetDefaultMembership().GetTenantID().empty());
    BOOST_CHECK(user3.GetDefaultMembership().GetTokenGroupName().empty());
    s2 = user3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}



const std::string certificateJsonString = R"foo(
{
	"statusCode": 200,
	"message": "OK",
	"serverTime": 1587954054228,
	"results": {
		"certficates": "-----BEGIN CERTIFICATE-----\n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\r\n-----END CERTIFICATE-----\n-----BEGIN CERTIFICATE-----\n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\r\n-----END CERTIFICATE-----\n-----BEGIN CERTIFICATE-----\nMIIFlTCCBH2gAwIBAgIkMGQyYTZjY2UtNTQ4MS00NDAzLWI0MTctNTBjNTlhYTM5YmRiMA0GCSqG\r\nSIb3DQEBBQUAMG4xCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRIwEAYDVQQHEwlT\r\nYW4gTWF0ZW8xDDAKBgNVBAsTA3JtczERMA8GA1UEChMIbmV4dGxhYnMxFTATBgNVBAMTDHIuc2t5\r\nZHJtLmNvbTAgFw0yMDA0MTUwMTA1MjdaGA8yMTIwMDMyMjAxMDUyN1owbjELMAkGA1UEBhMCVVMx\r\nEzARBgNVBAgTCkNhbGlmb3JuaWExEjAQBgNVBAcTCVNhbiBNYXRlbzEMMAoGA1UECxMDcm1zMREw\r\nDwYDVQQKEwhuZXh0bGFiczEVMBMGA1UEAxMMci5za3lkcm0uY29tMIIDJTCCAhgGCSqGSIb3DQED\r\nATCCAgkCggEBANMQElspTb2FaBTf1Lq03HZ99qmZye36j417ElUfjXHvYDI1dAXH8R7hR9sDMnFv\r\nyP2F7QJ1hSaDYNFr12FWPX0WWdTXPa7WF/PkIj9IvO+kIYYMP8Q5PSdUVneyJFnoUvUlTTrFjA1j\r\n3XneLY2GjNlA3s9aJ0YF2w7udiAgw50PZIZgZYDqrM4W+3D7fHWeqaq6tNy/lBiRsM6U7E09WVQh\r\nfG6EqSdPGrhgc7353IUeVjuQRVuDl9rjobmYYHu3aZzqCAWn/wE+9E/eevgw8f0FH/rsU5zkRS2C\r\nKQmK4+4gCKudt7LJSDEsvAE3wILWZyYY4b/l1QBugQ3Hqn8ebuMCggEAZKzrpfe8gD7ylzHJxq4A\r\nm4b8UgH4G8K4+EiQ/PccrVHBQp/SYaKnFciUYVTg5OKO9rLUk8wXOfVlnp8U3RQDf1/nKzuk2byz\r\nuVuEF72kjxGOYcghTPjVWNpndPCLWNl7LM4g9aovjpU5wBTndh5OYzbP/DUSfd1ScgZ2aucgRcEb\r\nD/TadhclI3E7Mcnxirq6kmEr3hBRQfBNtdo8Oc3lxod7f4zZaUn8yHbiwSJPuRiNcU/dbLgGgviW\r\neDOtS1E1So1YWY5rLe9FcaWXrTm9MXfVSyTKUY7amW7tuoox1Ydu/tiqRAI8yfE9hty03fzziceh\r\nQ1CC72lwNgNjgyWVTgOCAQUAAoIBADTj2JFEu5LLfJeG6Kt1ycbtFEVp5YgSN4vDIz5biLrjAhBZ\r\nyo5msQ//fG45I+4IKSW1MUz0JN+yYfXz3tjBIQI6pAJEjvCAii1Ms3cy/5dEp6nzQ45zk/zfTjtE\r\nRp+94s+dY+ANKive02vKyxmuZv0rapKcZ9KiqL25AW/kvgdpzi1qqahagZ22rtfqaToMbUh1+AOy\r\nur1LHc6HpmgdyZU7oKxt6+1SzQO2pM3o51oT8Ok4/XPdzXVV7dBLT7nr5Ud4hnI53yhrIcIk6oMf\r\nObGLsfSAXAnDqLn4psdLZqVZaX1pupIwa3MzJXPVHIv2DE72OL6QvGp4G89bTe1UZ3WjFjAUMBIG\r\nA1UdEwEB/wQIMAYBAf8CAQUwDQYJKoZIhvcNAQEFBQADggEBAAqtS2cmp9hfjGWkuahToukarrwP\r\nFfq+LCMP0U4q43++jmMyPyZf5SzZy57jZLlSZNcXU8GqQ0+pkUWVlxP/MAwinZxgc8VxeGAXJzYy\r\nO/Cq9cTnP98eOhdHZDpzbp10SpzNjNiGN5YsFYXG8wR6Mo7Ciu3PB8Qt0RVmG92FBTl4ienKprUj\r\nmX1ZO9Ci6AV38/wQhcMH73K2CgS8JmuQS2JExe5GKoZtfGzkWcM7K66kTCVaW+BacqBUBpwH1zFk\r\nUZ/ZBoIJMS8vkICCC9bKMn/it5kME9WkDGplXxoso26Af4+7WQeFCM+2qhSR6/U/tX8O6zgaoLKf\r\nfzrxadqrx7w=\r\n-----END CERTIFICATE-----\n"
	}
}
)foo";

void TestUserProtectCommon(const std::string &plainFile1Name, const std::string &plainFile2Name)
{
    RetValue result = RETSUCCEED();

    // Set up enough information to call the protect functions.
    RMUser user;
    result = user.ImportFromRMSResponse(userRMSResponse);
    BOOST_REQUIRE_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);

    const std::string memberId("m24@skydrm.com");
    RMMembership* rm = user.FindMembershipFromID(memberId);
    result = rm->m_certificate.ImportFromRMSResponse(certificateJsonString);
    BOOST_REQUIRE_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_REQUIRE(!rm->m_certificate.GetAgreement0().empty());
    BOOST_REQUIRE(!rm->m_certificate.GetAgreement1().empty());

    const NXLAttributes attrs = {NULL, NULL, NULL, ""};
    RMToken token(
        "12345678901234567890123456789012",                                 // dummy DUID
        "1234567890123456789012345678901234567890123456789012345678901234", // dummy key
        1);                                                                 // maintenance level
    RMActivityLog activityLog;
    std::size_t pos;



    // Test ProtectLocalFile()
    const std::string plainFile1Path(kExeDir.getExeDir() + '\\' + plainFile1Name);
    pos = plainFile1Name.rfind('.');
    const std::string plainFile1Ext = (pos == plainFile1Name.npos ? "" : plainFile1Name.substr(pos + 1));
    const std::string nxlFileName("new.extfake.nxl");  // File extension is differnt from the original extension.
    const std::string nxlFilePath(kExeDir.getExeDir() + '\\' + nxlFileName);
    RMNXLFile rmNxlFile1a("");
    result = user.ProtectLocalFile(plainFile1Path, nxlFilePath, attrs, token, activityLog, rmNxlFile1a, memberId);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFilePath(), plainFile1Path);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFileExt(), plainFile1Ext);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetOrgFileExtension(), '.' + plainFile1Ext);
    RMNXLFile rmNxlFile1b(nxlFilePath);
    BOOST_CHECK_EQUAL(rmNxlFile1b.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFile1b.GetOrgFileExtension(), '.' + plainFile1Ext);
    rmNxlFile1b.Close();
    rmNxlFile1a.Close();

#ifdef _WIN32
    const std::wstring plainFile1NameW(toUtf16(plainFile1Name));
    const std::wstring plainFile1PathW(toUtf16(plainFile1Path));
    const std::wstring nxlFileWName(L"newW.extfake.nxl");   // File extension is differnt from the original extension.
    const std::wstring nxlFileWPath(kExeDirW.getExeDir() + L'\\' + nxlFileWName);
    RMNXLFile rmNxlFileW1a(L"");
    result = user.ProtectLocalFile(plainFile1PathW, nxlFileWPath, attrs, token, activityLog, rmNxlFileW1a, memberId);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFilePath(), plainFile1Path);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFileExt(), plainFile1Ext);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetOrgFileExtension(), '.' + plainFile1Ext);
    RMNXLFile rmNxlFileW1b(nxlFileWPath);
    BOOST_CHECK_EQUAL(rmNxlFileW1b.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFileW1b.GetOrgFileExtension(), '.' + plainFile1Ext);
    rmNxlFileW1b.Close();
    rmNxlFileW1a.Close();
#endif



    // Test ReProtectLocalFile()
    const std::string plainFile2Path(kExeDir.getExeDir() + '\\' + plainFile2Name);
    pos = plainFile2Name.rfind('.');
    const std::string plainFile2Ext = (pos == plainFile2Name.npos ? "" : plainFile2Name.substr(pos + 1));
    result = user.ReProtectLocalFile(plainFile2Path, nxlFilePath, attrs, token, activityLog, rmNxlFile1a, memberId);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFilePath(), plainFile2Path);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFileName(), plainFile2Name);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetSourceFileExt(), plainFile2Ext);
    // Original file name and extension should remain unchanged.
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFile1a.GetOrgFileExtension(), '.' + plainFile1Ext);
    rmNxlFile1a.Close();

#ifdef _WIN32
    const std::wstring plainFile2NameW(toUtf16(plainFile2Name));
    const std::wstring plainFile2PathW(toUtf16(kExeDir.getExeDir() + '\\' + plainFile2Name));
    result = user.ReProtectLocalFile(plainFile2PathW, nxlFileWPath, attrs, token, activityLog, rmNxlFileW1a, memberId);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFilePath(), plainFile2Path);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFileName(), plainFile2Name);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetSourceFileExt(), plainFile2Ext);
    // Original file name and extension should remain unchanged.
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFileW1a.GetOrgFileExtension(), '.' + plainFile1Ext);
    rmNxlFileW1a.Close();
#endif



    // Test ReProtectLocalFileHeader()
    RMNXLFile rmNxlFile2("");
    result = user.ReProtectLocalFileHeader(nxlFilePath, token, NULL, attrs, memberId, "", activityLog, rmNxlFile2, true);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    // Original file name and extension should remain unchanged.
    BOOST_CHECK_EQUAL(rmNxlFile2.GetOrgFileName(), plainFile1Name);
    BOOST_CHECK_EQUAL(rmNxlFile2.GetOrgFileExtension(), '.' + plainFile1Ext);
    rmNxlFile2.Close();



    // Cleanup
    BOOST_CHECK(std::remove(nxlFilePath.c_str()) == 0);
#ifdef _WIN32
    BOOST_CHECK(std::remove(toUtf8(nxlFileWPath).c_str()) == 0);
#endif
}

BOOST_AUTO_TEST_CASE(TestUserProtectFileWithZeroExt)
{
    TestUserProtectCommon("ZeroExt1", "ZeroExt2");
}

BOOST_AUTO_TEST_CASE(TestUserProtectFileWithOneExt)
{
    TestUserProtectCommon("RMSTokenTest.txt", "ccn.doc");
}

BOOST_AUTO_TEST_CASE(TestUserProtectFileWithTwoExt)
{
    TestUserProtectCommon("TwoExt1.aaa.bbb", "TwoExt2.ccc.ddd");
}

BOOST_AUTO_TEST_SUITE_END()
