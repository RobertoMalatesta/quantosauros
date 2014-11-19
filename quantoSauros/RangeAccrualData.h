#pragma once
#include <ql/quantlib.hpp>
#include "AbstractData.h";
#include "HullWhiteParameters.h";

namespace quantoSauros{

	typedef QuantLib::PseudoRandom::rsg_type rsg_type;		
	typedef QuantLib::MultiPathGenerator<rsg_type>::sample_type sample_type;

	class RangeAccrualData : public AbstractData {
	public:
		RangeAccrualData(){};
		RangeAccrualData(QuantLib::Date asOfDate, quantoSauros::Period period, 
			QuantLib::TimeGrid timeGrid, QuantLib::Date maturityDate, int numOfIR,
			QuantLib::Time startTime,
			//상품정보
			std::vector<double> rangeUpperRate, std::vector<double> rangeLowerRate,
			double inCouponRate, double outCouponRate,
			//이자율 정보
			std::vector<double> floatCurveTenors,
			std::vector<quantoSauros::RateType> rateTypes,
			std::vector<QuantLib::Frequency> swapCouponFrequencies,
			std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
			//헐화이트정보
			sample_type* shortRatePath, 
			std::vector<quantoSauros::HullWhiteParameters> IRParams)
				: AbstractData(asOfDate, timeGrid, period, maturityDate, numOfIR) {

				m_startTime = startTime;
				m_floatCurveTenors = floatCurveTenors;
				m_IRParams = IRParams;
				m_rateTypes = rateTypes;
				m_swapCouponFrequencies = swapCouponFrequencies;
				m_floatTermStructure = floatTermStructures;
				m_rangeUpperRate = rangeUpperRate;
				m_rangeLowerRate = rangeLowerRate;
				m_inCouponRate = inCouponRate;
				m_outCouponRate = outCouponRate;
				savePathsInfo(shortRatePath);
				init();
		};
		virtual double getReferenceRate(int IRIndex){
			return m_referenceRates[m_timeGrid.size() - 2][IRIndex];
		};
		virtual double getPayoffs(){
			//TODO
			return m_accrualDt * m_inCouponRate;
		};
		virtual double getDiscountFactor(){
			return m_cumulatedDF;
		};
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
						vol = m_IRParams[i].getVolatility1F();
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
							vol = m_IRParams[i].getVolatility1F();
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
		virtual void calculateAccrualDays(){
			for (Size timeIndex = 0; timeIndex < m_timeGrid.size() - 1; timeIndex++){
				if (m_numOfIR == 1){
					double upperRate1 = m_rangeUpperRate[0];
					double lowerRate1 = m_rangeLowerRate[0];
					if (m_referenceRates[timeIndex][0] >= lowerRate1 
						&& m_referenceRates[timeIndex][0] <= upperRate1) {										
							m_accrualDt = m_accrualDt + m_timeGrid.dt(timeIndex);							
					}
				} else if (m_numOfIR == 2){
					double upperRate1 = m_rangeUpperRate[0];
					double lowerRate1 = m_rangeLowerRate[0];
					double upperRate2 = m_rangeUpperRate[1];
					double lowerRate2 = m_rangeLowerRate[1];
					if (m_referenceRates[timeIndex][0] >= lowerRate1 
						&& m_referenceRates[timeIndex][0] <= upperRate1) {
						if (m_referenceRates[timeIndex][1] >= lowerRate2 
							&& m_referenceRates[timeIndex][1] <= upperRate2) {
								m_accrualDt = m_accrualDt + m_timeGrid.dt(timeIndex);
						}
					}
				}
			}
		}
	private:
		sample_type* m_shortRatePath;
		QuantLib::Time m_startTime;
		std::vector<double> m_floatCurveTenors;
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;
		std::vector<HullWhiteParameters> m_IRParams;
		std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;

		std::vector<double> m_rangeUpperRate;
		std::vector<double> m_rangeLowerRate;
	};
}