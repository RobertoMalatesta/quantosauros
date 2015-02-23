#pragma once
#include <ql/quantlib.hpp>
#include "AbstractData.h";

namespace quantoSauros{

	class RangeAccrualData : public AbstractData {
	public:
		RangeAccrualData(){};
		RangeAccrualData(QuantLib::Date asOfDate, quantoSauros::Period period, 
			QuantLib::TimeGrid timeGrid, QuantLib::Date maturityDate, int numOfIR,
			QuantLib::Time startTime,
			//��ǰ����
			std::vector<double> rangeUpperRate, std::vector<double> rangeLowerRate,
			double inCouponRate, double outCouponRate,
			//������ ����
			std::vector<double> floatCurveTenors,
			std::vector<quantoSauros::RateType> rateTypes,
			std::vector<QuantLib::Frequency> swapCouponFrequencies,
			std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
			//��ȭ��Ʈ����
			sample_type* shortRatePath, 
			std::vector<quantoSauros::HullWhiteParameters> IRParams,
			std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
			QuantLib::HullWhiteVolatility discountHullWhiteVolatility)
				: AbstractData(asOfDate, period, timeGrid, maturityDate, numOfIR,
					startTime, 
					floatCurveTenors, rateTypes, swapCouponFrequencies,
					floatTermStructures, 
					shortRatePath, IRParams, 
					hullWhiteVolatilities, discountHullWhiteVolatility) {

				m_accrualDt = 0;
				m_rangeUpperRate = rangeUpperRate;
				m_rangeLowerRate = rangeLowerRate;
				m_inCouponRate = inCouponRate;
				m_outCouponRate = outCouponRate;		
				init();
		};
		
		virtual double getPayoffs(){
			//TODO
			return m_accrualDt * m_inCouponRate;
		};
		virtual double getDiscountFactor(){
			return m_cumulatedDF;
		};
		

	protected:		

		virtual void calculatePayoff(){
			for (Size timeIndex = 0; timeIndex < m_timeGrid.size() - 1; timeIndex++){
				if (m_numOfIR == 1){
					double upperRate1 = m_rangeUpperRate[0];
					double lowerRate1 = m_rangeLowerRate[0];
					if (m_referenceRates[timeIndex][0] >= lowerRate1 
						&& m_referenceRates[timeIndex][0] <= upperRate1) {										
							m_accrualDt = m_accrualDt + m_timeGrid.dt(timeIndex);							
					}
				} else if (m_numOfIR == 2){
					double upperRate1 = m_rangeUpperRate[0];
					double lowerRate1 = m_rangeLowerRate[0];
					double upperRate2 = m_rangeUpperRate[1];
					double lowerRate2 = m_rangeLowerRate[1];
					if (m_referenceRates[timeIndex][0] >= lowerRate1 
						&& m_referenceRates[timeIndex][0] <= upperRate1) {
						if (m_referenceRates[timeIndex][1] >= lowerRate2 
							&& m_referenceRates[timeIndex][1] <= upperRate2) {
								m_accrualDt = m_accrualDt + m_timeGrid.dt(timeIndex);
						}
					}
				}
			}
		}
	
	private:

		double m_accrualDt;
		double m_inCouponRate;
		double m_outCouponRate;
		std::vector<double> m_rangeUpperRate;
		std::vector<double> m_rangeLowerRate;

	};
}