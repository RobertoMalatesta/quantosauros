#pragma once
#include "AbstractPricer.h";
#include "CouponBondArguments.h";
#include "LongstaffSchwartz.h";
#include "CouponBondData.h"

namespace quantoSauros{
	class CouponBondPricer : public AbstractCouponBondPricer {
		public:
			CouponBondPricer(quantoSauros::CouponBondArguments args) : 
				AbstractCouponBondPricer(args.getAsOfDate(), args.getSimulationNum(),
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
				m_data = CouponBondData(m_args.getAsOfDate(), m_args.getMaturityDate(),
					m_args.getDayCounter(), 
					m_args.getSpread(), m_args.getLeverage(),
					m_args.getFloatCurveTenors(), m_args.getRateTypes(),
					m_args.getSwapCouponFrequencies(),
					m_floatTermStructure, 
					m_IRParams,
					m_args.getHullWhiteVolatilities(),
					m_args.getDiscountHullWhiteVolatility(),
					m_simulationNum, m_periodNum);

			}
			virtual void dividePeriods();
			virtual void generatePaths();
			virtual void calculatePrice();
		private:
			quantoSauros::CouponBondArguments m_args;

			std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;
			quantoSauros::InterestRateCurve m_discountTermStructure;

			std::vector<HullWhiteParameters> m_IRParams;
			HullWhiteParameters m_discountParams;

			quantoSauros::CouponBondData m_data;
	};
}