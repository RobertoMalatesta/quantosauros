#pragma once
#include <ql/quantlib.hpp>
#include <algorithm> 
#include "Period.h";
#include "AbstractArguments.h";

namespace quantoSauros{
	class AbstractPricer {
	public:
		AbstractPricer(){};
		AbstractPricer(QuantLib::Date today){
			m_today = today;
		};
		
		QuantLib::Money getResults(){
			return m_price;
		}
		virtual void calculate(){
			init();
			generateSeeds();
			generatePaths();
			calculatePrice();
		};
	protected:
		virtual void init(){};
		virtual void generateSeeds(){};
		virtual void generatePaths(){};
		virtual void calculatePrice(){};

		QuantLib::Date m_today;
		//seeds
		std::vector<std::vector<QuantLib::BigNatural>> m_seeds;
		int m_simulationNum;
		int m_periodNum;
		QuantLib::Money m_price;
	};

	class AbstractAccrualPricer : public AbstractPricer {
	public:				
		AbstractAccrualPricer(QuantLib::Date today) : AbstractPricer(today){				

		};


	protected:
		virtual void generateSeeds();
				
		QuantLib::Size m_timeGridSize;
		int m_irNum;

		//quantoSauros::AbstractArguments m_args;
	};

	class AbstractAveragePricer : public AbstractPricer {

	};
}