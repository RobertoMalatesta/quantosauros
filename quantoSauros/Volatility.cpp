#include "Volatility.h"

namespace quantoSauros {

	Volatility::Volatility(){
	}
	Volatility::Volatility(quantoSauros::Tenor tenor, QuantLib::Volatility vol)
	{
		m_tenor = tenor;
		m_vol = vol;
	}
	/*
	Volatility::Volatility(QuantLib::Time t, QuantLib::Volatility vol)
	{
		m_time = t;
		m_vol = vol;
	}
	*/
	QuantLib::Volatility Volatility::getVolatility()
	{
		return m_vol;
	}
	quantoSauros::Tenor Volatility::getTenor()
	{
		return m_tenor;
	}

	QuantLib::Time Volatility::getTime()
	{		
		return m_tenor.getTenorInTime();
	}
	
	Volatility::~Volatility(void)
	{
	}
}