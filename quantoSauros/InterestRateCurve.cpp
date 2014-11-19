#include "InterestRateCurve.h"

namespace quantoSauros{

	InterestRateCurve::InterestRateCurve(
			Date asOfDate, std::vector<quantoSauros::InterestRate> interestRates,
			Calendar calendar, Natural settlementDays,
			DayCounter daycounter, Compounding compound){
		this->m_today = asOfDate;
		this->m_calendar = calendar;
		this->m_settlementDate = calendar.advance(m_today, settlementDays, Days);
		this->m_dcf = daycounter;
		this->m_compoundingMethod = compound;	

		std::vector<quantoSauros::InterestRate> swapRates;
		std::vector<quantoSauros::InterestRate> depositRates;

		for (int i = 0; i < interestRates.size(); i++){
			quantoSauros::RateType type = interestRates[i].getRateType();
			if (type == quantoSauros::RateType::DepositRate){
				depositRates.push_back(interestRates[i]);
			} else if (type == quantoSauros::RateType::SwapRate){
				swapRates.push_back(interestRates[i]);
			}
		}

		m_depositeSize = depositRates.size();
		m_swapSize = swapRates.size();
	
		//deposit 정보
		DayCounter depositDcf = Actual365Fixed();
		BusinessDayConvention depositConvention = ModifiedFollowing;
		bool endOfMonthForDeposit = true;	

		////측정일
		//Date* termDate;
		//termDate = new Date[m_depositeSize + m_swapSize];

		std::vector<boost::shared_ptr<RateHelper>> tmpInstrument(m_depositeSize + m_swapSize);
		for (Size i = 0; i < m_depositeSize; i++) {
			tmpInstrument[i] = boost::shared_ptr<RateHelper>(
				new DepositRateHelper(depositRates[i].getRate(),
					depositRates[i].getPeriod().length() * depositRates[i].getPeriod().units(),
					settlementDays, calendar,
					depositConvention, endOfMonthForDeposit,
					depositDcf));
			//termDate[i] = tmpInstrument[i]->latestDate();
		}
	
		//swap 정보
		Frequency frequency = Annual;
		BusinessDayConvention swapConvention = Unadjusted;
		DayCounter swapDcf = Actual365Fixed();
		//IborIndex id(KRWibor3M());

		boost::shared_ptr<IborIndex> index(new KRWibor3M());
	
		for (Size i = 0; i < m_swapSize; ++i) {
			tmpInstrument[i + m_depositeSize] = boost::shared_ptr<RateHelper>(
				new SwapRateHelper(swapRates[i].getRate(),
					swapRates[i].getPeriod().length() * swapRates[i].getPeriod().units(),
					calendar, frequency, swapConvention, swapDcf, index));
			//termDate[i + m_depositeSize] = tmpInstrument[i + m_depositeSize]->latestDate();
		}

		m_instruments = tmpInstrument;
		double tolerance = 1.0e-15;
		m_termStructure = boost::shared_ptr<YieldTermStructure>(
			new PiecewiseYieldCurve<Discount,LogLinear>(asOfDate, m_instruments, m_dcf, tolerance));

		//double tmp = m_termStructure->discount(5);

	}
	#pragma region calculate Methods
	double InterestRateCurve::getDiscountFactor(Time t){
		return m_termStructure->discount(t, false);
	}
	double InterestRateCurve::getDiscountFactor(Date date){
		return m_termStructure->discount(date, false);
	}
	double InterestRateCurve::getForwardRate(Time t1, Time t2){
		return m_termStructure->forwardRate(t1, t2, this->m_compoundingMethod);
	}
	double InterestRateCurve::getForwardRate(Time t1, Time t2, Compounding compound){
		return m_termStructure->forwardRate(t1, t2, compound);
	}
	double InterestRateCurve::getForwardRate(Date d1, Date d2){
		return m_termStructure->forwardRate(d1, d2, this->m_dcf, this->m_compoundingMethod);
	}
	double InterestRateCurve::getForwardRate(Date d1, Date d2, DayCounter dcf){
		return m_termStructure->forwardRate(d1, d2, dcf, this->m_compoundingMethod);
	}
	double InterestRateCurve::getForwardRate(Date d1, Date d2, DayCounter dcf, Compounding compound){
		return m_termStructure->forwardRate(d1, d2, dcf, compound);
	}
	double InterestRateCurve::getZeroRate(Time t){
		return m_termStructure->zeroRate(t, this->m_compoundingMethod);
	}
	double InterestRateCurve::getZeroRate(Time t, Compounding compound){
		return m_termStructure->zeroRate(t, compound);
	}
	double InterestRateCurve::getZeroRate(Date d){
		return m_termStructure->zeroRate(d, this->m_dcf, this->m_compoundingMethod); 
	}
	double InterestRateCurve::getZeroRate(Date d, DayCounter dcf){
		return m_termStructure->zeroRate(d, dcf, this->m_compoundingMethod); 
	}
	double InterestRateCurve::getZeroRate(Date d, DayCounter dcf, Compounding compound){
		return m_termStructure->zeroRate(d, dcf, compound); 
	}
	double InterestRateCurve::getForwardSwapRate(Period maturity, Period tenor){

		Frequency swapFixedLegFrequency = Quarterly;
		BusinessDayConvention swapFixedLegConvention = ModifiedFollowing;
		Frequency swapFloatingLegFrequency = Quarterly;
		BusinessDayConvention swapFloatingLegConvention = ModifiedFollowing;

		Date swapStartDate = m_calendar.advance(m_settlementDate, maturity, swapFloatingLegConvention);
		Date swapEndDate = m_calendar.advance(m_settlementDate, tenor, swapFloatingLegConvention);	
		VanillaSwap::Type type = VanillaSwap::Payer;
		Real nominal = 1;
		Real fixedRate = 0.03;
		DayCounter fixedDayCount = Actual365Fixed();
		RelinkableHandle<YieldTermStructure> discountingTermStructure;	
		boost::shared_ptr<IborIndex> iborIndex(new KRWibor3M(discountingTermStructure));
		Real spread = 0;

		quantoSauros::vanillaSwap tmpSwap = quantoSauros::vanillaSwap(type, 
			nominal, swapStartDate, swapEndDate, swapFixedLegFrequency, swapFixedLegConvention,
			fixedRate, fixedDayCount, swapFloatingLegFrequency, swapFloatingLegConvention, iborIndex, spread);
		discountingTermStructure.linkTo(this->m_termStructure);

		Real fairRate = tmpSwap.getFairRate(this->m_termStructure);

		return fairRate;
	}
	double InterestRateCurve::getForwardSwapRate(QuantLib::Date swapStartDate, QuantLib::Date swapEndDate){
		Frequency swapFixedLegFrequency = Quarterly;
		BusinessDayConvention swapFixedLegConvention = ModifiedFollowing;
		Frequency swapFloatingLegFrequency = Quarterly;
		BusinessDayConvention swapFloatingLegConvention = ModifiedFollowing;

		VanillaSwap::Type type = VanillaSwap::Payer;
		Real nominal = 1;
		Real fixedRate = 0.03;
		DayCounter fixedDayCount = Actual365Fixed();
		RelinkableHandle<YieldTermStructure> discountingTermStructure;	
		boost::shared_ptr<IborIndex> iborIndex(new KRWibor3M(discountingTermStructure));
		Real spread = 0;

		quantoSauros::vanillaSwap tmpSwap = quantoSauros::vanillaSwap(type, 
			nominal, swapStartDate, swapEndDate, swapFixedLegFrequency, swapFixedLegConvention,
			fixedRate, fixedDayCount, swapFloatingLegFrequency, swapFloatingLegConvention, iborIndex, spread);
		discountingTermStructure.linkTo(this->m_termStructure);

		Real fairRate = tmpSwap.getFairRate(this->m_termStructure);

		return fairRate;
	}
	#pragma endregion

	boost::shared_ptr<YieldTermStructure> InterestRateCurve::getInterestRateCurve(){
		return m_termStructure;
	}

	InterestRateCurve::~InterestRateCurve(void){

	}
}
/*

	Frequency swapFixedLegFrequency = Quarterly;
	BusinessDayConvention swapFixedLegConvention = ModifiedFollowing;
	Frequency swapFloatingLegFrequency = Quarterly;
	BusinessDayConvention swapFloatingLegConvention = ModifiedFollowing;

	Date swapStartDate = m_calendar.advance(m_settlementDate, maturity, swapFloatingLegConvention);
	Date swapEndDate = m_calendar.advance(m_settlementDate, tenor, swapFloatingLegConvention);
	
	VanillaSwap::Type type = VanillaSwap::Payer;
	Real nominal = 1;
	//Fixed Leg
	//fixedSchedule	
	//Schedule fixedSchedule(swapStartDate, swapEndDate, Period(swapFixedLegFrequency),
	//	m_calendar, swapFixedLegConvention, swapFixedLegConvention, DateGeneration::Forward, false);

	Real fixedRate = 0.03;
	DayCounter fixedDayCount = Actual365Fixed();

	//Floating Leg
	//floatingSchedule
	//Schedule floatSchedule(swapStartDate, swapEndDate, Period(swapFloatingLegFrequency),
	//	m_calendar, swapFloatingLegConvention, swapFloatingLegConvention, DateGeneration::Forward, false);

	RelinkableHandle<YieldTermStructure> discountingTermStructure;	
	boost::shared_ptr<IborIndex> iborIndex(new KRWibor3M(discountingTermStructure));
	Real spread = 0;
	//DayCounter floatingDayCount = Actual365Fixed();
	
	VanillaSwap swap(type, nominal, fixedSchedule, fixedRate, fixedDayCount,
		floatSchedule, iborIndex, spread, floatingDayCount);
	
	
	quantoSauros::vanillaSwap tmpSwap = quantoSauros::vanillaSwap(type, 
		nominal, swapStartDate, swapEndDate, swapFixedLegFrequency, swapFixedLegConvention,
		fixedRate, fixedDayCount, swapFloatingLegFrequency, swapFloatingLegConvention, iborIndex, spread);
	discountingTermStructure.linkTo(this->m_termStructure);

	Real fairRate = tmpSwap.getFairRate(this->m_termStructure);

	
	boost::shared_ptr<PricingEngine> swapEngine(
							 new DiscountingSwapEngine(discountingTermStructure));
			
	swap.setPricingEngine(swapEngine);
	discountingTermStructure.linkTo(this->m_termStructure);	

	
	return fairRate;
*/