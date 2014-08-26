#pragma once
#include <ql/quantlib.hpp>
#include "Volatility.h";
#include "Tenor.h";
#include "util.h";

namespace quantoSauros {

	class VolatilityCurve
	{
	public:
		VolatilityCurve(QuantLib::Date today, std::vector<quantoSauros::Volatility> &vols, 
			QuantLib::DayCounter dcf, QuantLib::Period period);		
		VolatilityCurve(QuantLib::Date today, std::vector<quantoSauros::Volatility> &vols, 
			QuantLib::DayCounter dcf, quantoSauros::Tenor tenor);

		QuantLib::Date getDate();
		QuantLib::Volatility getVol(QuantLib::Date date);
		QuantLib::Volatility getVol(QuantLib::Time t);
		quantoSauros::Volatility getVol(int index);
		int Length();
		std::vector<quantoSauros::Volatility> getVolatilityCurve();
		QuantLib::DayCounter getDayCounter();
		QuantLib::Period getPeriod();
		quantoSauros::Tenor getTenor();

		~VolatilityCurve(void);
	private:
		QuantLib::Date m_today;
		QuantLib::DayCounter m_dcf;
		std::vector<quantoSauros::Volatility> m_vols;		
		QuantLib::Period m_period;
		quantoSauros::Tenor m_tenor;
	};
}
