#pragma once
#include <ql/quantlib.hpp>
#include <algorithm> 
#include "AbstractRangeAccrualNote.h";
#include "AccrualPricer.h";
#include "RangeAccrualArguments.h";

namespace quantoSauros {

	class RangeAccrualNote : public AbstractRangeAccrualNote {
	public:
		RangeAccrualNote(
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
			int simulationNumber);

		QuantLib::Money getPrice(
			QuantLib::Date asOfDate,
			std::vector<QuantLib::HullWhiteVolatility> hwVolatilities,
			QuantLib::HullWhiteVolatility discountHWVolatility);

		~RangeAccrualNote(void);
		
	private:		
		quantoSauros::RangeAccrualArguments m_args;
	};
}

