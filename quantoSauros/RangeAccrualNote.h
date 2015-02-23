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
			//���رݸ� ����
			std::vector<quantoSauros::IRInfo> irInfos,
			//���αݸ� ����
			quantoSauros::IRInfo discountInfo,
			//������ ����
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

