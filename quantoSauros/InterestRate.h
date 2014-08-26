#pragma once
#include <ql/quantlib.hpp>
#include <math.h>
#include "Tenor.h";

namespace quantoSauros{

	enum RateType 
	{
		SwapRate, DepositRate, None,
	};
	class InterestRate
	{
	public:
		InterestRate(void);
		InterestRate(Tenor tenor, QuantLib::Real rate);
		InterestRate(Tenor tenor, QuantLib::Real rate, quantoSauros::RateType rateType);
		QuantLib::Time getTime();
		QuantLib::Period getPeriod();
		QuantLib::Real getRate();
		quantoSauros::RateType getRateType();

		~InterestRate(void);

	private:
		QuantLib::Real m_rate;
		QuantLib::Time m_time;
		QuantLib::Period m_period;
		quantoSauros::RateType m_rateType;
	};
}

