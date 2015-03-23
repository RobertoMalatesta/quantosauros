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
			virtual void generateSeeds(){
				srand(time(NULL));
				if (m_seeds.size() == 0){
					for (int i = 0; i < m_simulationNum; i++){
						std::vector<QuantLib::BigNatural> seed;
						for (int j = 0; j < m_periodNum; j++){
							seed.push_back(QuantLib::BigNatural(rand()));
						}
						m_seeds.push_back(seed);
					}
				}
			};
			virtual void generatePaths(){};
			virtual void calculatePrice(){};

			QuantLib::Date m_today;
			//seeds
			std::vector<std::vector<QuantLib::BigNatural>> m_seeds;			
			QuantLib::Money m_price;
			std::vector<std::vector<QuantLib::Real>> m_payoffs;

			QuantLib::Size m_timeGridSize;
			int m_irNum;			
			int m_simulationNum;
			int m_periodNum;
	};

	class AbstractAccrualPricer : public AbstractPricer {
		public:				
			AbstractAccrualPricer(QuantLib::Date today,
				int simulationNum, int irNum, int periodNum) 
					: AbstractPricer(today){	

						m_simulationNum = simulationNum;
						m_irNum = irNum;						
						m_periodNum = periodNum;			
						//payoff
						m_payoffs = std::vector<std::vector<double>>(m_periodNum + 1);
			};

		protected:

			quantoSauros::AbstractArguments m_args;
			//std::vector<std::vector<quantoSauros::AbstractData>> m_data;
	};

	class AbstractCouponBondPricer : public AbstractPricer {
		public:
			AbstractCouponBondPricer(QuantLib::Date today,
				int simulationNum, int irNum, int periodNum) 
					: AbstractPricer(today){

						m_simulationNum = simulationNum;
						m_irNum = irNum;						
						m_periodNum = periodNum;
						//payoff
						m_payoffs = std::vector<std::vector<double>>(m_periodNum + 1);
			};

		protected:

			quantoSauros::AbstractArguments m_args;
			//std::vector<std::vector<quantoSauros::AbstractData>> m_data;
	};

	class AbstractAveragePricer : public AbstractPricer {

	};
}