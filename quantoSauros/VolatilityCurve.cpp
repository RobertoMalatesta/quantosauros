#include "VolatilityCurve.h"

namespace quantoSauros {
	VolatilityCurve::VolatilityCurve(Date today, std::vector<quantoSauros::Volatility> &vols,
		DayCounter dcf, Period period)
	{
		m_dcf = dcf;
		m_today = today;
		m_vols = vols;
		m_period = period;
	}	
	VolatilityCurve::VolatilityCurve(QuantLib::Date today, std::vector<quantoSauros::Volatility> &vols, 
			QuantLib::DayCounter dcf, quantoSauros::Tenor tenor){
		m_dcf = dcf;
		m_today = today;
		m_vols = vols;
		m_tenor = tenor;
		m_period = tenor.getTenorInPeriod();
	}
	Date VolatilityCurve::getDate()
	{
		return m_today;
	}
	QuantLib::Volatility VolatilityCurve::getVol(Date date)
	{
		double dt = m_dcf.yearFraction(m_today, date);

		return VolatilityCurve::getVol(dt);
	}
	QuantLib::Volatility VolatilityCurve::getVol(Time t)
	{
		int size = m_vols.size();

		for (int i = 1; i < size; ++i) {			
			double x1 = m_vols[i-1].getTenor().getTenorInTime();
			QuantLib::Volatility y1 = m_vols[i-1].getVolatility();
			double x2 = m_vols[i].getTenor().getTenorInTime();
			QuantLib::Volatility y2 = m_vols[i].getVolatility();

			if (t == x2) return QuantLib::Volatility(m_vols[i].getTenor().getTenorInPeriod().length());
			if (t > x1 && t < x2) {
				QuantLib::Volatility y = (y2 - y1) / (x2 - x1) * (t - x1) + y1;
				return y;
			}
		}
	}
	Volatility VolatilityCurve::getVol(int index){
		return m_vols[index];
	}
	Period VolatilityCurve::getPeriod(){
		return m_period;
	}
	quantoSauros::Tenor VolatilityCurve::getTenor(){
		return m_tenor;
	}
	int VolatilityCurve::Length()
	{
		return m_vols.size();
	}
	std::vector<quantoSauros::Volatility> VolatilityCurve::getVolatilityCurve()
	{
		return m_vols;
	}
	DayCounter VolatilityCurve::getDayCounter()
	{
		return m_dcf;
	}

	VolatilityCurve::~VolatilityCurve(void)
	{
	}
}