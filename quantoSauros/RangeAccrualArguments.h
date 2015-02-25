#pragma once
#include <ql/quantlib.hpp>
#include "AbstractArguments.h"


namespace quantoSauros {
	class RangeAccrualArguments : public AbstractArguments {
	public:
		RangeAccrualArguments(){};
		RangeAccrualArguments(quantoSauros::NoteLegScheduleInfo* scheduleInfo,
			quantoSauros::NoteLegAmortizationInfo* amortizationInfo,
			std::vector<quantoSauros::NoteLegRangeCouponInfo*> couponInfos,
			quantoSauros::NoteLegDataInfo* dataInfo,
			quantoSauros::NoteLegOptionInfo* optionInfo,
			//���رݸ� ����
			std::vector<quantoSauros::IRInfo> irInfos,
			//���αݸ� ����
			quantoSauros::IRInfo discountInfo,
			//������ ����
			quantoSauros::CorrelationInfo correlationInfo,
			//��Ÿ
			int simulationNum
			){
				//��ǰ �⺻ ����				
				m_notional = amortizationInfo->getNotional();
				m_issueDate = scheduleInfo->getIssueDate();
				m_maturityDate = scheduleInfo->getMaturityDate();
				m_dcf = scheduleInfo->getDayCounter();				
				m_includePrincipal = amortizationInfo->getIncludePrincipal();
				
				//Range ���� ����, ���رݸ� ����
				m_periods = scheduleInfo->getPeriods();				
				
				for (int i = 0; i <couponInfos.size(); i++){
					if (couponInfos[i]->getClassName() == "NoteLegSpreadRangeCouponInfo"){
						//TODO
					} else if (couponInfos[i]->getClassName() == "NoteLegSpotRangeCouponInfo"){
						m_rateTypes.push_back(couponInfos[i]->getRateType1());
						m_floatCurveTenors.push_back(couponInfos[i]->getTenor1());
						m_swapCouponFrequencies.push_back(couponInfos[i]->getSwapCouponFrequency1());
					}	
				}

				//range coupon
				for (int i = 0; i < m_periods.size(); i++){
					std::vector<double> tmpRangeUpperRate(couponInfos.size());
					std::vector<double> tmpRangeLowerRate(couponInfos.size());
					for (int j = 0; j < couponInfos.size(); j++){
						std::vector<double> tmpUpperBounds = couponInfos[j]->getUpperBounds();
						std::vector<double> tmpLowerBounds = couponInfos[j]->getLowerBounds();												
						
						tmpRangeUpperRate[j] = tmpUpperBounds[i];
						tmpRangeLowerRate[j] = tmpLowerBounds[i];
					}
					m_rangeUpperRates.push_back(tmpRangeUpperRate);
					m_rangeLowerRates.push_back(tmpRangeLowerRate);
				}

				m_inCouponRates = couponInfos[0]->getInCouponRates();
				m_outCouponRates = couponInfos[0]->getOutCouponRates();
								
				//��� ����
				m_optionType = optionInfo->getOptionType();
				
				//���嵥���� ����
				m_correlationMatrix = correlationInfo.getCorrelationMatrix();
				m_irInfos = irInfos;
				m_discountInfo = discountInfo;

				//��Ÿ ����
				m_monitorFrequency = dataInfo->getMonitorFrequency();
				m_accruedCoupon = dataInfo->getAccruedCoupon();
				m_simulationNum = simulationNum;
		};

		std::vector<QuantLib::Rate> getInCouponRates(){
			return m_inCouponRates;
		}
		void setInCouponRates(std::vector<QuantLib::Rate> inCouponRates){
			m_inCouponRates = inCouponRates;
		}
		std::vector<QuantLib::Rate> getOutCouponRates(){
			return m_outCouponRates;
		}
		void setOutCouponRates(std::vector<QuantLib::Rate> outCouponRates){
			m_outCouponRates = outCouponRates;
		}
		std::vector<std::vector<QuantLib::Rate>> getRangeUpperRates(){
			return m_rangeUpperRates;
		}
		void setRangeUpperRates(std::vector<std::vector<QuantLib::Rate>> rangeUpperRates){
			m_rangeUpperRates = rangeUpperRates;
		}
		std::vector<std::vector<QuantLib::Rate>> getRangeLowerRates(){
			return m_rangeLowerRates;
		}
		void setRangeLowerRates(std::vector<std::vector<QuantLib::Rate>> rangeLowerRates){
			m_rangeLowerRates = rangeLowerRates;
		}

	private:
		//Range ���� ����
		std::vector<std::vector<QuantLib::Rate>> m_rangeUpperRates; 
		std::vector<std::vector<QuantLib::Rate>> m_rangeLowerRates;
		std::vector<QuantLib::Rate> m_inCouponRates;
		std::vector<QuantLib::Rate> m_outCouponRates;		
				
		//Range ��Ÿ ����
		double m_accruedCoupon;		
			
	};
}