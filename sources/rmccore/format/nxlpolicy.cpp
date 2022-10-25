//
//  nxlpolicy.cpp
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright � 2017 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <ctime>
#include "rmccore/format/nxlrights.h"
#include "rmccore/format/nxlpolicy.h"
#include "rmccore/utils/string.h"
#include "rmccore/utils/time.h"
#include "rmccore/crypto/aes.h"
#include "rmccore/crypto/sha.h"

using namespace RMCCORE;
using namespace RMCCORE::NXLFMT;

Rights::Rights()
    : rights(0)
{
}

Rights::Rights(uint64_t v)
    : rights(v)  // & BUILTIN_RIGHT_ALL
{
}

Rights::Rights(const Rights& rhs)
    : rights(rhs.rights)
{
}

Rights::~Rights()
{
}

Rights& Rights::operator = (const Rights& rhs)
{
    if (this != &rhs) {
        rights = rhs.rights;
    }
    return *this;
}

Rights& Rights::operator + (const Rights& rhs)
{
    rights |= (rhs.get() & BUILTIN_RIGHT_ALL);
    return *this;
}

Rights& Rights::operator - (const Rights& rhs)
{
    rights &= ~(rhs.get() & BUILTIN_RIGHT_ALL);
    return *this;
}

size_t  Rights::ToStrings(std::vector<std::string> & rightstr) const
{
	rightstr.clear();
	if (rights & BUILTIN_RIGHT_VIEW) {
		rightstr.push_back(RIGHT_ACTION_VIEW);
	}
	if (rights & BUILTIN_RIGHT_EDIT) {
		rightstr.push_back(RIGHT_ACTION_EDIT);
	}
	if (rights & BUILTIN_RIGHT_PRINT) {
		rightstr.push_back(RIGHT_ACTION_PRINT);
	}
	if (rights & BUILTIN_RIGHT_CLIPBOARD) {
		rightstr.push_back(RIGHT_ACTION_CLIPBOARD);
	}
	if (rights & BUILTIN_RIGHT_SAVEAS) {
		rightstr.push_back(RIGHT_ACTION_SAVEAS);
	}
	if (rights & BUILTIN_RIGHT_DECRYPT) {
		rightstr.push_back(RIGHT_ACTION_DECRYPT);
	}
	if (rights & BUILTIN_RIGHT_SCREENCAP) {
		rightstr.push_back(RIGHT_ACTION_SCREENCAP);
	}
	if (rights & BUILTIN_RIGHT_SEND) {
		rightstr.push_back(RIGHT_ACTION_SEND);
	}
	if (rights & BUILTIN_RIGHT_CLASSIFY) {
		rightstr.push_back(RIGHT_ACTION_CLASSIFY);
	}
	if (rights & BUILTIN_RIGHT_SHARE) {
		rightstr.push_back(RIGHT_ACTION_SHARE);
	}
	if (rights & BUILTIN_RIGHT_DOWNLOAD) {
		rightstr.push_back(RIGHT_ACTION_DOWNLOAD);
	}

	return rightstr.size();
}
bool Rights::contains(const Rights& rhs,  Constrain type) const
{
    if(ALL)
        return (rhs.get() == (rights & rhs.get()));
    else
        return (0 != (rights & rhs.get()));
}

bool Rights::contains(uint64_t r,  Constrain type) const
{
    r &= BUILTIN_RIGHT_ALL;
    if(ALL)
        return (r == (rights & r));
    else
        return (0 != (rights & r));
}

bool Rights::full() const
{
    return contains(BUILTIN_RIGHT_ALL, ALL);
}

FullRights::FullRights()
    : Rights(BUILTIN_RIGHT_ALL)
{
}


Obligation::Obligation()
{
}

Obligation::Obligation(const std::string& obName)
    : name(obName)
{
}

Obligation::Obligation(const Obligation& rhs)
    : name(rhs.name)
    , args(rhs.args)
{
}

Obligation& Obligation::operator = (const Obligation& rhs)
{
    if (this != &rhs)
    {
        name = rhs.name;
        args = rhs.args;
    }
    return *this;
}

bool Obligation:: operator== (const Obligation &rhs) const
{
	if (this == &rhs)
		return true;

	if (0 != name.compare(rhs.name))
		return false;

	if (args == rhs.args)
		return true;

	return false;
}

std::string Obligation::getArg(const std::string& key) const
{
	std::string s(key);
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	const auto it = args.find(s);
    return (it == args.end()) ? "" : (*it).second;
}

void Obligation::setArg(const std::string& key, const std::string& val)
{
    std::string s(key);
    std::transform(s.begin(), s.end(), s.begin(), tolower);
    args[s] = val;
}

int64_t Expiration::secondsPriorLive() const
{
	int64_t seconds = 0;
	if (_type == RangeExpire)
		seconds = (int64_t)std::difftime((time_t)_start, (time_t)RMCCORE::NXTime::nowTime());
	if (seconds < 0)
		seconds = 0;
	return seconds;
}

int64_t Expiration::secondsToLive() const
{
	int64_t seconds = 0;

	if (_type == NeverExpire)
	{
		seconds = 0x7FFFFFFF;
	}
	else if (_type == AbsoluteExpire)
	{
		seconds = (int64_t)std::difftime((time_t)_end, (time_t)RMCCORE::NXTime::nowTime());
	}
	else if (_type == RangeExpire)
	{
		if (0 == secondsPriorLive())
			seconds = (int64_t)std::difftime((time_t)_end, (time_t)RMCCORE::NXTime::nowTime());
	}
	else
	{
		;
	}
	if (seconds < 0)
		seconds = 0;
	return seconds;
}


