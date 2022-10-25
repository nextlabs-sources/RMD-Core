/*!
 * \file condition.cpp
 *
 * \author hbwang
 * \date 2018/05/03 13:31
 *
 * 
 */


#include "stdafx.h"
#include <stdio.h>
#include "rmccore/restful/rmtenant.h"
#include "rmccore/network/httpConst.h"
#include "rmccore/policy/condition.h"


#include "../restful/rmrestkey.h"
#include <assert.h>

using namespace RMCCORE;
using namespace RMCCORE::CONDITION;
using namespace RMCCORE::NXLFMT;

Expiry::Expiry(Expiration &expiration): Expiration(expiration.getType(), expiration.getStartDate(), expiration.getEndDate())
{
}

Expiry::Expiry (ExpiryType type, uint64_t end, uint64_t start) : Expiration((ExpirationType)type, start, end)
{
}


Expiry & Expiry::operator= (const Expiry & rhs)
{
	if (this != &rhs)
	{
		setType(rhs.getType());
		setStartDate(rhs.getStartDate());
		setEndData(rhs.getEndDate());
	}
	return *this;
}

Expiry &Expiry::operator= (const Expiration &rhs)
{
	setType(rhs.getType());
	setStartDate(rhs.getStartDate());
	setEndData(rhs.getEndDate());

	return *this;
}
