#pragma once
#include <ql/quantlib.hpp>
#include "AbstractPeriod.h"
namespace quantoSauros {

	class Period : public AbstractPeriod {
	public:
		Period(QuantLib::Date startDate, QuantLib::Date endDate)
		{
			m_startDate = startDate;
			m_endDate = endDate;
		}		
		QuantLib::Time getPeriodTenor(QuantLib::DayCounter dcf){
			return dcf.yearFraction(m_startDate, m_endDate);
		}
	};
}
