#include "vanillaSwap.h"

namespace quantoSauros {

	vanillaSwap::vanillaSwap(
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
		)
	{
		m_calendar = iborIndex->fixingCalendar();

		QuantLib::Schedule fixedSchedule(swapStartDate, swapEndDate,
			QuantLib::Period(swapFixedFrequency),
			m_calendar, swapFixedConvention, swapFixedConvention,
			QuantLib::DateGeneration::Forward, false);

		QuantLib::Schedule floatSchedule(swapStartDate, swapEndDate, 
			QuantLib::Period(swapFloatingFrequency),
			m_calendar, swapFloatingConvention, swapFloatingConvention, 
			QuantLib::DateGeneration::Forward, false);

		m_swap = new QuantLib::VanillaSwap(
			type, nominal, fixedSchedule, fixedRate, fixedDayCounter,
			floatSchedule, iborIndex, spread, iborIndex->dayCounter()
			);

	}
	void vanillaSwap::setEngine(boost::shared_ptr<QuantLib::PricingEngine> engine){
		m_swap->setPricingEngine(engine);
	}
	QuantLib::Real vanillaSwap::getFixedLegNPV(){
		return m_swap->fixedLegNPV();
	}
	QuantLib::Real vanillaSwap::getFloatingLegNPV(){		
		return m_swap->floatingLegNPV();
	}
	QuantLib::Real vanillaSwap::getNPV(){		
		return m_swap->NPV();
	}
	QuantLib::Real vanillaSwap::getFairRate(
		boost::shared_ptr<QuantLib::YieldTermStructure> termStructure){

		QuantLib::Handle<QuantLib::YieldTermStructure> discountingTermStructure(termStructure);
		boost::shared_ptr<QuantLib::PricingEngine> swapEngine(
							 new QuantLib::DiscountingSwapEngine(discountingTermStructure));
		m_swap->setPricingEngine(swapEngine);
		//discountingTermStructure.linkTo(termStructure);
		return m_swap->fairRate();
	}
	vanillaSwap::~vanillaSwap(void)
	{
	}
}