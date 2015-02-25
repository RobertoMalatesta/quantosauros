#pragma once
#include <ql/quantlib.hpp>
#include "AbstractData.h";

namespace quantoSauros {

	class CouponBondData : public AbstractData {
		public:
			CouponBondData(){};
			CouponBondData(QuantLib::Date asOfDate, quantoSauros::Period period, 
				QuantLib::TimeGrid timeGrid, QuantLib::Date maturityDate,
				QuantLib::Time startTime, QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Real spread,
				//이자율 정보
				std::vector<double> floatCurveTenors,
				std::vector<quantoSauros::RateType> rateTypes,
				std::vector<QuantLib::Frequency> swapCouponFrequencies,
				std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
				//헐화이트정보
				sample_type* shortRatePath, 
				std::vector<quantoSauros::HullWhiteParameters> IRParams,
				std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility)
					: AbstractData(asOfDate, maturityDate, period, 
						startTime, timeGrid, dcf,
						floatCurveTenors, rateTypes, swapCouponFrequencies,
						floatTermStructures, 
						shortRatePath, IRParams, 
						hullWhiteVolatilities, discountHullWhiteVolatility) {

					m_spread = spread;
					init();
			};

			virtual double getPayoffs(){
				//TODO
				return (m_referenceRates[0][0] + m_spread) * m_period.getPeriodTenor(m_dcf);
			};
			virtual double getDiscountFactor(){
				return m_cumulatedDF;
			};
			
		protected:

			virtual void calculatePayoff(){
				
			}

		private:

			Real m_spread;
						
	};

}
