/*!
 * \file condition.h
 *
 * \author hbwang
 * \date 2018/05/03 13:30
 *
 * 
 */

#ifndef __RMCCORE_POLICY_CONDITION_H__
#define __RMCCORE_POLICY_CONDITION_H__


#include "rmccore/format/nxlpolicy.h"
#include "rmccore/restful/rmrestbase.h"
#include "rmccore/utils/time.h"

using namespace RMCCORE::NXLFMT;

namespace RMCCORE {
	namespace CONDITION {
		enum ExpiryType {
			NEVEREXPIRE = 0,
			RELATIVEEXPIRE,
			ABSOLUTEEXPIRE,
			RANGEEXPIRE
		};
		class Expiry : public Expiration
		{
		public:
			Expiry(Expiration &expiration);
			Expiry(ExpiryType type, uint64_t end = 0, uint64_t start = RMCCORE::NXTime::nowTime() * 1000);
			virtual ~Expiry() {}

			Expiry& operator = (const Expiry& rhs);
			Expiry& operator = (const Expiration & rhs);
		};
	}
}

#endif /* condition_h */
