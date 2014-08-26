#pragma once
#include <ql/quantlib.hpp>
#include "AbstractPeriod.h"
namespace quantoSauros {

	class PaymentPeriod : public AbstractPeriod {
	public:
		PaymentPeriod(QuantLib::Date startDate, QuantLib::Date endDate, QuantLib::Date paymentDate)
		{
			m_startDate = startDate;
			m_endDate = endDate;
			m_paymentDate = paymentDate;
		}
	private:
		QuantLib::Date m_paymentDate;
	};
}
