#include "RangeAccrualNote.h"

namespace quantoSauros {
	RangeAccrualNote::RangeAccrualNote(QuantLib::Money notional,
		QuantLib::Date issueDate, QuantLib::Date maturityDate,
		QuantLib::DayCounter dcf, bool includePrincipal,
		//기준금리1 정보
		double floatCurveTenor1, QuantLib::Frequency swapCouponFrequency1,
		//Range 구간 정보
		std::vector<double> inCouponRates, std::vector<double> outCouponRates,
		std::vector<quantoSauros::Period> rangePeriods,
		std::vector<double> rangeUpperRates, std::vector<double> rangeLowerRates,
		//행사 정보
		std::vector<QuantLib::Date> exerciseDates, std::vector<double> exercisePrices,
		QuantLib::Option::Type optionType,
		//기타정보
		int monitorFrequency
		){
			m_notional = notional;
			m_issueDate = issueDate;
			m_maturityDate = maturityDate;
			m_dcf = dcf;
			m_includePrincipal = includePrincipal;
			m_floatCurveTenor1 = floatCurveTenor1;
			m_swapCouponFrequency = swapCouponFrequency1;
			m_inCouponRates = inCouponRates;
			m_outCouponRates = outCouponRates;
			m_rangePeriods = rangePeriods;
			m_rangeUpperRates = rangeUpperRates;
			m_rangeLowerRates = rangeLowerRates;
			m_exerciseDates = exerciseDates;
			m_exercisePrices = exercisePrices;
			m_optionType = optionType;
			m_monitorFrequency = monitorFrequency;
	}
	QuantLib::Money RangeAccrualNote::getPrice(QuantLib::Date today, 
		quantoSauros::InterestRateCurve floatCurve,
		quantoSauros::VolatilitySurface volatilitySurface,
		QuantLib::Real meanReversion, QuantLib::Real sigma, BigNatural seed){

			typedef PseudoRandom::rsg_type rsg_type;
			typedef PathGenerator<rsg_type>::sample_type sample_type;

			double t = m_dcf.yearFraction(today, m_maturityDate);

			Handle<YieldTermStructure> floatTermStructure(floatCurve.getInterestRateCurve());

			//dividing Range Periods
			int sizeOfRangePeriod = m_rangePeriods.size();
			int idx = 0;
			while(today - m_rangePeriods[idx].getEndDate() >= 0){
				if (idx == sizeOfRangePeriod - 1){
					break;
				}
				idx++;
			}
			m_rangePeriods.erase(m_rangePeriods.begin(), m_rangePeriods.begin() + idx);
			m_rangeLowerRates.erase(m_rangeLowerRates.begin(), m_rangeLowerRates.begin() + idx);
			m_rangeUpperRates.erase(m_rangeUpperRates.begin(), m_rangeUpperRates.begin() + idx);
			m_inCouponRates.erase(m_inCouponRates.begin(), m_inCouponRates.begin() + idx);
			m_outCouponRates.erase(m_outCouponRates.begin(), m_outCouponRates.begin() + idx);


			QuantLib::Size timeGridSize = 5;
			
			std::vector<BigNatural> seeds;
			Real x0 = 0;

			//Calculating module
			for (int periodIndex = 0; periodIndex < m_rangePeriods.size(); periodIndex++){
				seeds.push_back(BigNatural(rand()));
				quantoSauros::Period period = m_rangePeriods[periodIndex];
				QuantLib::Time startTenor = std::max(
					(double) m_dcf.yearFraction(today, period.getStartDate()), 0.0);
				QuantLib::Time periodTenor = period.getPeriodTenor(m_dcf);

				//1. process
				//TODO initial value 넣어야됨
				boost::shared_ptr<HullWhiteProcess> process(
					new HullWhiteProcess(floatTermStructure, meanReversion, sigma));
				
				//set the initialized short rate value into the process
				if (periodIndex != 0){
					process->setX0(x0);
				}
				//2. timeGrid
				//TODO timeGrid에 exercise Date 추가
				TimeGrid timeGrid(periodTenor, timeGridSize, startTenor);

				//3. path Generator
				rsg_type rsg = PseudoRandom::make_sequence_generator(timeGrid.size() - 1, 
					seeds[periodIndex]);
								
				PathGenerator<rsg_type> generator(process, timeGrid, rsg, false);
				sample_type path1 = generator.next();
				HullWhite hullWhite(floatTermStructure, meanReversion, sigma);

				int accruedDays = 0;
				for (Size j = 0; j < generator.size(); j++){
					//4. Calculate the reference Rate
					QuantLib::Time tenor = m_floatCurveTenor1;
					QuantLib::Time start = timeGrid[j] + startTenor;
					Real referenceRate = - log(hullWhite.discountBond(start, start + tenor, path1.value[j])) / tenor;
				
					double lowerRate = m_rangeLowerRates[periodIndex];
					double upperRate = m_rangeUpperRates[periodIndex];
					if (referenceRate >= lowerRate && referenceRate <= upperRate) {
						accruedDays++;
					}
				}
				//initialize the short rate value
				x0 = path1.value[generator.size() - 1];
			}
			//process->setX0(1);
			
		
			return m_notional;
	}

	RangeAccrualNote::~RangeAccrualNote(void){
	}
}