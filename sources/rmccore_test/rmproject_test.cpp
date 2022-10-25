#include "stdafx.h"

#include "rmccore/restful/rmproject.h"
#include <boost/test/unit_test.hpp>

using namespace RMCCORE;

BOOST_AUTO_TEST_SUITE(RmcCoreRMProjectTest)



const std::string projectMemberRMSResponse = R"foo(
    {
      "userId": 57,
      "displayName": "althaf nextlabs",
      "email": "rmsuser720@gmail.com",
      "creationTime": 1496886387439
    }
)foo";

BOOST_AUTO_TEST_CASE(TestProjectMemberImportExport)
{
    RetValue result = RETSUCCEED();

    RMProjectMember projectMember1, projectMember2, projectMember3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = projectMember1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = projectMember1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = projectMember1.ImportFromRMSResponse(projectMemberRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(projectMember1.GetUserID(), 0U);
    BOOST_CHECK(!projectMember1.GetUserName().empty());
    BOOST_CHECK(!projectMember1.GetUserEmail().empty());
    BOOST_CHECK_GT(projectMember1.GetJoinTime(), 0);

    // Test ExportToString()
    s1 = projectMember1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = projectMember2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(projectMember2.GetUserID(), 0U);
    BOOST_CHECK(!projectMember2.GetUserName().empty());
    BOOST_CHECK(!projectMember2.GetUserEmail().empty());
    BOOST_CHECK_GT(projectMember2.GetJoinTime(), 0);
    s2 = projectMember2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = projectMember1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = projectMember3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(projectMember3.GetUserID(), 0U);
    BOOST_CHECK(!projectMember3.GetUserName().empty());
    BOOST_CHECK(!projectMember3.GetUserEmail().empty());
    BOOST_CHECK_GT(projectMember3.GetJoinTime(), 0);
    s2 = projectMember3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}



const std::string projectRMSResponse = R"foo(
    {
        "id": 13,
        "parentTenantId": "99406fce-dcde-4ab2-be66-17e2d1406bd1",
        "parentTenantName": "t-1578639918fa481080701faf9301a58e",
        "tokenGroupName": "Token Group One",
        "name": "Project Nova",
        "description": "Collaborate in project nova",
        "displayName": "Project Nova",
        "creationTime": 1483605615817,
        "totalMembers": 1,
        "totalFiles": 0,
        "ownedByMe": true,
        "owner": {
          "userId": 2,
          "name": "admin@nextlabs.com",
          "email": "admin@nextlabs.com"
        },
        "accountType": "PROJECT_TRIAL",
        "trialEndTime": 148706243714,
        "projectMembers": {
          "totalMembers": 6,
          "members": [
            {
              "userId": 57,
              "displayName": "althaf nextlabs",
              "email": "rmsuser720@gmail.com",
              "creationTime": 1496886387439
            },
            {
              "userId": 4,
              "displayName": "althaf.ashraf@nextlabs.com",
              "email": "althaf.ashraf@nextlabs.com",
              "creationTime": 1496885208735
            },
            {
              "userId": 56,
              "displayName": "althaf.rms1@gmail.com",
              "email": "althaf.rms1@gmail.com",
              "creationTime": 1496886126044
            },
            {
              "userId": 7,
              "displayName": "mohammed althaf",
              "email": "althaf1088@gmail.com",
              "creationTime": 1496886413244
            },
            {
              "userId": 55,
              "displayName": "rmsuser0@gmail.com",
              "email": "rmsuser0@gmail.com",
              "creationTime": 1496886176971
            }
          ]
        }
    }
)foo";


BOOST_AUTO_TEST_CASE(TestProjectImportExport)
{
    RetValue result = RETSUCCEED();

    RMProject project1, project2, project3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = project1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = project1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = project1.ImportFromRMSResponse(projectRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(project1.GetProjectID(), 0U);
    BOOST_CHECK(!project1.GetProjectName().empty());
    BOOST_CHECK(!project1.GetDisplayName().empty());
    BOOST_CHECK(!project1.GetDescription().empty());
    BOOST_CHECK(!project1.GetTokenGroupName().empty());
    BOOST_CHECK(!project1.GetParentTenantId().empty());
    BOOST_CHECK(!project1.GetParentTenantName().empty());
    BOOST_CHECK(project1.IsOwnbyMe());
    BOOST_CHECK_GT(project1.GetOwnerID(), 0U);
    BOOST_CHECK(!project1.GetOwnerName().empty());
    BOOST_CHECK(!project1.GetOwnerEmail().empty());

    // Test ExportToString()
    s1 = project1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = project2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(project2.GetProjectID(), 0U);
    BOOST_CHECK(!project2.GetProjectName().empty());
    BOOST_CHECK(!project2.GetDisplayName().empty());
    BOOST_CHECK(!project2.GetDescription().empty());
    BOOST_CHECK(!project2.GetTokenGroupName().empty());
    BOOST_CHECK(!project2.GetParentTenantId().empty());
    BOOST_CHECK(!project2.GetParentTenantName().empty());
    BOOST_CHECK(project2.IsOwnbyMe());
    BOOST_CHECK_GT(project2.GetOwnerID(), 0U);
    BOOST_CHECK(!project2.GetOwnerName().empty());
    BOOST_CHECK(!project2.GetOwnerEmail().empty());
    s2 = project2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = project1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = project3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(project3.GetProjectID(), 0U);
    BOOST_CHECK(!project3.GetProjectName().empty());
    BOOST_CHECK(!project3.GetDisplayName().empty());
    BOOST_CHECK(!project3.GetDescription().empty());
    BOOST_CHECK(!project3.GetTokenGroupName().empty());
    BOOST_CHECK(!project3.GetParentTenantId().empty());
    BOOST_CHECK(!project3.GetParentTenantName().empty());
    BOOST_CHECK(project3.IsOwnbyMe());
    BOOST_CHECK_GT(project3.GetOwnerID(), 0U);
    BOOST_CHECK(!project3.GetOwnerName().empty());
    BOOST_CHECK(!project3.GetOwnerEmail().empty());
    s2 = project3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}

const std::string myProjectsRMSResponse = R"foo(
    {
      "statusCode": 200,
      "message": "OK",
      "serverTime": 1484537953900,
      "results": {
        "totalProjects":2,
        "detail": [
             {
                "id": 13,
                "parentTenantId": "99406fce-dcde-4ab2-be66-17e2d1406bd1",
                "parentTenantName": "t-1578639918fa481080701faf9301a58e",
                "name": "Project Nova",
                "tokenGroupName": "Token Group One",
                "description": "Collaborate in project nova",
                "displayName": "Project Nova",
                "creationTime": 1483605615817,
                "totalMembers": 1,
                "totalFiles": 0,
                "ownedByMe": true,
                "owner": {
                  "userId": 2,
                  "name": "admin@nextlabs.com",
                  "email": "admin@nextlabs.com"
                },
                "accountType": "PROJECT_TRIAL",
                "trialEndTime": 148706243714,
                "projectMembers": {
                  "totalMembers": 6,
                  "members": [
                    {
                      "userId": 57,
                      "displayName": "althaf nextlabs",
                      "email": "rmsuser720@gmail.com",
                      "creationTime": 1496886387439
                    },
                    {
                      "userId": 4,
                      "displayName": "althaf.ashraf@nextlabs.com",
                      "email": "althaf.ashraf@nextlabs.com",
                      "creationTime": 1496885208735
                    },
                    {
                      "userId": 56,
                      "displayName": "althaf.rms1@gmail.com",
                      "email": "althaf.rms1@gmail.com",
                      "creationTime": 1496886126044
                    },
                    {
                      "userId": 7,
                      "displayName": "mohammed althaf",
                      "email": "althaf1088@gmail.com",
                      "creationTime": 1496886413244
                    },
                    {
                      "userId": 55,
                      "displayName": "rmsuser0@gmail.com",
                      "email": "rmsuser0@gmail.com",
                      "creationTime": 1496886176971
                    }
                  ]
                }
              },
              {
                "id": 14,
                "parentTenantId": "33138865-ed68-4645-8365-57386a63c719",
                "parentTenantName": "t-8f3ecc2e206c46a7839b7cf39c523b37",
                "tokenGroupName": "Token Group Two",
                "name": "Project SkyDRM",
                "description": "Share your documents securely",
                "displayName": "Project SkyDRM",
                "creationTime": 1483606243712,
                "totalMembers": 1,
                "totalFiles": 0,
                "ownedByMe": true,
                "owner": {
                  "userId": 2,
                  "name": "admin@nextlabs.com",
                  "email": "admin@nextlabs.com"
                },
                "accountType": "PROJECT_TRIAL",
                "trialEndTime": 148706243714,
                "projectMembers": {
                  "totalMembers": 6,
                  "members": [
                    {
                      "userId": 57,
                      "displayName": "althaf nextlabs",
                      "email": "rmsuser720@gmail.com",
                      "creationTime": 1496886387439
                    },
                    {
                      "userId": 4,
                      "displayName": "althaf.ashraf@nextlabs.com",
                      "email": "althaf.ashraf@nextlabs.com",
                      "creationTime": 1496885208735
                    },
                    {
                      "userId": 55,
                      "displayName": "rmsuser0@gmail.com",
                      "email": "rmsuser0@gmail.com",
                      "creationTime": 1496886176971
                    }
                  ]
                }
              }
            ]
      }
    }
)foo";

BOOST_AUTO_TEST_CASE(TestRMCertificateImportExport)
{
    RetValue result = RETSUCCEED();

    RMMyProjects myProjects1, myProjects2, myProjects3;
    std::string s1, s2;

    // Test exporting empty object
    s1 = myProjects1.ExportToString();
    BOOST_CHECK(!s1.empty());
    nlohmann::json root = myProjects1.ExportToJson();
    BOOST_CHECK(!root.empty());

    // Test ImportFromRMSResponse()
    result = myProjects1.ImportFromRMSResponse(myProjectsRMSResponse);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myProjects1.GetProjectNumber(), 0U);

    // Test ExportToString()
    s1 = myProjects1.ExportToString();
    BOOST_CHECK(!s1.empty());

    // Test ImportFromString()
    result = myProjects2.ImportFromString(s1);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myProjects2.GetProjectNumber(), 0U);
    s2 = myProjects2.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);

    // Test ExportToJson()
    nlohmann::json root2 = myProjects1.ExportToJson();
    BOOST_CHECK(!root2.empty());

    // Test ImportFromJson()
    result = myProjects3.ImportFromJson(root2);
    BOOST_CHECK_EQUAL(result.GetCode(), RMCCORE_ERROR_SUCCESS);
    BOOST_CHECK_GT(myProjects3.GetProjectNumber(), 0U);
    s2 = myProjects3.ExportToString();
    BOOST_CHECK_EQUAL(s1, s2);
}


BOOST_AUTO_TEST_SUITE_END()
