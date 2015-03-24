#pragma once
#include <ql/quantlib.hpp>
#include "AbstractData.h";

namespace quantoSauros {

	class CouponBondData : public AbstractData {
	public:
		CouponBondData(){};
		CouponBondData(QuantLib::Date asOfDate, 
			QuantLib::Date maturityDate,
			QuantLib::DayCounter dcf,
			//상품정보
			QuantLib::Rate spread, QuantLib::Real leverage,
			//이자율 정보
			std::vector<double> floatCurveTenors,
			std::vector<quantoSauros::RateType> rateTypes,
			std::vector<QuantLib::Frequency> swapCouponFrequencies,
			//이자율텀스트럭쳐
			std::vector<quantoSauros::InterestRateCurve> floatTermStructures,			
			//헐화이트 정보
			std::vector<quantoSauros::HullWhiteParameters> IRParams,
			std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
			QuantLib::HullWhiteVolatility discountHullWhiteVolatility,
			//기타 정보
			int simNum, int periodNum)
				: AbstractData(asOfDate, maturityDate, dcf, simNum, periodNum) {
					m_spread = spread;
					m_leverage = leverage;
					m_floatCurveTenors =  floatCurveTenors;
					m_rateTypes = rateTypes;
					m_swapCouponFrequencies = swapCouponFrequencies;
					m_IRParams = IRParams;
					m_hullWhiteVolatilities = hullWhiteVolatilities;
					m_discountHullWhiteVolatility = discountHullWhiteVolatility;

					m_floatTermStructure = floatTermStructures;

					m_numOfIR = floatTermStructures.size();

					//vector 사이즈 지정
					m_cumulatedDF.resize(simNum);					
					m_referenceRates.resize(simNum);
					for (int i = 0; i < simNum; i++){
						m_cumulatedDF[i].resize(periodNum);					
						m_referenceRates[i].resize(periodNum);
					}

		}

		virtual std::vector<double> getLSMCData(int simIndex, int periodIndex, int IRIndex){
			std::vector<double> data;
			std::vector<std::vector<Rate>> referenceRates = m_referenceRates[simIndex][periodIndex];

			data.push_back(1);
			data.push_back(referenceRates[m_timeGrids[simIndex][periodIndex].size() - 2][IRIndex]);

			QuantLib::Date swapStartDate = m_periods[simIndex][periodIndex].getEndDate();
			QuantLib::Date swapEndDate = m_maturityDate;
			if (swapStartDate != swapEndDate){
				double coterminalSwapRate = m_floatTermStructure[IRIndex].
					getForwardSwapRate(swapStartDate, swapEndDate);
				data.push_back(coterminalSwapRate);
			}			

			return data;
		}
		virtual double getPayoffs(int simIndex, int periodIndex){
			//TODO
			return 0;
		}
		virtual double getDiscountFactor(int simIndex, int periodIndex){
			return m_cumulatedDF[simIndex][periodIndex];
		}

	protected:
		virtual void calcReferenceRates(int simIndex, int periodIndex){
			//TODO
			QuantLib::TimeGrid timeGrid = m_timeGrids[simIndex][periodIndex];

			std::vector<std::vector<Rate>> referenceRates(timeGrid.size() - 1);
			for (Size timeIndex = 0; timeIndex < timeGrid.size() - 1; timeIndex++){
				//4.1. Calculate the reference Rate
				QuantLib::Time start = timeGrid[timeIndex] + m_startTimes[simIndex][periodIndex];				
				referenceRates[timeIndex].resize(m_numOfIR);

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
							m_paths[simIndex][periodIndex]->value[i][timeIndex]);
						referenceRates[timeIndex][i] = - log(bondPrice) / tenor;
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
								m_paths[simIndex][periodIndex]->value[i][timeIndex]);
							bondPriceSum += bondPrice * swapTenor;
						}								
						referenceRates[timeIndex][i] = (1 - bondPrice) / bondPriceSum;
					}							
				}
				m_referenceRates[simIndex][periodIndex] = referenceRates;
			}
		}
		virtual void calculateDiscountFactor(int simIndex, int periodIndex){
			QuantLib::TimeGrid timeGrid = m_timeGrids[simIndex][periodIndex];
			m_cumulatedDF[simIndex][periodIndex] = 1;

			for (Size timeIndex = 0; timeIndex < timeGrid.size() - 1; timeIndex++){

				//double shortRate = m_shortRatePath->value[0][timeIndex];
				double dfRate = m_paths[simIndex][periodIndex]->value[m_numOfIR][timeIndex];
				m_cumulatedDF[simIndex][periodIndex] *= exp(- dfRate * timeGrid.dt(timeIndex));
			}
		}
		virtual void calculatePayoff(int simIndex, int periodIndex){
			//TODO
		};

	private:		
		QuantLib::Rate m_spread;
		QuantLib::Real m_leverage;

		std::vector<double> m_floatCurveTenors;
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;

		std::vector<quantoSauros::HullWhiteParameters> m_IRParams;
		std::vector<QuantLib::HullWhiteVolatility> m_hullWhiteVolatilities;
		QuantLib::HullWhiteVolatility m_discountHullWhiteVolatility;
		std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;

		int m_numOfIR;

		std::vector<std::vector<double>> m_cumulatedDF;
		std::vector<std::vector<std::vector<std::vector<Rate>>>> m_referenceRates;


	};

	/*
	class CouponBondData : public AbstractData {
		public:
			CouponBondData(){};
			//For Float
			CouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,
				//헐화이트정보
				sample_type* shortRatePath, 
				std::vector<quantoSauros::HullWhiteParameters> IRParams,
				std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility,
				//이자율 텀스트럭쳐
				std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
				//이자율 정보
				std::vector<double> floatCurveTenors,
				std::vector<quantoSauros::RateType> rateTypes,
				std::vector<QuantLib::Frequency> swapCouponFrequencies)
					: AbstractData(asOfDate, maturityDate, period, 
						startTime, timeGrid, dcf,						
						shortRatePath, IRParams, 
						hullWhiteVolatilities, discountHullWhiteVolatility,
						floatTermStructures,
						floatCurveTenors, rateTypes, swapCouponFrequencies) {

					m_spread = spread;
					m_leverage = leverage;
					init();

			};
			//For Fixed
			CouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,				
				//헐화이트정보
				sample_type* shortRatePath)
					: AbstractData(asOfDate, maturityDate, period, 
						startTime, timeGrid, dcf,						
						shortRatePath){

					m_spread = spread;
					m_leverage = leverage;
					init();
			};

			virtual double getDiscountFactor(){
				return m_cumulatedDF;
			};
			
		protected:

			virtual void calculatePayoff(){
				
			}
			QuantLib::Rate m_spread;
			QuantLib::Real m_leverage;
			
	};

	class FixedCouponBondData : public CouponBondData {
		public:
			FixedCouponBondData(){};
			FixedCouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,
				QuantLib::Rate coupon,
				//헐화이트정보
				sample_type* shortRatePath, 
				//std::vector<quantoSauros::HullWhiteParameters> IRParams,
				//std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility) 
				:  CouponBondData(asOfDate, maturityDate, period, startTime, timeGrid, dcf,
					spread, leverage, shortRatePath){  
						m_coupon = coupon;
			};

		protected:
			virtual double getPayoffs(){
				return m_leverage * (m_coupon + m_spread) * m_period.getPeriodTenor(m_dcf);			
			};
			virtual void calcReferenceRates(){};

		private:
			QuantLib::Rate m_coupon;
	};

	class FloatCouponBondData : public CouponBondData {
		public:
			FloatCouponBondData(){};
			FloatCouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보				
				QuantLib::Rate spread, QuantLib::Real leverage,
				//헐화이트정보
				sample_type* shortRatePath, 
				std::vector<quantoSauros::HullWhiteParameters> IRParams,
				std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility,
				//이자율 텀스트럭쳐
				std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
				//이자율 정보
				std::vector<double> floatCurveTenors,
				std::vector<quantoSauros::RateType> rateTypes,
				std::vector<QuantLib::Frequency> swapCouponFrequencies)
				: CouponBondData(asOfDate, maturityDate, period, 
					startTime, timeGrid, dcf,
					spread, leverage, 
					shortRatePath, IRParams, 
					hullWhiteVolatilities, discountHullWhiteVolatility,
					floatTermStructures,
					floatCurveTenors, rateTypes, swapCouponFrequencies){		

			};

		protected:
			virtual double getPayoffs(){
				return m_leverage * (m_referenceRates[0][0] + m_spread) *
					m_period.getPeriodTenor(m_dcf);			
			};
		private:

	};
	*/
}
