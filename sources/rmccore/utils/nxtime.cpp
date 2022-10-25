//
//  nxtime.cpp
//  rmccore
//
//  Created by Haobo Wang on 3/28/18.
//  Copyright Â© 2018 NextLabs, Inc. All rights reserved.
//

#include "stdafx.h"
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include "rmccore/utils/time.h"


using namespace RMCCORE;

uint64_t NXTime::stringToTime(const std::string &s)
{
    // "1900-01-01T00:00:00Z" --> 20
    if(s.length() != 20)
        return 0;
    
    std::tm t = {0};
    t.tm_year = std::stoi(std::string(s.substr(0, 4)));
    t.tm_mon = std::stoi(std::string(s.substr(5, 7)));
    t.tm_mday = std::stoi(std::string(s.substr(8, 10)));
    t.tm_hour = std::stoi(std::string(s.substr(11, 13)));
    t.tm_min = std::stoi(std::string(s.substr(14, 16)));
    t.tm_sec = std::stoi(std::string(s.substr(17, 19)));
    return (uint64_t)std::mktime(&t);
}

std::string NXTime::timeToString(uint64_t t)
{
    std::time_t dt = (std::time_t)t;
    std::string ss;
    
    char mbstr[100];
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&dt));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    ss = mbstr;
    return ss;
}

// Returns the time zone difference, in minutes, from local time zone to UTC.
// It returns the same result as the JavaScript method
// Date.prototype.getTimezoneOffset().
//
// Adapted from an answer in
// https://codereview.stackexchange.com/questions/175353/getting-current-timezone
// (https://codereview.stackexchange.com/a/175956).
int NXTime::getTimeZoneOffset(void)
{
    const time_t when = std::time(nullptr);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
    auto const tm = *std::localtime(&when);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    std::ostringstream os;
    os << std::put_time(&tm, "%z");
    std::string s = os.str();
    // s is in ISO 8601 format: "±HHMM"
    int h = std::stoi(s.substr(0,3), nullptr, 10);
    int m = std::stoi(s[0]+s.substr(3), nullptr, 10);

    // Return positive value if the local time zone is behind UTC, and
    // negative value if it is ahead.
    return -(h * 60 + m);
}
