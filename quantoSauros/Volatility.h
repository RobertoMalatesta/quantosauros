#pragma once
#include <ql/quantlib.hpp>
#include "Tenor.h";

namespace quantoSauros{
	class Volatility
	{
	public:
		Volatility();
		//TODO
		//Volatility(QuantLib::Time t, QuantLib::Volatility vol);
		Volatility(quantoSauros::Tenor tenor, QuantLib::Volatility vol);
		QuantLib::Volatility getVolatility();
		QuantLib::Time getTime();
		quantoSauros::Tenor getTenor();

		~Volatility(void);
	private:
		//QuantLib::Time m_time;
		QuantLib::Volatility m_vol;
		quantoSauros::Tenor m_tenor;
	};

}
