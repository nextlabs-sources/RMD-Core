#pragma once
#ifndef __RMCCORE_RESTFUL_CLASSIFICATION_H__
#define __RMCCORE_RESTFUL_CLASSIFICATION_H__


#include "rmccore/restful/rmsyspara.h"
#include "rmccore/restful/rmtenant.h"

namespace RMCCORE {
	class RMClassification
	{
	public:
		RMClassification();
		~RMClassification();

		bool IsInitialized(void) { return m_initialized; }

        //function inherit from RestfulBase
	public:

    public:

		int GetMaxCategoryNum() const { return m_maxCategoryNum; }
		int GetMaxLabelNum() const { return m_maxLabelNum; }
		std::vector<CLASSIFICATION_CAT> GetClassificationCategories() const { return m_classification; }
		HTTPRequest GetClassificationQuery(const std::string& tenantid);

	//protected:
		RetValue Initialize(const RMSystemPara &sys, const RMTenant & tenant, unsigned int userid, const std::string ticket);

	protected:
		bool          m_initialized;
		RMSystemPara  m_params;
		RMTenant      m_tenant;
		unsigned int  m_userid;
		std::string   m_ticket;

		uint64_t	m_servertime;
		int         m_maxCategoryNum;
		int         m_maxLabelNum;
		std::vector<CLASSIFICATION_CAT> m_classification;

	};
}

#endif /* __RMCCORE_RESTFUL_CLASSIFICATION_H__*/