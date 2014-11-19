#pragma once
#include <ql/quantlib.hpp>
#include "AbstractArguments.h"
#include "LegAmortizationInfo.h";
#include "LegCouponInfo.h";
#include "LegDataInfo.h";
#include "LegOptionInfo.h";
#include "LegScheduleInfo.h";
#include "IRInfo.h";
#include "CorrelationInfo.h";

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
				m_rangePeriods = scheduleInfo->getPeriods();				
				
				for (int i = 0; i <couponInfos.size(); i++){
					if (couponInfos[i]->getClassName() == "NoteLegSpreadRangeCouponInfo"){
						//TODO
					} else if (couponInfos[i]->getClassName() == "NoteLegSpotRangeCouponInfo"){
						m_rateTypes.push_back(couponInfos[i]->getRateType1());
						m_floatCurveTenors.push_back(couponInfos[i]->getTenor1());
						m_swapCouponFrequencies.push_back(couponInfos[i]->getSwapCouponFrequency1());
					}									
					/*
					std::vector<double> tmpUpperBounds = couponInfos[i]->getUpperBounds();
					std::vector<double> tmpLowerBounds = couponInfos[i]->getLowerBounds();
					for (int j = 0; j < tmpUpperBounds.size(); j++){
						m_rangeUpperRates[i].push_back(tmpUpperBounds[j]);
						m_rangeLowerRates[i].push_back(tmpLowerBounds[j]);
					}
					*/
				}

				//range coupon
				for (int i = 0; i < m_rangePeriods.size(); i++){

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
		}

		std::vector<quantoSauros::Period> getRangePeriods(){
			return m_rangePeriods;
		}
		std::vector<double> getInCouponRates(){
			return m_inCouponRates;
		}
		std::vector<double> getOutCouponRates(){
			return m_outCouponRates;
		}
		int getMonitorFrequency(){
			return m_monitorFrequency;
		}
		QuantLib::Matrix getCorrelationMatrix(){
			return m_correlationMatrix;
		}
		std::vector<double> getFloatCurveTenors(){
			return m_floatCurveTenors;
		}
		std::vector<quantoSauros::RateType> getRateTypes(){
			return m_rateTypes;
		}
		std::vector<QuantLib::Frequency> getSwapCouponFrequencies(){
			return m_swapCouponFrequencies;
		}
		std::vector<std::vector<double>> getRangeUpperRates(){
			return m_rangeUpperRates;
		}
		std::vector<std::vector<double>> getRangeLowerRates(){
			return m_rangeLowerRates;
		}
		int getSimulationNum(){
			return m_simulationNum;
		}
		std::vector<quantoSauros::IRInfo> getIRInfos(){
			return m_irInfos;
		}
		quantoSauros::IRInfo getDiscountInfo(){
			return m_discountInfo;
		}
	private:
		//��ǰ �⺻����
		bool m_includePrincipal;

		//���رݸ� ����
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<double> m_floatCurveTenors;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;

		//Range ���� ����
		std::vector<quantoSauros::Period> m_rangePeriods;
		std::vector<std::vector<double>> m_rangeUpperRates; 
		std::vector<std::vector<double>> m_rangeLowerRates;
		std::vector<double> m_inCouponRates;
		std::vector<double> m_outCouponRates;		
				
		//��� ����
		QuantLib::Option::Type m_optionType;

		//��Ÿ ����
		double m_accruedCoupon;
		int m_monitorFrequency;
		int m_simulationNum;

		//���嵥���� ����
		QuantLib::Matrix m_correlationMatrix;
		std::vector<quantoSauros::IRInfo> m_irInfos;
		quantoSauros::IRInfo m_discountInfo;
	};
}