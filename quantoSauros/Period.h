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
			m_hasExercise = false;
		}		
		Period(QuantLib::Date startDate, QuantLib::Date endDate, 
			QuantLib::Date exerciseDate, double exercisePrice)
		{
			m_startDate = startDate;
			m_endDate = endDate;
			m_exerciseDate = exerciseDate;
			m_exercisePrice = exercisePrice;
			m_hasExercise = true;
		}		
		QuantLib::Date getExerciseDate();
		bool hasExercise();
		double getExercisePrice();
		QuantLib::Time getPeriodTenor(QuantLib::DayCounter dcf){
			return dcf.yearFraction(m_startDate, m_endDate);
		};
	protected:
		QuantLib::Date m_exerciseDate;
		bool m_hasExercise;
		double m_exercisePrice;
	};	
	inline QuantLib::Date Period::getExerciseDate() {
		return m_exerciseDate;
	}
	inline double Period::getExercisePrice() {
		return m_exercisePrice;
	}
	inline bool Period::hasExercise() {
		return m_hasExercise;
	}

}
