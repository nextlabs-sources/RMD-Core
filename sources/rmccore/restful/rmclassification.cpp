
//
//  rmproject.cpp
//  rmccore
//
//  Created by Haobo Wang on 5/22/18.
//  Copyright � 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include "common/celog2/celog.h"
#include "rmccore/network/httpConst.h"
#include "RMHttpRequest.h"

#include "rmccore/restful/rmclassification.h"
#include "rmrestkey.h"
#include <assert.h>

using namespace RMCCORE;

#define CELOG_CUR_MODULE "rmccore"
#define CELOG_CUR_FILE CELOG_FILEPATH_SOURCES_RMCCORE_RESTFUL_RMCLASSIFICATION_CPP

RMClassification::RMClassification()
{

}


RMClassification::~RMClassification()
{
}

RetValue RMClassification::Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket)
{
	RetValue ret = RETSUCCEED();

	m_params = sys;
	m_tenant = tenant;
	m_userid = userid;
	m_ticket = ticket;
	m_initialized = true;
	if (ticket.length() != 32) {
		ret = RETMESSAGE(RMCCORE_ERROR_INVALID_DATA, "Invalid user ticket to initialize Project!");
		return ret;
	}

	return ret;
}

HTTPRequest RMClassification::GetClassificationQuery(const std::string& tenantid)
{
    CELOG_ENTER;
    RMClassificationQuery request(m_tenant, m_params, std::string(itos<char>(m_userid)), m_ticket, tenantid);
    CELOG_RETURN_VAL_T(request);
}
