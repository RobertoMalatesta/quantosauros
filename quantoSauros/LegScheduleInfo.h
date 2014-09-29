#pragma once

#include "AbstractLegInfo.h"
#include "Period.h"
#include <ql/quantlib.hpp>

namespace quantoSauros{

#pragma region NoteLegScheduleInfo

	class NoteLegScheduleInfo : public AbstractLegScheduleInfo {
		public:
			//Constructor1
			NoteLegScheduleInfo(
				QuantLib::Currency currency,
				std::vector<quantoSauros::Period> periods,
				QuantLib::Frequency couponFrequency,
				QuantLib::DayCounter dayCounter
				){
					m_currency = currency;
					m_periods = periods;
					m_couponFrequency = couponFrequency;
					m_dayCounter = dayCounter;
			}
			//Constructor2
			NoteLegScheduleInfo(
				QuantLib::Currency currency,
				std::vector<quantoSauros::Period> periods,
				QuantLib::Frequency couponFrequency,
				QuantLib::DayCounter dayCounter,
				QuantLib::Date issueDate,
				QuantLib::Date maturityDate
				){
					m_currency = currency;
					m_periods = periods;
					m_couponFrequency = couponFrequency;
					m_dayCounter = dayCounter;
					m_issueDate = issueDate;
					m_maturityDate = maturityDate;
			}
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

#pragma endregion

#pragma region inline-functions of NoteLegScheduleInfo
	inline QuantLib::Currency NoteLegScheduleInfo::getCurrency(){
		return m_currency;
	}
	inline std::vector<quantoSauros::Period> NoteLegScheduleInfo::getPeriods(){
		return m_periods;
	}
	inline QuantLib::Frequency NoteLegScheduleInfo::getCouponFrequency(){
		return m_couponFrequency;
	}
	inline QuantLib::DayCounter NoteLegScheduleInfo::getDayCounter(){
		return m_dayCounter;
	}
	inline QuantLib::Date NoteLegScheduleInfo::getIssueDate(){
		return m_issueDate;
	}
	inline QuantLib::Date NoteLegScheduleInfo::getMaturityDate(){
		return m_maturityDate;;
	}
#pragma endregion

#pragma region SwapLegScheduleInfo
	class SwapLegScheduleInfo : public AbstractLegScheduleInfo {
		public:

	};

#pragma endregion
}
