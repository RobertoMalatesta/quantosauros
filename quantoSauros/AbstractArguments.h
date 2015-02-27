#pragma once
#include <ql/quantlib.hpp>
#include "LegAmortizationInfo.h";
#include "LegCouponInfo.h";
#include "LegDataInfo.h";
#include "LegOptionInfo.h";
#include "LegScheduleInfo.h";
#include "IRInfo.h";
#include "CorrelationInfo.h";

namespace quantoSauros {
	
	class AbstractArguments {
	public:
		AbstractArguments(){};
		AbstractArguments(quantoSauros::AbstractLegScheduleInfo* scheduleInfo,
			quantoSauros::AbstractLegAmortizationInfo* amortizationInfo,			
			quantoSauros::AbstractLegDataInfo* dataInfo,
			quantoSauros::AbstractLegOptionInfo* optionInfo){

				//상품 기본 정보
				m_notional = amortizationInfo->getNotional();
				m_issueDate = scheduleInfo->getIssueDate();
				m_maturityDate = scheduleInfo->getMaturityDate();
				m_dcf = scheduleInfo->getDayCounter();				
				
				//구간 정보
				m_periods = scheduleInfo->getPeriods();		

				//행사 정보
				m_optionType = optionInfo->getOptionType();

				//기타 정보
				m_monitorFrequency = dataInfo->getMonitorFrequency();				
		};

		QuantLib::Date getAsOfDate();
		void setAsOfDate(QuantLib::Date asOfDate);
		QuantLib::Date getIssueDate();
		void setIssueDate(QuantLib::Date issueDate);
		QuantLib::Date getMaturityDate();
		void setMaturityDate(QuantLib::Date maturityDate);
		QuantLib::Money getNotional();
		void setNotional(QuantLib::Money notional);
		QuantLib::DayCounter getDayCounter();
		void setDayCounter(QuantLib::DayCounter dcf);
		std::vector<quantoSauros::Period> getPeriods();
		void setPeriods(std::vector<quantoSauros::Period> periods);
		int getMonitorFrequency();
		QuantLib::Matrix getCorrelationMatrix();
		int getSimulationNum();
		void setHullWhiteVolatilities(std::vector<QuantLib::HullWhiteVolatility> hwVolatilities);
		std::vector<QuantLib::HullWhiteVolatility> getHullWhiteVolatilities();
		void setDiscountHullWhiteVolatility(QuantLib::HullWhiteVolatility discountHWVolatility);
		QuantLib::HullWhiteVolatility getDiscountHullWhiteVolatility();
		std::vector<double> getFloatCurveTenors();
		std::vector<quantoSauros::RateType> getRateTypes();
		std::vector<QuantLib::Frequency> getSwapCouponFrequencies();
		std::vector<quantoSauros::IRInfo> getIRInfos();
		quantoSauros::IRInfo getDiscountInfo();

		//Range Accrual Arguments
		/*
		virtual std::vector<QuantLib::Rate> getInCouponRates() const = 0;
		virtual void setInCouponRates(std::vector<QuantLib::Rate> inCouponRates) const = 0;
		virtual std::vector<QuantLib::Rate> getOutCouponRates() const = 0;
		virtual void setOutCouponRates(std::vector<QuantLib::Rate> outCouponRates) const = 0;
		virtual std::vector<std::vector<QuantLib::Rate>> getRangeUpperRates() const = 0;
		virtual void setRangeUpperRates(std::vector<std::vector<QuantLib::Rate>> rangeUpperRates) const = 0;
		virtual std::vector<std::vector<QuantLib::Rate>> getRangeLowerRates() const = 0;
		virtual void setRangeLowerRates(std::vector<std::vector<QuantLib::Rate>> rangeLowerRates) const = 0;
		*/

	protected:		
		QuantLib::Date m_asOfDate;
		QuantLib::Date m_issueDate;
		QuantLib::Date m_maturityDate;
		QuantLib::Money m_notional;
		QuantLib::DayCounter m_dcf;
		std::vector<quantoSauros::Period> m_periods;
		int m_monitorFrequency;
		int m_simulationNum;

		//시장데이터 정보		
		std::vector<quantoSauros::IRInfo> m_irInfos;
		quantoSauros::IRInfo m_discountInfo;	
		QuantLib::Matrix m_correlationMatrix;

		//상품 기본정보
		bool m_includePrincipal;

		//행사 정보
		QuantLib::Option::Type m_optionType;

		//헐화이트 변동성
		std::vector<QuantLib::HullWhiteVolatility> m_hwVolatilities;
		QuantLib::HullWhiteVolatility m_discountHWVolatility;

		//기준금리 정보
		std::vector<quantoSauros::RateType> m_rateTypes;
		std::vector<double> m_floatCurveTenors;
		std::vector<QuantLib::Frequency> m_swapCouponFrequencies;
	};

	inline QuantLib::Date AbstractArguments::getAsOfDate(){
		return m_asOfDate;
	}
	inline void AbstractArguments::setAsOfDate(QuantLib::Date asOfDate){
		m_asOfDate = asOfDate;
	}
	inline QuantLib::Date AbstractArguments::getIssueDate(){
		return m_issueDate;
	}
	inline void AbstractArguments::setIssueDate(QuantLib::Date issueDate){
		m_issueDate = issueDate;
	}
	inline QuantLib::Date AbstractArguments::getMaturityDate(){
		return m_maturityDate;
	}
	inline void AbstractArguments::setMaturityDate(QuantLib::Date maturityDate){
		m_maturityDate = maturityDate;
	}
	inline QuantLib::Money AbstractArguments::getNotional(){
		return m_notional;
	}
	inline void AbstractArguments::setNotional(QuantLib::Money notional){
		m_notional = notional;
	}
	inline QuantLib::DayCounter AbstractArguments::getDayCounter(){
		return m_dcf;
	}
	inline void AbstractArguments::setDayCounter(QuantLib::DayCounter dcf){
		m_dcf = dcf;
	}
	inline std::vector<quantoSauros::Period> AbstractArguments::getPeriods(){
		return m_periods;
	}
	inline void AbstractArguments::setPeriods(std::vector<quantoSauros::Period> periods){
		m_periods = periods;
	}
	inline int AbstractArguments::getMonitorFrequency(){
		return m_monitorFrequency;
	}
	inline QuantLib::Matrix AbstractArguments::getCorrelationMatrix(){
		return m_correlationMatrix;
	}
	inline int AbstractArguments::getSimulationNum(){
		return m_simulationNum;
	}
	inline void AbstractArguments::setHullWhiteVolatilities(
		std::vector<QuantLib::HullWhiteVolatility> hwVolatilities){
		m_hwVolatilities = hwVolatilities;
	}
	inline std::vector<QuantLib::HullWhiteVolatility> AbstractArguments::getHullWhiteVolatilities(){
		return m_hwVolatilities;
	}
	inline void AbstractArguments::setDiscountHullWhiteVolatility(
		QuantLib::HullWhiteVolatility discountHWVolatility){
		m_discountHWVolatility = discountHWVolatility;
	}
	inline QuantLib::HullWhiteVolatility AbstractArguments::getDiscountHullWhiteVolatility(){
		return m_discountHWVolatility;
	}
	inline std::vector<double> AbstractArguments::getFloatCurveTenors(){
		return m_floatCurveTenors;
	}
	inline std::vector<quantoSauros::RateType> AbstractArguments::getRateTypes(){
		return m_rateTypes;
	}
	inline std::vector<QuantLib::Frequency> AbstractArguments::getSwapCouponFrequencies(){
		return m_swapCouponFrequencies;
	}
	inline std::vector<quantoSauros::IRInfo> AbstractArguments::getIRInfos(){
		return m_irInfos;
	}
	inline quantoSauros::IRInfo AbstractArguments::getDiscountInfo(){
		return m_discountInfo;
	}
}