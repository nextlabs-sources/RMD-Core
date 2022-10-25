//
//  rmtoken.cpp
//  rmccore
//
//  Created by Haobo Wang on 12/8/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/restful/rmtoken.h"
#include "rmccore/network/httpConst.h"
#include "rmrestkey.h"

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMTOKEN_CPP

using namespace RMCCORE;

#define TOKENS_KEY_NAME                         "Tokens"
#define TOKENID_ELEMENT_NAME                    "Duid"
#define TOKENKEY_ELEMENT_NAME                   "Key"
#define TOKENMAINTAINLEVEL_ELEMENT_NAME         "Ml"
#define TOKEOTP_ELEMENT_NAME                    "Otp"

RMToken::RMToken() :m_ml(0), m_tokenid(""), m_key(""), m_initialized(false), m_otp("")
{

}

RMToken::RMToken(const nlohmann::json& root)
    : m_ml(0)
{
    m_initialized = ImportFromJson(root);
}

RMToken::RMToken(std::string duid, std::string key, unsigned int ml) :m_ml(ml), m_initialized(true)
{
    m_tokenid = duid;
    m_key = key;
}

RMToken::~RMToken()
{
}

RMToken & RMToken::operator= (const RMToken &rhs)
{
    if (this != &rhs)
    {
        m_tokenid = rhs.m_tokenid;
        m_key = rhs.m_key;
		m_ml = rhs.m_ml;
		m_initialized = rhs.m_initialized;
		m_otp = rhs.m_otp;
    }
    return *this;
}

RetValue RMToken::ImportFromRMSJson(const nlohmann::json& root)
{
    CELOG_ENTER;
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();

        if (status != http::status_codes::OK.id) {
            CELOG_RETURN_VAL_T(RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message));
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        m_key = result.at(TOKEN_ELEMENT_NAME).get<std::string>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        CELOG_RETURN_VAL_T(RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Tenant result"));
    }

    CELOG_RETURN_VAL_T(RETSUCCEED());
}

RetValue RMToken::ImportFromJson(const nlohmann::json& root)
{
    CELOG_ENTER;
    RetValue res = RETSUCCEED();
    try {
        m_tokenid = root.at(TOKENID_ELEMENT_NAME).get<std::string>();
        m_key = root.at(TOKENKEY_ELEMENT_NAME).get<std::string>();
        m_ml = root.at(TOKENMAINTAINLEVEL_ELEMENT_NAME).get<int>();
        m_otp = root.at(TOKEOTP_ELEMENT_NAME).get<std::string>();

        m_initialized = (m_key.length() > 0);
        CELOG_LOG(CELOG_DEBUG, "m_tokenid=%s, m_ml=%u\n", m_tokenid.c_str(), m_ml);
    }
    catch (std::exception &e) {
        std::string strError = "JSON Token value is not correct, error : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON Token value is not correct");
    }

    CELOG_RETURN_VAL_T(res);
}

nlohmann::json RMToken::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[TOKENID_ELEMENT_NAME] = m_tokenid;
        root[TOKENKEY_ELEMENT_NAME] = m_key;
        root[TOKENMAINTAINLEVEL_ELEMENT_NAME] = m_ml;
        root[TOKEOTP_ELEMENT_NAME] = m_otp;
    }
    catch (std::exception &e) {
        std::string strError = "Export Token Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        CELOG_LOG(CELOG_ERROR, "Export Token Json error\n");
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Token Json error");
    }
    return root;
}

RMTokenPool::RMTokenPool()
{
    m_tokenarr.empty();
}
RMTokenPool::~RMTokenPool()
{
}

const RMToken RMTokenPool::pop()
{
	RMToken token;
	if (m_tokenarr.size() == 0) {
		return token;
	}

	token = m_tokenarr.front();
	m_tokenarr.pop_front();
	
	return token;
}

RetValue RMTokenPool::ImportFromRMSJson(const nlohmann::json& root)
{
    CELOG_ENTER;
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id)
        {
            CELOG_RETURN_VAL_T(RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message));
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        unsigned int ml = std::stoi(result.at(TOKEN_MAINTAINLEVEL_ELEMENT_NAME).get<std::string>());
        const nlohmann::json& tokens = result.at(TOKEN_ARRAY_KEY_NAME);
        for (auto& element : tokens.items())
        {
            std::string duid = element.key();
            std::string key;
            std::string otp;
            const nlohmann::json& item = element.value();
            if (item.is_object())
            {
                if (item.end() != item.find("token"))
                {
                    key = item.at("token").get<std::string>();
                }

                if (item.end() != item.find("otp"))
                {
                    otp = item.at("otp").get<std::string>();
                }
            }
            else if (item.is_string())
            {
                key = item.get<std::string>();
            }

            RMToken token(duid, key, ml);
            if (otp.length() > 2)
            {
                token.SetOtp(otp);
            }

            m_tokenarr.push_back(token);
        }
    }
    catch (const RetValue &e) {
        CELOG_RETURN_VAL_T(e);
    }
    catch (...) {
        // The JSON data is NOT correct
        CELOG_RETURN_VAL_T(RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Token result"));
    }

    CELOG_RETURN_VAL_T(RETSUCCEED());
}

RetValue RMTokenPool::ImportFromJson(const nlohmann::json& root)
{
    CELOG_ENTER;
    RetValue res = RETSUCCEED();
    try {
        nlohmann::json tokenarr = root.at(TOKENS_KEY_NAME);
        for (auto& item : tokenarr)
        {
            RMToken toke(item);
            m_tokenarr.push_back(toke);
        }
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON User value is not correct");
    }

    CELOG_RETURN_VAL_T(res);
}

nlohmann::json RMTokenPool::ExportToJson()
{
    CELOG_ENTER;
    nlohmann::json root = nlohmann::json::object();
    try {
        root[TOKENS_KEY_NAME] = nlohmann::json::array();
        nlohmann::json& tokenarr = root[TOKENS_KEY_NAME];
        for (std::list<RMToken>::iterator it = m_tokenarr.begin(); it != m_tokenarr.end(); it++)
        {
            tokenarr.push_back((*it).ExportToJson());
        }
    }
    catch (...) {
        // The JSON data is NOT correct
        CELOG_LOG(CELOG_ERROR, "RMTokenPool Json error");
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMTokenPool Json error");
    }
    CELOG_RETURN_VAL_NOPRINT(root);
}
