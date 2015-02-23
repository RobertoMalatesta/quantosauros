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
		}

		std::vector<quantoSauros::Period> getPeriods(){
			return m_periods;
		}
		void setPeriods(std::vector<quantoSauros::Period> periods){
			m_periods = periods;
		}
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
		int getSimulationNum(){
			return m_simulationNum;
		}
		std::vector<quantoSauros::IRInfo> getIRInfos(){
			return m_irInfos;
		}
		quantoSauros::IRInfo getDiscountInfo(){
			return m_discountInfo;
		}
		void setHullWhiteVolatilities(std::vector<QuantLib::HullWhiteVolatility> hwVolatilities){
			m_hwVolatilities = hwVolatilities;
		}
		std::vector<QuantLib::HullWhiteVolatility> getHullWhiteVolatilities(){
			return m_hwVolatilities;
		}
		void setDiscountHullWhiteVolatility(QuantLib::HullWhiteVolatility discountHWVolatility){
			m_discountHWVolatility = discountHWVolatility;
		}
		QuantLib::HullWhiteVolatility getDiscountHullWhiteVolatility(){
			return m_discountHWVolatility;
		}

	private:
		//상품 기본정보
		bool m_includePrincipal;

		//기준금리 정보
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<double> m_floatCurveTenors;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;

		//Range 구간 정보
		std::vector<quantoSauros::Period> m_periods;
		std::vector<std::vector<QuantLib::Rate>> m_rangeUpperRates; 
		std::vector<std::vector<QuantLib::Rate>> m_rangeLowerRates;
		std::vector<QuantLib::Rate> m_inCouponRates;
		std::vector<QuantLib::Rate> m_outCouponRates;		
				
		//행사 정보
		QuantLib::Option::Type m_optionType;

		//기타 정보
		double m_accruedCoupon;
		int m_monitorFrequency;
		int m_simulationNum;

		//시장데이터 정보
		QuantLib::Matrix m_correlationMatrix;
		std::vector<quantoSauros::IRInfo> m_irInfos;
		quantoSauros::IRInfo m_discountInfo;

		//헐화이트 변동성
		std::vector<QuantLib::HullWhiteVolatility> m_hwVolatilities;
		QuantLib::HullWhiteVolatility m_discountHWVolatility;
	};
}