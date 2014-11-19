#include "AbstractPricer.h"

namespace quantoSauros {

	void AbstractAccrualPricer::generateSeeds(){
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
	}

}