#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{

#pragma region NoteLegAmortizationInfo

	class NoteLegAmortizationInfo : public AbstractLegAmortizationInfo {
		public:
			NoteLegAmortizationInfo(QuantLib::Money notional)
				: AbstractLegAmortizationInfo(notional){				
			}

			NoteLegAmortizationInfo(QuantLib::Money notional, bool includePrincipal)
				: AbstractLegAmortizationInfo(notional){
					m_includePrincipal = includePrincipal;
			}			
			bool getIncludePrincipal();
		protected:			
			//include Principal
			bool m_includePrincipal;
	};

#pragma region inline-functions of NoteLegAmortizationInfo
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
