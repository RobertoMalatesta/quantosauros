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
			//기준금리 정보
			std::vector<quantoSauros::IRInfo> irInfos,
			//할인금리 정보
			quantoSauros::IRInfo discountInfo,
			//상관계수 정보
			quantoSauros::CorrelationInfo correlationInfo,
			//기타
			int simulationNum
			){
				//상품 기본 정보				
				m_notional = amortizationInfo->getNotional();
				m_issueDate = scheduleInfo->getIssueDate();
				m_maturityDate = scheduleInfo->getMaturityDate();
				m_dcf = scheduleInfo->getDayCounter();				
				m_includePrincipal = amortizationInfo->getIncludePrincipal();
				
				//Range 구간 정보, 기준금리 정보
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
								
				//행사 정보
				m_optionType = optionInfo->getOptionType();
				
				//시장데이터 정보
				m_correlationMatrix = correlationInfo.getCorrelationMatrix();
				m_irInfos = irInfos;
				m_discountInfo = discountInfo;

				//기타 정보
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
		//Range 구간 정보
		std::vector<std::vector<QuantLib::Rate>> m_rangeUpperRates; 
		std::vector<std::vector<QuantLib::Rate>> m_rangeLowerRates;
		std::vector<QuantLib::Rate> m_inCouponRates;
		std::vector<QuantLib::Rate> m_outCouponRates;		
				
		//Range 기타 정보
		double m_accruedCoupon;		
			
	};
}