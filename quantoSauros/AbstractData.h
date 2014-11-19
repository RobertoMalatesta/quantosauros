#pragma once
#include <ql/quantlib.hpp>
#include "InterestRate.h"

namespace quantoSauros{

	class AbstractData {
	public:	
		AbstractData(){};
		AbstractData(QuantLib::Date asOfDate, QuantLib::TimeGrid timeGrid, 
			quantoSauros::Period period, QuantLib::Date maturityDate, int numOfIR){
				m_asOfDate = asOfDate;
				m_timeGrid = timeGrid;
				m_period = period;
				m_maturityDate = maturityDate;
				m_numOfIR = numOfIR;
				m_referenceRates = std::vector<std::vector<double>>(m_timeGrid.size());
				m_cumulatedDF = 1;
				m_dt = m_timeGrid.dt(0);
				m_accrualDt = 0;
		};	
		virtual double getReferenceRate(int IRIndex){ return 0; };
		virtual double getPayoffs(){ return 0; };
		virtual double getDiscountFactor(){ return 0; };

	protected:
		void init(){
			calcReferenceRates();
			calculateDiscountFactor();
			calculateAccrualDays();
		}
		virtual void savePathsInfo(){};
		virtual void calcReferenceRates(){};
		virtual void calculateDiscountFactor(){};
		virtual void calculateAccrualDays(){};

		std::vector<std::vector<double>> m_referenceRates;
		double m_accrualDt;
		double m_cumulatedDF;
		double m_inCouponRate;
		double m_outCouponRate;
		QuantLib::Date m_asOfDate;
		QuantLib::TimeGrid m_timeGrid;
		double m_dt;
		int m_numOfIR;
		quantoSauros::Period m_period;
		QuantLib::Date m_maturityDate;
	};
}