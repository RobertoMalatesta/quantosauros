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
		InterestRate(Tenor tenor, QuantLib::Rate rate);
		InterestRate(Tenor tenor, QuantLib::Rate rate, quantoSauros::RateType rateType);
		QuantLib::Time getTime();
		QuantLib::Period getPeriod();
		QuantLib::Rate getRate();
		quantoSauros::RateType getRateType();

		~InterestRate(void);

	private:
		QuantLib::Rate m_rate;
		QuantLib::Time m_time;
		QuantLib::Period m_period;
		quantoSauros::RateType m_rateType;
	};
}

