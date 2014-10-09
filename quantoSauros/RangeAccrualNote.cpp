#include "RangeAccrualNote.h"

namespace quantoSauros {
	/*
	RangeAccrualNote::RangeAccrualNote(QuantLib::Money notional,
		//날짜정보
		QuantLib::Date issueDate, QuantLib::Date maturityDate,
		//기타정보
		QuantLib::DayCounter dcf, bool includePrincipal,
		//기준금리1 정보
		double floatCurveTenor1, QuantLib::Frequency swapCouponFrequency1,
		//Range 구간 정보
		std::vector<double> inCouponRates, std::vector<double> outCouponRates,
		std::vector<quantoSauros::Period> rangePeriods,
		std::vector<double> rangeUpperRates, std::vector<double> rangeLowerRates,		
		//행사정보
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
			
			m_optionType = optionType;
			m_monitorFrequency = monitorFrequency;			
	}
	*/
	RangeAccrualNote::RangeAccrualNote(
		quantoSauros::NoteLegScheduleInfo* scheduleInfo,
		quantoSauros::NoteLegAmortizationInfo* amortizationInfo,
		std::vector<quantoSauros::NoteLegRangeCouponInfo*> couponInfos,
		quantoSauros::NoteLegDataInfo* dataInfo,
		quantoSauros::NoteLegOptionInfo* optionInfo,
		//기준금리 정보
		std::vector<quantoSauros::IRInfo> irInfos,
		//할인금리 정보
		quantoSauros::IRInfo discountInfo,
		//상관계수 정보
		quantoSauros::CorrelationInfo correlationInfo
		){
			m_notional = amortizationInfo->getNotional();
			m_issueDate = scheduleInfo->getIssueDate();
			m_maturityDate = scheduleInfo->getMaturityDate();
			m_dcf = scheduleInfo->getDayCounter();
			m_includePrincipal = amortizationInfo->getIncludePrincipal();
			
			for (int i = 0; i <couponInfos.size(); i++){
				if (couponInfos[i]->getClassName() == "NoteLegSpreadRangeCouponInfo"){
					//TODO
				} else if (couponInfos[i]->getClassName() == "NoteLegSpotRangeCouponInfo"){
					m_rateTypes.push_back(couponInfos[i]->getRateType1());
					m_floatCurveTenors.push_back(couponInfos[i]->getTenor1());
					m_swapCouponFrequencies.push_back(couponInfos[i]->getSwapCouponFrequency1());
				}				
			}

			m_inCouponRates = couponInfos[0]->getInCouponRates();
			m_outCouponRates = couponInfos[0]->getOutCouponRates();

			m_rangePeriods = scheduleInfo->getPeriods();

			m_rangeUpperRates = couponInfos[0]->getUpperBounds();
			m_rangeLowerRates = couponInfos[0]->getLowerBounds();
			
			m_optionType = optionInfo->getOptionType();
			m_monitorFrequency = dataInfo->getMonitorFrequency();
			m_accruedCoupon = dataInfo->getAccruedCoupon();
			m_correlationMatrix = correlationInfo.getCorrelationMatrix();
	}
	QuantLib::Money RangeAccrualNote::getPrice(
		//날짜정보
		QuantLib::Date today, 
		//기준금리 정보
		std::vector<quantoSauros::IRInfo> irInfos,
		//할인금리 정보
		quantoSauros::IRInfo discountInfo,
		//기타
		int simulationNum){

			typedef PseudoRandom::rsg_type rsg_type;
			//typedef PathGenerator<rsg_type>::sample_type sample_type;
			typedef MultiPathGenerator<rsg_type>::sample_type sample_type;
			
			//데이터 정리
			m_simulationNum = simulationNum;
			m_periodNum = m_rangePeriods.size();
			m_today = today;

			int irNum = irInfos.size();
			
			std::vector<HullWhiteParameters> IRParams(irNum);
			std::vector<quantoSauros::InterestRateCurve> floatCurves(irNum);
			std::vector<Handle<YieldTermStructure>> floatTermStructure(irNum);
			for (int i = 0; i < irNum; i++){
				IRParams[i] = irInfos[i].getHullWhiteParams();
				floatCurves[i] = irInfos[i].getInterestRateCurve();
				floatTermStructure[i] = Handle<YieldTermStructure>(
					floatCurves[i].getInterestRateCurve());
			}
			HullWhiteParameters discountParams = discountInfo.getHullWhiteParams(); 
			quantoSauros::InterestRateCurve discountCurve = discountInfo.getInterestRateCurve();
			Handle<YieldTermStructure> discountTermStructure(discountCurve.getInterestRateCurve());

			//dividing Range Periods
			DividingPeriods();
						
			double periodTenor = m_rangePeriods[0].getPeriodTenor(m_dcf);			
			QuantLib::Size timeGridSize = ceil(periodTenor / ((double)m_monitorFrequency / 360));

			//Range 별 seed 생성
			generateSeeds();

			//initial 변수 선언
			std::vector<Real> x0ForIRProcess(irNum);			
			Real x0ForDiscountProcess = 0;

			//payoff 저장 함수 선언
			std::vector<std::vector<double>> payoffs(m_periodNum + 1);
			std::vector<std::vector<double>> coupons(m_simulationNum);
			std::vector<std::vector<double>> dfs(m_simulationNum);

			//Calculating module
			for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
				//data 저장
				std::vector<double> coupon(m_periodNum);
				std::vector<double> df(m_periodNum);

				for (int periodIndex = 0; periodIndex < m_periodNum; periodIndex++){				
					quantoSauros::Period period = m_rangePeriods[periodIndex];
					QuantLib::Time startTenor = std::max(
						(double) m_dcf.yearFraction(m_today, period.getStartDate()), 0.0);
					QuantLib::Time periodTenor = period.getPeriodTenor(m_dcf);

					//1. process				
					std::vector<boost::shared_ptr<StochasticProcess1D>> processes(irNum + 1);
					boost::shared_ptr<StochasticProcess> process;
					std::vector<boost::shared_ptr<HullWhiteProcess>> IRProcess(irNum);
					for (int i = 0; i < irNum; i++){
						IRProcess[i] = boost::shared_ptr<HullWhiteProcess>(
							new HullWhiteProcess(floatTermStructure[i], 
							IRParams[i].getMeanReversion1F(), IRParams[i].getVolatility1F()));
						processes[i] = boost::shared_ptr<StochasticProcess1D>(IRProcess[i]);
					}
					boost::shared_ptr<HullWhiteProcess> discountProcess(
						new HullWhiteProcess(discountTermStructure, 
						discountParams.getMeanReversion1F(), discountParams.getVolatility1F()));
					processes[irNum] = boost::shared_ptr<StochasticProcess1D>(discountProcess);

					//set the initialized short rate value into the process
					if (periodIndex != 0){
						for (int i = 0; i < irNum; i++){
							IRProcess[i]->setX0(x0ForIRProcess[i]);
						}						
						discountProcess->setX0(x0ForDiscountProcess);
					}

					process = boost::shared_ptr<StochasticProcess>(
                           new StochasticProcessArray(processes, m_correlationMatrix));
					
					//2. timeGrid
					//TODO timeGrid에 exercise Date 추가
					TimeGrid timeGrid(periodTenor, timeGridSize, startTenor);

					//3. path Generator
					rsg_type rsg = PseudoRandom::make_sequence_generator(
						timeGridSize * process->size(), m_seeds[simIndex][periodIndex]);
					MultiPathGenerator<rsg_type> generator(process, timeGrid, rsg, false);
					sample_type path = generator.next();

					int accruedDays = 0;
					double cumulatedDF = 1;
					double dt = timeGrid.dt(0);

					for (Size timeIndex = 0; timeIndex < timeGrid.size() - 1; timeIndex++){
						//4.1. Calculate the reference Rate
						QuantLib::Time start = timeGrid[timeIndex] + startTenor;

						std::vector<Real> referenceRates(irNum);
						for (int i = 0; i < irNum; i++){							
							double end = 0;
							double vol = 0;
							double bondPrice = 0;
							double bondPriceSum = 0;
							QuantLib::Time tenor = m_floatCurveTenors[i];

							quantoSauros::RateType type = m_rateTypes[i];
							if (type == quantoSauros::RateType::DepositRate){								
								end = start + tenor;
								vol = IRParams[i].getVolatility1F();
								QuantLib::HullWhite hullWhite(floatTermStructure[i], 
									IRParams[i].getMeanReversion1F(), vol);
								bondPrice = hullWhite.discountBond(start, end, path.value[i][timeIndex]);
								referenceRates[i] = - log(bondPrice) / tenor;
							} else if (type == quantoSauros::RateType::SwapRate){								
								//QuantLib::Frequency swapCouponFrequency = m_swapCouponFrequencies[i];
								double swapTenor = 1 / m_swapCouponFrequencies[i];
								int swapRateNum = tenor / swapTenor;
								for (int j = 0; j < swapRateNum; j++){
									double tmpTenor = swapTenor * (j + 1);
									end = start + tmpTenor;
									vol = IRParams[i].getVolatility1F();
									QuantLib::HullWhite hullWhite(floatTermStructure[i], 
										IRParams[i].getMeanReversion1F(), vol);
									bondPrice = hullWhite.discountBond(start, end, path.value[i][timeIndex]);
									bondPriceSum += bondPrice * swapTenor;
								}								
								referenceRates[i] = (1 - bondPrice) / bondPriceSum;
							}
							
						}

						//4.2. Calculate the accrual Days
						double lowerRate = m_rangeLowerRates[periodIndex];
						double upperRate = m_rangeUpperRates[periodIndex];
						if (irNum == 1){
							if (referenceRates[0] >= lowerRate && referenceRates[0] <= upperRate) {								
								accruedDays++;
							}
						} else if (irNum == 2){
							if (referenceRates[0] >= lowerRate && referenceRates[0] <= upperRate) {
								if (referenceRates[1] >= lowerRate && referenceRates[1] <= upperRate) {
									accruedDays++;
								}
							}
						}

						//4.3. Calculate the discount Rate
						double shortRate = path.value[0][timeIndex];
						double dfRate = path.value[irNum][timeIndex];
						cumulatedDF *= exp(- dfRate * timeGrid.dt(timeIndex));
					}
					//initialize the short rate value
					for (int i = 0; i < irNum; i++){
						x0ForIRProcess[i] = path.value[i][timeGrid.size() - 1];
					}					
					x0ForDiscountProcess = path.value[irNum][timeGrid.size() - 1];
					//calculate coupon amount
					coupon[periodIndex] = accruedDays * m_inCouponRates[periodIndex] * dt;
					//save the calculated discount factor
					df[periodIndex] = cumulatedDF;
				}
				coupons[simIndex] = coupon;
				dfs[simIndex] = df;
			}

			//calculate price	
			//1. set a notional into the payoffs
			std::vector<double> notional(m_simulationNum);
			for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){				
				notional[simIndex] = 1;
			}
			payoffs[m_periodNum] = notional;

			//2. calculate payoffs by the backward method
			for (int periodIndex = m_periodNum - 1; periodIndex >= 0; periodIndex--){
				
				std::vector<double> payoff(m_simulationNum);
				bool hasExercise = m_rangePeriods[periodIndex].hasExercise();
				double exercisePrice = m_rangePeriods[periodIndex].getExercisePrice();
				
				for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){				
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

			//3. calculate the current value
			double value = 0;
			for (int simIndex = 0; simIndex < m_simulationNum; simIndex++){
				value += payoffs[0][simIndex];
			}		
		
			return Money(m_notional.currency(), value / m_simulationNum);
	}

	RangeAccrualNote::~RangeAccrualNote(void){
	}

	void RangeAccrualNote::DividingPeriods(){
		int sizeOfRangePeriod = m_rangePeriods.size();
		int idx = 0;
		while(m_today - m_rangePeriods[idx].getEndDate() >= 0){
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
	}	

	void RangeAccrualNote::generateSeeds(){
		srand(time(NULL));
		if (m_seeds.size() == 0){
			for (int i = 0; i < m_simulationNum; i++){
				std::vector<BigNatural> seed;
				for (int j = 0; j < m_periodNum; j++){
					seed.push_back(BigNatural(rand()));
				}
				m_seeds.push_back(seed);
			}
		}
	}

}