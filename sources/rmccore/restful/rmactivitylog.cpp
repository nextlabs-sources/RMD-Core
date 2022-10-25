//
//  rmactivitylog.cpp
//  rmccore
//
//  Created by Haobo Wang on 2/21/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include "rmccore/restful/rmactivitylog.h"
#include "rmccore/network/httpConst.h"
#include "rmrestkey.h"
#include "rmccore/utils/string.h"

using namespace RMCCORE;

#define ACTIVITYLOG_KEY_NAME                    "ActivityLogs"


RMActivityLog::RMActivityLog()
{
	m_logstring = ",,,,,,,,,,,,,,,,"; 
}

RMActivityLog::RMActivityLog(const RMSystemPara& param, RM_USER_DATA& userData, RM_NXLFILE_INFO& fileInfo, const std::string& extraData)
{
	m_logstring = "\"";
	// Field 0: DUID
	m_logstring += fileInfo.duid; //nxlduid;
	// Field 1:  Owner Id
	m_logstring += "\",\""; m_logstring += fileInfo.ownerId; // fileownerid;
	// Field 2:  User Id
	m_logstring += "\",\""; m_logstring += userData.userId; // userid;
	// Field 3:  Operation
	m_logstring += "\",\""; m_logstring += itos<char>(fileInfo.operation);
	// Field 4:  Device Id
	m_logstring += "\",\""; m_logstring += param.GetDeviceID();
	// Field 5:  Device Type
	m_logstring += "\",\""; m_logstring += itos<char>(param.GetPlatformID());
	// Field 6:  Repository Id
	m_logstring += "\",\""; m_logstring += fileInfo.repoId;
	// Field 7:  File pathId
	m_logstring += "\",\""; m_logstring += fileInfo.fileId;
	// Field 8:  File Name
	m_logstring += "\",\""; m_logstring += fileInfo.fileName;
	// Field 9:  File Display Path
	m_logstring += "\",\""; m_logstring += fileInfo.filePath;
	// Field 10: Application Name
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetName();
	// Field 11: Application Path
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetPath();
	// Field 12: Application Publisher
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetPublisherName();
	// Field 13: Access Result
	m_logstring += "\",\""; m_logstring += itos<char>(fileInfo.logResult);
	// Field 14: Access Time
	m_logstring += "\",\""; m_logstring += i64tos<char>(fileInfo.accessTime);
	// Field 15: Activity Data
	m_logstring += "\",\""; m_logstring += extraData;
	// Field 16: Account Type
	m_logstring += "\",\""; m_logstring += itos<char>(userData.accountType);
	m_logstring += "\"";
}

RMActivityLog::RMActivityLog(const std::string& nxlduid, const std::string& fileownerid, const std::string& userid, const RMActivityLogOperation operation,
	const RMSystemPara & param, const std::string& repoId, const std::string& fileId, const std::string& fileName, const std::string& filePath,
	const RMActivityLogResult accessResult, const std::string& extraData, uint64_t accessTime, const RMActivityLogAccountType accountType)
{
	// There are total 16 fields as Version 2
	// Reference: https://bitbucket.org/nxtlbs-devops/rightsmanagement-wiki/wiki/RMS/RESTful%20API/Log%20REST%20API

	m_logstring = "\"";
	// Field 0: DUID
	m_logstring += nxlduid;
	// Field 1:  Owner Id
	m_logstring += "\",\""; m_logstring += fileownerid;
	// Field 2:  User Id
	m_logstring += "\",\""; m_logstring += userid;
	// Field 3:  Operation
	m_logstring += "\",\""; m_logstring += itos<char>(operation);
	// Field 4:  Device Id
	m_logstring += "\",\""; m_logstring += param.GetDeviceID();
	// Field 5:  Device Type
	m_logstring += "\",\""; m_logstring += itos<char>(param.GetPlatformID());
	// Field 6:  Repository Id
	m_logstring += "\",\""; m_logstring += repoId;
	// Field 7:  File pathId
	m_logstring += "\",\""; m_logstring += fileId;
	// Field 8:  File Name
	m_logstring += "\",\""; m_logstring += fileName;
	// Field 9:  File Display Path
	m_logstring += "\",\""; m_logstring += filePath;
	// Field 10: Application Name
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetName();
	// Field 11: Application Path
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetPath();
	// Field 12: Application Publisher
	m_logstring += "\",\""; m_logstring += param.GetProduct().GetPublisherName();
	// Field 13: Access Result
	m_logstring += "\",\""; m_logstring += itos<char>(accessResult);
	// Field 14: Access Time
	m_logstring += "\",\""; m_logstring += i64tos<char>(accessTime);
	// Field 15: Activity Data
	m_logstring += "\",\""; m_logstring += extraData;
	// Field 16: Account Type
	m_logstring += "\",\""; m_logstring += itos<char>(accountType);
	m_logstring += "\"";
}

RMActivityLog::~RMActivityLog()
{
}

RMActivityLog & RMActivityLog::operator= (const RMActivityLog &rhs)
{
    if (this != &rhs)
    {
        m_logstring = rhs.Serialize();
    }
    return *this;
}

RMLogPool::RMLogPool()
{
    m_logarr.empty();
}
RMLogPool::~RMLogPool()
{
}

void RMLogPool::AddLog(RMActivityLog & log)
{
	m_logarr.push_back(log);
}

const std::string RMLogPool::ExportLogStringForUpload(unsigned int num)
{
    std::string ret = "";
    size_t retlognum = num;
    if(num == 0)
        retlognum = size();

    while(m_uploadarr.size())
	{//reset array and push all log back to array
        RMActivityLog log = m_uploadarr.back();
        m_logarr.push_front(log);
        m_uploadarr.pop_back();
    }
    while(retlognum && m_logarr.size())
	{
        RMActivityLog log = m_logarr.front();
        ret += log.Serialize();
        m_uploadarr.push_back(log);
        m_logarr.pop_front();

		// add new line for multiple logs
		if (m_logarr.size() > 0)
			ret += "\n";

		retlognum--;
	}
    
    return ret;
}

#define LOGPOOL_ELEMENT_NAME				"Logs"
#define LOGPOOL_NUM_ELEMENT_NAME			"LogNum"



RMNXLFileActivity::RMNXLFileActivity() : m_email(""), m_operation(""), m_deviceType(""), m_deviceId(""), m_accessTime(0), m_accessResult("")
{
}

RMNXLFileActivity & RMNXLFileActivity::operator= (const RMNXLFileActivity &rhs)
{
    if (this != &rhs)
    {
        m_email = rhs.m_email;
        m_operation = rhs.m_operation;
        m_deviceType = rhs.m_deviceType;
        m_deviceId = rhs.m_deviceId;
        m_accessResult = rhs.m_accessResult;
        m_accessTime = rhs.m_accessTime;
    }
    return *this;
}

bool RMNXLFileActivity::operator== (const RMNXLFileActivity &rhs) const
{
    if (this == &rhs)
        return false;

    if (0 != m_duid.compare(rhs.m_duid))
        return false;

    if (0 != m_email.compare(rhs.m_email))
        return false;

    if (0 != m_operation.compare(rhs.m_operation))
        return false;

    if (0 != m_deviceType.compare(rhs.m_deviceType))
        return false;

    if (0 != m_deviceId.compare(rhs.m_deviceId))
        return false;

    if (m_accessTime != rhs.m_accessTime)
        return false;

    if (0 != m_accessResult.compare(rhs.m_accessResult))
        return false;

    return true;
}

#define ACTIVITYLOG_EMAIL_ELEMENT_NAME          "Email"
#define ACTIVITYLOG_OPERATION_ELEMENT_NAME      "Operation"
#define ACTIVITYLOG_DEVICETYPE_ELEMENT_NAME     "DeviceType"
#define ACTIVITYLOG_DEVICEID_ELEMENT_NAME       "DeviceID"
#define ACTIVITYLOG_ACCESSRESULT_ELEMENT_NAME   "AccessResult"
#define ACTIVITYLOG_ACCESSTIME_ELEMENT_NAME     "AccessTime"


RetValue RMNXLFileActivity::ImportFromRMSJson(const nlohmann::json& root)
{
    try
    {
        m_email = root.at(LOG_EMAIL_ELEMENT_NAME).get<std::string>();
        m_operation = root.at(LOG_OPERATION_ELEMENT_NAME).get<std::string>();
        m_deviceType = root.at(LOG_DEVICETYPE_ELEMENT_NAME).get<std::string>();
        m_deviceId = root.at(LOG_DEVICEID_ELEMENT_NAME).get<std::string>();
        m_accessTime = root.at(LOG_ACCESSTIME_ELEMENT_NAME).get<uint64_t>();
        m_accessResult = root.at(LOG_ACCESSRESULT_ELEMENT_NAME).get<std::string>();
    }
    catch (const RetValue &e)
    {
        return e;
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Activity result");
    }

    return RETSUCCEED();
}

//ACTIVITYLOG_EMAIL_ELEMENT_NAME
//ACTIVITYLOG_OPERATION_ELEMENT_NAME
//ACTIVITYLOG_DEVICETYPE_ELEMENT_NAME
RetValue RMNXLFileActivity::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_email = root.at(ACTIVITYLOG_EMAIL_ELEMENT_NAME).get<std::string>();
        m_operation = root.at(ACTIVITYLOG_OPERATION_ELEMENT_NAME).get<std::string>();
        m_deviceType = root.at(ACTIVITYLOG_DEVICETYPE_ELEMENT_NAME).get<std::string>();
        m_deviceId = root.at(ACTIVITYLOG_DEVICEID_ELEMENT_NAME).get<std::string>();
        m_accessTime = root.at(ACTIVITYLOG_ACCESSTIME_ELEMENT_NAME).get<uint64_t>();
        m_accessResult = root.at(ACTIVITYLOG_ACCESSRESULT_ELEMENT_NAME).get<std::string>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMNXLFileActivity::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try
    {
        root[ACTIVITYLOG_EMAIL_ELEMENT_NAME] = m_email;
        root[ACTIVITYLOG_OPERATION_ELEMENT_NAME] = m_operation;
        root[ACTIVITYLOG_DEVICETYPE_ELEMENT_NAME] = m_deviceType;
        root[ACTIVITYLOG_DEVICEID_ELEMENT_NAME] = m_deviceId;
        root[ACTIVITYLOG_ACCESSTIME_ELEMENT_NAME] = m_accessTime;
        root[ACTIVITYLOG_ACCESSRESULT_ELEMENT_NAME] = m_accessResult;
    }
    catch (...)
    {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export File Activity Json error");
    }
    return root;
}

RMNXLFileLogs::RMNXLFileLogs(void): m_filename(""), m_duid("")
{
	m_logarr.clear();
}

bool RMNXLFileLogs::operator== (const RMNXLFileLogs &rhs) const
{
    if (this == &rhs)
        return true;

    if (0 != m_filename.compare(rhs.m_filename))
        return false;

    if (0 != m_duid.compare(rhs.m_duid))
        return false;

    if (!(m_logarr == rhs.m_logarr))
        return false;

    return true;
}

#define ACTIVITYLOG_FILENAME_ELEMENT_NAME       "FileName"
#define ACTIVITYLOG_DUID_ELEMENT_NAME           "DUID"
#define ACTIVITYLOG_LOG_KEY_NAME                "Logs"
#define ACTIVITYLOG_TOTALLOG_ELEMENT_NAME		"TotalLogNum"


RetValue RMNXLFileLogs::ImportFromRMSJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        if (root.end() != root.find(RESULT_KEY_NAME))
        {
            const nlohmann::json& result = root[RESULT_KEY_NAME];
            if (result.end() != result.find(LOG_RECORDNUM_ELEMENT_NAME))
            {
                m_totallogs = result.at(LOG_RECORDNUM_ELEMENT_NAME).get<uint32_t>();
            }

            const nlohmann::json& data = result[LOG_DATA_KEY_NAME];

            m_filename = data.at(LOG_FILENAME_ELEMENT_NAME).get<std::string>();
            m_duid = data.at(LOG_FILEDUID_ELEMENT_NAME).get<std::string>();

            const nlohmann::json& records = data.at(LOG_RECORDS_KEY_NAME);
            for (auto& item : records)
            {
                RMNXLFileActivity activitylog;
                activitylog.ImportFromRMSJson(item);
                m_logarr.push_back(activitylog);
            }
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

//LOG_FILENAME_ELEMENT_NAME,  ACTIVITYLOG_FILENAME_ELEMENT_NAME
//LOG_FILEDUID_ELEMENT_NAME,  ACTIVITYLOG_DUID_ELEMENT_NAME
//ACTIVITYLOG_LOG_KEY_NAME,
RetValue RMNXLFileLogs::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_filename = root.at(ACTIVITYLOG_FILENAME_ELEMENT_NAME).get<std::string>();
        m_duid = root.at(ACTIVITYLOG_DUID_ELEMENT_NAME).get<std::string>();
        m_totallogs = root.at(ACTIVITYLOG_TOTALLOG_ELEMENT_NAME).get<uint32_t>();

        const nlohmann::json& logRecords = root.at(ACTIVITYLOG_LOG_KEY_NAME);
        for (auto& item : logRecords)
        {
            RMNXLFileActivity activitylog;
            activitylog.ImportFromJson(item);
            activitylog.SetDuid(m_duid);
            m_logarr.push_back(activitylog);
        }
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMNXLFileLogs::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[ACTIVITYLOG_FILENAME_ELEMENT_NAME] = m_filename;
        root[ACTIVITYLOG_DUID_ELEMENT_NAME] = m_duid;
        root[ACTIVITYLOG_TOTALLOG_ELEMENT_NAME] = m_totallogs;
        root[ACTIVITYLOG_LOG_KEY_NAME] = nlohmann::json::array();

        nlohmann::json& logarr = root[ACTIVITYLOG_LOG_KEY_NAME];
        for (size_t i = 0; i < m_logarr.size(); i++)
        {
            logarr.push_back(m_logarr[i].ExportToJson());
        }
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export File Activity log Json error");
    }

    return root;
}

const RMNXLFileActivity * RMNXLFileLogs::GetAt(size_t index)
{
	if (index >= m_logarr.size())
		return NULL;

	return &m_logarr[index];
}
