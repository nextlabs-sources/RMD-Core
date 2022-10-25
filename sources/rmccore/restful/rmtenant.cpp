//
//  rmtenant.cpp
//  rmccore
//
//  Created by Haobo Wang on 11/29/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/restful/rmtenant.h"
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"

#include "rmrestkey.h"
#include <assert.h>

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMTENANT_CPP

using namespace RMCCORE;


//using namespace NX::REST::http;


#define DEFAULT_ROUTER_URL        "https://www.skydrm.com"
#define DEFAULT_TENANT            "skydrm.com"

#define TENANT_ELEMENT_NAME       "Tenant"
#define ROUTERURL_ELEMENT_NAME    "RouterURL"
#define SERVER_ELEMENT_NAME       "ServerURL"

RMTenant::RMTenant(std::string router, std::string tenant) : RestfulBase(), m_serverurl("")
{
    SetRouter(router);
    SetTenant(tenant);

	if (router.length() == 0 && tenant.length() == 0)//only set the RMS when all values are default
		SetRMS("https://www.skydrm.com/rms");
}


RMTenant::~RMTenant()
{
}

void RMTenant::SetRMS(const std::string rms)
{
	if (rms.length() == 0)
	{
		// log error
		return;
	}
    m_serverurl = rms;
}

void RMTenant::SetRouter(const std::string router)
{
    if (router.length() == 0)
    {
        m_routeurl = DEFAULT_ROUTER_URL;
    }
    else
    {
        m_routeurl = router;
    }
}

void RMTenant::SetTenant(const std::string tenant)
{
    m_tenant = tenant;
}

RMTenant & RMTenant::operator= (const RMTenant & tenant)
{
    if (this != &tenant)
    {
        m_tenant = tenant.m_tenant;
        m_routeurl = tenant.m_routeurl;
        m_serverurl = tenant.m_serverurl;
    }
    
    return *this;
}

bool RMTenant::operator== (const RMTenant &rhs) const
{
    if (this == &rhs)
        return true;

    if (0 != m_tenant.compare(rhs.m_tenant))
    {
        return false;
    }

    if (0 != m_routeurl.compare(rhs.m_routeurl))
    {
        return false;
    }

    if (0 != m_serverurl.compare(rhs.m_serverurl))
    {
        return false;
    }

    return true;
}

RetValue RMTenant::ImportFromRMSJson(const nlohmann::json& root)
{
    try {
        int status = root.at(STATUS_ELEMENT_NAME).get<int>();
        std::string message = root.at(MESSAGE_ELEMENT_NAME).get<std::string>();
        if (status != http::status_codes::OK.id) {
            return RETMESSAGE((RMCCORE_RMS_ERRORCODE_BASE + status), message);
        }

        const nlohmann::json& result = root.at(RESULT_KEY_NAME);
        m_serverurl = result.at(TENANT_SERVER_ELEMENT_NAME).get<std::string>();
    }
    catch (std::exception &e) {
        std::string strError = "JSON value is not correct, error : " + std::string(e.what());
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        return RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "RMS returned invalid Tenant result");
    }

    return RETSUCCEED();
}

RetValue RMTenant::ImportFromJson(const nlohmann::json& root)
{
    RetValue res = RETSUCCEED();
    try {
        m_tenant = root.at(TENANT_ELEMENT_NAME).get<std::string>();
        m_routeurl = root.at(ROUTERURL_ELEMENT_NAME).get<std::string>();
        m_serverurl = root.at(SERVER_ELEMENT_NAME).get<std::string>();
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

nlohmann::json RMTenant::ExportToJson()
{
    nlohmann::json root = nlohmann::json::object();
    try {
        root[TENANT_ELEMENT_NAME] = m_tenant;
        root[ROUTERURL_ELEMENT_NAME] = m_routeurl;
        root[SERVER_ELEMENT_NAME] = m_serverurl;
    }
    catch (std::exception &e) {
        std::string strError = "JSON response is not correct, error : " + std::string(e.what());
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, strError);
    }
    catch (...) {
        // The JSON data is NOT correct
        throw RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Export Tenant Json error");
    }

    return root;
}

HTTPRequest RMTenant::GetTenantQuery(void)
{
    CELOG_ENTER;
    if (m_tenant.empty())
    {
        CELOG_RETURN_VAL_T(RMQueryTenantRequest(m_routeurl));
    }
    else
    {
        CELOG_RETURN_VAL_T(RMQueryTenantRequest(m_routeurl, m_tenant));
    }
}

HTTPRequest RMTenant::GetProductUpdateQuery(const RMSystemPara &para)
{
    CELOG_ENTER;
    RMAutoUpdateQuery query(m_tenant, m_serverurl, para);
    CELOG_RETURN_VAL_T(query);
}
