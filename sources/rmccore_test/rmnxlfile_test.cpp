#include "stdafx.h"

#include "rmccore/restful/rmnxlfile.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "common.h"

using namespace RMCCORE;



const ExeDir<char> kExeDir;

BOOST_AUTO_TEST_SUITE(RmcCoreRMNXLFileTest)

const std::string metaDataRequestJsonString = R"foo(
{
	"statusCode": 200,
	"message": "OK",
	"serverTime": 1585301701477,
	"results": {
		"detail": {
			"name": "123-2020-03-24-02-23-23.txt.nxl",
			"recipients": [
				"john.tyler@qapf1.qalab01.nextlabs.com",
				"user2002@qapf1.qalab01.nextlabs.com"
			],
			"fileLink": "https://rms-centos7309.qapf1.qalab01.nextlabs.com:8444/rms/main#/personal/viewSharedFile?d=cbb22512-2c70-4e7c-900b-ff391cf3d43c&c=42EAE9FC94D9F79D70BC151F9713F94B8360A5D20E78C691781E7F2C098D4D84",
			"protectedOn": 1585016603703,
			"sharedOn": 1585018356047,
			"rights": [
				"WATERMARK",
				"SHARE",
				"DOWNLOAD",
				"VIEW",
				"EDIT",
				"PRINT"
			],
			"shared": true,
			"deleted": false,
			"revoked": false,
			"validity": {
				"endDate": 1587657599999
			},
			"protectionType": 0
		}
	}
}
)foo";


BOOST_AUTO_TEST_CASE(TestRMNXLFileImportMetadataQueryFromRMSResponse)
{
    RetValue result = RETSUCCEED();

    RMNXLFile nxlFile("");

    //Test ImportMetadataQueryFromRMSResponse
    result = nxlFile.ImportMetadataQueryFromRMSResponse(metaDataRequestJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);

    std::string strFileLink = "https://rms-centos7309.qapf1.qalab01.nextlabs.com:8444/rms/main#/personal/viewSharedFile?d=cbb22512-2c70-4e7c-900b-ff391cf3d43c&c=42EAE9FC94D9F79D70BC151F9713F94B8360A5D20E78C691781E7F2C098D4D84";
    BOOST_CHECK_EQUAL(nxlFile.Meta_GetFileLink(), strFileLink);

    uint64_t u64SharedOn = 1585018356047;
    BOOST_CHECK_EQUAL(nxlFile.Meta_GetSharedOn(), u64SharedOn);
}

const std::string metaDataProjectRequestJsonString = R"foo(
{
	"statusCode": 200,
	"message": "OK",
	"serverTime": 1585304402398,
	"results": {
		"fileInfo": {
			"pathDisplay": "/100-2020-03-27-03-23-20.txt.nxl",
			"pathId": "/100-2020-03-27-03-23-20.txt.nxl",
			"name": "100-2020-03-27-03-23-20.txt.nxl",
			"fileType": "txt",
			"lastModified": 1585279401028,
			"size": 17408,
			"rights": [
				"WATERMARK",
				"SHARE",
				"VIEW",
				"DOWNLOAD",
				"EDIT",
				"PRINT"
			],
			"owner": false,
			"nxl": true,
			"tags": {},
			"protectionType": 0,
			"expiry": {},
			"creationTime": 1585279401028,
			"createdBy": {
				"userId": 5,
				"displayName": "jimmy carter",
				"email": "jimmy.carter@qapf1.qalab01.nextlabs.com"
			},
			"lastModifiedUser": {
				"userId": 5,
				"displayName": "jimmy carter",
				"email": "jimmy.carter@qapf1.qalab01.nextlabs.com"
			}
		}
	}
}
)foo";

BOOST_AUTO_TEST_CASE(TestRMNXLFileImportProjectFileMetadataQueryFromRMSResponse)
{
    RetValue result = RETSUCCEED();

    RMNXLFile nxlFile("");

    result = nxlFile.ImportProjectFileMetadataQueryFromRMSResponse(metaDataProjectRequestJsonString);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);

    std::string strJson = nxlFile.ExportToString();
    BOOST_CHECK(!strJson.empty());

    std::string strName = "100-2020-03-27-03-23-20.txt.nxl";
    BOOST_CHECK_EQUAL(nxlFile.Meta_GetName(), strName);

    uint64_t u64LastModified = 1585279401028;
    BOOST_CHECK_EQUAL(nxlFile.Meta_GetLastModify(), u64LastModified);
}

BOOST_AUTO_TEST_CASE(TestRMNXLFileAccessReadOnlyFile)
{
    const std::string nxlFilePath(kExeDir.getExeDir() + "\\RMSTokenTest.txt.nxl");

    // Set test file to read-only.
    boost::filesystem::permissions(nxlFilePath,
                                   boost::filesystem::remove_perms |
                                   boost::filesystem::owner_write |
                                   boost::filesystem::group_write |
                                   boost::filesystem::others_write );

    RMNXLFile nxlFile(nxlFilePath);
    BOOST_REQUIRE(nxlFile.IsOpened());
    BOOST_CHECK(!nxlFile.GetOwnerID().empty());
    BOOST_CHECK(!nxlFile.GetAgreement().empty());
    BOOST_CHECK(!nxlFile.GetAgreement1().empty());
    BOOST_CHECK(!nxlFile.GetDuid().empty());
    BOOST_CHECK(!nxlFile.GetTags().empty());
    BOOST_CHECK(!nxlFile.GetPolicy().empty());
    BOOST_CHECK(!nxlFile.GetTenant().empty());
    BOOST_CHECK(!nxlFile.GetOrgFileName().empty());
    BOOST_CHECK(!nxlFile.GetOrgFileExtension().empty());

#ifdef _WIN32
    const std::wstring nxlFilePathW(toUtf16(nxlFilePath));
    RMNXLFile nxlFileW(nxlFilePathW);
    BOOST_REQUIRE(nxlFileW.IsOpened());
    BOOST_CHECK(!nxlFileW.GetOwnerID().empty());
    BOOST_CHECK(!nxlFileW.GetAgreement().empty());
    BOOST_CHECK(!nxlFileW.GetAgreement1().empty());
    BOOST_CHECK(!nxlFileW.GetDuid().empty());
    BOOST_CHECK(!nxlFileW.GetTags().empty());
    BOOST_CHECK(!nxlFileW.GetPolicy().empty());
    BOOST_CHECK(!nxlFileW.GetTenant().empty());
    BOOST_CHECK(!nxlFileW.GetOrgFileName().empty());
    BOOST_CHECK(!nxlFileW.GetOrgFileExtension().empty());
#endif

    // Restore test file to read-write.
    boost::filesystem::permissions(nxlFilePath,
                                   boost::filesystem::add_perms |
                                   boost::filesystem::owner_write |
                                   boost::filesystem::group_write |
                                   boost::filesystem::others_write );
}

BOOST_AUTO_TEST_SUITE_END()
