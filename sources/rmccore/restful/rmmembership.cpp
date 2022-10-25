//
//  rmmembership.cpp
//  rmccore
//
//  Created by NextLabs Inc on 11/30/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include <stdio.h>
#include "stdafx.h"
#include <stdio.h>
#include "rmccore/restful/rmmembership.h"
#include "rmccore/network/httpConst.h"
#include "rmrestkey.h"

using namespace RMCCORE;

#define ID_ELEMENT_NAME             "Id"
#define TYPE_ELEMENT_NAME           "Type"
#define TENANTID_ELEMENT_NAME       "TenantId"
#define TOKENGROUPNAME_ELEMENT_NAME "TokenGroupName"
#define PROJECTID_ELEMENT_NAME      "ProjectId"
#define CERTIFICATE_KEY_NAME        "Certificate"


RMMembership::RMMembership() : RestfulBase(), m_rmsid(""), m_tenantid(""), m_tokenGroupName(""), m_type(MTTenant), m_projectid(0)
{
}


RMMembership::~RMMembership()
{
}

RMMembership & RMMembership::operator= (const RMMembership &rhs)
{
    if (this != &rhs)
    {
        m_rmsid = rhs.m_rmsid;
        m_tenantid = rhs.m_tenantid;
		m_tokenGroupName = rhs.m_tokenGroupName;
		m_type = rhs.m_type;
        m_projectid = rhs.m_projectid;
        
        m_certificate = rhs.m_certificate;
    }
    return *this;
}

RetValue RMMembership::ImportFromRMSJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_rmsid = root.at(MEMBERSHIP_ID_ELEMENT_NAME).get<std::string>();

        if (root.end() != root.find(MEMBERSHIP_TOKENGROUPNAME_ELEMENT_NAME))
        {
            m_tokenGroupName = root[MEMBERSHIP_TOKENGROUPNAME_ELEMENT_NAME].get<std::string>();
        }

        m_type = root.at(MEMBERSHIP_TYPE_ELEMENT_NAME).get<MembershipType>();
        if (m_type == MTTenant)
        {
            // default tenant
            m_projectid = -1;
        }
        else if (m_type == MTProject)
        {
            // project
        }
        else if (m_type == MTSystemBucket)
        {
            // system bucket
            m_projectid = 0;
        }


        if (root.end() != root.find(MEMBERSHIP_TENANTID_ELEMENT_NAME))
        {
            m_tenantid = root[MEMBERSHIP_TENANTID_ELEMENT_NAME].get<std::string>();
        }

        if (root.end() != root.find(MEMBERSHIP_PROJID_ELEMENT_NAME))
        {
            m_projectid = root[MEMBERSHIP_PROJID_ELEMENT_NAME].get<uint32_t>();
        }

        RMCertificate cert;
        m_certificate = cert;//clear certificate class
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON membership value is not correct");
    }

    return res;
}

RetValue RMMembership::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_rmsid = root.at(ID_ELEMENT_NAME).get<std::string>();
        m_tenantid = root.at(TENANTID_ELEMENT_NAME).get<std::string>();
        m_tokenGroupName = root.at(TOKENGROUPNAME_ELEMENT_NAME).get<std::string>();
        m_type = root.at(TYPE_ELEMENT_NAME).get<MembershipType>();
        m_projectid = root.at(PROJECTID_ELEMENT_NAME).get<uint32_t>();

        const nlohmann::json& certificate = root.at(CERTIFICATE_KEY_NAME);
        m_certificate.ImportFromJson(certificate);
    }
    catch (std::exception &e) {
        std::string strError = "JSON data is not correct : " + std::string(e.what());
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        res = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "JSON data is not correct");
    }

    return res;
}

nlohmann::json RMMembership::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[ID_ELEMENT_NAME] = m_rmsid;
        root[TENANTID_ELEMENT_NAME] = m_tenantid;
        root[TOKENGROUPNAME_ELEMENT_NAME] = m_tokenGroupName;
        root[TYPE_ELEMENT_NAME] = m_type;
        root[PROJECTID_ELEMENT_NAME] = m_projectid;

        root[CERTIFICATE_KEY_NAME] = m_certificate.ExportToJson();
    }
    catch (std::exception &e) {
        std::string strError = "Export membership Json error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export membership Json error");
    }
    return root;
}
