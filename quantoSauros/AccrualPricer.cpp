#include "AccrualPricer.h"

namespace quantoSauros {

	void RangeAccrualPricer::dividePeriods(){
		
		std::vector<quantoSauros::Period> rangePeriods = m_args.getRangePeriods();
		std::vector<double> inCouponRates = m_args.getInCouponRates();
		std::vector<double> outCouponRates = m_args.getOutCouponRates();
		std::vector<std::vector<double>> rangeLowerRates = m_args.getRangeLowerRates();
		std::vector<std::vector<double>> rangeUpperRates = m_args.getRangeUpperRates();
		int idx = 0;
		while(m_today - rangePeriods[idx].getEndDate() >= 0){
			if (idx == m_periodNum - 1){
				break;
			}
			idx++;
		}
		rangePeriods.erase(rangePeriods.begin(), rangePeriods.begin() + idx);
		inCouponRates.erase(inCouponRates.begin(), inCouponRates.begin() + idx);
		outCouponRates.erase(outCouponRates.begin(), outCouponRates.begin() + idx);
		for (int i = 0; i < m_irNum; i++){
			rangeLowerRates[i].erase(rangeLowerRates[i].begin(), rangeLowerRates[i].begin() + idx);			
			rangeUpperRates[i].erase(rangeUpperRates[i].begin(), rangeUpperRates[i].begin() + idx);		
		}		
	}

	void RangeAccrualPricer::generatePaths(){

		typedef PseudoRandom::rsg_type rsg_type;		
		typedef MultiPathGenerator<rsg_type>::sample_type sample_type;
		//initial 변수 선언
		std::vector<Real> x0ForIRProcess(m_irNum);			
		Real x0ForDiscountProcess = 0;

		//Path Generating module
		for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
			//data 저장
			std::vector<double> coupon(m_periodNum);
			std::vector<double> df(m_periodNum);	
			//std::vector<quantoSauros::RangeAccrualData> data(m_periodNum);
			for (int periodIndex = 0; periodIndex < m_periodNum; periodIndex++){			
				quantoSauros::Period period = m_args.getRangePeriods()[periodIndex];
				QuantLib::Time startTenor = std::max(
					(double) m_args.getDayCounter().yearFraction(m_today, period.getStartDate()), 0.0);
				QuantLib::Time periodTenor = period.getPeriodTenor(m_args.getDayCounter());

				//1. process				
				std::vector<boost::shared_ptr<StochasticProcess1D>> processes(m_irNum + 1);
				boost::shared_ptr<StochasticProcess> process;
				std::vector<boost::shared_ptr<HullWhiteProcess>> IRProcess(m_irNum);
				for (int i = 0; i < m_irNum; i++){
					/*IRProcess[i] = boost::shared_ptr<HullWhiteProcess>(
						new HullWhiteProcess(m_floatTermStructure[i].getInterestRateCurve(), 
						m_IRParams[i].getMeanReversion1F(), m_IRParams[i].getVolatility1F()));*/
					IRProcess[i] = boost::shared_ptr<HullWhiteProcess>(
						new HullWhiteProcess(Handle<YieldTermStructure>(m_floatTermStructure[i].getInterestRateCurve()), 
						m_IRParams[i].getMeanReversion1F(), m_IRParams[i].getVolatility1F()));
					processes[i] = boost::shared_ptr<StochasticProcess1D>(IRProcess[i]);
				}
				boost::shared_ptr<HullWhiteProcess> discountProcess(
					new HullWhiteProcess(Handle<YieldTermStructure>(m_discountTermStructure.getInterestRateCurve()), 
					m_discountParams.getMeanReversion1F(), m_discountParams.getVolatility1F()));
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
					&path, m_IRParams);
				
				/*
				int accruedDays = 0;
				double cumulatedDF = 1;
				double dt = timeGrid.dt(0);

				for (Size timeIndex = 0; timeIndex < timeGrid.size() - 1; timeIndex++){
					//4.1. Calculate the reference Rate
					QuantLib::Time start = timeGrid[timeIndex] + startTenor;

					std::vector<Real> referenceRates(m_irNum);
					for (int i = 0; i < m_irNum; i++){							
						double end = 0;
						double vol = 0;
						double bondPrice = 0;
						double bondPriceSum = 0;
						QuantLib::Time tenor = m_args.getFloatCurveTenors()[i];

						quantoSauros::RateType type = m_args.getRateTypes()[i];
						if (type == quantoSauros::RateType::DepositRate){								
							end = start + tenor;
							vol = m_IRParams[i].getVolatility1F();
							QuantLib::HullWhite hullWhite(m_floatTermStructure[i], 
								m_IRParams[i].getMeanReversion1F(), vol);
							bondPrice = hullWhite.discountBond(start, end, path.value[i][timeIndex]);
							referenceRates[i] = - log(bondPrice) / tenor;
						} else if (type == quantoSauros::RateType::SwapRate){								
							//QuantLib::Frequency swapCouponFrequency = m_swapCouponFrequencies[i];
							double swapTenor = 1 / m_args.getSwapCouponFrequencies()[i];
							int swapRateNum = tenor / swapTenor;
							for (int j = 0; j < swapRateNum; j++){
								double tmpTenor = swapTenor * (j + 1);
								end = start + tmpTenor;
								vol = m_IRParams[i].getVolatility1F();
								QuantLib::HullWhite hullWhite(m_floatTermStructure[i], 
									m_IRParams[i].getMeanReversion1F(), vol);
								bondPrice = hullWhite.discountBond(start, end, path.value[i][timeIndex]);
								bondPriceSum += bondPrice * swapTenor;
							}								
							referenceRates[i] = (1 - bondPrice) / bondPriceSum;
						}							
					}
					//4.2. Calculate the accrual Days										
					if (m_irNum == 1){
						double upperRate1 = m_args.getRangeUpperRates()[0][periodIndex];
						double lowerRate1 = m_args.getRangeLowerRates()[0][periodIndex];
						if (referenceRates[0] >= lowerRate1 && referenceRates[0] <= upperRate1) {								
							accruedDays++;
						}
					} else if (m_irNum == 2){
						double upperRate1 = m_args.getRangeUpperRates()[0][periodIndex];
						double lowerRate1 = m_args.getRangeLowerRates()[0][periodIndex];
						double upperRate2 = m_args.getRangeUpperRates()[1][periodIndex];
						double lowerRate2 = m_args.getRangeLowerRates()[1][periodIndex];
						if (referenceRates[0] >= lowerRate1 && referenceRates[0] <= upperRate1) {
							if (referenceRates[1] >= lowerRate2 && referenceRates[1] <= upperRate2) {
								accruedDays++;
							}
						}
					}

					//4.3. Calculate the discount Rate
					double shortRate = path.value[0][timeIndex];
					double dfRate = path.value[m_irNum][timeIndex];
					cumulatedDF *= exp(- dfRate * timeGrid.dt(timeIndex));
				}
					*/
				
				//initialize the short rate value
				for (int i = 0; i < m_irNum; i++){
					x0ForIRProcess[i] = path.value[i][timeGrid.size() - 1];
				}	
				
				x0ForDiscountProcess = path.value[m_irNum][timeGrid.size() - 1];
			
				//calculate coupon amount
				//coupon[periodIndex] = accruedDays * m_args.getInCouponRates()[periodIndex] * dt;
				//save the calculated discount factor
				//df[periodIndex] = cumulatedDF;
				
			}
			m_coupons[simIndex] = coupon;
			m_dfs[simIndex] = df;
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
			bool hasExercise = m_args.getRangePeriods()[periodIndex].hasExercise();
			double exercisePrice = m_args.getRangePeriods()[periodIndex].getExercisePrice();
			std::vector<bool> LSMCCandidateFlags(m_simulationNum);			
			int numOfCandidate = 0;
			for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){				
				double previousPayoff = m_payoffs[periodIndex + 1][simIndex];
				//double coupon = m_coupons[simIndex][periodIndex];
				//double df = m_dfs[simIndex][periodIndex];
				double coupon = m_data[simIndex][periodIndex].getPayoffs();
				double df = m_data[simIndex][periodIndex].getDiscountFactor();
				if (hasExercise){
					//Select LSMC Candidates
					//if (){}

					payoff[simIndex] = (previousPayoff + coupon) * df;
					LSMCCandidateFlags[simIndex] = true;
					numOfCandidate++;
					/*
					if (previousPayoff < exercisePrice){
						payoff[simIndex] = (previousPayoff + coupon) * df;
					} else {
						payoff[simIndex] = (exercisePrice + coupon) * df;
					}
					*/
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
						lsmcData.exerciseValue = 1;
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
					//payoff 수정

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