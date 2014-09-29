#pragma once
#include <ql/quantlib.hpp>
#include "util.h";
#include "Volatility.h";
#include "VolatilityCurve.h";
using namespace QuantLib;

namespace quantoSauros {
	class VolatilitySurface
	{
	public:
		quantoSauros::VolatilitySurface(){}
		quantoSauros::VolatilitySurface(Date today, 
			std::vector<quantoSauros::VolatilityCurve> &volCurves, DayCounter dcf);
		Date getDate();
		QuantLib::Volatility getVol(Time maturity, Time tenor);
		int Length();
		quantoSauros::VolatilityCurve getVolatilityCurve(int index);
		std::vector<quantoSauros::VolatilityCurve> getVolatilitySurface();

		~VolatilitySurface(void);
	private:
		Date m_today;
		DayCounter m_dcf;
		std::vector<quantoSauros::VolatilityCurve> m_volCurves;
	};
}
