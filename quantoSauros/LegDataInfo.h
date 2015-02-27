#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{
	class NoteLegDataInfo : public AbstractLegDataInfo {
		public:
			NoteLegDataInfo()
				: AbstractLegDataInfo(){
			};
			NoteLegDataInfo(double accruedCoupon, int monitorFrequency)
				: AbstractLegDataInfo(monitorFrequency){
				m_accruedCoupon = accruedCoupon;				
			};

			double NoteLegDataInfo::getAccruedCoupon();
			
		protected:
			double m_accruedCoupon;			
	};

	inline double NoteLegDataInfo::getAccruedCoupon(){
		return m_accruedCoupon;
	}

	class SwapLegDataInfo : public AbstractLegDataInfo {
		public:

	};

}
