#pragma once
#include <ql/quantlib.hpp>
#include "InterestRateCurve.h";
#include "VolatilitySurface.h";
#include "HullWhiteParameters.h";

namespace quantoSauros{

	class IRInfo {
		public:
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

		private:
			quantoSauros::InterestRateCurve m_interestRateCurve;
			quantoSauros::VolatilitySurface m_volatilitySurface;
			quantoSauros::HullWhiteParameters m_hullWhiteParams;
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
}