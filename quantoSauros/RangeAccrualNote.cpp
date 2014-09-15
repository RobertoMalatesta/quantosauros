#include "RangeAccrualNote.h"

namespace quantoSauros {
	RangeAccrualNote::RangeAccrualNote(QuantLib::Money notional,
		//��¥����
		QuantLib::Date issueDate, QuantLib::Date maturityDate,
		//��Ÿ����
		QuantLib::DayCounter dcf, bool includePrincipal,
		//���رݸ�1 ����
		double floatCurveTenor1, QuantLib::Frequency swapCouponFrequency1,
		//Range ���� ����
		std::vector<double> inCouponRates, std::vector<double> outCouponRates,
		std::vector<quantoSauros::Period> rangePeriods,
		std::vector<double> rangeUpperRates, std::vector<double> rangeLowerRates,		
		//�������
		QuantLib::Option::Type optionType,
		//��Ÿ����
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
			
			m_optionType = optionType;
			m_monitorFrequency = monitorFrequency;
	}
	QuantLib::Money RangeAccrualNote::getPrice(
		//��¥����
		QuantLib::Date today, 
		//���رݸ� ����
		quantoSauros::InterestRateCurve floatCurve,
		quantoSauros::VolatilitySurface volatilitySurface,
		QuantLib::Real meanReversion, QuantLib::Real sigma, 
		//���αݸ� ����
		quantoSauros::InterestRateCurve discountCurve,
		quantoSauros::VolatilitySurface discountVolatilitySurface,
		QuantLib::Real discountMeanReversion, QuantLib::Real discountSigma,
		//��Ÿ
		int simulationNum){

			typedef PseudoRandom::rsg_type rsg_type;
			//typedef PathGenerator<rsg_type>::sample_type sample_type;
			typedef MultiPathGenerator<rsg_type>::sample_type sample_type;

			double t = m_dcf.yearFraction(today, m_maturityDate);

			Handle<YieldTermStructure> floatTermStructure(floatCurve.getInterestRateCurve());
			Handle<YieldTermStructure> discountTermStructure(discountCurve.getInterestRateCurve());

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
			
			QuantLib::Size timeGridSize = 10;			
			int periodLength = m_rangePeriods.size();
			
			srand(time(NULL));
			//Range �� seed ����
			if (m_seeds.size() == 0){
				for (int i = 0; i < simulationNum; i++){
					std::vector<BigNatural> seed;
					for (int j = 0; j < periodLength; j++){
						seed.push_back(BigNatural(rand()));
					}
					m_seeds.push_back(seed);
				}
			}
			//initial ���� ����
			Real x0ForReference = 0;
			Real x0ForDiscount = 0;
			//payoff ���� �Լ� ����
			std::vector<std::vector<double>> payoffs(periodLength + 1);
			std::vector<std::vector<double>> coupons(simulationNum);
			std::vector<std::vector<double>> dfs(simulationNum);

			//Calculating module
			for (int simIndex = 0; simIndex < simulationNum; simIndex++){
				//data ����
				std::vector<double> coupon(periodLength);
				std::vector<double> df(periodLength);

				for (int periodIndex = 0; periodIndex < periodLength; periodIndex++){				
					quantoSauros::Period period = m_rangePeriods[periodIndex];
					QuantLib::Time startTenor = std::max(
						(double) m_dcf.yearFraction(today, period.getStartDate()), 0.0);
					QuantLib::Time periodTenor = period.getPeriodTenor(m_dcf);

					//1. process				
					std::vector<boost::shared_ptr<StochasticProcess1D>> processes(2);
					boost::shared_ptr<StochasticProcess> process;
					boost::shared_ptr<HullWhiteProcess> referenceProcess(
						new HullWhiteProcess(floatTermStructure, meanReversion, sigma));
					boost::shared_ptr<HullWhiteProcess> discountProcess(
						new HullWhiteProcess(discountTermStructure, discountMeanReversion, discountSigma));
					processes[0] = boost::shared_ptr<StochasticProcess1D>(referenceProcess);
					processes[1] = boost::shared_ptr<StochasticProcess1D>(discountProcess);

/*
					boost::shared_ptr<HullWhiteProcess> processForReference(
						new HullWhiteProcess(floatTermStructure, meanReversion, sigma));
					boost::shared_ptr<HullWhiteProcess> processForDiscount(
						new HullWhiteProcess(discountTermStructure, discountMeanReversion, discountSigma));
*/				
					//set the initialized short rate value into the process
					if (periodIndex != 0){
						//processes[0]->setX0(x0ForReference);
						//processes[1]->setX0(x0ForDiscount);
						referenceProcess->setX0(x0ForReference);
						discountProcess->setX0(x0ForDiscount);
					}
					Matrix correlation(2,2);
					correlation[0][0] = 1.0; correlation[0][1] = 0.9;
					correlation[1][0] = 0.9; correlation[1][1] = 1.0;
					
					process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes,correlation));
					
					//2. timeGrid
					//TODO timeGrid�� exercise Date �߰�
					TimeGrid timeGrid(periodTenor, timeGridSize, startTenor);

					//3. path Generator
					rsg_type rsg = PseudoRandom::make_sequence_generator(
						timeGridSize * process->size(), m_seeds[simIndex][periodIndex]);
								
					//PathGenerator<rsg_type> generator(processForReference, timeGrid, rsg, false);
					MultiPathGenerator<rsg_type> generator(process, timeGrid, rsg, false);
					sample_type path1 = generator.next();
					HullWhite hullWhite(floatTermStructure, meanReversion, sigma);

					int accruedDays = 0;
					double cumulatedDF = 1;
					for (Size j = 0; j < timeGrid.size(); j++){
						//4. Calculate the reference Rate
						QuantLib::Time tenor = m_floatCurveTenor1;
						QuantLib::Time start = timeGrid[j] + startTenor;
						Real referenceRate = - log(
							hullWhite.discountBond(start, start + tenor, path1.value[0][j])) / tenor;
				
						double lowerRate = m_rangeLowerRates[periodIndex];
						double upperRate = m_rangeUpperRates[periodIndex];
						if (referenceRate >= lowerRate && referenceRate <= upperRate) {
							accruedDays++;
						}
						double rate1 = path1.value[0][j];
						double rate2 = path1.value[1][j];
						cumulatedDF *= exp(- rate2 * timeGrid.dt(0));
					}
					//initialize the short rate value
					x0ForReference = path1.value[0][timeGrid.size() - 1];
					x0ForDiscount = path1.value[1][timeGrid.size() - 1];
					coupon[periodIndex] = accruedDays * m_inCouponRates[periodIndex] * timeGrid.dt(0);
					df[periodIndex] = cumulatedDF;
				}
				coupons[simIndex] = coupon;
				dfs[simIndex] = df;
			}

			//calculate price	
			std::vector<double> notional(simulationNum);
			for (int simIndex = 0; simIndex < simulationNum; simIndex++){
				
				notional[simIndex] = 1;
			}
			payoffs[periodLength] = notional;

			for (int periodIndex = periodLength - 1; periodIndex >= 0; periodIndex--){
				
				std::vector<double> payoff(simulationNum);
				bool hasExercise = m_rangePeriods[periodIndex].hasExercise();
				double exercisePrice = m_rangePeriods[periodIndex].getExercisePrice();
				
				for (int simIndex = 0; simIndex < simulationNum; simIndex++){				
					double previousPayoff = payoffs[periodIndex + 1][simIndex];
					double coupon = coupons[simIndex][periodIndex];
					double df = dfs[simIndex][periodIndex];
					if (hasExercise){
						if (previousPayoff < exercisePrice){
							payoff[simIndex] = (previousPayoff + coupon) * df;
						} else {
							payoff[simIndex] = (exercisePrice + coupon) * df;
						}
					} else {
						payoff[simIndex] = (previousPayoff + coupon) * df;
					}
				}
				payoffs[periodIndex] = payoff;
			}

			double value = 0;
			for (int simIndex = 0; simIndex < simulationNum; simIndex++){
				value += payoffs[0][simIndex];
			}
			
		
			return Money(m_notional.currency(), value / simulationNum);
	}

	RangeAccrualNote::~RangeAccrualNote(void){
	}
}