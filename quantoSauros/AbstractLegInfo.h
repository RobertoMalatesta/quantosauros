#pragma once
#include "Period.h"
#include "InterestRate.h"
#include "InterestRateCurve.h"
#include "IRInfo.h"
#include <ql/quantlib.hpp>

namespace quantoSauros{

	class AbstractLegInfo {
		public:
			AbstractLegInfo(){}
	};

#pragma region AbstractLegCouponInfo

	class AbstractLegCouponInfo : public AbstractLegInfo {
		public:
			AbstractLegCouponInfo(){};
			AbstractLegCouponInfo(quantoSauros::RateType rateType1, double tenor1, 
				QuantLib::Frequency swapCouponFrequency1,
				quantoSauros::InterestRateCurve referenceRateCurve1){
					m_rateType1 = rateType1;
					m_tenor1 = tenor1;
					m_swapCouponFrequency1 = swapCouponFrequency1;
					m_referenceRateCurve1 = referenceRateCurve1;

			};

			quantoSauros::RateType getRateType1();
			double getTenor1();
			QuantLib::Frequency getSwapCouponFrequency1();
			quantoSauros::InterestRateCurve getReferenceRateCurve1();	

		protected:
			//기초자산1 유형
			quantoSauros::RateType m_rateType1;
			//기초자산1 테너
			double m_tenor1;
			//기초자산1 스왑쿠폰주기
			QuantLib::Frequency m_swapCouponFrequency1;
			//기초자산1 금리커브
			quantoSauros::InterestRateCurve m_referenceRateCurve1;
	};

	inline quantoSauros::RateType AbstractLegCouponInfo::getRateType1(){
		return m_rateType1;
	}
	inline double AbstractLegCouponInfo::getTenor1(){
		return m_tenor1;
	}
	inline QuantLib::Frequency AbstractLegCouponInfo::getSwapCouponFrequency1(){
		return m_swapCouponFrequency1;
	}
	inline quantoSauros::InterestRateCurve AbstractLegCouponInfo::getReferenceRateCurve1(){
		return m_referenceRateCurve1;
	}
#pragma endregion

#pragma region AbstractLegScheduleInfo

	class AbstractLegScheduleInfo : public AbstractLegInfo {
		public:
			AbstractLegScheduleInfo(){};
			AbstractLegScheduleInfo(QuantLib::Currency currency,
				std::vector<quantoSauros::Period> periods,
				QuantLib::Frequency couponFrequency,
				QuantLib::DayCounter dayCounter,
				QuantLib::Date issueDate,
				QuantLib::Date maturityDate){
					m_currency = currency;
					m_periods = periods;
					m_couponFrequency = couponFrequency;
					m_dayCounter = dayCounter;
					m_issueDate = issueDate;
					m_maturityDate = maturityDate;
			};
			QuantLib::Currency getCurrency();
			std::vector<quantoSauros::Period> getPeriods();
			QuantLib::Frequency getCouponFrequency();
			QuantLib::DayCounter getDayCounter();
			QuantLib::Date getIssueDate();
			QuantLib::Date getMaturityDate();
		protected:
			//통화
			QuantLib::Currency m_currency;
			//지급 일정
			std::vector<quantoSauros::Period> m_periods;
			//쿠폰 주기
			QuantLib::Frequency m_couponFrequency;
			//Day Count Convention
			QuantLib::DayCounter m_dayCounter;
			//Issue Date
			QuantLib::Date m_issueDate;
			//Maturity Date
			QuantLib::Date m_maturityDate;
	};


	inline QuantLib::Currency AbstractLegScheduleInfo::getCurrency(){
		return m_currency;
	}
	inline std::vector<quantoSauros::Period> AbstractLegScheduleInfo::getPeriods(){
		return m_periods;
	}
	inline QuantLib::Frequency AbstractLegScheduleInfo::getCouponFrequency(){
		return m_couponFrequency;
	}
	inline QuantLib::DayCounter AbstractLegScheduleInfo::getDayCounter(){
		return m_dayCounter;
	}
	inline QuantLib::Date AbstractLegScheduleInfo::getIssueDate(){
		return m_issueDate;
	}
	inline QuantLib::Date AbstractLegScheduleInfo::getMaturityDate(){
		return m_maturityDate;;
	}

#pragma endregion

#pragma region AbstractLegDataInfo

	class AbstractLegDataInfo : public AbstractLegInfo {
		public:
			AbstractLegDataInfo(){};
			AbstractLegDataInfo(int monitorFrequency){
				m_monitorFrequency = monitorFrequency;
			};
			int AbstractLegDataInfo::getMonitorFrequency();

		protected:
			int m_monitorFrequency;
	};
	
	inline int AbstractLegDataInfo::getMonitorFrequency(){
		return m_monitorFrequency;
	}

#pragma endregion

#pragma region AbstractLegOptionInfo
	class AbstractLegOptionInfo : public AbstractLegInfo {
		public:
			AbstractLegOptionInfo(){};
			AbstractLegOptionInfo(QuantLib::Option::Type optionType){
				m_optionType = optionType;
			};
			QuantLib::Option::Type getOptionType();
		protected:
			QuantLib::Option::Type m_optionType;
	};

	inline QuantLib::Option::Type AbstractLegOptionInfo::getOptionType(){
		return m_optionType;
	}

#pragma endregion
	
#pragma region AbstractLegAmortizationInfo

	class AbstractLegAmortizationInfo : public AbstractLegInfo {
		public:
			AbstractLegAmortizationInfo(){};
			AbstractLegAmortizationInfo(QuantLib::Money notional){
				m_notional = notional;
			};
			QuantLib::Money getNotional();
			
		protected:
			//Notional
			QuantLib::Money m_notional;
	};


	inline QuantLib::Money AbstractLegAmortizationInfo::getNotional(){
		return m_notional;
	}
#pragma endregion

}