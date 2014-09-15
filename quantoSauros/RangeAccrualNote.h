#pragma once
#include <ql/quantlib.hpp>
#include <algorithm> 
#include "Period.h";
#include "AbstractRangeAccrualNote.h";
namespace quantoSauros {

	class RangeAccrualNote : public AbstractRangeAccrualNote {
	public:
		RangeAccrualNote(QuantLib::Money notional, 
			QuantLib::Date issueDate, QuantLib::Date maturityDate, 
			QuantLib::DayCounter dcf, bool includePrincipal,
			double floatCurveTenor1, QuantLib::Frequency swapCouponFrequency1,
			std::vector<double> inCouponRates, std::vector<double> outCouponRates,
			std::vector<quantoSauros::Period> rangePeriods,
			std::vector<double> rangeUpperRates, std::vector<double> rangeLowerRates,			
			QuantLib::Option::Type optionType,
			int monitorFrequency
			);

		QuantLib::Money getPrice(QuantLib::Date today, 
			quantoSauros::InterestRateCurve floatCurve,
			quantoSauros::VolatilitySurface volatilitySurface,
			QuantLib::Real meanReversion, QuantLib::Real sigma,
			quantoSauros::InterestRateCurve discountCurve,
			quantoSauros::VolatilitySurface discountVolatilitySurface,
			QuantLib::Real discountMeanReversion, QuantLib::Real discountSigma,
			int simulationNum);

		~RangeAccrualNote(void);
	};
}

