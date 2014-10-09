#pragma once
#include <ql/quantlib.hpp>
#include <algorithm> 
#include "AbstractRangeAccrualNote.h";

namespace quantoSauros {

	class RangeAccrualNote : public AbstractRangeAccrualNote {
	public:
		/*
		RangeAccrualNote(
			QuantLib::Money notional, 
			QuantLib::Date issueDate, QuantLib::Date maturityDate, 
			QuantLib::DayCounter dcf, bool includePrincipal,
			double floatCurveTenor1, QuantLib::Frequency swapCouponFrequency1,
			std::vector<double> inCouponRates, std::vector<double> outCouponRates,
			std::vector<quantoSauros::Period> rangePeriods,
			std::vector<double> rangeUpperRates, std::vector<double> rangeLowerRates,			
			QuantLib::Option::Type optionType,
			int monitorFrequency
			);
		*/
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
			quantoSauros::CorrelationInfo correlationInfo
			);

		QuantLib::Money getPrice(
			//날짜정보
			QuantLib::Date today, 
			//기준금리 정보
			std::vector<quantoSauros::IRInfo> irInfos,
			//할인금리 정보
			quantoSauros::IRInfo discountInfo,
			//기타
			int simulationNum
			);
		~RangeAccrualNote(void);

	private:
		void DividingPeriods();
		void generateSeeds();
		
	};
}

