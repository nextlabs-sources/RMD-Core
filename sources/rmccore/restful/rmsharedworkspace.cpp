#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"
#include "rmccore/restful/rmsharedworkspace.h"
#include "rmrestkey.h"
#include <assert.h>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMSHAREDWORKSPACE_CPP

using namespace RMCCORE;

RMSharedWorkspace::RMSharedWorkspace() : m_userid(0), m_ticket("")
{

}

RMSharedWorkspace::~RMSharedWorkspace()
{

}

RetValue RMSharedWorkspace::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize RMSharedWorkspace!");
		return ret;
	}

	return ret;
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceFilesQuery(const std::string& repoId, uint32_t pageId, uint32_t pageSize, const std::string& searchString, const std::string& orderBy, const std::string& pathId)
{
	CELOG_ENTER;
	RMSharedWorkspaceGetFileListQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, pathId);
	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceFileMetadataQuery(const std::string& repoId, const std::string& pathId)
{
	CELOG_ENTER;
	RMSharedWorkspaceGetFileMetadataQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, pathId);
	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceDownloadFileQuery(const std::string& repoId, const std::string& pathId, uint32_t start, uint32_t length, const int type, const bool isNXL)
{
	CELOG_ENTER;
	RMSharedWorkspaceDownloadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, pathId, start, length, type, isNXL);
	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceCheckFileExistsQuery(const std::string& repoId, const std::string& path)
{
	CELOG_ENTER;
	RMSharedWorkspaceCheckFileExistsQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, path);
	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceNXLFileHeaderQuery(const std::string& repoId, const std::string& pathId)
{
	CELOG_ENTER;
	RMSharedWorkspaceGetNXLFileHeaderQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, pathId);
	CELOG_RETURN_VAL_T(request);
}

const HTTPRequest RMSharedWorkspace::GetSharedWorkspaceUpoadFileQuery(const std::string& repoId, const std::string& parentPathId, const RMNXLFile& file, bool userConfirmedFileOverwrite, const int type)
{
	CELOG_ENTER;
	RMSharedWorkspaceUploadFileQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, repoId, file, parentPathId, type, userConfirmedFileOverwrite);
	CELOG_RETURN_VAL_T(request);
}

RetValue RMSharedWorkspace::AnalyseSharedWorkspaceListFilesResponse(const std::string& jsonstr, std::vector<SHAREDWORKSPACE_FILE_INFO>& fileList)
{
	try
	{
		auto root = nlohmann::json::parse(jsonstr);
		if (!root.is_object())
		{
			return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json Error!");
		}

		int status = root.at(STATUS_ELEMENT_NAME).get<int>();
		std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
		if (status != http::status_codes::OK.id) {
			return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
		}

		const nlohmann::json& results = root.at("results");
		const nlohmann::json& entries = results.at("detail");
		for (auto item : entries)
		{
			SHAREDWORKSPACE_FILE_INFO info;

			info.fileId = item.at("fileId").get<std::string>();
			info.fileName = item.at("name").get<std::string>();
			info.path = item.at("path").get<std::string>();
			info.pathId = item.at("pathId").get<std::string>();
			info.isFolder = item.at("isFolder").get<bool>();
			info.lastModified = item.at("lastModifiedTime").get<uint64_t>();
			info.isProtectedFile = item.at("protectedFile").get<bool>();
			if (!info.isFolder)
			{
				info.fileSize = item.at("fileSize").get<uint32_t>();
				info.fileType = item.at("fileType").get<std::string>();
			}

			fileList.push_back(info);
		}
	}
	catch (std::exception &e) {
		std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
	}
	catch (...) {
		// The JSON data is NOT correct
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct");
	}

	return RETSUCCEED();

}

RetValue RMSharedWorkspace::AnalyseSharedWorkspaceFileMetaDataResponse(const std::string& jsonstr, SHAREDWORKSPACE_FILE_METADATA& fileMetaData)
{
	try
	{
		SHAREDWORKSPACE_FILE_METADATA a;

		auto root = nlohmann::json::parse(jsonstr);
		if (!root.is_object())
		{
			return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json Error!");
		}

		int status = root.at(STATUS_ELEMENT_NAME).get<int>();
		std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
		if (status != http::status_codes::OK.id) {
			return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
		}

		const nlohmann::json& results = root.at("results");
		const nlohmann::json& item = results.at("fileInfo");

		fileMetaData.fileName = item.at("name").get<std::string>();
		fileMetaData.path = item.at("path").get<std::string>();
		fileMetaData.pathId = item.at("pathId").get<std::string>();
		fileMetaData.lastModified = item.at("lastModified").get<uint64_t>();
		fileMetaData.isProtectedFile = item.at("protectedFile").get<bool>();

		// Server bug no size.
		if (item.find("size") != item.end()) {
			fileMetaData.fileSize = item.at("size").get<uint32_t>();
		}
		else {
			fileMetaData.fileSize = 0;
		}

		fileMetaData.fileType = item.at("fileType").get<std::string>();
		fileMetaData.fileTags = item.at("tags").dump();
		fileMetaData.protectionType = item.at("protectionType").get<uint32_t>();
		fileMetaData.creationTime = item.at("creationTime").get<uint64_t>();
		fileMetaData.fileRights = item.at("rights").get<std::vector<std::string>>();

		const nlohmann::json& uploadedUser = item.at("uploadedBy");
		uint32_t userId = uploadedUser.at("userId").get<uint32_t>();
		std::string displayName = uploadedUser.at("displayName").get<std::string>();
		std::string email = uploadedUser.at("email").get<std::string>();
		fileMetaData.createByUser = SHAREDWORKSPACE_USER(userId, displayName, email);

		userId = uploadedUser.at("userId").get<uint32_t>();
		displayName = uploadedUser.at("displayName").get<std::string>();
		email = uploadedUser.at("email").get<std::string>();
		fileMetaData.lastModifiedByUser = SHAREDWORKSPACE_USER(userId, displayName, email);
	}
	catch (std::exception &e) {
		std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
	}
	catch (...) {
		// The JSON data is NOT correct
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct");
	}

	return RETSUCCEED();
}

RetValue RMSharedWorkspace::AnalyseSharedWorkspaceCheckFileExistResponse(const std::string& jsonstr, bool& isFileExist)
{
	try
	{
		auto root = nlohmann::json::parse(jsonstr);
		if (!root.is_object())
		{
			return RETMESSAGE(RMCCORE_INVALID_JSON_FORMAT, "RMS Json Error!");
		}

		int status = root.at(STATUS_ELEMENT_NAME).get<int>();
		std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
		if (status != http::status_codes::OK.id) {
			return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
		}

		const nlohmann::json& results = root.at("results");
		isFileExist = results.at("fileExists").get<bool>();
	}
	catch (std::exception &e) {
		std::string strError = "RMS JSON data is not correct : " + std::string(e.what());
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
	}
	catch (...) {
		// The JSON data is NOT correct
		return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS JSON data is not correct");
	}

	return RETSUCCEED();
}
