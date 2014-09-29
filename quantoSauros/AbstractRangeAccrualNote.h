#pragma once
#include <ql/quantlib.hpp>
#include "Period.h";
#include "InterestRateCurve.h";
#include "VolatilitySurface.h";
#include "HullWhiteParameters.h";
#include "LegAmortizationInfo.h";
#include "LegCouponInfo.h";
#include "LegDataInfo.h";
#include "LegOptionInfo.h";
#include "LegScheduleInfo.h";
#include "IRInfo.h";
#include "CorrelationInfo.h";

namespace quantoSauros {

	class AbstractRangeAccrualNote {
	public:
		AbstractRangeAccrualNote(){}
		QuantLib::Money getNotional();
		QuantLib::Date getIssueDate();
		QuantLib::Date getMaturityDate();
		QuantLib::DayCounter getDayCounter();
		bool getIncludePrincipal();


	protected:
		//측정일 정보
		QuantLib::Date m_today;
		//quantoSauros::InterestRateCurve m_floatCurve;
		//quantoSauros::VolatilitySurface m_volatilitySurface;

		//기본 정보
		QuantLib::Money m_notional;
		QuantLib::Date m_issueDate;
		QuantLib::Date m_maturityDate;
		QuantLib::DayCounter m_dcf;
		bool m_includePrincipal;
		//기준금리1 정보
		double m_floatCurveTenor1;
		QuantLib::Frequency m_swapCouponFrequency;
		//Range 구간 정보
		std::vector<double> m_inCouponRates;
		std::vector<double> m_outCouponRates;
		std::vector<quantoSauros::Period> m_rangePeriods;
		std::vector<double> m_rangeUpperRates; 
		std::vector<double> m_rangeLowerRates;
		//행사 정보
		//std::vector<QuantLib::Date> m_exerciseDates;
		//std::vector<double> m_exercisePrices;
		QuantLib::Option::Type m_optionType;
		//기타 정보
		int m_monitorFrequency;
		double m_accruedCoupon;
		QuantLib::Matrix m_correlationMatrix;

		//seed
		std::vector<std::vector<BigNatural>> m_seeds;
	};
	
	inline QuantLib::Money AbstractRangeAccrualNote::getNotional(){
		return m_notional;
	}
	inline QuantLib::Date AbstractRangeAccrualNote::getIssueDate(){
		return m_issueDate;
	}
	inline QuantLib::Date AbstractRangeAccrualNote::getMaturityDate(){
		return m_maturityDate;
	}
	inline QuantLib::DayCounter AbstractRangeAccrualNote::getDayCounter(){
		return m_dcf;
	}
	inline bool AbstractRangeAccrualNote::getIncludePrincipal(){
		return m_includePrincipal;
	}
}

