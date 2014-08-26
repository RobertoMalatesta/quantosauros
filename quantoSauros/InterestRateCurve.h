#pragma once
#include <ql/quantlib.hpp>
#include "util.h";
#include "vanillaSwap.h";
#include "InterestRate.h"
using namespace QuantLib;

namespace quantoSauros{
	class InterestRateCurve
	{
	public:
		InterestRateCurve(
			Date asOfDate, std::vector<quantoSauros::InterestRate> interestRates,
			Calendar calendar, Natural settlementDays, DayCounter daycounter,
			Compounding compound
			);
	
		double getForwardRate(Time t1, Time t2);	
		double getForwardRate(Time t1, Time t2, Compounding compound);
		double getForwardRate(Date d1, Date d2);
		double getForwardRate(Date d1, Date d2, DayCounter dcf);	
		double getForwardRate(Date d1, Date d2, DayCounter dcf, Compounding compound);

		double getDiscountFactor(Time t);
		double getDiscountFactor(Date date);

		double getZeroRate(Time t);
		double getZeroRate(Time t, Compounding compound);
		double getZeroRate(Date d);
		double getZeroRate(Date d, DayCounter dcf);
		double getZeroRate(Date d, DayCounter dcf, Compounding compound);
	
		double getForwardSwapRate(Period maturity, Period tenor);

		boost::shared_ptr<YieldTermStructure> getInterestRateCurve();

		~InterestRateCurve(void);

	private:
		Date m_today;
		Date m_settlementDate;
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
3. (�Էº�������) deposit ����, swap ����
4. Interpolation ��� �߰�

[DONE]
1. (�޼ҵ��߰�) ForwardSwapRate ���
2. (�Էº�������) RateData Struct

*/