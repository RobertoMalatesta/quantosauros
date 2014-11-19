#pragma once
#include <ql/quantlib.hpp>

namespace quantoSauros {
	
	class AbstractArguments {
	public:
		AbstractArguments(){};
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

	protected:		
		QuantLib::Date m_asOfDate;
		QuantLib::Date m_issueDate;
		QuantLib::Date m_maturityDate;
		QuantLib::Money m_notional;
		QuantLib::DayCounter m_dcf;
			
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

}