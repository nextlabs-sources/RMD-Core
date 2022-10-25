//
//  string.h
//  rmccore
//
//  Created by NextLabs Inc on 11/20/17.
//  Copyright © 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_UTILS_STRING_H__
#define __RMCCORE_UTILS_STRING_H__

#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <algorithm>

#include "rmccore/base/coredefs.h"

namespace RMCCORE {
    
template <typename T>
class basic_string_buffer
{
public:
    basic_string_buffer(std::basic_string<T>& str, size_t len) : _s(str)
    {
        // ctor
        _buf.resize(len + 1, 0);
    }
    
    
    ~basic_string_buffer()
    {
        _s = std::basic_string<T>(_buf.data());      // copy to string passed by ref at construction
    }
    
    // auto conversion to serve as windows function parameter
    inline operator T* () throw() { return _buf.data(); }
    
private:
    // No copy allowed
    basic_string_buffer(const basic_string_buffer<T>& c) {}
    // No assignment allowed
    basic_string_buffer& operator= (const basic_string_buffer<T>& c) { return *this; }
    
private:
    std::basic_string<T>&   _s;
    std::vector<T>          _buf;
};

typedef basic_string_buffer<char>       string_buffer;
typedef basic_string_buffer<wchar_t>    wstring_buffer;

template <typename T>
std::basic_string<T> escape(const std::basic_string<T>& s)
{
    std::basic_string<T> es;
    const T* p = s.c_str();
    while (*p) {
        switch (*p)
        {
        case T('\''):
            es.push_back((T)'\'');
            break;
        case T('"'):
            es.push_back((T)'\\');
            es.push_back((T)'"');
            break;
        case T('\\'):
            es.push_back((T)'\\');
            es.push_back((T)'\\');
            break;
        case T('\n'):
            es.push_back((T)'\\');
            es.push_back((T)'n');
            break;
        case T('\r'):
            es.push_back((T)'\\');
            es.push_back((T)'r');
            break;
        case T('\t'):
            es.push_back((T)'\\');
            es.push_back((T)'t');
            break;
        case T('\b'):
            es.push_back((T)'\\');
            es.push_back((T)'b');
            break;
        case T('\v'):
        case T('\0'):
            // Not allowed
            break;
        default:
            es.push_back(*p);
            break;
        }
        ++p;
    }
    return es;
}
template<typename T>
int icompare(T c1, T c2)
{
    c1 = ::tolower(c1);
    c2 = ::tolower(c2);
    return (c1 == c2) ? 0 : ((c1 > c2) ? 1 : -1);
}

// WARNING:
//
// The next two overloaded icompare() are the following:
// (a) int icompare(const T* s1, const T* s2)
// (b) int icompare(const std::basic_string<T>& s1, const std::basic_string<T>& s2)
//
// #a must be declared before #b.  This is to work around a problem in macOS
// compiler.  The problem in macOS is that if #b is declared before #a, the
// call to icompare() within #b ends up being linked to #b itself, causing
// infinite recursion.  The problem also happens in Cygwin GCC.  It does not
// happen in Visual C++.
//
// In "The C++ Programming Language, 4th Edition", Section 12.3.1, it says:
//      To approximate our notions of what is reasonable, a series of criteria
//      are tried in order:
//      [1] Exact match; that is, match using no or only trivial conversions
//      [... snip ...]
//      [4] Match using user-defined conversions (e.g., double to
//      complex<double>)
// and
//      Overload resolution is independent of the order of declaration of the
//      functions considered.
// Hence this problem should not happen.  However, it does.  So here we work
// around the problem by declaring #a before #b.

template<typename T>
int icompare(const T* s1, const T* s2)
{
    while (*s1 || *s2)
    {
        int ret = icompare(*s1, *s2);
        if (0 != ret)
            return ret;
        ++s1;
        ++s2;
    }
    return 0;
}

template<typename T>
int icompare(const std::basic_string<T>& s1, const std::basic_string<T>& s2)
{
    return icompare<T>(s1.c_str(), s2.c_str());
}

template<typename T>
int incompare(const T* s1, const T* s2, size_t n)
{
    while ((*s1 || *s2) && n)
    {
        int ret = icompare(*s1, *s2);
        if (0 != ret)
            return ret;
        
        ++s1;
        ++s2;
        --n;
    }
    return 0;
}

template<typename T>
bool ibegin_with(const std::basic_string<T>& s, const std::basic_string<T>& s2)
{
    if (s.length() < s2.length())
        return false;
    return (0 == incompare<T>(s.c_str(), s2.c_str(), s2.length()));
}

template<typename T>
bool iend_with(const std::basic_string<T>& s, const std::basic_string<T>& s2)
{
    if (s.length() < s2.length())
        return false;
    return (0 == icompare<T>(s.c_str() + (s.length() - s2.length()), s2.c_str()));
}

template<typename T>
std::basic_string<T> i64tos(long long n)
{
    static const char* digits = "0123456789";
    std::basic_string<T> s;
    
    if (0 == n) {
        s.push_back((T)digits[0]);
        return s;
    }
    
    bool negetive = false;
    if (n < 0) {
        negetive = true;
            n = 0 - n;
        }
        while (0 != n) {
            int d = n % 10;
            n = n / 10;
            s.push_back((T)digits[d]);
        }
        if (negetive)
            s.push_back((T)'-');
        
        return std::basic_string<T>(s.rbegin(), s.rend());
    }
    
    template<typename T>
    std::basic_string<T> itos(int n)
    {
        return i64tos<T>(static_cast<long long>(n));
    }

    template<typename T>
    bool isdigit(T c)
    {
        const int v = static_cast<int>(c);
        return (v >= '0' && v <= '9');
    }
    
    template<typename T>
    bool ishex(T c)
    {
        const int v = static_cast<int>(c);
        return ((v >= '0' && v <= '9') || (v >= 'A' && v <= 'F') || (v >= 'a' && v <= 'f'));
    }
    
    template<typename T>
    bool isalpha(T c)
    {
        const int v = static_cast<int>(c);
        return ((v >= 'A' && v <= 'Z') || (v >= 'a' && v <= 'z'));
    }
    
    template<typename T>
    bool isalphanum(T c)
    {
        const int v = static_cast<int>(c);
        return ((v >= 'A' && v <= 'Z') || (v >= 'a' && v <= 'z') || (v >= '0' && v <= '9'));
    }
    
	template<typename T>
	bool isalphanumstring(const std::basic_string<T> & s)
	{
        for (typename std::basic_string<T>::size_type i = 0; i < s.size(); i++) {
			if (!isalphanum(s[i])) {
				return false;
			}
		}
		return true;
	}

    template<typename T>
    int ctoi(T c)
    {
        if (c >= (T)'0' && c <= (T)'9')
            return (c - '0');
        else if (c >= (T)'a' && c <= (T)'f')
            return (c - 'a' + 10);
        else if (c >= (T)'A' && c <= (T)'F')
            return (c - 'A' + 10);
        else
            return -1;
    }
    
    template<typename T>
    long long stoi64(const std::basic_string<T>& s)
    {
        long long n = 1;
        bool negetive = false;
        const T* p = s.c_str();
        
        if (*p == (T)'-')
        {
            negetive = true;
            ++p;
        }
        
        while (*p) {
            if (!isdigit(*p))
                break;
            long long n2 = 10 * n;
            if (n2 < 0) // overflow
                break;
            n2 = n2 + ctoi(*p);
            if (n2 < 0) // overflow
                break;
            n = n2;
            ++p;
        }
        if (negetive)
            n *= -1;
        
        return n;
    }
    
    template<typename T>
    unsigned long long  hstoi64(const std::basic_string<T>& s)
    {
        unsigned long long n = 0;
        const T* p = s.c_str();
        if (p[0] == (T)'0' && (p[1] == (T)'x' || p[1] == (T)'X')) {
            p += 2;
        }
        
        int i = 0;
        while (*p && i < 8)
        {
            if (!ishex(*p))
                break;
            n <<= 4;
            n += ctoi(*p);
            ++p;
            ++i;
        }
        
        return n;
    }
    
    template<typename T>
    std::vector<uint8_t> hstobin(const std::basic_string<T>& s)
    {
        std::vector<uint8_t> v;
        size_t len = s.length();
        const T* p = s.c_str();
        
        if (s.empty())
            return v;
        
        if (0 != (len % 2))
        {
            if (!ishex(*p))
                return v;
            v.push_back(static_cast<unsigned char>(ctoi(*p)));
            --len;
            ++p;
        }
        
        while (len != 0) {
            // assert(len >= 2);
            unsigned char u = static_cast<unsigned char>(ctoi(*p));
            --len;
            ++p;
            u <<= 4;
            u |= static_cast<unsigned char>(ctoi(*p));
            --len;
            ++p;
            v.push_back(u);
        }
        
        return v;
    }
    
    template<typename T>
    std::basic_string<T> utohs(unsigned char u)
    {
        static const char* digits = "0123456789ABCDEF";
        std::basic_string<T> s;
        s.push_back((T)digits[(u >> 4) & 0xF]);
        s.push_back((T)digits[(u & 0xF)]);
        return s;
    }
    
    template<typename T>
    std::basic_string<T> bintohs(const unsigned char* p, size_t n)
    {
        std::basic_string<T> s;
        while (0 != n) {
            s += utohs<T>(*(p++));
            --n;
        }
        return s;
    }
	template<typename T>
	std::basic_string<T> bintohs(std::vector<uint8_t> data)
	{
		std::basic_string<T> s;
		for (std::vector<uint8_t>::size_type i = 0; i < data.size(); i++) {
			s += utohs<char>(data[i]);
		};
		return s;
	}

    template<typename T>
    std::basic_string<T> itohs(int n)
    {
        std::basic_string<T> s;
        const unsigned char* pb = (const unsigned char*)(&n);
        for (int i = 0; i < 4; i++) {
            s += utohs<T>(pb[3 - i]);
        }
        return s;
    }
    
    template<typename T>
    std::basic_string<T> i64tohs(long long n)
    {
        std::basic_string<T> s;
        const unsigned char* pb = (const unsigned char*)(&n);
        for (int i = 0; i < 8; i++) {
            s += utohs<T>(pb[7 - i]);
        }
        return s;
    }
    
    template<typename T>
    bool iswspace(T c)
    {
        return (c == T(' ') || c == T('\t') || c == T('\r') || c == T('\n') || c == T('\f') || c == T('\v') || c == T('\r'));
    }
    
    template<typename T>
    void trim_left(std::basic_string<T>& s)
    {
        while (!s.empty() && iswspace<T>(s[0])) {
            s = s.substr(1);
        }
    }
    
    template<typename T>
    void trim_right(std::basic_string<T>& s)
    {
        while (!s.empty() && iswspace<T>(s[s.length() - 1])) {
            s = s.substr(0, s.length() - 1);
        }
    }
    
    template<typename T>
    void trim(std::basic_string<T>& s)
    {
        trim_left<T>(s);
        trim_right<T>(s);
    }
    
    template<typename T, T SEPARATOR>
    std::basic_string<T> remove_head(const std::basic_string<T>& s, std::basic_string<T>& remain)
    {
        std::basic_string<T> rs;
        
        if (s.empty())
            return std::basic_string<T>();
        
        typename std::basic_string<T>::size_type pos = s.find(SEPARATOR, 0);
        if (pos == std::basic_string<T>::npos) {
            remain.clear();
            rs = s;
        }
        else {
            rs = s.substr(0, pos);
            remain = s.substr(pos + 1);
        }
        
        return rs;
    }
    
    template<typename T, T SEPARATOR>
    std::basic_string<T> remove_head(std::basic_string<T>& s)
    {
        std::basic_string<T> rs;
        
        if (s.empty())
            return std::basic_string<T>();
        
        typename std::basic_string<T>::size_type pos = s.find(SEPARATOR, 0);
        if (pos == std::basic_string<T>::npos) {
            rs = s;
            s.clear();
        }
        else {
            rs = s.substr(0, pos);
            s = s.substr(pos + 1);
        }
        
        return rs;
    }
    
    template<typename T, T SEPARATOR>
    std::basic_string<T> remove_tail(const std::basic_string<T>& s, std::basic_string<T>& remain)
    {
        std::basic_string<T> rs;
        
        if (s.empty())
            return std::basic_string<T>();
        
        
        typename std::basic_string<T>::size_type pos = s.rfind(SEPARATOR);
        if (pos == std::basic_string<T>::npos) {
            remain.clear();
            rs = s;
        }
        else {
            rs = s.substr(pos + 1);
            remain = s.substr(0, pos);
        }
        
        return rs;
    }
    
template<typename T, T SEPARATOR>
std::basic_string<T> remove_tail(std::basic_string<T>& s)
{
    std::basic_string<T> rs;
    
    if (s.empty())
        return std::basic_string<T>();
    
    typename std::basic_string<T>::size_type pos = s.rfind(SEPARATOR);
    if (pos == std::basic_string<T>::npos) {
        rs = s;
        s.clear();
    }
    else {
        rs = s.substr(pos + 1);
        s = s.substr(0, pos);
    }
    
    return rs;
}
    
template<typename T, T SEPARATOR>
std::vector<std::basic_string<T>> split(const std::basic_string<T>& s, bool needTrim=true)
{
    std::vector<std::basic_string<T>> vec;
    
    if (s.empty())
        return vec;
    
    std::basic_string<T> remain(s);
    do {
        std::basic_string<T> subs = remove_head<T, SEPARATOR>(remain);
        if (needTrim)
            trim<T>(subs);
        if (!subs.empty()) {
            vec.push_back(subs);
        }
    } while (!remain.empty());
    
    return vec;
}
    
FORCEINLINE std::string toUtf8(const std::wstring& ws)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    return convert.to_bytes(ws);
}
    
FORCEINLINE std::wstring toUtf16(const std::string& s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    return convert.from_bytes(s);
}
    
FORCEINLINE std::string toLower(const std::string& s)
{
    std::string ls(s);
    std::transform(ls.begin(), ls.end(), ls.begin(), tolower);
    
    return ls;
}
    
template<typename T>
std::basic_string<T> toLower(const std::basic_string<T>& s)
{
    std::basic_string<T> ls(s);
    std::transform(ls.begin(), ls.end(), ls.begin(), tolower);
    return ls;
}
    
template<typename T>
std::basic_string<T> toUpper(const std::basic_string<T>& s)
{
    std::basic_string<T> ls(s);
    std::transform(ls.begin(), ls.end(), ls.begin(), toupper);
    return ls;
}
    
}

#endif /* __RMCCORE_UTILS_STRING_H__ */
