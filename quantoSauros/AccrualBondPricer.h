#pragma once
#include "AbstractPricer.h";
#include "AbstractRangeAccrualNote.h";
#include "RangeAccrualArguments.h";
#include "LongstaffSchwartz.h";
#include "RangeAccrualData.h"
//#include "NewAbstractData.h";

namespace quantoSauros{
	class RangeAccrualPricer : public AbstractAccrualPricer {
		public:
			RangeAccrualPricer(quantoSauros::RangeAccrualArguments args) 
				: AbstractAccrualPricer(args.getAsOfDate(), args.getSimulationNum(),
					args.getIRInfos().size(), args.getPeriods().size()){

						m_args = args;
						
			};

		protected:
			void init(){
				dividePeriods();
		
				m_timeGridSize = ceil(m_args.getPeriods()[0].getPeriodTenor(m_args.getDayCounter()) 
					/ ((double)m_args.getMonitorFrequency() / 360));
						
				//이자율커브 셋팅
				m_floatTermStructure = std::vector<quantoSauros::InterestRateCurve>(m_irNum);
				m_IRParams = std::vector<HullWhiteParameters>(m_irNum);			
				for (int i = 0; i < m_irNum; i++){
					m_IRParams[i] = m_args.getIRInfos()[i].getHullWhiteParams();				
					m_floatTermStructure[i] = quantoSauros::InterestRateCurve(
						m_args.getIRInfos()[i].getInterestRateCurve());
				}
				m_discountParams = m_args.getDiscountInfo().getHullWhiteParams(); 			
				m_discountTermStructure = quantoSauros::InterestRateCurve(
					m_args.getDiscountInfo().getInterestRateCurve());

				//data class
				m_data = RangeAccrualData(m_args.getAsOfDate(), m_args.getMaturityDate(),
					m_args.getDayCounter(), 
					m_args.getRangeUpperRates(), m_args.getRangeLowerRates(),
					m_args.getInCouponRates(), m_args.getOutCouponRates(),
					m_args.getFloatCurveTenors(), m_args.getRateTypes(),
					m_args.getSwapCouponFrequencies(),
					m_floatTermStructure, 
					m_IRParams,
					m_args.getHullWhiteVolatilities(),
					m_args.getDiscountHullWhiteVolatility(),
					m_simulationNum, m_periodNum);

				/*
				m_data = std::vector<std::vector<quantoSauros::RangeAccrualData>>(m_simulationNum);
				for (int i = 0; i < m_simulationNum; i++){
					m_data[i] = std::vector<quantoSauros::RangeAccrualData>(m_periodNum);
				}
				*/
			}
			virtual void dividePeriods();
			virtual void generatePaths();
			virtual void calculatePrice();
		
		private:
			quantoSauros::RangeAccrualArguments m_args;

			std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;
			quantoSauros::InterestRateCurve m_discountTermStructure;

			std::vector<HullWhiteParameters> m_IRParams;
			HullWhiteParameters m_discountParams;

			//std::vector<std::vector<quantoSauros::RangeAccrualData>> m_data;	

			quantoSauros::RangeAccrualData m_data;
	};

	class DualRangeAccrualPricer : public AbstractAccrualPricer {

	};

}

/*
1. seed 생성
2. Path Generating module
3. Pricer Calculating module

*/