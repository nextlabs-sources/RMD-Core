//
//  rmheartbeat.cpp
//  rmccore
//
//  Created by Haobo Wang on 4/3/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>

#include "rmccore/restful/rmheartbeat.h"
#include "rmccore/network/httpConst.h"

#include "rmrestkey.h"
#include <assert.h>

using namespace RMCCORE;

#define DEFAULT_TIMESTAMP_VALUE			9999999999999

RMPolicyConfig::RMPolicyConfig() : RestfulBase(), m_lastmodify(DEFAULT_TIMESTAMP_VALUE)
{

}

RMPolicyConfig & RMPolicyConfig::operator = (const RMPolicyConfig & rhs)
{
    if (this != &rhs)
    {
        m_lastmodify = rhs.m_lastmodify;
        m_watermark = rhs.m_watermark;
        m_classification = rhs.m_classification;
    }
    return *this;
}

bool RMPolicyConfig::operator== (const RMPolicyConfig &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_lastmodify != rhs.m_lastmodify)
        return false;

    if (!(m_watermark == rhs.m_watermark))
        return false;

    return true;
}

RetValue RMPolicyConfig::ImportFromRMSJson(const nlohmann::json& root)
{
    return ImportFromJson(root);
}

RetValue RMPolicyConfig::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_lastmodify = root.at(HEARTBEAT_POLICY_MODIFY_ELEMENT_NAME).get<uint64_t>();

        const nlohmann::json& watermarkConfig = root.at(HEARTBEAT_WATERMARK_CONFIG_KEY_NAME);
        WatermarkConfig config;
        config.ImportFromJson(watermarkConfig);
        m_watermark = config.GetWatermarkSetting();

        const nlohmann::json& classficationCategories = root.at(HEARTBEAT_CLASIFY_KEY_NAME);
        m_classification.clear();
        for (auto it = classficationCategories.begin(); classficationCategories.end() != it; it++)
        {
            CLASSIFICATION_CAT classi;
            classi.name = (*it).at(HEARTBEAT_CLASIFY_NAME).get<std::string>();
            classi.multiSelect = (*it).at(HEARTBEAT_CLASIFY_MULISELECT).get<bool>();
            classi.mandatory = (*it).at(HEARTBEAT_CLASIFY_MANDATORY).get<bool>();

            const nlohmann::json& labels = (*it).at(HEARTBEAT_CLASIFY_LABELS);
            for (auto j = labels.begin(); labels.end() != j; j++)
            {
                CLASSIFICATION_LABELS label;
                label.name = (*j).at(HEARTBEAT_CLASIFY_LABELS_NAME).get<std::string>();
                label.allow = (*j).at(HEARTBEAT_CLASIFY_LABELS_DEFAULT).get<bool>();
                classi.labels.push_back(label);
            }
            m_classification.push_back(classi);
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

nlohmann::json RMPolicyConfig::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[HEARTBEAT_POLICY_MODIFY_ELEMENT_NAME] = m_lastmodify;
        root[HEARTBEAT_WATERMARK_CONFIG_KEY_NAME] = m_watermark.ExportToJson();

        root[HEARTBEAT_CLASIFY_KEY_NAME] = nlohmann::json::array();
        nlohmann::json& categories = root[HEARTBEAT_CLASIFY_KEY_NAME];
        for (auto & category : m_classification)
        {
            nlohmann::json item = nlohmann::json::object();
            item[HEARTBEAT_CLASIFY_NAME] = category.name;
            item[HEARTBEAT_CLASIFY_MULISELECT] = category.multiSelect;
            item[HEARTBEAT_CLASIFY_MANDATORY] = category.mandatory;

            item[HEARTBEAT_CLASIFY_LABELS] = nlohmann::json::array();
            nlohmann::json& labels = item[HEARTBEAT_CLASIFY_LABELS];
            for (auto & label : category.labels)
            {
                nlohmann::json obj = nlohmann::json::object();
                obj[HEARTBEAT_CLASIFY_LABELS_NAME] = label.name;
                obj[HEARTBEAT_CLASIFY_LABELS_DEFAULT] = label.allow;
                labels.push_back(obj);
            }

            categories.push_back(item);
        }
    }
    catch (std::exception &e) {
        std::string strError = "ExportToJson error, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "ExportToJson error");
    }

    return root;
}


RMHeartbeat::RMHeartbeat() : RestfulBase(), m_hbfrequency(DEFAULT_HEARTBEAT_FREQUENCY_VALUE), m_icenetUrl("")
{
    m_policyconfigs.clear();
	m_tokenGroupResourceTypeMap.clear();
}

bool RMHeartbeat::operator== (const RMHeartbeat &rhs) const
{
    if (this == &rhs)
        return true;

    if (m_hbfrequency != rhs.m_hbfrequency)
        return false;

    if (!(m_defaultwatermark == rhs.m_defaultwatermark))
        return false;

	if (m_tokenGroupResourceTypeMap != rhs.m_tokenGroupResourceTypeMap)
		return false;

    if (m_policyconfigs == rhs.m_policyconfigs)
        return true;

    return false;
}

const std::string RMHeartbeat::GetPolicyConfigTokenGroupName(int index) const
{
    std::string s;
    policymap::const_iterator it = m_policyconfigs.cbegin();
    for (; it != m_policyconfigs.cend(); ++it, index--)
        {
            if (index == 0) {
                s = it->first;
            }
        }

    return s;
}

bool RMHeartbeat::GetPolicyConfig(const std::string & tokenGroupName, RMPolicyConfig & config) const
{
    const auto it = m_policyconfigs.find(tokenGroupName);

    if (it == m_policyconfigs.end()) {
        return false;
    };

    config = it->second;

    return true;
}

bool RMHeartbeat::GetTokenGroupResourceType(const std::string & tokenGroupName, std::string & resourcetype) const
{
	const auto it = m_tokenGroupResourceTypeMap.find(tokenGroupName);

	if (it == m_tokenGroupResourceTypeMap.end()) {
		return false;
	};

	resourcetype = it->second;

	return true;
}

RetValue RMHeartbeat::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();

        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        return ImportFromJson(result);
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Heartbeat result");
    }

    return RETSUCCEED();
}

RetValue RMHeartbeat::ImportFromJson(const nlohmann::json& root)
{
    try {
        m_hbfrequency = root.at(HEARTBEAT_FREQUENCY_ELEMENT_NAME).get<uint32_t>();

        const nlohmann::json& watermarkConfig = root.at(HEARTBEAT_WATERMARK_CONFIG_KEY_NAME);
        m_defaultwatermark.ImportFromJson(watermarkConfig);

        const nlohmann::json& policyConfigData = root.at(HEARTBEAT_POLICY_KEY_NAME);
        m_policyconfigs.clear();
        for (auto it = policyConfigData.begin(); policyConfigData.end() != it; it++)
        {
            std::string tokengroupname = (*it).at(HEARTBEAT_POLICY_TOKENGROUPNAME_ELEMENT_NAME).get<std::string>();
            RMPolicyConfig config;
            config.ImportFromJson(*it);
            m_policyconfigs[tokengroupname] = config;
        }

		m_tokenGroupResourceTypeMap.clear();
		if (root.end() != root.find(HEARTBEAT_TOKENGROUP_RESOURCETYPE_MAPPING_KEY_NAME))
		{
			const nlohmann::json& tokengroupResTyMap = root.at(HEARTBEAT_TOKENGROUP_RESOURCETYPE_MAPPING_KEY_NAME);
			std::for_each(m_policyconfigs.cbegin(), m_policyconfigs.cend(), [&](const auto & it) {
				std::string tokengroup = it.first;
				if (tokengroupResTyMap.end() != tokengroupResTyMap.find(tokengroup))
				{
					std::string resourceType = tokengroupResTyMap.at(tokengroup).get<std::string>();
					m_tokenGroupResourceTypeMap[tokengroup] = resourceType;
				}
			});
		}
		if (root.end() != root.find(HEARTBEAT_ICENET_URL_KEY_NAME))
		{
			m_icenetUrl = root.at(HEARTBEAT_ICENET_URL_KEY_NAME).get<std::string>();
		}
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Heartbeat result");
    }

    return RETSUCCEED();
}

nlohmann::json RMHeartbeat::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[HEARTBEAT_FREQUENCY_ELEMENT_NAME] = m_hbfrequency;
        root[HEARTBEAT_WATERMARK_CONFIG_KEY_NAME] = m_defaultwatermark.ExportToJson();
        root[HEARTBEAT_POLICY_KEY_NAME] = nlohmann::json::array();
        
        nlohmann::json& policys = root[HEARTBEAT_POLICY_KEY_NAME];
        std::for_each(m_policyconfigs.cbegin(), m_policyconfigs.cend(), [&](const auto & it) {
            std::string tokengroupname = it.first;
            RMPolicyConfig policyConfig = it.second;
            nlohmann::json config = policyConfig.ExportToJson();
            config[HEARTBEAT_POLICY_TOKENGROUPNAME_ELEMENT_NAME] = tokengroupname;

            policys.push_back(config);
        });

		nlohmann::json tokenGroupRTmapObject = nlohmann::json::object();
		std::for_each(m_tokenGroupResourceTypeMap.cbegin(), m_tokenGroupResourceTypeMap.cend(), [&](const auto & it) {
			std::string tokengroupname = it.first;
			std::string resourceType = it.second;
			tokenGroupRTmapObject[tokengroupname] = resourceType;
		});
		root[HEARTBEAT_TOKENGROUP_RESOURCETYPE_MAPPING_KEY_NAME] = tokenGroupRTmapObject;

		root[HEARTBEAT_ICENET_URL_KEY_NAME] = m_icenetUrl;
    }
    catch (std::exception &e) {
        std::string strError = "ExportToJson error, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "ExportToJson error");
    }

    return root;
}
