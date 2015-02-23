#pragma once
#include <ql/quantlib.hpp>
#include "InterestRate.h"
#include "HullWhiteParameters.h";

namespace quantoSauros{

	typedef QuantLib::PseudoRandom::rsg_type rsg_type;		
	typedef QuantLib::MultiPathGenerator<rsg_type>::sample_type sample_type;

	class AbstractData {
	public:	
		AbstractData(){};
		AbstractData(QuantLib::Date asOfDate, quantoSauros::Period period,
			QuantLib::TimeGrid timeGrid, QuantLib::Date maturityDate, int numOfIR,
			QuantLib::Time startTime,
			//이자율 정보
			std::vector<double> floatCurveTenors,
			std::vector<quantoSauros::RateType> rateTypes,
			std::vector<QuantLib::Frequency> swapCouponFrequencies,
			std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
			//헐화이트정보
			sample_type* shortRatePath, 
			std::vector<quantoSauros::HullWhiteParameters> IRParams,
			std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
			QuantLib::HullWhiteVolatility discountHullWhiteVolatility){

				m_asOfDate = asOfDate;
				m_timeGrid = timeGrid;
				m_period = period;
				m_maturityDate = maturityDate;
				m_numOfIR = numOfIR;
				m_referenceRates = std::vector<std::vector<double>>(m_timeGrid.size());

				m_startTime = startTime;
				m_floatCurveTenors = floatCurveTenors;
				m_IRParams = IRParams;
				m_rateTypes = rateTypes;
				m_swapCouponFrequencies = swapCouponFrequencies;
				m_floatTermStructure = floatTermStructures;

				m_hullWhiteVolatilities = hullWhiteVolatilities;
				m_discountHullWhiteVolatilities = discountHullWhiteVolatility;

				m_cumulatedDF = 1;
				m_dt = m_timeGrid.dt(0);		
				savePathsInfo(shortRatePath);				
		};	
		virtual double getReferenceRate(int IRIndex, int timeIndex){
			//TODO
			if (timeIndex >= m_timeGrid.size()){
				return 0;
			}
			if (IRIndex >= m_referenceRates.size()){
				return 0;
			}

			return m_referenceRates[m_timeGrid.size() - 2][IRIndex];
		};
		virtual double getPayoffs(){ return 0; };
		virtual double getDiscountFactor(){ return 0; };

		virtual std::vector<double> getLSMCData(int IRIndex){
			std::vector<double> data;

			data.push_back(1);
			data.push_back(m_referenceRates[m_timeGrid.size() - 2][IRIndex]);

			QuantLib::Date swapStartDate = m_period.getEndDate();
			QuantLib::Date swapEndDate = m_maturityDate;
			if (swapStartDate != swapEndDate){
				double coterminalSwapRate = m_floatTermStructure[IRIndex].
					getForwardSwapRate(swapStartDate, swapEndDate);
				data.push_back(coterminalSwapRate);
			}			

			return data;
		};

	protected:
		virtual void init(){
			calcReferenceRates();
			calculateDiscountFactor();
			calculatePayoff();
		}
		virtual void savePathsInfo(sample_type* shortRatePath){
			m_shortRatePath = shortRatePath;
		}
		virtual void calcReferenceRates(){
			for (Size timeIndex = 0; timeIndex < m_timeGrid.size() - 1; timeIndex++){
				//4.1. Calculate the reference Rate
				QuantLib::Time start = m_timeGrid[timeIndex] + m_startTime;
				std::vector<double> referenceRates(m_numOfIR);
				for (int i = 0; i < m_numOfIR; i++){							
					double end = 0;
					double vol = 0;
					double bondPrice = 0;
					double bondPriceSum = 0;
					QuantLib::Time tenor = m_floatCurveTenors[i];

					quantoSauros::RateType type = m_rateTypes[i];
					if (type == quantoSauros::RateType::DepositRate){								
						end = start + tenor;						
						vol = m_hullWhiteVolatilities[i].vol()(start);;//m_IRParams[i].getVolatility1F();
						QuantLib::HullWhite hullWhite(
							Handle<YieldTermStructure>(m_floatTermStructure[i].getInterestRateCurve()), 
							m_IRParams[i].getMeanReversion1F(), vol);
						bondPrice = hullWhite.discountBond(start, end, 
							m_shortRatePath->value[i][timeIndex]);
						referenceRates[i] = - log(bondPrice) / tenor;
					} else if (type == quantoSauros::RateType::SwapRate){								
						//QuantLib::Frequency swapCouponFrequency = m_swapCouponFrequencies[i];
						double swapTenor = 1 / m_swapCouponFrequencies[i];
						int swapRateNum = tenor / swapTenor;
						for (int j = 0; j < swapRateNum; j++){
							double tmpTenor = swapTenor * (j + 1);
							end = start + tmpTenor;
							vol = m_hullWhiteVolatilities[i].vol()(start);//m_IRParams[i].getVolatility1F();
							QuantLib::HullWhite hullWhite(
								Handle<YieldTermStructure>(m_floatTermStructure[i].getInterestRateCurve()), 
								m_IRParams[i].getMeanReversion1F(), vol);
							bondPrice = hullWhite.discountBond(start, end, 
								m_shortRatePath->value[i][timeIndex]);
							bondPriceSum += bondPrice * swapTenor;
						}								
						referenceRates[i] = (1 - bondPrice) / bondPriceSum;
					}							
				}
				m_referenceRates[timeIndex] = referenceRates;
			}
		}
		virtual void calculateDiscountFactor(){
			for (Size timeIndex = 0; timeIndex < m_timeGrid.size() - 1; timeIndex++){

				//double shortRate = m_shortRatePath->value[0][timeIndex];
				double dfRate = m_shortRatePath->value[m_numOfIR][timeIndex];
				m_cumulatedDF *= exp(- dfRate * m_timeGrid.dt(timeIndex));
			}
		}
		virtual void calculatePayoff() {};

		//Hull-White Path
		sample_type* m_shortRatePath;

		//Hull-White Volatility
		std::vector<QuantLib::HullWhiteVolatility> m_hullWhiteVolatilities;
		QuantLib::HullWhiteVolatility m_discountHullWhiteVolatilities;

		//Information of the Reference Rate
		std::vector<Real> m_floatCurveTenors;
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;
		std::vector<HullWhiteParameters> m_IRParams;
		std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;

		//Time
		QuantLib::Time m_startTime;
		QuantLib::Date m_asOfDate;
		QuantLib::TimeGrid m_timeGrid;
		double m_dt;
		QuantLib::Date m_maturityDate;
		quantoSauros::Period m_period;

		//Generated Results
		std::vector<std::vector<Rate>> m_referenceRates;		
		double m_cumulatedDF;

		//etc
		int m_numOfIR;
	};
}