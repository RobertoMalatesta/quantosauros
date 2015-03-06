#pragma once
#include <ql/quantlib.hpp>
#include "AbstractData.h";

namespace quantoSauros {

	class CouponBondData : public AbstractData {
		public:
			CouponBondData(){};
			//For Float
			CouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,
				//헐화이트정보
				sample_type* shortRatePath, 
				std::vector<quantoSauros::HullWhiteParameters> IRParams,
				std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility,
				//이자율 텀스트럭쳐
				std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
				//이자율 정보
				std::vector<double> floatCurveTenors,
				std::vector<quantoSauros::RateType> rateTypes,
				std::vector<QuantLib::Frequency> swapCouponFrequencies)
					: AbstractData(asOfDate, maturityDate, period, 
						startTime, timeGrid, dcf,						
						shortRatePath, IRParams, 
						hullWhiteVolatilities, discountHullWhiteVolatility,
						floatTermStructures,
						floatCurveTenors, rateTypes, swapCouponFrequencies) {

					m_spread = spread;
					m_leverage = leverage;
					init();

			};
			//For Fixed
			CouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,				
				//헐화이트정보
				sample_type* shortRatePath)
					: AbstractData(asOfDate, maturityDate, period, 
						startTime, timeGrid, dcf,						
						shortRatePath){

					m_spread = spread;
					m_leverage = leverage;
					init();
			};

			virtual double getDiscountFactor(){
				return m_cumulatedDF;
			};
			
		protected:

			virtual void calculatePayoff(){
				
			}
			QuantLib::Rate m_spread;
			QuantLib::Real m_leverage;
			
	};

	class FixedCouponBondData : public CouponBondData {
		public:
			FixedCouponBondData(){};
			FixedCouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보
				QuantLib::Rate spread, QuantLib::Real leverage,
				QuantLib::Rate coupon,
				//헐화이트정보
				sample_type* shortRatePath, 
				//std::vector<quantoSauros::HullWhiteParameters> IRParams,
				//std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility) 
				:  CouponBondData(asOfDate, maturityDate, period, startTime, timeGrid, dcf,
					spread, leverage, shortRatePath){  
						m_coupon = coupon;
			};

		protected:
			virtual double getPayoffs(){
				return m_leverage * (m_coupon + m_spread) * m_period.getPeriodTenor(m_dcf);			
			};
			virtual void calcReferenceRates(){};

		private:
			QuantLib::Rate m_coupon;
	};

	class FloatCouponBondData : public CouponBondData {
		public:
			FloatCouponBondData(){};
			FloatCouponBondData(QuantLib::Date asOfDate, QuantLib::Date maturityDate,
				quantoSauros::Period period, 
				QuantLib::Time startTime, QuantLib::TimeGrid timeGrid, 
				QuantLib::DayCounter dcf,
				//상품정보				
				QuantLib::Rate spread, QuantLib::Real leverage,
				//헐화이트정보
				sample_type* shortRatePath, 
				std::vector<quantoSauros::HullWhiteParameters> IRParams,
				std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities,
				QuantLib::HullWhiteVolatility discountHullWhiteVolatility,
				//이자율 텀스트럭쳐
				std::vector<quantoSauros::InterestRateCurve> floatTermStructures,
				//이자율 정보
				std::vector<double> floatCurveTenors,
				std::vector<quantoSauros::RateType> rateTypes,
				std::vector<QuantLib::Frequency> swapCouponFrequencies)
				: CouponBondData(asOfDate, maturityDate, period, 
					startTime, timeGrid, dcf,
					spread, leverage, 
					shortRatePath, IRParams, 
					hullWhiteVolatilities, discountHullWhiteVolatility,
					floatTermStructures,
					floatCurveTenors, rateTypes, swapCouponFrequencies){		

			};

		protected:
			virtual double getPayoffs(){
				return m_leverage * (m_referenceRates[0][0] + m_spread) *
					m_period.getPeriodTenor(m_dcf);			
			};
		private:

	};

}
