#pragma once
#include "AbstractPricer.h";
#include "AbstractRangeAccrualNote.h";
#include "RangeAccrualArguments.h";
#include "LongstaffSchwartz.h";
#include "RangeAccrualData.h"

namespace quantoSauros{
	class RangeAccrualPricer : public AbstractAccrualPricer {
	public:
		RangeAccrualPricer(quantoSauros::RangeAccrualArguments args){
			m_args = args;
		};
		/*
		void calculate(){
			init();
			generateSeeds();
			generatePaths();
			calculatePrice();
		}
		*/
	protected:
		void init(){
			//TODO
			m_today = m_args.getAsOfDate();
			m_simulationNum = m_args.getSimulationNum();
			m_periodNum = m_args.getRangePeriods().size();
			m_payoffs = std::vector<std::vector<double>>(m_periodNum + 1);
			m_timeGridSize = ceil(m_args.getRangePeriods()[0].getPeriodTenor(m_args.getDayCounter()) 
				/ ((double)m_args.getMonitorFrequency() / 360));
			m_irNum = m_args.getIRInfos().size();
			
			//이자율커브 셋팅
			/*m_floatTermStructure = std::vector<Handle<YieldTermStructure>>(m_irNum);*/
			m_floatTermStructure = std::vector<quantoSauros::InterestRateCurve>(m_irNum);
			m_IRParams = std::vector<HullWhiteParameters>(m_irNum);			
			for (int i = 0; i < m_irNum; i++){
				m_IRParams[i] = m_args.getIRInfos()[i].getHullWhiteParams();				
				/*m_floatTermStructure[i] = Handle<YieldTermStructure>(
					m_args.getIRInfos()[i].getInterestRateCurve().getInterestRateCurve());*/
				m_floatTermStructure[i] = quantoSauros::InterestRateCurve(
					m_args.getIRInfos()[i].getInterestRateCurve());
			}
			m_discountParams = m_args.getDiscountInfo().getHullWhiteParams(); 			
			/*m_discountTermStructure = Handle<YieldTermStructure>(
				m_args.getDiscountInfo().getInterestRateCurve().getInterestRateCurve());*/
			m_discountTermStructure = quantoSauros::InterestRateCurve(
				m_args.getDiscountInfo().getInterestRateCurve());

			m_coupons = std::vector<std::vector<double>>(m_simulationNum);
			m_dfs = std::vector<std::vector<double>>(m_simulationNum);
			m_data = std::vector<std::vector<quantoSauros::RangeAccrualData>>(m_simulationNum);
			for (int i = 0; i < m_simulationNum; i++){
				m_data[i] = std::vector<quantoSauros::RangeAccrualData>(m_periodNum);
			}
			dividePeriods();
		}
		virtual void dividePeriods();
		virtual void generatePaths();
		virtual void calculatePrice();
		
	private:
		//void dividePeriods();		
		std::vector<std::vector<double>> m_coupons;
		std::vector<std::vector<double>> m_dfs;
		quantoSauros::RangeAccrualArguments m_args;

		std::vector<std::vector<double>> m_payoffs;
		/*std::vector<Handle<YieldTermStructure>> m_floatTermStructure;
		Handle<YieldTermStructure> m_discountTermStructure;*/
		std::vector<quantoSauros::InterestRateCurve> m_floatTermStructure;
		quantoSauros::InterestRateCurve m_discountTermStructure;

		std::vector<HullWhiteParameters> m_IRParams;
		HullWhiteParameters m_discountParams;

		std::vector<std::vector<quantoSauros::RangeAccrualData>> m_data;
	};

	class DualRangeAccrualPricer : public AbstractAccrualPricer {

	};

}

/*
1. seed 생성
2. Path Generating module
3. Pricer Calculating module

*/