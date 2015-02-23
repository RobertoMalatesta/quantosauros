#include "RangeAccrualNote.h"

namespace quantoSauros {

	RangeAccrualNote::RangeAccrualNote(
		quantoSauros::NoteLegScheduleInfo* scheduleInfo,
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
		int simulationNumber){

			m_args = quantoSauros::RangeAccrualArguments(scheduleInfo, 
				amortizationInfo, couponInfos, dataInfo, optionInfo, 
				irInfos, discountInfo, correlationInfo, simulationNumber);

	}

	QuantLib::Money RangeAccrualNote::getPrice(
			QuantLib::Date asOfDate, 
			std::vector<QuantLib::HullWhiteVolatility> hwVolatilities,
			QuantLib::HullWhiteVolatility discountHWVolatility){

		m_args.setAsOfDate(asOfDate);
		m_args.setHullWhiteVolatilities(hwVolatilities);
		m_args.setDiscountHullWhiteVolatility(discountHWVolatility);

		RangeAccrualPricer pricer(m_args);
		pricer.calculate();

		return pricer.getResults();
	}

	RangeAccrualNote::~RangeAccrualNote(void){
	}

}