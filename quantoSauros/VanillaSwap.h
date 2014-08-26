#pragma once
#include <ql/quantlib.hpp>
using namespace QuantLib;

namespace quantoSauros {

	class vanillaSwap
	{
	public:
		vanillaSwap(
			//for common
			VanillaSwap::Type type, Real nominal,			
			Date swapStartDate, Date swapEndDate,
			//for Fixed Leg
			//fixed Schedule
			Frequency swapFixedFrequency, BusinessDayConvention swapFixedConvention, 
			Real fixedRate, DayCounter fixedDayCounter,
			//for Floating Leg
			//floating Schedule
			Frequency swapFloatingFrequency, BusinessDayConvention swapFloatingConvention,
			boost::shared_ptr<IborIndex> iborIndex, Real spread
			);

		void setEngine(boost::shared_ptr<PricingEngine> engine);
		Real getFixedLegNPV();
		Real getFloatingLegNPV();
		Real getNPV();
		Real getFairRate(boost::shared_ptr<YieldTermStructure> termStructure);
		~vanillaSwap(void);
	private:
		Calendar m_calendar;
		VanillaSwap *m_swap;
	};
}