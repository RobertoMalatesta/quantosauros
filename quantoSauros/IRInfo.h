#pragma once
#include <ql/quantlib.hpp>
#include "InterestRateCurve.h";
#include "VolatilitySurface.h";
#include "HullWhiteParameters.h";

namespace quantoSauros{

	class IRInfo {
		public:
			quantoSauros::IRInfo(){};
			quantoSauros::IRInfo(
				quantoSauros::InterestRateCurve interestRateCurve, 
				quantoSauros::VolatilitySurface volatilitySurface,
				quantoSauros::HullWhiteParameters hullWhiteParams,
				quantoSauros::RateType rateType,
				double floatCurveTenor,
				QuantLib::Frequency swapCouponFrequency
				){
					m_interestRateCurve = interestRateCurve;
					m_volatilitySurface = volatilitySurface;
					m_hullWhiteParams = hullWhiteParams;
					m_rateType = rateType;
					m_floatCurveTenor = floatCurveTenor;
					m_swapCouponFrequency = swapCouponFrequency;
			}
			quantoSauros::IRInfo(
				quantoSauros::InterestRateCurve interestRateCurve, 
				quantoSauros::VolatilitySurface volatilitySurface,
				quantoSauros::HullWhiteParameters hullWhiteParams
				){
					m_interestRateCurve = interestRateCurve;
					m_volatilitySurface = volatilitySurface;
					m_hullWhiteParams = hullWhiteParams;
			}
			quantoSauros::InterestRateCurve getInterestRateCurve();
			quantoSauros::VolatilitySurface getVolatilitySurface();
			quantoSauros::HullWhiteParameters getHullWhiteParams();
			quantoSauros::RateType getRateType();
			double getFloatCurveTenor();
			QuantLib::Frequency getSwapCouponFrequency();

		private:
			quantoSauros::InterestRateCurve m_interestRateCurve;
			quantoSauros::VolatilitySurface m_volatilitySurface;
			quantoSauros::HullWhiteParameters m_hullWhiteParams;
			quantoSauros::RateType m_rateType;
			double m_floatCurveTenor;
			QuantLib::Frequency m_swapCouponFrequency;
	};

	inline quantoSauros::InterestRateCurve IRInfo::getInterestRateCurve(){
		return m_interestRateCurve;
	}
	inline quantoSauros::VolatilitySurface IRInfo::getVolatilitySurface(){
		return m_volatilitySurface;
	}
	inline quantoSauros::HullWhiteParameters IRInfo::getHullWhiteParams(){
		return m_hullWhiteParams;
	}
	inline quantoSauros::RateType IRInfo::getRateType(){
		return m_rateType;
	}
	inline double IRInfo::getFloatCurveTenor(){
		return m_floatCurveTenor;
	}
	inline QuantLib::Frequency IRInfo::getSwapCouponFrequency(){
		return m_swapCouponFrequency;
	}
}