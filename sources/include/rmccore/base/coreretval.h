//
//  coreretval.h
//  rmccore
//
//  Created by Gavin Ye on 10/19/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_BASE_CORERETVAL_H__
#define __RMCCORE_BASE_CORERETVAL_H__

#include <string>
#include "rmccore/base/coredefs.h"

namespace RMCCORE {
    
class RetValue
{
public:
    RetValue(const char* f, const char* fn, int32_t l, int32_t c, std::string msg = "")
        : code(c)
        , line(l)
        , file(f)
        , function(fn)
    {
        message = msg;
    }
    RetValue(const RetValue& rhs)
        : code(rhs.code)
        , line(rhs.line)
        , file(rhs.file)
        , function(rhs.function)
        , message(rhs.message)
    {
    }
    ~RetValue()
    {
    }
    
    RetValue& operator = (const RetValue& rhs)
    {
        if(this != &rhs)
        {
            code = rhs.code;
            line = rhs.line;
            file = rhs.file;
            message = rhs.message;
            function = rhs.function;
        }
        return *this;
    }
    
    inline int32_t GetCode() const { return code; }
    inline int32_t GetLine() const { return code; }
    inline const char* GetFile() const { return file; }
    inline const char* GetFunction() const { return function; }
    inline const char* GetMessage() const { return message.c_str();}
    inline operator bool () const { return (0 == code); }
    inline operator int32_t () const { return code; }
    std::string ToString(void) const {
        return std::string("{") + std::to_string(code) + ", \"" + message + "\"}";
    }
    
protected:
private:
    int32_t code;
    int32_t line;
    std::string message;
    const char* file;
    const char* function;
};

}

#define RETSUCCEED()            RMCCORE::RetValue(__FILE__, __FUNCTION__, __LINE__, 0)
#define RETVALUE(code)          RMCCORE::RetValue(__FILE__, __FUNCTION__, __LINE__, code)
#define RETMESSAGE(code, msg)   RMCCORE::RetValue(__FILE__, __FUNCTION__, __LINE__, code, msg)

#endif /* __RMCCORE_BASE_CORERETVAL_H__ */
