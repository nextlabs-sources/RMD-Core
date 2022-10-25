//
//  nxlpolicy.h
//  rmccore
//
//  Created by Gavin Ye on 10/20/17.
//  Copyright � 2017 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_FORMAT_NXLPOLICY_H__
#define __RMCCORE_FORMAT_NXLPOLICY_H__

#include "rmccore/base/coredefs.h"
#include "rmccore/format/nxlrights.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace RMCCORE {
namespace NXLFMT {

    class Rights
    {
    public:
        Rights();
        Rights(uint64_t v);
        Rights(const Rights& rhs);
        virtual ~Rights();

        inline bool empty() const { return (0 == rights); }
        inline uint64_t get() const { return rights; }

        Rights& operator = (const Rights& rhs);
        Rights& operator + (const Rights& rhs);
        Rights& operator - (const Rights& rhs);

        size_t  ToStrings(std::vector<std::string> & rightstr) const;
        //   When strict is set, this function return true only
        // if current rights contain all of given rights.
        //   When strict is NOT set, this function return true as
        // long as current rights contain any of given rights.
        enum Constrain {
            ALL = 0,
            ANY
        };
        bool contains(const Rights& rhs, Constrain type = ALL) const;
        bool contains(uint64_t r, Constrain type = ALL) const;
        bool full() const;

    private:
        uint64_t rights;
    };

    class FullRights : public Rights
    {
    public:
        FullRights();
        virtual ~FullRights() {}
    };

    class Obligation
    {
    public:
        Obligation();
        Obligation(const std::string& obName);
        Obligation(const Obligation& rhs);
        virtual ~Obligation() {}
        Obligation& operator = (const Obligation& rhs);
        
		bool operator== (const Obligation &rhs) const;

        inline const std::string& getName() const { return name; }
        
        std::string getArg(const std::string& key) const;
        void setArg(const std::string& key, const std::string& val);
    private:
        std::string name;
        std::unordered_map<std::string, std::string> args;
    };
    typedef std::vector<Obligation> Obligations;

	class Expiration
	{
	public:
		enum ExpirationType {
			NeverExpire = 0,
			RelativeExpire,
			AbsoluteExpire,
			RangeExpire
		};
		Expiration()
			: _type(NeverExpire)
			, _start(0)
			, _end(0)
		{
		}
		Expiration(ExpirationType type, uint64_t start, uint64_t end)
			: _type(type)
			, _start(start)
			, _end(end)
		{
		}
		Expiration(const Expiration& rhs)
			: _type(rhs._type)
			, _start(rhs._start)
			, _end(rhs._end)
		{
		}
		~Expiration() {}

		bool expired() const { return (secondsToLive() <= 0); }
		ExpirationType getType() const { return _type; }
		uint64_t getStartDate() const { return _start; }
		uint64_t getEndDate() const { return _end; }

		void setType(ExpirationType type) { _type = type; }
		void setStartDate(uint64_t start) { _start = start; }
		void setEndData(uint64_t end) { _end = end; }

		int64_t secondsPriorLive() const;
		int64_t secondsToLive() const;

	private:
		ExpirationType _type;
		uint64_t _start;
		uint64_t _end;
	};

}
}

#endif /* __RMCCORE_FORMAT_NXLPOLICY_H__ */
