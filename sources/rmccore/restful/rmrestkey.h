//
//  rmrestkey.h
//  rmccore
//
//  Created by Haobo Wang on 11/29/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_RESTFUL_RMRESTKEY_H__
#define __RMCCORE_RESTFUL_RMRESTKEY_H__

#define STATUS_ELEMENT_NAME         "statusCode"
#define MESSAGE_ELEMENT_NAME        "message"
#define SERVERTIME_ELEMENT_NAME     "serverTime"
#define RESULT_KEY_NAME             "results"

#define UPLOAD_PARAMETERS_KEY_NAME  "parameters"
#define GENERIC_ELEMENT_NAME        "name"

//For Tenant API
#define TENANT_SERVER_ELEMENT_NAME  "server"

//For Login API
#define LOGIN_EXTRA_KEY_NAME        "extra"
#define LOGIN_IDPTYPE_ELEMENT_NAME  "idpType"
#define LOGIN_USERID_ELEMENT_NAME   "userId"
#define LOGIN_TICKET_ELEMENT_NAME   "ticket"
#define LOGIN_TENANTID_ELEMENT_NAME "tenantId"
#define LOGIN_TTL_ELEMENT_NAME      "ttl"
#define LOGIN_USERNAME_ELEMENT_NAME "name"
#define LOGIN_EMAIL_ELEMENT_NAME    "email"
#define LOGIN_MEMBERSHIP_KEY_NAME   "memberships"
#define LOGIN_USER_ATTR_KEY_NAME	"attributes"

//For User API
#define USER_EXTRA_KEY_NAME			LOGIN_EXTRA_KEY_NAME
#define USER_DISPLAYNAME_ELEMENT_NAME	"displayName"
#define USER_EMAIL_ELEMENT_NAME		"email"
#define USER_EMAILS_ELEMENT_NAME	"emails"
#define USER_ID_ELEMENT_NAME		LOGIN_USERID_ELEMENT_NAME
#define USER_TTL_ELEMENT_NAME		LOGIN_TTL_ELEMENT_NAME
#define USER_NAME_ELEMENT_NAME		LOGIN_USERNAME_ELEMENT_NAME
#define USER_PREFERENCE_KEY_NAME	"preferences"
#define USER_PREF_TENANT_ELEMENT_NAME	"defaultTenant"
#define USER_PREF_MEMBER_ELEMENT_NAME	"defaultMember"
#define USER_PREF_SECMODE_ELEMENT_NAME	"securityMode"
#define USER_MEMBERSHIP_KEY_NAME	LOGIN_MEMBERSHIP_KEY_NAME

//for Membership API
#define MEMBERSHIP_ID_ELEMENT_NAME          "id"
#define MEMBERSHIP_TYPE_ELEMENT_NAME        "type"
#define MEMBERSHIP_TENANTID_ELEMENT_NAME    "tenantId"
#define MEMBERSHIP_TOKENGROUPNAME_ELEMENT_NAME  "tokenGroupName"
#define MEMBERSHIP_PROJID_ELEMENT_NAME      "projectId"
#define MEMBERSHIP_RESULT_KEY_NAME          "results"
#define MEMBERSHIP_CERTIFICATE_ELEMENT_NAME "certficates"

//For Token API
#define TOKEN_ARRAY_KEY_NAME                "tokens"
#define TOKEN_MAINTAINLEVEL_ELEMENT_NAME    "ml"
#define TOKEN_ELEMENT_NAME					"token"

//For Upgrade API
#define UPGRADE_PLATFORMID_ELEMENT_NAME     "platformId"
#define UPGRADE_PROCESSOR_ELEMENT_NAME      "processorArch"
#define UPGRADE_CURVERSION_ELEMENT_NAME     "currentVersion"
#define UPGRADE_NEWVERSION_ELEMENT_NAME     "newVersion"
#define UPGRADE_DOWNLOADURL_ELEMENT_NAME    "downloadURL"
#define UPGRADE_CHECKSUM_ELEMENT_NAME       "sha1Checksum"

//For Log API
#define LOG_EMAIL_ELEMENT_NAME              "email"
#define LOG_OPERATION_ELEMENT_NAME          "operation"
#define LOG_DEVICETYPE_ELEMENT_NAME         "deviceType"
#define LOG_DEVICEID_ELEMENT_NAME           "deviceId"
#define LOG_ACCESSTIME_ELEMENT_NAME         "accessTime"
#define LOG_ACCESSRESULT_ELEMENT_NAME       "accessResult"

#define LOG_DATA_KEY_NAME                   "data"
#define LOG_FILENAME_ELEMENT_NAME           "name"
#define LOG_FILEDUID_ELEMENT_NAME           "duid"
#define LOG_RECORDS_KEY_NAME                "logRecords"
#define LOG_RECORDNUM_ELEMENT_NAME          "totalCount"

//For MyVault API
#define MYVAULT_NXL_FILENAME_ELEMENT_NAME       "name"
#define MYVAULT_NXL_LASTMODIFIED_ELEMENT_NAME   "lastModified"
#define MYVAULT_NXL_FILESIZE_ELEMENT_NAME       "size"
#define MYVAULT_NXL_DUID_ELEMENT_NAME           "duid"
#define MYVAULT_NXL_PATHID_ELEMENT_NAME         "pathId"
#define MYVAULT_NXL_DISPLAYPATH_ELEMENT_NAME    "pathDisplay"
#define MYVALUT_NXL_DETAIL_KEY_NAME				"detail"
#define MYVALUT_NXL_RECIPIENTS_KEY_NAME			"recipients"
#define MYVALUT_NXL_FILELINK_ELEMENT_NAME		"fileLink"
#define MYVALUT_NXL_PROTECTTIME_ELEMENT_NAME	"protectedOn"
#define MYVALUT_NXL_SHAREDTIME_ELEMENT_NAME		"sharedOn"
#define MYVALUT_NXL_RIGHTS_ELEMENT_NAME			"righst"
#define MYVALUT_NXL_SHAREDFLAG_ELEMENT_NAME		"shared"
#define MYVALUT_NXL_DELETEDFLAG_ELEMENT_NAME	"deleted"
#define MYVALUT_NXL_REVOKEDFLAG_ELEMENT_NAME	"revoked"
#define MYVALUT_NXL_VALIDITY_KEY_NAME			"validity"
#define MYVALUT_NXL_VSTARTDATE_ELEMENT_NAME		"startDate"
#define MYVALUT_NXL_VENDDATE_ELEMENT_NAME		"endDate"
#define MYVALUT_NXL_PROTECTION_TYPE				"protectionType"

#define MYVALUT_UPLOAD_SRCPATH_ELEMENT_NAME     "srcPathDisplay"
#define MYVALUT_UPLOAD_SRCREPONAME_ELEMENT_NAME "srcRepoName"
#define MYVALUT_UPLOAD_SRCREPOTYPE_ELEMENT_NAME "srcRepoType"

// For Copy nxl file API
#define COPY_NXL_FILE_PARAMETERS_KEY_NAME       "parameters"
#define COPY_NXL_FILE_SRC_KEY_NAME              "src"
#define COPY_NXL_FILE_DEST_KEY_NAME             "dest"
#define COPY_NXL_FILE_FILENAME_KEY_NAME         "fileName"
#define COPY_NXL_FILE_PARENTPATHID_KEY_NAME     "parentPathId"
#define COPY_NXL_FILE_FILEPATHID_KEY_NAME       "filePathId"
#define COPY_NXL_FILE_SPACE_TYPE_KEY_NAME       "spaceType"
#define COPY_NXL_FILE_SPACE_ID_KEY_NAME         "spaceId"
#define COPY_NXL_FILE_OVERWRITE_KEY_NAME        "overwrite"
#define COPY_NXL_FILE_TRANSACTIONCODE_KEY_NAME  "transactionCode"
#define COPY_NXL_FILE_TRANSACTIONID_KEY_NAME    "transactionId"

//For Share API
#define SHARE_EMAIL_ELEMENT_NAME                "email"
#define SHARE_ADDRECIPIENTS_KEY_NAME            "newRecipients"
#define SHARE_REMOVERECIPIENTS_KEY_NAME         "removedRecipients"
#define SHARE_COMMENT_KEY_NAME                  "comment"

#define SHARE_DUID_ELEMENT_NAME                 "duid"
#define SHARE_NEWSHARE_KEY_NAME                 "newSharedList"
#define SHARE_SHARED_KEY_NAME                   "alreadySharedList"
#define SHARE_FILENAME_ELEMENT_NAME             "fileName"

#define SHARE_ATTACH_ELEMENT_NAME               "asAttachment"
#define	SHARE_SHAREDOC_KEY_NAME             	"sharedDocument"
#define SHARE_MEMBERSHIP_ELEMENT_NAME           "membershipId"
#define SHARE_PERMISSION_ELEMENT_NAME      	    "permissions"
#define SHARE_METADATA_ELEMANET_NAME       	    "metadata"
#define SHARE_EXPIRETIME_ELEMENT_NAME      	    "expireTime"
#define SHARE_EXPIRY_KEY_NAME					"expiry"
#define SHARE_FILEEXT_ELEMENT_NAME             	"fileExtension"
#define SHARE_PATHID_ELEMENT_NAME          	    "filePathId"
#define SHARE_FILEPATH_ELEMENT_NAME         	"filePath"
#define SHARE_TAGS_KEY_NAME             	    "tags"
#define SHARE_RECIPIENTS_KEY_NAME           	"recipients"
#define SHARE_COMMENT_ELEMENT_NAME				"comment"

//For Heartbeat API
#define HEARTBEAT_FREQUENCY_ELEMENT_NAME            "heartbeatFrequency"
#define HEARTBEAT_POLICY_KEY_NAME                   "policyConfigData"
#define HEARTBEAT_POLICY_TOKENGROUPNAME_ELEMENT_NAME      "tokenGroupName"
#define HEARTBEAT_POLICY_MODIFY_ELEMENT_NAME        "configurationModifiedTimeStamp"
#define HEARTBEAT_CLASIFY_KEY_NAME                  "classificationCategories"
#define HEARTBEAT_CLASIFY_NAME                      "name"
#define HEARTBEAT_CLASIFY_MULISELECT                "multiSelect"
#define HEARTBEAT_CLASIFY_MANDATORY                 "mandatory"
#define HEARTBEAT_CLASIFY_LABELS                    "labels"
#define HEARTBEAT_CLASIFY_LABELS_NAME               "name"
#define HEARTBEAT_CLASIFY_LABELS_DEFAULT            "default"
#define HEARTBEAT_TOKENGROUP_RESOURCETYPE_MAPPING_KEY_NAME         "tokenGroupResourceTypeMapping"
#define HEARTBEAT_ICENET_URL_KEY_NAME         "icenetUrl"
#define HEARTBEAT_WATERMARK_CONFIG_KEY_NAME         "watermarkConfig"
#define HEARTBEAT_WATERMARK_SERIAL_ELEMENT_NAME     "serialNumber"
#define HEARTBEAT_WATERMARK_CONTENT_ELEMENT_NAME    "content"
#define HEARTBEAT_WATERMARK_TEXT_ELEMENT_NAME       "text"
#define HEARTBEAT_WATERMARK_FONTNAME_ELEMENT_NAME   "fontName"
#define HEARTBEAT_WATERMARK_FONTSIZE_ELEMENT_NAME   "fontSize"
#define HEARTBEAT_WATERMARK_FONTCOLOR_ELEMENT_NAME  "fontColor"
#define HEARTBEAT_WATERMARK_TRANSPARENTCY_ELEMENT_NAME  "transparentRatio"
#define HEARTBEAT_WATERMARK_ROTATION_ELEMENT_NAME   "rotation"
#define HEARTBEAT_WATERMARK_REPEAT_ELEMENT_NAME     "repeat"
#define HEARTBEAT_WATERMARK_DENSITY_ELEMENT_NAME    "density"
#define HEARTBEAT_CLIENTDATA_KEY_NAME				"clientData"

//For MyDrive API
#define MYDRIVE_USAGE_ELEMENT_NAME                  "usage"
#define MYDRIVE_QUOTA_ELEMENT_NAME                  "quota"
#define VAULT_QUOTA_ELEMENT_NAME					"vaultQuota"
#define VAULT_USAGE_ELEMENT_NAME					"myVaultUsage"


//For Project API
#define PROJECT_TOTALNUM_ELEMENT_NAME				"totalProjects"
#define PROJECT_DETAIL_KEY_NAME						"detail"
#define PROJECT_ID_ELEMENT_NAME						"id"
#define PROJECT_NAME_ELEMENT_NAME					"name"
#define PROJECT_DESCRIPTION_ELEMENT_NAME			"description"
#define PROJECT_DISPLAYNAME_ELEMENT_NAME			"displayName"
#define PROJECT_CREATETIME_ELEMENT_NAME				"creationTime"
#define PROJECT_PARENTTENANTID_ELEMENT_NAME				"parentTenantId"
#define PROJECT_PARENTTENANTNAME_ELEMENT_NAME				"parentTenantName"
#define PROJECT_TOKENGROUPNAME_ELEMENT_NAME				"tokenGroupName"
#define PROJECT_MEMBERNUM_ELEMENT_NAME				"totalMembers"
#define PROJECT_TOTALFILE_ELEMENT_NAME				"totalFiles"
#define PROJECT_OWNBYME_ELEMENT_NAME				"ownedByMe"
#define PROJECT_OWNER_KEY_NAME						"owner"
#define PROJECT_OWNER_ID_ELEMENT_NAME				"userId"
#define PROJECT_OWNER_NAME_ELEMENT_NAME				"name"
#define PROJECT_OWNER_EMAIL_ELEMENT_NAME			"email"
#define PROJECT_ACCOUNT_TYPE_ELEMENT_NAME			"accountType"
#define PROJECT_TRIALTTL_ELEMENT_NAME				"trialEndTime"
#define PROJECT_MEMBER_KEY_NAME						"projectMembers"
#define PROJECT_MEMBER_TOTALNUM_ELEMENT_NAME		"totalMembers"
#define PROJECT_MEMBER_ARRAY_KEY_NAME				"members"
#define PROJECT_MEMBER_USERID_ELEMENT_NAME			"userId"
#define PROJECT_MEMBER_USERNAME_ELEMENT_NAME		"displayName"
#define PROJECT_MEMBER_USEREMAIL_ELEMENT_NAME		"email"
#define PROJECT_MEMBER_CREATETIME_ELEMENT_NAME		"creationTime"

#define PROJECT_FOLDER_ELEMENT_NAME				    "folder"
#define PROJECT_LAST_UPDATED_TIME		            "folderLastUpdatedTime"
#define PROJECT_FILEs_ELEMENT_NAME				    "files"

//For Remote Viewer API
#define REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_USERNAME_ELEMENT_NAME   "userName"
#define REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_FILENAME_ELEMENT_NAME   "fileName"
#define REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_OFFSET_ELEMENT_NAME     "offset"
#define REMOTEVIEWER_UPLOADANDVIEWLOCALFILE_OPERATIONS_ELEMENT_NAME "operations"
#define REMOTEVIEWER_VIEWREPOFILE_REPOID_ELEMENT_NAME               "repoId"
#define REMOTEVIEWER_VIEWREPOFILE_PATHID_ELEMENT_NAME               "pathId"
#define REMOTEVIEWER_VIEWREPOFILE_PATHDISPLAY_ELEMENT_NAME          "pathDisplay"
#define REMOTEVIEWER_VIEWREPOFILE_OFFSET_ELEMENT_NAME               "offset"
#define REMOTEVIEWER_VIEWREPOFILE_REPONAME_ELEMENT_NAME             "repoName"
#define REMOTEVIEWER_VIEWREPOFILE_REPOTYPE_ELEMENT_NAME             "repoType"
#define REMOTEVIEWER_VIEWREPOFILE_EMAIL_ELEMENT_NAME                "email"
#define REMOTEVIEWER_VIEWREPOFILE_TENANTNAME_ELEMENT_NAME           "tenantName"
#define REMOTEVIEWER_VIEWREPOFILE_LASTMODIFIEDDATE_ELEMENT_NAME     "lastModifiedDate"
#define REMOTEVIEWER_VIEWREPOFILE_OPERATIONS_ELEMENT_NAME           "operations"
#define REMOTEVIEWER_VIEWPROJECTFILE_PROJECTID_ELEMENT_NAME         "projectId"
#define REMOTEVIEWER_VIEWPROJECTFILE_PATHID_ELEMENT_NAME            "pathId"
#define REMOTEVIEWER_VIEWPROJECTFILE_PATHDISPLAY_ELEMENT_NAME       "pathDisplay"
#define REMOTEVIEWER_VIEWPROJECTFILE_OFFSET_ELEMENT_NAME            "offset"
#define REMOTEVIEWER_VIEWPROJECTFILE_EMAIL_ELEMENT_NAME             "email"
#define REMOTEVIEWER_VIEWPROJECTFILE_TENANTNAME_ELEMENT_NAME        "tenantName"
#define REMOTEVIEWER_VIEWPROJECTFILE_LASTMODIFIEDDATE_ELEMENT_NAME  "lastModifiedDate"
#define REMOTEVIEWER_VIEWPROJECTFILE_OPERATIONS_ELEMENT_NAME        "operations"

// Classification Profile
#define CLASSIFICATION_PROFILE_MAX_CAT_NUM		    "maxCategoryNum"
#define CLASSIFICATION_PROFILE_MAX_LABEL_NUM		"maxLabelNum"
#define CLASSIFICATION_PROFILE_CATEGORIES		    "categories"

// File Info Section
#define NXLFMT_FILEINFO_FILENAME             		"fileName"
#define NXLFMT_FILEINFO_FILEEXTENSION             	"fileExtension"
#define NXLFMT_FILEINFO_MODIFIEDBY             		"modifiedBy"
#define NXLFMT_FILEINFO_DATEMODIFIED             	"dateModified"
#define NXLFMT_FILEINFO_CREATEDBY             		"createdBy"
#define NXLFMT_FILEINFO_DATECREATED             	"dateCreated"

#endif /* __RMCCORE_RESTFUL_RMRESTKEY_H__ */
