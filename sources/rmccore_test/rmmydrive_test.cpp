#include "stdafx.h"

#include "rmccore/restful/rmmyDrive.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMMyDriveTest)



const std::string myDriveRMSResponse = R"foo(
    {
       "statusCode":200,
       "message":"OK",
       "serverTime":1479283719376,
       "results":{
          "usage":2502740,
          "quota":1000000000
       }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestMyDriveImportExport)
{
    RetValue result = RETSUCCEED();

    RMMyDrive myDrive1, myDrive2, myDrive3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = myDrive1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = myDrive1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = myDrive1.ImportFromRMSResponse(myDriveRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myDrive1.GetUsage(), 0);
    BOOST_CHECK_GT(myDrive1.GetQuota(), 0);

    // Test ExportToString()
    s1 = myDrive1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = myDrive2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myDrive2.GetUsage(), 0);
    BOOST_CHECK_GT(myDrive2.GetQuota(), 0);
    s2 = myDrive2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = myDrive1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = myDrive3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myDrive3.GetUsage(), 0);
    BOOST_CHECK_GT(myDrive3.GetQuota(), 0);
    s2 = myDrive3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}



const std::string myVaultFileRMSResponse = R"foo(
    {
       "pathId":"/nxl_myvault_nxl/get started with dropbox-2017-01-13-14-30-28.pdf.nxl",
       "pathDisplay":"/Get Started with Dropbox-2017-01-13-14-30-28.pdf.nxl",
       "repoId":"2c5ee284-8156-483e-abc6-f030c9f037e1",
       "sharedOn":1484289028934,
       "sharedWith": [
               "abraham.lincoln@qapf1.qalab01.nextlabs.com",
               "prateek.kumar@nextlabs.com",
               "john.tyler@qapf1.qalab01.nextlabs.com",
               "barack.obama@qapf1.qalab01.nextlabs.com"
       ],
       "name":"Get Started with Dropbox-2017-01-13-14-30-28.pdf.nxl",
       "duid":"F8DC0C3FB945B3671CE994DD7D82761A",
       "revoked":false,
       "deleted":false,
       "shared":false,
       "size":708608,
       "customMetadata":{
          "SourceRepoType":"DROPBOX",
          "SourceFilePathDisplay":"/beacon/Get Started with Dropbox.pdf",
          "SourceFilePathId":"id:GiWQfAKOyAAAAAAAAAAAAg",
          "SourceRepoName":"DB",
          "SourceRepoId":"a081a600-2a8a-4181-99d6-b3b7d57f3707"
       }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestMyVaultFileImportExport)
{
    RetValue result = RETSUCCEED();

    RMMyVaultFile myVaultFile1, myVaultFile2, myVaultFile3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = myVaultFile1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = myVaultFile1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = myVaultFile1.ImportFromRMSResponse(myVaultFileRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!myVaultFile1.GetPathId().empty());
    BOOST_CHECK(!myVaultFile1.GetPathDisplay().empty());
    BOOST_CHECK(!myVaultFile1.GetRepoId().empty());
    BOOST_CHECK(!myVaultFile1.GetDuid().empty());
    BOOST_CHECK(!myVaultFile1.GetFileName().empty());
    BOOST_CHECK_GT(myVaultFile1.GetSharedTime(), 0);
    BOOST_CHECK(!myVaultFile1.GetSharedWith().empty());
    BOOST_CHECK_GT(myVaultFile1.GetFileSize(), 0);

    // Test ExportToString()
    s1 = myVaultFile1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = myVaultFile2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!myVaultFile2.GetPathId().empty());
    BOOST_CHECK(!myVaultFile2.GetPathDisplay().empty());
    BOOST_CHECK(!myVaultFile2.GetRepoId().empty());
    BOOST_CHECK(!myVaultFile2.GetDuid().empty());
    BOOST_CHECK(!myVaultFile2.GetFileName().empty());
    BOOST_CHECK_GT(myVaultFile2.GetSharedTime(), 0);
    BOOST_CHECK(!myVaultFile2.GetSharedWith().empty());
    BOOST_CHECK_GT(myVaultFile2.GetFileSize(), 0);
    s2 = myVaultFile2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = myVaultFile1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = myVaultFile3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!myVaultFile3.GetPathId().empty());
    BOOST_CHECK(!myVaultFile3.GetPathDisplay().empty());
    BOOST_CHECK(!myVaultFile3.GetRepoId().empty());
    BOOST_CHECK(!myVaultFile3.GetDuid().empty());
    BOOST_CHECK(!myVaultFile3.GetFileName().empty());
    BOOST_CHECK_GT(myVaultFile3.GetSharedTime(), 0);
    BOOST_CHECK(!myVaultFile3.GetSharedWith().empty());
    BOOST_CHECK_GT(myVaultFile3.GetFileSize(), 0);
    s2 = myVaultFile3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}


const std::string myVaultRMSResponse = R"foo(
    {
       "statusCode":200,
       "message":"OK",
       "serverTime":1484526776414,
       "results":{
          "detail":{
             "totalFiles":2,
             "files":[
                {
                   "pathId":"/nxl_myvault_nxl/get started with dropbox-2017-01-13-14-30-28.pdf.nxl",
                   "pathDisplay":"/Get Started with Dropbox-2017-01-13-14-30-28.pdf.nxl",
                   "repoId":"2c5ee284-8156-483e-abc6-f030c9f037e1",
                   "sharedOn":1484289028934,
                   "sharedWith": [
                           "abraham.lincoln@qapf1.qalab01.nextlabs.com",
                           "prateek.kumar@nextlabs.com",
                           "john.tyler@qapf1.qalab01.nextlabs.com",
                           "barack.obama@qapf1.qalab01.nextlabs.com"
                   ],
                   "name":"Get Started with Dropbox-2017-01-13-14-30-28.pdf.nxl",
                   "duid":"F8DC0C3FB945B3671CE994DD7D82761A",
                   "revoked":false,
                   "deleted":false,
                   "shared":false,
                   "size":708608,
                   "customMetadata":{
                      "SourceRepoType":"DROPBOX",
                      "SourceFilePathDisplay":"/beacon/Get Started with Dropbox.pdf",
                      "SourceFilePathId":"id:GiWQfAKOyAAAAAAAAAAAAg",
                      "SourceRepoName":"DB",
                      "SourceRepoId":"a081a600-2a8a-4181-99d6-b3b7d57f3707"
                   }
                },
                {
                   "pathId":"/nxl_myvault_nxl/helloworld-2017-01-13-14-29-43.pptx.nxl",
                   "pathDisplay":"/HelloWorld-2017-01-13-14-29-43.pptx.nxl",
                   "repoId":"2c5ee284-8156-483e-abc6-f030c9f037e1",
                   "sharedOn":1484288983946,
                   "name":"HelloWorld-2017-01-13-14-29-43.pptx.nxl",
                   "duid":"467BC12F710CB27D028BE0FD9989903F",
                   "revoked":false,
                   "deleted":false,
                   "shared":true,
                   "size":55808,
                   "customMetadata":{
                      "SourceRepoType":"DROPBOX",
                      "SourceFilePathDisplay":"/HelloWorld.pptx",
                      "SourceFilePathId":"id:GiWQfAKOyAAAAAAAAAAALg",
                      "SourceRepoName":"DB",
                      "SourceRepoId":"a081a600-2a8a-4181-99d6-b3b7d57f3707"
                   }
                }
             ]
          }
       }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestMyVaultImportExport)
{
    RetValue result = RETSUCCEED();

    RMMyVault myVault1, myVault2, myVault3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = myVault1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = myVault1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = myVault1.ImportFromRMSResponse(myVaultRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myVault1.GetMyVaultFiles()->size(), 0U);
    BOOST_CHECK_GT(myVault1.GetMyVaultFileInfos()->size(), 0U);

    // Test ExportToString()
    s1 = myVault1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = myVault2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myVault2.GetMyVaultFiles()->size(), 0U);
    BOOST_CHECK_GT(myVault2.GetMyVaultFileInfos()->size(), 0U);
    s2 = myVault2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = myVault1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = myVault3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myVault3.GetMyVaultFiles()->size(), 0U);
    BOOST_CHECK_GT(myVault3.GetMyVaultFileInfos()->size(), 0U);
    s2 = myVault3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}


BOOST_AUTO_TEST_SUITE_END()
