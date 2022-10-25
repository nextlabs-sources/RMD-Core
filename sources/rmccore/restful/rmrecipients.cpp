//
//  rmrecipients.cpp
//  rmccore
//
//  Created by Haobo Wang on 2/2/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "rmccore/restful/rmrecipients.h"
#include "rmccore/network/httpConst.h"

#include "rmrestkey.h"


using namespace RMCCORE;

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMRECIPIENTS_CPP


//using namespace NX::REST::http;

#define RECIPIENTS_KEY_NAME        "CurrentRecipients"
#define ADDRECIPIENTS_KEY_NAME     "AddRecipients"
#define REMOVERECIPIENTS_KEY_NAME  "RemoveRecipients"
#define COMMENTS_ELEMENT_NAME	   "Comments"
#define RECIPIENT_ELEMENT_NAME     "Recipient"

RMRecipients::RMRecipients(const RMRecipientList & recipients): RestfulBase()
{
    ResetData();
    m_recipients = recipients;
}
RMRecipients::RMRecipients() : RestfulBase()
{
    ResetData();
}


RMRecipients::~RMRecipients()
{
}

void RMRecipients::ResetData()
{
    m_recipients.clear();
    m_remove_recipients.clear();
    m_add_recipients.clear();

	m_comments = "";
}

uint8_t RMRecipients::AddRecipients(RMRecipientList recipients, bool buploaded)
{
    uint8_t ret = 0;
	for_each(recipients.cbegin(), recipients.cend(), [&](const std::string &recipient)
	{
		size_t i;
		size_t j;
		bool found = false;
		for (i = 0; i < m_recipients.size(); i++)
		{
			if (0 == recipient.compare(m_recipients[i]))
			{
				found = true;
				break;
			}
		}
		if (!found) {//check if the recipients in remove list.
			for (i = 0; i < m_remove_recipients.size(); i++) {
				if (0 == recipient.compare(m_remove_recipients[i])) {
					found = true;
					m_remove_recipients.erase(m_remove_recipients.begin() + i);
					m_recipients.push_back(recipient);//add it back to recipients list;
					ret++;
					break;
				}
			}
		}
		if (!found)
		{
			for (j = 0; j < m_add_recipients.size(); j++)
			{
				if (0 == recipient.compare(m_add_recipients[j]))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				ret++;
				if (buploaded)
					m_add_recipients.push_back(recipient);
				else
					m_recipients.push_back(recipient);
			}
		}
	}
	       
    );
    return ret;
}

uint8_t RMRecipients::RemoveRecipients(RMRecipientList recipients, bool buploaded)
{
    uint8_t ret = 0;
    for_each(recipients.cbegin(), recipients.cend(), [&](const std::string &recipient)
	{
		bool found = false;
		for (size_t i = 0; i < m_recipients.size(); i++)
		{
			if (0 == recipient.compare(m_recipients[i]))
			{
				m_recipients.erase(m_recipients.begin() + i);
				found = true;
				break;
			}
		}
		if (!found) {//check if the recipients added to add_recipients list.
			for (size_t i = 0; i < m_add_recipients.size(); i++) {
				if (0 == recipient.compare(m_add_recipients[i])) {
					m_add_recipients.erase(m_add_recipients.begin() + i);
					ret++;
					break;
				}
			}
		}
		if (found)
		{
			found = false;
			for (size_t j = 0; j < m_remove_recipients.size(); j++)
			{
				if (0 == recipient.compare(m_remove_recipients[j]))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				ret++;
				if (buploaded)
					m_remove_recipients.push_back(recipient);
			}
		}
	});
    return ret;
}

RMRecipients & RMRecipients::operator= (const RMRecipients & rhs)
{
    if (this != &rhs)
    {
        ResetData();
        m_recipients = rhs.m_recipients;
        m_remove_recipients = rhs.m_remove_recipients;
        m_add_recipients = rhs.m_add_recipients;
		
		m_comments = rhs.m_comments;
    }
    
    return *this;
}

RetValue RMRecipients::ImportFromRMSJson(const nlohmann::json& root)
{
    if (!NeedUpdateRMS()) {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "No remove/add recipients request");
    }

    RetValue res = RETSUCCEED();
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        m_comments = "";//clear comments after update

        m_recipients.clear();
        if (result.end() != result.find(SHARE_SHARED_KEY_NAME))
        {
            m_recipients = result[SHARE_SHARED_KEY_NAME].get<std::vector<std::string>>();
            std::sort(m_recipients.begin(), m_recipients.end());
        }

        if (result.end() != result.find(SHARE_ADDRECIPIENTS_KEY_NAME))
        {
            RMRecipientList addRecipients = result[SHARE_ADDRECIPIENTS_KEY_NAME].get<std::vector<std::string>>();
            std::sort(addRecipients.begin(), addRecipients.end());

            RMRecipientList  recipients = std::move(m_recipients);
            std::set_union(recipients.begin(), recipients.end(), addRecipients.begin(), addRecipients.end(),
                std::inserter(m_recipients, m_recipients.begin()));
            std::sort(m_recipients.begin(), m_recipients.end());

            RMRecipientList request = std::move(m_add_recipients);
            std::set_difference(request.begin(), request.end(), addRecipients.begin(),
                addRecipients.end(), std::inserter(m_add_recipients, m_add_recipients.begin()));
        }

        if (result.end() != result.find(SHARE_REMOVERECIPIENTS_KEY_NAME))
        {
            RMRecipientList removeRecipients = result[SHARE_REMOVERECIPIENTS_KEY_NAME].get<std::vector<std::string>>();
            std::sort(removeRecipients.begin(), removeRecipients.end());

            RMRecipientList request = std::move(m_remove_recipients);
            std::set_difference(request.begin(), request.end(), removeRecipients.begin(),
                removeRecipients.end(), std::inserter(m_remove_recipients, m_remove_recipients.begin()));

            RMRecipientList recipients = std::move(m_recipients);
            std::sort(recipients.begin(), recipients.end());
            std::set_difference(recipients.begin(), recipients.end(), removeRecipients.begin(),
                removeRecipients.end(), std::inserter(m_recipients, m_recipients.begin()));
        }
        res = RETMESSAGE(0, message);
    }
    catch (std::exception &e) {
        std::string strError = "RMS returned Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned Json error");
    }

    return res;
}

RetValue RMRecipients::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    ResetData();
    try {
        m_comments = root.at(COMMENTS_ELEMENT_NAME).get<std::string>();

        const nlohmann::json& currentRecipients = root.at(RECIPIENTS_KEY_NAME);
        m_recipients = currentRecipients.get<RMRecipientList>();

        const nlohmann::json& addRecipients = root.at(ADDRECIPIENTS_KEY_NAME);
        m_add_recipients = addRecipients.get<RMRecipientList>();

        const nlohmann::json& removeRecipients = root.at(REMOVERECIPIENTS_KEY_NAME);
        m_remove_recipients = removeRecipients.get<RMRecipientList>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON value is not correct, error : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON value is not correct");
    }

    return res;
}

nlohmann::json RMRecipients::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();

    //JsonValue * recipients = JsonValue::CreateObject();
    try {
        root[COMMENTS_ELEMENT_NAME] = m_comments;

        //CurrentRecipients
        root[RECIPIENTS_KEY_NAME] = m_recipients;

        //AddRecipients
        root[ADDRECIPIENTS_KEY_NAME] = m_add_recipients;

        //RemoveRecipients
        root[REMOVERECIPIENTS_KEY_NAME] = m_remove_recipients;
    }
    catch (std::exception &e) {
        std::string strError = "Export Recipients Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        CELOG_LOG(CELOG_ERROR, "Export Recipients Json error\n");
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Recipients Json error");
    }

    return root;
}

