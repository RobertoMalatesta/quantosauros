#include "InterestRate.h";

namespace quantoSauros{

	InterestRate::InterestRate(void)
	{
	}

	InterestRate::InterestRate(Tenor tenor, QuantLib::Real rate)
	{
		m_period = tenor.getTenorInPeriod();
		m_rate = rate;
		TimeUnit timeUnitStr = tenor.getTimeUnit();
		quantoSauros::RateType type;
		if (timeUnitStr == 0 ){	
			type = quantoSauros::DepositRate;
		} else if (timeUnitStr == 2) {				
			type = quantoSauros::DepositRate;
		} else if (timeUnitStr == 3) {				
			type = quantoSauros::SwapRate;
		}
		m_rateType = type;
	}
	InterestRate::InterestRate(Tenor tenor, QuantLib::Real rate, 
		quantoSauros::RateType rateType)
	{
		m_period = tenor.getTenorInPeriod();
		m_rate = rate;
		m_rateType = rateType;
	}
	QuantLib::Time InterestRate::getTime()
	{
		if (m_time != 0) {
			return m_time;	
		} else {
			QuantLib::Time time = 0;

			QuantLib::TimeUnit timeUnit = m_period.units();
			QuantLib::Integer timeInteger = m_period.length();

			if (timeUnit == QuantLib::Years){
				time = timeInteger * 1.0;
			} else if (timeUnit == QuantLib::Months){
				time = timeInteger * 1.0 / 12.0;
			} else {
				//TODO dcf Àû¿ë
				time = timeInteger * 1.0 / 365.0;
			}
			return time;
		}		
	}
	QuantLib::Period InterestRate::getPeriod()
	{
		if (m_period.length() != 0){
			return m_period;
		} else {
			return QuantLib::Period();
		}
	}
	QuantLib::Real InterestRate::getRate()
	{
		return m_rate;
	}
	quantoSauros::RateType InterestRate::getRateType()
	{
		return m_rateType;
	}
	InterestRate::~InterestRate(void)
	{
	}
}
