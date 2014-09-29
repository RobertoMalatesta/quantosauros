#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{
	class NoteLegDataInfo : public AbstractLegDataInfo {
		public:
			NoteLegDataInfo(){}
			NoteLegDataInfo(double accruedCoupon, int monitorFrequency)
			{
				m_accruedCoupon = accruedCoupon;
				m_monitorFrequency = monitorFrequency;
			}
			double NoteLegDataInfo::getAccruedCoupon();
			int NoteLegDataInfo::getMonitorFrequency();
		protected:
			double m_accruedCoupon;
			int m_monitorFrequency;
	};

	inline double NoteLegDataInfo::getAccruedCoupon(){
		return m_accruedCoupon;
	}
	inline int NoteLegDataInfo::getMonitorFrequency(){
		return m_monitorFrequency;
	}

	class SwapLegDataInfo : public AbstractLegDataInfo {
		public:

	};

}
