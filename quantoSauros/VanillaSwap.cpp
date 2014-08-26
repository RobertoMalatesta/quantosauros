#include "vanillaSwap.h"

namespace quantoSauros {

	vanillaSwap::vanillaSwap(
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
		)
	{
		m_calendar = iborIndex->fixingCalendar();

		Schedule fixedSchedule(swapStartDate, swapEndDate, Period(swapFixedFrequency),
			m_calendar, swapFixedConvention, swapFixedConvention, DateGeneration::Forward, false);

		Schedule floatSchedule(swapStartDate, swapEndDate, Period(swapFloatingFrequency),
			m_calendar, swapFloatingConvention, swapFloatingConvention, DateGeneration::Forward, false);

		m_swap = new VanillaSwap(type, nominal, fixedSchedule, fixedRate, fixedDayCounter,
			floatSchedule, iborIndex, spread, iborIndex->dayCounter());

	}
	void vanillaSwap::setEngine(boost::shared_ptr<PricingEngine> engine){
		m_swap->setPricingEngine(engine);
	}
	Real vanillaSwap::getFixedLegNPV(){
		return m_swap->fixedLegNPV();
	}
	Real vanillaSwap::getFloatingLegNPV(){		
		return m_swap->floatingLegNPV();
	}
	Real vanillaSwap::getNPV(){		
		return m_swap->NPV();
	}
	Real vanillaSwap::getFairRate(boost::shared_ptr<YieldTermStructure> termStructure){
		RelinkableHandle<YieldTermStructure> discountingTermStructure;
		boost::shared_ptr<PricingEngine> swapEngine(
							 new DiscountingSwapEngine(discountingTermStructure));
		m_swap->setPricingEngine(swapEngine);
		discountingTermStructure.linkTo(termStructure);
		return m_swap->fairRate();
	}
	vanillaSwap::~vanillaSwap(void)
	{
	}
}