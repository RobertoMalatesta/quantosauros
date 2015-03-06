#pragma once
#include <ql/quantlib.hpp>
#include "AbstractArguments.h"

namespace quantoSauros {
	class CouponBondArguments : public AbstractArguments {
		public:
			CouponBondArguments(){};
			CouponBondArguments(quantoSauros::NoteLegScheduleInfo* scheduleInfo,
				quantoSauros::NoteLegAmortizationInfo* amortizationInfo,
				quantoSauros::NoteLegCouponInfo* couponInfo,
				quantoSauros::NoteLegDataInfo* dataInfo,
				quantoSauros::NoteLegOptionInfo* optionInfo,
				//���رݸ� ����
				std::vector<quantoSauros::IRInfo> irInfos,
				//���αݸ� ����
				quantoSauros::IRInfo discountInfo,
				//������ ����
				quantoSauros::CorrelationInfo correlationInfo,
				//��Ÿ
				int simulationNum)
					: AbstractArguments(scheduleInfo, amortizationInfo, dataInfo, optionInfo) {

					//��ǰ �⺻ ����									
					m_includePrincipal = amortizationInfo->getIncludePrincipal();

					//���رݸ�����
					if (couponInfo->getClassName() == "NoteLegFixedCouponInfo"){
						m_coupon = couponInfo->getCoupon();
					} else if (couponInfo->getClassName() == "NoteLegFloatCouponInfo"){
						m_rateTypes.push_back(couponInfo->getRateType1());
						m_floatCurveTenors.push_back(couponInfo->getTenor1());
						m_swapCouponFrequencies.push_back(couponInfo->getSwapCouponFrequency1());						
					}	
					m_spread = couponInfo->getSpread();
					m_leverage = couponInfo->getLeverage();
					
					//TODO : CouponINFO Ŭ�������� ���� ���
										
					//���嵥���� ����
					m_correlationMatrix = correlationInfo.getCorrelationMatrix();
					m_irInfos = irInfos;
					m_discountInfo = discountInfo;

					//��Ÿ ����					
					m_simulationNum = simulationNum;
			};

			void setSpread(QuantLib::Rate spread){
				m_spread = spread;
			}
			QuantLib::Rate getSpread(){
				return m_spread;
			}
			void setLeverage(QuantLib::Rate leverage){
				m_leverage = leverage;
			}
			QuantLib::Rate getLeverage(){
				return m_leverage;
			}
			void setCoupon(QuantLib::Rate coupon){
				m_coupon = coupon;
			}
			QuantLib::Rate getCoupon(){
				return m_coupon;
			}

		private:
			QuantLib::Rate m_coupon;
			QuantLib::Rate m_spread;
			QuantLib::Real m_leverage;

	};
}