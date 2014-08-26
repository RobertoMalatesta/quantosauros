#pragma once
#include <ql/quantlib.hpp>
#include <atlstr.h>

using namespace QuantLib;

namespace quantoSauros{
	class Tenor
	{
	public:
		Tenor(void);
		Tenor(LPCSTR code);				
		Tenor(QuantLib::Period period);
		Time getTenorInTime();
		//Time getTenorInTime(DayCounter dcf);
		Period getTenorInPeriod();

		TimeUnit getTimeUnit();
		Integer getTimeInteger();

		Integer getYear();
		Integer getMonth();
		Integer getDay();

		~Tenor(void);
	private:
		TimeUnit m_timeUnit;
		Integer m_timeInteger;
		Period m_period;

		Integer m_Year;
		Integer m_Month;
		Integer m_Day;
		DayCounter m_dcf;
		Real m_tenor;
	};
}
