#include "AccrualPricer.h"

namespace quantoSauros {

	void RangeAccrualPricer::dividePeriods(){
		
		std::vector<quantoSauros::Period> periods = m_args.getPeriods();
		std::vector<double>& inCouponRates = m_args.getInCouponRates();
		std::vector<double>& outCouponRates = m_args.getOutCouponRates();
		std::vector<std::vector<double>>& rangeLowerRates = m_args.getRangeLowerRates();
		std::vector<std::vector<double>>& rangeUpperRates = m_args.getRangeUpperRates();
		int idx = 0;
		int periodNum = periods.size();
		while(m_today - periods[idx].getEndDate() >= 0){
			if (idx == periodNum - 1){
				break;
			}
			idx++;
		}
		
		periods.erase(periods.begin(), periods.begin() + idx);
		inCouponRates.erase(inCouponRates.begin(), inCouponRates.begin() + idx);
		outCouponRates.erase(outCouponRates.begin(), outCouponRates.begin() + idx);
		rangeLowerRates.erase(rangeLowerRates.begin(), rangeLowerRates.begin() + idx);			
		rangeUpperRates.erase(rangeUpperRates.begin(), rangeUpperRates.begin() + idx);		
		
		m_args.setPeriods(periods);
		m_args.setInCouponRates(inCouponRates);
		m_args.setOutCouponRates(outCouponRates);
		m_args.setRangeLowerRates(rangeLowerRates);
		m_args.setRangeUpperRates(rangeUpperRates);
		//m_periodNum = rangePeriods.size();
	}

	void RangeAccrualPricer::generatePaths(){

		typedef PseudoRandom::rsg_type rsg_type;		
		typedef MultiPathGenerator<rsg_type>::sample_type sample_type;
		//initial 변수 선언
		std::vector<Real> x0ForIRProcess(m_irNum);			
		Real x0ForDiscountProcess = 0;

		//Path Generating module
		for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
			for (int periodIndex = 0; periodIndex < m_periodNum; periodIndex++){			
				quantoSauros::Period period = m_args.getPeriods()[periodIndex];
				QuantLib::Time startTenor = std::max(
					(double) m_args.getDayCounter().yearFraction(m_today, period.getStartDate()), 0.0);
				QuantLib::Time periodTenor = period.getPeriodTenor(m_args.getDayCounter());
				
				//1. process				
				std::vector<boost::shared_ptr<StochasticProcess1D>> processes(m_irNum + 1);
				boost::shared_ptr<StochasticProcess> process;
				std::vector<boost::shared_ptr<HullWhiteProcess>> IRProcess(m_irNum);
				for (int i = 0; i < m_irNum; i++){
					
					IRProcess[i] = boost::shared_ptr<HullWhiteProcess>(
						new HullWhiteProcess(Handle<YieldTermStructure>(m_floatTermStructure[i].getInterestRateCurve()), 
						m_IRParams[i].getMeanReversion1F(), 
						m_args.getHullWhiteVolatilities()[i]));
						//m_IRParams[i].getVolatility1F()));
					processes[i] = boost::shared_ptr<StochasticProcess1D>(IRProcess[i]);
				}
				boost::shared_ptr<HullWhiteProcess> discountProcess(
					new HullWhiteProcess(Handle<YieldTermStructure>(m_discountTermStructure.getInterestRateCurve()), 
					m_discountParams.getMeanReversion1F(), 
					m_args.getDiscountHullWhiteVolatility()));
					//m_discountParams.getVolatility1F()));
				processes[m_irNum] = boost::shared_ptr<StochasticProcess1D>(discountProcess);

				//set the initialized short rate value into the process
				if (periodIndex != 0){
					for (int i = 0; i < m_irNum; i++){
						IRProcess[i]->setX0(x0ForIRProcess[i]);
					}						
					discountProcess->setX0(x0ForDiscountProcess);
				}

				process = boost::shared_ptr<StochasticProcess>(
					new StochasticProcessArray(processes, m_args.getCorrelationMatrix()));

				//2. timeGrid
				//TODO timeGrid에 exercise Date 추가
				TimeGrid timeGrid(periodTenor, m_timeGridSize, startTenor);

				//3. path Generator
				rsg_type rsg = PseudoRandom::make_sequence_generator(
					m_timeGridSize * process->size(), m_seeds[simIndex][periodIndex]);
				MultiPathGenerator<rsg_type> generator(process, timeGrid, rsg, false);
				sample_type path = generator.next();

				m_data[simIndex][periodIndex] = RangeAccrualData(
					m_today, period, timeGrid, m_args.getMaturityDate(),
					m_irNum, startTenor, 
					m_args.getRangeUpperRates()[periodIndex], 
					m_args.getRangeLowerRates()[periodIndex], 
					m_args.getInCouponRates()[periodIndex],
					m_args.getOutCouponRates()[periodIndex],
					m_args.getFloatCurveTenors(), m_args.getRateTypes(), 
					m_args.getSwapCouponFrequencies(), 
					m_floatTermStructure, 
					&path, m_IRParams,
					m_args.getHullWhiteVolatilities(),
					m_args.getDiscountHullWhiteVolatility());

				//initialize the short rate value
				for (int i = 0; i < m_irNum; i++){
					x0ForIRProcess[i] = path.value[i][timeGrid.size() - 1];
				}					
				x0ForDiscountProcess = path.value[m_irNum][timeGrid.size() - 1];
			}
		}
	}
	
	void RangeAccrualPricer::calculatePrice(){
		//1. set a notional into the payoffs
		std::vector<double> notional(m_simulationNum);
		for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){				
			notional[simIndex] = 1;
		}
		m_payoffs[m_periodNum] = notional;

		//2. calculate payoffs by the backward method
		for (int periodIndex = m_periodNum - 1; periodIndex >= 0; periodIndex--){
				
			std::vector<double> payoff(m_simulationNum);
			bool hasExercise = m_args.getPeriods()[periodIndex].hasExercise();
			double exercisePrice = m_args.getPeriods()[periodIndex].getExercisePrice();
			std::vector<bool> LSMCCandidateFlags(m_simulationNum);			
			int numOfCandidate = 0;
			for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){				
				double previousPayoff = m_payoffs[periodIndex + 1][simIndex];
				double coupon = m_data[simIndex][periodIndex].getPayoffs();
				double df = m_data[simIndex][periodIndex].getDiscountFactor();
				if (hasExercise){
					//TODO : Select LSMC Candidates
					//if (){}

					payoff[simIndex] = (previousPayoff + coupon) * df;
					LSMCCandidateFlags[simIndex] = true;
					numOfCandidate++;

				} else {
					payoff[simIndex] = (previousPayoff + coupon) * df;
				}
			}
			m_payoffs[periodIndex] = payoff;
			
			//LSMC 적용
			if (hasExercise){
				std::vector<quantoSauros::LSMCData> LSMCInputData(numOfCandidate);
				int candiIndex = 0;

				for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
					if (LSMCCandidateFlags[simIndex] == true){
						quantoSauros::LSMCData lsmcData;
						std::vector<double> xData;

						lsmcData.values = m_data[simIndex][periodIndex].getLSMCData(0);
						lsmcData.cumulatedCashFlows = m_payoffs[periodIndex][simIndex];
						lsmcData.isVaild = true;	
						lsmcData.controlValue = 0;
						lsmcData.exerciseValue = exercisePrice;
						LSMCInputData[candiIndex] = lsmcData;
						candiIndex++;
					}
				}		

				std::vector<double> LSMCContinuation(numOfCandidate);
				//TODO : LSMC module
				quantoSauros::LongstaffSchwartz(LSMCInputData, LSMCContinuation);

				int LSMCIndex = 0;
				for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){

					if (LSMCCandidateFlags[simIndex] == true){
						
						if (true){
							//call
							if (exercisePrice < LSMCContinuation[LSMCIndex]){
								m_payoffs[periodIndex][simIndex] = exercisePrice;
							}
						} else if (false) {
							//put
							if (exercisePrice > LSMCContinuation[LSMCIndex]){
								m_payoffs[periodIndex][simIndex] = exercisePrice;
							}
						}
						LSMCIndex++;
					}
				}
			}
		}

		//3. calculate the current value
		double value = 0;
		for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
			value += m_payoffs[0][simIndex];
		}		
		
		m_price = QuantLib::Money(m_args.getNotional().currency(), value / m_simulationNum);
	}

}