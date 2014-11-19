#pragma once
#include <ql/quantlib.hpp>
#include "util.h";
#include "vanillaSwap.h";
#include "InterestRate.h"
//using namespace QuantLib;

namespace quantoSauros{
	class InterestRateCurve
	{
	public:
		InterestRateCurve(){
		};
		InterestRateCurve(
			QuantLib::Date asOfDate, std::vector<quantoSauros::InterestRate> interestRates,
			Calendar calendar, Natural settlementDays, DayCounter daycounter,
			Compounding compound
			);
	
		double getForwardRate(Time t1, Time t2);	
		double getForwardRate(Time t1, Time t2, Compounding compound);
		double getForwardRate(QuantLib::Date d1, QuantLib::Date d2);
		double getForwardRate(QuantLib::Date d1, QuantLib::Date d2, QuantLib::DayCounter dcf);	
		double getForwardRate(QuantLib::Date d1, QuantLib::Date d2, QuantLib::DayCounter dcf, Compounding compound);

		double getDiscountFactor(Time t);
		double getDiscountFactor(QuantLib::Date date);

		double getZeroRate(Time t);
		double getZeroRate(Time t, Compounding compound);
		double getZeroRate(QuantLib::Date d);
		double getZeroRate(QuantLib::Date d, DayCounter dcf);
		double getZeroRate(QuantLib::Date d, DayCounter dcf, Compounding compound);
	
		double getForwardSwapRate(Period maturity, Period tenor);
		double getForwardSwapRate(QuantLib::Date swapStartDate, QuantLib::Date swapEndDate);

		boost::shared_ptr<YieldTermStructure> getInterestRateCurve();

		~InterestRateCurve(void);

	private:
		QuantLib::Date m_today;
		QuantLib::Date m_settlementDate;
		Calendar m_calendar;

		DayCounter m_dcf;
		Frequency m_frequency;
		Compounding m_compoundingMethod;
	
		Size m_depositeSize;
		Size m_swapSize;

		std::vector<boost::shared_ptr<RateHelper>> m_instruments;
		boost::shared_ptr<YieldTermStructure> m_termStructure;
	};
}
/*
20140414
[TODO] 
3. (입력변수수정) deposit 정보, swap 정보
4. Interpolation 방법 추가

[DONE]
1. (메소드추가) ForwardSwapRate 계산
2. (입력변수수정) RateData Struct

*/