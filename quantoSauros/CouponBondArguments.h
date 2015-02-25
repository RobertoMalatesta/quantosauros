#pragma once
#include <ql/quantlib.hpp>
#include "AbstractArguments.h"

namespace quantoSauros {
	class CouponBondArguments : public AbstractArguments {
		public:
			CouponBondArguments(){};
			CouponBondArguments(quantoSauros::NoteLegScheduleInfo* scheduleInfo,
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
				int simulationNum){
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
					//TODO : CouponINFO Ŭ�������� ���� ���
					/*
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
					*/

					//��� ����
					m_optionType = optionInfo->getOptionType();

					//���嵥���� ����
					m_correlationMatrix = correlationInfo.getCorrelationMatrix();
					m_irInfos = irInfos;
					m_discountInfo = discountInfo;

					//��Ÿ ����
					m_monitorFrequency = dataInfo->getMonitorFrequency();
					//m_accruedCoupon = dataInfo->getAccruedCoupon();
					m_simulationNum = simulationNum;
			};
	};
}