#pragma once
#include <ql/quantlib.hpp>
#include "Period.h";
#include "InterestRateCurve.h";
#include "VolatilitySurface.h";

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
		//������ ����
		QuantLib::Date m_today;
		//quantoSauros::InterestRateCurve m_floatCurve;
		//quantoSauros::VolatilitySurface m_volatilitySurface;

		//�⺻ ����
		QuantLib::Money m_notional;
		QuantLib::Date m_issueDate;
		QuantLib::Date m_maturityDate;
		QuantLib::DayCounter m_dcf;
		bool m_includePrincipal;
		//���رݸ�1 ����
		double m_floatCurveTenor1;
		QuantLib::Frequency m_swapCouponFrequency;
		//Range ���� ����
		std::vector<double> m_inCouponRates;
		std::vector<double> m_outCouponRates;
		std::vector<quantoSauros::Period> m_rangePeriods;
		std::vector<double> m_rangeUpperRates; 
		std::vector<double> m_rangeLowerRates;
		//��� ����
		std::vector<QuantLib::Date> m_exerciseDates;
		std::vector<double> m_exercisePrices;
		QuantLib::Option::Type m_optionType;
		//��Ÿ ����
		int m_monitorFrequency;
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

