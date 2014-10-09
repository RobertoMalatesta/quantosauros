#pragma once

#include "AbstractLegInfo.h"
#include "InterestRate.h"
#include "Period.h"
#include "InterestRateCurve.h"
#include "IRInfo.h"

#include <ql/quantlib.hpp>

namespace quantoSauros{

#pragma region NoteLegCouponInfo

	class NoteLegCouponInfo : public AbstractLegCouponInfo {
		public:
			NoteLegCouponInfo(){}
			std::string getClassName(){
				return m_className;
			};
	protected:			
			std::string m_className;
	};

	//Range Coupon Info
	class NoteLegRangeCouponInfo : public NoteLegCouponInfo {
		public:
			quantoSauros::RateType getRateType1();
			double getTenor1();
			QuantLib::Frequency getSwapCouponFrequency1();
			quantoSauros::InterestRateCurve getReferenceRateCurve1();		

			std::vector<double> getUpperBounds();
			std::vector<double> getLowerBounds();
			std::vector<double> getInCouponRates();
			std::vector<double> getOutCouponRates();			
		protected:	
			//기초자산1 유형
			quantoSauros::RateType m_rateType1;
			//기초자산1 테너
			double m_tenor1;
			//기초자산1 스왑쿠폰주기
			QuantLib::Frequency m_swapCouponFrequency1;
			//기초자산1 금리커브
			quantoSauros::InterestRateCurve m_referenceRateCurve1;
			//기초자산금리 정보
			std::vector<quantoSauros::IRInfo> m_irInfos;

			//Range Accrual In-Coupon 이자율
			std::vector<double> m_inCouponRates;
			//Range Accrual Out-Coupon 이자율
			std::vector<double> m_outCouponRates;
			//기준자산1 Upper-Barrier
			std::vector<double> m_upperBounds;
			//기준자산1 Lower-Barrier
			std::vector<double> m_lowerBounds;
	};
	class NoteLegSpreadRangeCouponInfo : public NoteLegRangeCouponInfo {
		public:
			NoteLegSpreadRangeCouponInfo(
				quantoSauros::RateType rateType1, double tenor1, 
				QuantLib::Frequency swapCouponFrequency1,
				quantoSauros::InterestRateCurve referenceRateCurve1,
				quantoSauros::RateType rateType2, double tenor2, 
				QuantLib::Frequency swapCouponFrequency2,
				quantoSauros::InterestRateCurve referenceRateCurve2,
				//std::vector<quantoSauros::Period> rangePeriods,
				std::vector<double> inCouponRates, std::vector<double> outCouponRates,
				std::vector<double> upperBounds, std::vector<double> lowerBounds){					
					m_rateType1 = rateType1;
					m_tenor1 = tenor1;
					m_swapCouponFrequency1 = swapCouponFrequency1;
					m_referenceRateCurve1 = referenceRateCurve1;
					m_rateType2 = rateType2;
					m_tenor2 = tenor2;
					m_swapCouponFrequency2 = swapCouponFrequency2;
					m_referenceRateCurve2 = referenceRateCurve2;
					//m_rangePeriods = rangePeriods;
					m_inCouponRates = inCouponRates;
					m_outCouponRates = outCouponRates;
					m_upperBounds = upperBounds;
					m_lowerBounds = lowerBounds;
					m_className = "NoteLegSpreadRangeCouponInfo";
			}

			quantoSauros::RateType getRateType2();
			double getTenor2();
			QuantLib::Frequency getSwapCouponFrequency2();
			quantoSauros::InterestRateCurve getReferenceRateCurve2();

		private:
			//기초자산2 유형
			quantoSauros::RateType m_rateType2;
			//기초자산2 테너
			double m_tenor2;			
			//기초자산2 스왑쿠폰주기
			QuantLib::Frequency m_swapCouponFrequency2;
			//기초자산2 금리커브
			quantoSauros::InterestRateCurve m_referenceRateCurve2;
	};
	class NoteLegSpotRangeCouponInfo : public NoteLegRangeCouponInfo {
		public:
			NoteLegSpotRangeCouponInfo(
				quantoSauros::RateType rateType1, double tenor1, 
				QuantLib::Frequency swapCouponFrequency1,
				quantoSauros::InterestRateCurve referenceRateCurve1,				
				//std::vector<quantoSauros::Period> rangePeriods,
				std::vector<double> inCouponRates, std::vector<double> outCouponRates,
				std::vector<double> upperBounds, std::vector<double> lowerBounds){
					m_rateType1 = rateType1;
					m_tenor1 = tenor1;
					m_swapCouponFrequency1 = swapCouponFrequency1;
					m_referenceRateCurve1 = referenceRateCurve1;
					//m_rangePeriods = rangePeriods;
					m_inCouponRates = inCouponRates;
					m_outCouponRates = outCouponRates;
					m_upperBounds = upperBounds;
					m_lowerBounds = lowerBounds;
					m_className = "NoteLegSpotRangeCouponInfo";
			}

		private:

	};

#pragma region inline-functions of NoteLegRangeCouponInfo
	inline quantoSauros::RateType NoteLegRangeCouponInfo::getRateType1(){
		return m_rateType1;
	}
	inline double NoteLegRangeCouponInfo::getTenor1(){
		return m_tenor1;
	}
	inline QuantLib::Frequency NoteLegRangeCouponInfo::getSwapCouponFrequency1(){
		return m_swapCouponFrequency1;
	}
	inline quantoSauros::InterestRateCurve NoteLegRangeCouponInfo::getReferenceRateCurve1(){
		return m_referenceRateCurve1;
	}
	inline std::vector<double> NoteLegRangeCouponInfo::getInCouponRates(){
		return m_inCouponRates;
	}
	inline std::vector<double> NoteLegRangeCouponInfo::getOutCouponRates(){
		return m_outCouponRates;
	}
	inline std::vector<double> NoteLegRangeCouponInfo::getUpperBounds(){
		return m_upperBounds;
	}
	inline std::vector<double> NoteLegRangeCouponInfo::getLowerBounds(){
		return m_lowerBounds;
	}
#pragma endregion
#pragma region inline-functions of NoteLegSpreadRangeCouponInfo
	inline quantoSauros::RateType NoteLegSpreadRangeCouponInfo::getRateType2(){
		return m_rateType2;
	}
	inline double NoteLegSpreadRangeCouponInfo::getTenor2(){
		return m_tenor2;
	}
	inline QuantLib::Frequency NoteLegSpreadRangeCouponInfo::getSwapCouponFrequency2(){
		return m_swapCouponFrequency2;
	}
	inline quantoSauros::InterestRateCurve NoteLegSpreadRangeCouponInfo::getReferenceRateCurve2(){
		return m_referenceRateCurve2;
	}
#pragma endregion
	
	//TODO float Coupon Info

	//TODO fixed Coupon Info

#pragma endregion

#pragma region SwapLegCouponInfo

	class SwapLegCouponInfo : public AbstractLegCouponInfo {
		public:

	};

#pragma endregion

}
