#include "Tenor.h"

namespace quantoSauros{
	Tenor::Tenor(void)
	{
	}
	Tenor::Tenor(LPCSTR code)
	{
		USES_CONVERSION;
		CString cstr = A2CT(code);
		CString timeUnitStr = cstr.Left(1);
		CString integerStr = cstr.Right(cstr.GetLength() - 1);
		
		m_timeInteger = atoi(integerStr);

		//timeUnit 결정
		if (timeUnitStr == "D"){
			m_timeUnit = Days;
			m_Day = m_timeInteger;
		} else if (timeUnitStr == "M") {
			m_timeUnit = Months;
			m_Month = m_timeInteger;
		} else if (timeUnitStr == "Y") {
			m_timeUnit = Years;
			m_Year = m_timeInteger;
		}
		m_period = Period(m_timeInteger, m_timeUnit);
	}
	Tenor::Tenor(QuantLib::Period period){
		m_period = period;
	}
	TimeUnit Tenor::getTimeUnit()
	{
		return m_timeUnit;
	}
	Integer Tenor::getTimeInteger()
	{
		return m_timeInteger;
	}
	
	Time Tenor::getTenorInTime()
	{
		//TODO dcf 적용 : dcf에 해당하는 1년간 날짜수 어떻게 계산?
		TimeUnit unit = m_period.units();
		Time time;
		if (unit == Days){
			time = 1.0 / 365.0;
		} else if (unit == Weeks){
			time = 7.0 / 365.0;
		} else if (unit == Months){
			time = 30.0 / 365.0;
		} else if (unit == Years){
			time = 1.0;
		}
		return m_period.length() * time;
	}
	/*
	Time Tenor::getTenorInTime(DayCounter dcf)
	{
		//TODO dcf 적용 : dcf에 해당하는 1년간 날짜수 어떻게 계산?
		return m_Year + m_Month / 12.0 + m_Day / 365.0;
	}
	*/
	Period Tenor::getTenorInPeriod()
	{
		return m_period;
	}
	Integer Tenor::getYear()
	{
		return m_Year;
	}
	Integer Tenor::getMonth()
	{
		return m_Month;
	}
	Integer Tenor::getDay()
	{
		return m_Day;
	}
	Tenor::~Tenor(void)
	{
	}
}