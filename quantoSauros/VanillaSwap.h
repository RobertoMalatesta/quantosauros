#pragma once
#include <ql/quantlib.hpp>
//using namespace QuantLib;

namespace quantoSauros {

	class vanillaSwap
	{
	public:
		quantoSauros::vanillaSwap(
			//for common
			QuantLib::VanillaSwap::Type type, QuantLib::Real nominal,			
			QuantLib::Date swapStartDate, QuantLib::Date swapEndDate,
			//for Fixed Leg
			//fixed Schedule
			QuantLib::Frequency swapFixedFrequency, 
			QuantLib::BusinessDayConvention swapFixedConvention, 
			QuantLib::Real fixedRate, QuantLib::DayCounter fixedDayCounter,
			//for Floating Leg
			//floating Schedule
			QuantLib::Frequency swapFloatingFrequency, 
			QuantLib::BusinessDayConvention swapFloatingConvention,
			boost::shared_ptr<QuantLib::IborIndex> iborIndex, QuantLib::Real spread
			);

		void setEngine(boost::shared_ptr<QuantLib::PricingEngine> engine);
		QuantLib::Real getFixedLegNPV();
		QuantLib::Real getFloatingLegNPV();
		QuantLib::Real getNPV();
		QuantLib::Real getFairRate(boost::shared_ptr<QuantLib::YieldTermStructure> termStructure);
		~vanillaSwap(void);
	private:
		QuantLib::Calendar m_calendar;
		QuantLib::VanillaSwap *m_swap;
	};
}