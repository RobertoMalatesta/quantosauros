#pragma once

#include "AbstractLegInfo.h"
#include <ql/quantlib.hpp>

namespace quantoSauros{

#pragma region NoteLegCouponInfo

	class NoteLegCouponInfo : public AbstractLegCouponInfo {
		public:

	};

	//Range Coupon Info
	class NoteLegRangeCouponInfo : public NoteLegCouponInfo {

		protected:
			double* m_inCouponRates;
			double* m_outCouponRates;
			double* m_upperBounds;
			double* m_lowerBounds;
	};
	class NoteLegSpreadRangeCouponInfo : public NoteLegRangeCouponInfo {
		
		private:
			double m_tenor1;
			double m_tenor2;
			QuantLib::Frequency m_swapCouponFrequency1;
			QuantLib::Frequency m_swapCouponFrequency2;
	};
	class NoteLegSpotRangeCouponInfo : public NoteLegRangeCouponInfo {

		private:
			double m_tenor;
			QuantLib::Frequency m_swapCouponFrequency;
	};

	//TODO float Coupon Info

	//TODO fixed Coupon Info

#pragma endregion

#pragma region SwapLegCouponInfo

	class SwapLegCouponInfo : public AbstractLegCouponInfo {
		public:

	};

#pragma endregion

}
