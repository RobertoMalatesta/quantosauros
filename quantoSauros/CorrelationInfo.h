#pragma once
#include <ql/quantlib.hpp>

namespace quantoSauros{
	class CorrelationInfo{
		public:
			CorrelationInfo(QuantLib::Matrix correlationMatrix){
				m_correlationMatrix = correlationMatrix;
			}
			QuantLib::Matrix getCorrelationMatrix();
		private:
			QuantLib::Matrix m_correlationMatrix;

	};

	inline QuantLib::Matrix CorrelationInfo::getCorrelationMatrix(){
		return m_correlationMatrix;
	}
}