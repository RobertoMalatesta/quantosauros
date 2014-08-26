#pragma once
#include <ql/quantlib.hpp>
namespace quantoSauros {

	class AbstractPeriod
	{
	public:
		AbstractPeriod() {}	
		QuantLib::Date getStartDate();
		QuantLib::Date getEndDate();

	protected:
		QuantLib::Date m_startDate;
		QuantLib::Date m_endDate;
	};

	inline QuantLib::Date AbstractPeriod::getStartDate() {
		return m_startDate;
	}
	inline QuantLib::Date AbstractPeriod::getEndDate() {
		return m_endDate;
	}
}
