#pragma once
#include <ql/quantlib.hpp>
#include "InterestRate.h";
#include "InterestRateCurve.h";
#include "HullWhiteParameters.h";
#include "Period.h";

namespace quantoSauros{

	typedef QuantLib::PseudoRandom::rsg_type rsg_type;		
	typedef QuantLib::MultiPathGenerator<rsg_type>::sample_type sample_type;

	class AbstractData {
	public:
		AbstractData(){};
		AbstractData(QuantLib::Date asOfDate, 
			QuantLib::Date maturityDate,
			QuantLib::DayCounter dcf,
			int simNum, int periodNum)
			: m_asOfDate(asOfDate), m_maturityDate(maturityDate), m_dcf(dcf) {

				m_paths.resize(simNum);
				m_timeGrids.resize(simNum);
				m_startTimes.resize(simNum);
				m_periods.resize(simNum);
				for (int i = 0; i < simNum; i++){
					m_paths[i].resize(periodNum);
					m_timeGrids[i].resize(periodNum);
					m_startTimes[i].resize(periodNum);
					m_periods[i].resize(periodNum);
				}					
		};

		virtual double getReferenceRate(int IRIndex, int simIndex, int periodIndex){
			return 0;
		}
		virtual double getPayoffs(int simIndex, int periodIndex){
			return 0;
		}
		virtual double getDiscountFactor(int simIndex, int periodIndex){
			return 0;
		}
		virtual std::vector<double> getLSMCData(int simIndex, int periodIndex, int IRIndex){
			std::vector<double> data;			
			return data;
		}

		// 변하는 데이터
		void setData(int simIndex, int periodIndex, 
			QuantLib::TimeGrid timeGrid, quantoSauros::Period period,
			QuantLib::Time startTime, sample_type* shortRatePath){

				setTimeGrid(simIndex, periodIndex, timeGrid);
				setPath(simIndex, periodIndex, shortRatePath);
				setStartTime(simIndex, periodIndex, startTime);
				setPeriod(simIndex, periodIndex, period);

				init(simIndex, periodIndex);
		}
		void setTimeGrid(int simIndex, int periodIndex, QuantLib::TimeGrid timeGrid){
			m_timeGrids[simIndex][periodIndex] = timeGrid;
		};
		void setPath(int simIndex, int periodIndex, sample_type* shortRatePath){
			m_paths[simIndex][periodIndex] = shortRatePath;
		};
		void setStartTime(int simIndex, int periodIndex, QuantLib::Time startTime){
			m_startTimes[simIndex][periodIndex] = startTime;
		};
		void setPeriod(int simIndex, int periodIndex, quantoSauros::Period period){
			m_periods[simIndex][periodIndex] = period;
		};

	protected:
		virtual void calcReferenceRates(int simIndex, int periodIndex){};
		virtual void calculateDiscountFactor(int simIndex, int periodIndex){};
		virtual void calculatePayoff(int simIndex, int periodIndex) {};
		virtual void init(int simIndex, int periodIndex){
			calcReferenceRates(simIndex, periodIndex);
			calculateDiscountFactor(simIndex, periodIndex);
			calculatePayoff(simIndex, periodIndex);
		};
		QuantLib::Date m_asOfDate;
		QuantLib::Date m_maturityDate;
		QuantLib::DayCounter m_dcf;

		std::vector<std::vector<sample_type*>> m_paths;
		std::vector<std::vector<QuantLib::TimeGrid>> m_timeGrids;
		std::vector<std::vector<QuantLib::Time>> m_startTimes;
		std::vector<std::vector<quantoSauros::Period>> m_periods;

	private:

	};
}