#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{

#pragma region NoteLegCouponInfo

	class NoteLegCouponInfo : public AbstractLegCouponInfo {
		public:
			NoteLegCouponInfo(){};
			NoteLegCouponInfo(quantoSauros::RateType rateType1, double tenor1, 
				QuantLib::Frequency swapCouponFrequency1,
				quantoSauros::InterestRateCurve referenceRateCurve1)
					: AbstractLegCouponInfo(rateType1, tenor1, 
						swapCouponFrequency1, referenceRateCurve1){

			};
			std::string getClassName(){
				return m_className;
			};
		protected:			
			std::string m_className;
	};

	//Range Coupon Info
	class NoteLegRangeCouponInfo : public NoteLegCouponInfo {
		public:
			NoteLegRangeCouponInfo(quantoSauros::RateType rateType1, double tenor1, 
				QuantLib::Frequency swapCouponFrequency1,
				quantoSauros::InterestRateCurve referenceRateCurve1,
				std::vector<QuantLib::Rate> inCouponRates, std::vector<QuantLib::Rate> outCouponRates,
				std::vector<QuantLib::Rate> upperBounds, std::vector<QuantLib::Rate> lowerBounds)
					: NoteLegCouponInfo(rateType1, tenor1, 
						swapCouponFrequency1, referenceRateCurve1) {

							m_inCouponRates = inCouponRates;
							m_outCouponRates = outCouponRates;
							m_upperBounds = upperBounds;
							m_lowerBounds = lowerBounds;
			};

			std::vector<QuantLib::Rate> getUpperBounds();
			std::vector<QuantLib::Rate> getLowerBounds();
			std::vector<QuantLib::Rate> getInCouponRates();
			std::vector<QuantLib::Rate> getOutCouponRates();			
		protected:	
			//기초자산금리 정보
			std::vector<quantoSauros::IRInfo> m_irInfos;

			//Range Accrual In-Coupon 이자율
			std::vector<QuantLib::Rate> m_inCouponRates;
			//Range Accrual Out-Coupon 이자율
			std::vector<QuantLib::Rate> m_outCouponRates;
			//기준자산1 Upper-Barrier
			std::vector<QuantLib::Rate> m_upperBounds;
			//기준자산1 Lower-Barrier
			std::vector<QuantLib::Rate> m_lowerBounds;
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
				std::vector<QuantLib::Rate> inCouponRates, std::vector<QuantLib::Rate> outCouponRates,
				std::vector<QuantLib::Rate> upperBounds, std::vector<QuantLib::Rate> lowerBounds)
					: NoteLegRangeCouponInfo(rateType1, tenor1, swapCouponFrequency1, referenceRateCurve1,
						inCouponRates, outCouponRates, upperBounds, lowerBounds) {					
												
					m_rateType2 = rateType2;
					m_tenor2 = tenor2;
					m_swapCouponFrequency2 = swapCouponFrequency2;
					m_referenceRateCurve2 = referenceRateCurve2;
					//m_rangePeriods = rangePeriods;					
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
				std::vector<QuantLib::Rate> inCouponRates, std::vector<QuantLib::Rate> outCouponRates,
				std::vector<QuantLib::Rate> upperBounds, std::vector<QuantLib::Rate> lowerBounds)
					: NoteLegRangeCouponInfo(rateType1, tenor1, 
						swapCouponFrequency1, referenceRateCurve1,
						inCouponRates, outCouponRates, upperBounds, lowerBounds) {

						//m_rangePeriods = rangePeriods;
						
						m_className = "NoteLegSpotRangeCouponInfo";
			}
	};

#pragma region inline-functions of NoteLegRangeCouponInfo
	
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
