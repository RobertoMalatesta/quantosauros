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
				//기준금리 정보
				std::vector<quantoSauros::IRInfo> irInfos,
				//할인금리 정보
				quantoSauros::IRInfo discountInfo,
				//상관계수 정보
				quantoSauros::CorrelationInfo correlationInfo,
				//기타
				int simulationNum)
					: AbstractArguments(scheduleInfo, amortizationInfo, dataInfo, optionInfo) {

					//상품 기본 정보									
					m_includePrincipal = amortizationInfo->getIncludePrincipal();

					//기준금리정보
					if (couponInfo->getClassName() == "NoteLegFixedCouponInfo"){
						m_coupon = couponInfo->getCoupon();
					} else if (couponInfo->getClassName() == "NoteLegFloatCouponInfo"){
						m_rateTypes.push_back(couponInfo->getRateType1());
						m_floatCurveTenors.push_back(couponInfo->getTenor1());
						m_swapCouponFrequencies.push_back(couponInfo->getSwapCouponFrequency1());						
					}	
					m_spread = couponInfo->getSpread();
					m_leverage = couponInfo->getLeverage();
					
					//TODO : CouponINFO 클래스부터 구현 요망
										
					//시장데이터 정보
					m_correlationMatrix = correlationInfo.getCorrelationMatrix();
					m_irInfos = irInfos;
					m_discountInfo = discountInfo;

					//기타 정보					
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