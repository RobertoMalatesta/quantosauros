#pragma once

#include "AbstractLegInfo.h"
#include <ql/quantlib.hpp>

namespace quantoSauros{

#pragma region NoteLegAmortizationInfo

	class NoteLegAmortizationInfo : public AbstractLegAmortizationInfo {
		public:
			NoteLegAmortizationInfo(QuantLib::Money notional){
				m_notional = notional;
			}
			NoteLegAmortizationInfo(QuantLib::Money notional, bool includePrincipal){
				m_notional = notional;
				m_includePrincipal = includePrincipal;
			}
			QuantLib::Money getNotional();
			bool getIncludePrincipal();
		protected:
			//Notional
			QuantLib::Money m_notional;
			//include Principal
			bool m_includePrincipal;
	};

#pragma region inline-functions of NoteLegAmortizationInfo
	inline QuantLib::Money NoteLegAmortizationInfo::getNotional(){
		return m_notional;
	}
	inline bool NoteLegAmortizationInfo::getIncludePrincipal(){
		return m_includePrincipal;
	}
#pragma endregion

#pragma endregion

#pragma region SwapLegAmortizationInfo

	class SwapLegAmortizationInfo : public AbstractLegAmortizationInfo {
		public:

	};

#pragma endregion
}
