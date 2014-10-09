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
			//���رݸ� ����
			std::vector<quantoSauros::IRInfo> irInfos,
			//���αݸ� ����
			quantoSauros::IRInfo discountInfo,
			//������ ����
			quantoSauros::CorrelationInfo correlationInfo
			);

		QuantLib::Money getPrice(
			//��¥����
			QuantLib::Date today, 
			//���رݸ� ����
			std::vector<quantoSauros::IRInfo> irInfos,
			//���αݸ� ����
			quantoSauros::IRInfo discountInfo,
			//��Ÿ
			int simulationNum
			);
		~RangeAccrualNote(void);

	private:
		void DividingPeriods();
		void generateSeeds();
		
	};
}

