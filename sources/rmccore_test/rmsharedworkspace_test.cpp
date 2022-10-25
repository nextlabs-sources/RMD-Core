#include "stdafx.h"

#include "rmccore/restful/rmsharedworkspace.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMSharedWorkspaceTest)



const std::string sharedWorkspaceListFilesResponse = R"foo(
    {
        "statusCode": 200,
        "message": "OK",
        "serverTime": 1598592318208,
        "results": {
            "detail": [
                {
                    "fileId": "015YTHE6SD2MVAY75ZKRGKLR3UWURSPDGV",
                    "name": "Test02",
                    "path": "/Test02",
                    "pathId": "015YTHE6SD2MVAY75ZKRGKLR3UWURSPDGV",
                    "isFolder": true,
                    "lastModifiedTime": 1593679773000,
                    "protectedFile": false
                },
                {
                    "fileId": "015YTHE6S2WNP7H4EPS5HZJ4LF7VY27VCO",
                    "name": "Document.docx",
                    "path": "/Document.docx",
                    "pathId": "015YTHE6S2WNP7H4EPS5HZJ4LF7VY27VCO",
                    "isFolder": false,
                    "lastModifiedTime": 1598505930000,
                    "fileSize": 17258,
                    "protectedFile": false,
                    "fileType": "docx"
                },
                {
                    "fileId": "015YTHE6TTK22INOBVGFB2I2HQVCBMOVMV",
                    "name": "JUnitTest_DeleteItem.jpg",
                    "path": "/JUnitTest_DeleteItem.jpg",
                    "pathId": "015YTHE6TTK22INOBVGFB2I2HQVCBMOVMV",
                    "isFolder": false,
                    "lastModifiedTime": 1598357578000,
                    "fileSize": 106779,
                    "protectedFile": false,
                    "fileType": "jpg"
                }
            ]
        }
    }
)foo";

const std::string sharedWorkspaceFileMetaDataResponse = R"foo(
    {
        "statusCode": 200,
        "message": "OK",
        "serverTime": 1563271323985,
        "results": {
            "fileInfo": {
                "path": "/Untitled-2019-07-16-14-55-36.png.nxl",
                "pathId": "/untitled-2019-07-16-14-55-36.png.nxl",
                "name": "Untitled-2019-07-16-14-55-36.png.nxl",
                "fileType": "png",
                "lastModified": 1563260137120,
                "size": 256512,
                "rights": [ ],
                "protectedFile": true,
                "tags": { "Confidentiality": [ "SECRET", "TOP SECRET" ] },
                "protectionType": 1,
                "expiry": { },
                "creationTime": 1563260137120,
                "uploadedBy": {
                    "userId": 3,
                    "displayName": "nextlabs.test.pk",
                    "email": "nextlabs.test.pk@gmail.com"
                },
                "lastModifiedUser": {
                    "userId": 3,
                    "displayName": "nextlabs.test.pk",
                    "email": "nextlabs.test.pk@gmail.com"
                }
            }
        }
    }
)foo";

const std::string sharedWorkspaceCheckFileExistResponse = R"foo(
    {
        "statusCode": 200,
        "message": "OK",
        "serverTime": 1587960682496,
        "results": { "fileExists": true }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestSharedWorkspaceAnalyse)
{
    RetValue result = RETSUCCEED();

    RMSharedWorkspace sharedWorkspace1, sharedWorkspace2, sharedWorkspace3;

    // Test AnalyseSharedWorkspaceListFilesResponse()
    std::vector<SHAREDWORKSPACE_FILE_INFO> fileList;
    result = sharedWorkspace1.AnalyseSharedWorkspaceListFilesResponse(sharedWorkspaceListFilesResponse, fileList);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(fileList.size(), 0U);
    for (const auto &file : fileList) {
        BOOST_CHECK(!file.fileId.empty());
        BOOST_CHECK(!file.path.empty());
        BOOST_CHECK(!file.pathId.empty());
        BOOST_CHECK(!file.fileName.empty());
        BOOST_CHECK(!file.fileType.empty() || file.isFolder);
        BOOST_CHECK_GT(file.lastModified, 0U);
    }

    // Test AnalyseSharedWorkspaceFileMetaDataResponse()
    SHAREDWORKSPACE_FILE_METADATA fileMetaData;
    result = sharedWorkspace2.AnalyseSharedWorkspaceFileMetaDataResponse(sharedWorkspaceFileMetaDataResponse, fileMetaData);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK(!fileMetaData.path.empty());
    BOOST_CHECK(!fileMetaData.pathId.empty());
    BOOST_CHECK(!fileMetaData.fileName.empty());
    BOOST_CHECK(!fileMetaData.fileType.empty());
    BOOST_CHECK(fileMetaData.isProtectedFile);
    BOOST_CHECK_GT(fileMetaData.lastModified, 0U);
    BOOST_CHECK_GT(fileMetaData.creationTime, 0U);
    BOOST_CHECK_GT(fileMetaData.fileSize, 0U);
    BOOST_CHECK(!fileMetaData.isFolder);
    BOOST_CHECK_GT(fileMetaData.createByUser.userId, 0U);
    BOOST_CHECK(!fileMetaData.createByUser.displayName.empty());
    BOOST_CHECK(!fileMetaData.createByUser.email.empty());
    BOOST_CHECK_GT(fileMetaData.lastModifiedByUser.userId, 0U);
    BOOST_CHECK(!fileMetaData.lastModifiedByUser.displayName.empty());
    BOOST_CHECK(!fileMetaData.lastModifiedByUser.email.empty());
    BOOST_CHECK((fileMetaData.protectionType == 0 && fileMetaData.fileRights.size() > 0) ||
                (fileMetaData.protectionType == 1 && !fileMetaData.fileTags.empty()));

    // Test AnalyseSharedWorkspaceCheckFileExistResponse()
    bool isFileExist;
    result = sharedWorkspace3.AnalyseSharedWorkspaceCheckFileExistResponse(sharedWorkspaceCheckFileExistResponse, isFileExist);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
}



BOOST_AUTO_TEST_SUITE_END()
