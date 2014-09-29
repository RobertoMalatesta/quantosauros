#pragma once

namespace quantoSauros {
	class HullWhiteParameters {
		public:
			HullWhiteParameters(){}
			HullWhiteParameters(double meanReversion1F, double volatility1F){
				m_meanReversion1F = meanReversion1F;
				m_volatility1F = volatility1F;
			}
			HullWhiteParameters(double meanReversion2F_1, double meanReversion2F_2,
				double volatility2F_1, double volatility2F_2, double correlation){
					m_meanReversion2F_1 = meanReversion2F_1;
					m_meanReversion2F_2 = meanReversion2F_2;
					m_volatility2F_1 = volatility2F_1;
					m_volatility2F_2 = volatility2F_2;
					m_correlation = correlation;
			}
			double getMeanReversion1F();
			double getVolatility1F();
			double getMeanReversion2F_1();
			double getMeanReversion2F_2();
			double getVolatility2F_1();
			double getVolatility2F_2();
			double getCorrelation();
		private:
			//1F Parameters
			double m_meanReversion1F;
			double m_volatility1F;

			//2F Parameters
			double m_meanReversion2F_1;
			double m_meanReversion2F_2;
			double m_volatility2F_1;
			double m_volatility2F_2;
			double m_correlation;
	};
	inline double HullWhiteParameters::getMeanReversion1F(){
		return m_meanReversion1F;
	}
	inline double HullWhiteParameters::getVolatility1F(){
		return m_volatility1F;
	}
	inline double HullWhiteParameters::getMeanReversion2F_1(){
		return m_meanReversion2F_1;
	}
	inline double HullWhiteParameters::getMeanReversion2F_2(){
		return m_meanReversion2F_2;
	}
	inline double HullWhiteParameters::getVolatility2F_1(){
		return m_volatility2F_1;
	}
	inline double HullWhiteParameters::getVolatility2F_2(){
		return m_volatility2F_2;
	}
	inline double HullWhiteParameters::getCorrelation(){
		return m_correlation;
	}
}