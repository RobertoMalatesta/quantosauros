#pragma once
#include <ql/quantlib.hpp>
#include "HullWhiteVolatilityCalibration.h"
#include <cmath>

using namespace QuantLib;
namespace quantoSauros {
	std::vector<Volatility> HullWhiteVolatilityCalibration::calibration(){
		int swaptionN = _exerciseDates.size();
		int maxloop = 1000;
		double tolerance = 1.0e-5;
		double pa = 0.00001;
		int outloopcnt = 0;

		double *swaptionVols = new double[swaptionN];
		double *spotVol = new double[swaptionN];
		double *atmStrike = new double[swaptionN];
		double *bkPrice = new double[swaptionN];
		double *hwPrice = new double[swaptionN];
		double *vol = new double[swaptionN];
		int *maturityDays = new int[swaptionN];
		double *maturityDt = new double[swaptionN];

		Date adjustedAsOfDate = calendar.adjust(_asOfDate, _businessDayConvention);
		boost::shared_ptr<YieldTermStructure> depoFloatCurve = _spotCurve.getInterestRateCurve();
		RelinkableHandle<YieldTermStructure> floatingTermStructure;
		floatingTermStructure.linkTo(depoFloatCurve);
		boost::shared_ptr<IborIndex> rateIndex;
		if (countryCode == "KRW"){
			boost::shared_ptr<IborIndex> rateIndex2(new KRWibor3M(floatingTermStructure));
		} else if (countryCode == "USD"){
			boost::shared_ptr<IborIndex> rateIndex2(new USDLibor(Period(0.5, Years),floatingTermStructure));
		} else if (countryCode == "EUR"){
			boost::shared_ptr<IborIndex> rateIndex2(new Euribor365_6M(floatingTermStructure));
		} else {
			boost::shared_ptr<IborIndex> rateIndex2(new KRWibor3M(floatingTermStructure));
		}
		
		VanillaSwap::Type swaptype = VanillaSwap::Payer;
		boost::shared_ptr<Swaption> bkSwaptions;
		boost::shared_ptr<Swaption> HWSwaptions;
		for(int i = 0; i < swaptionN; i++){
			swaptionVols[i] = _swaptionSurface.getVol(_swaptionInfo[i][0], _swaptionInfo[i][1]);
			maturityDays[i] = (int)(_swaptionInfo[i][1] * _dcf.daysOfYear()) - 1;
			Date swapStartDate = calendar.adjust(adjustedAsOfDate.operator+(maturityDays[i]),
								_businessDayConvention);
			Period d1 = Period(_dcf.yearFraction(_asOfDate, swapStartDate), Years);
			Period d2 = Period(_dcf.yearFraction(swapStartDate, _maturityDate), Years);
			Schedule floatSchedule(_exerciseDates[i],_maturityDate, Period(_floatCouponFrequency), 
				calendar, _businessDayConvention, _businessDayConvention, DateGeneration::Rule(), false);
			Schedule fixedSchedule(_exerciseDates[i],_maturityDate, Period(_fixedCouponFrequency), 
				calendar, _businessDayConvention, _businessDayConvention, DateGeneration::Rule(), false);
			
			VanillaSwap tmpSwap(swaptype, 1.0, fixedSchedule, 0.03, _dcf, 
				floatSchedule, rateIndex, 0.0, _dcf);
			boost::shared_ptr<PricingEngine> swapEngine(new DiscountingSwapEngine(floatingTermStructure));
			atmStrike[i] = tmpSwap.fairRate();

			boost::shared_ptr<VanillaSwap> swapPtr(new VanillaSwap(
										swaptype, 1.0,
										fixedSchedule, atmStrike[i], _dcf,
										floatSchedule, rateIndex, 0.0,
										_dcf));
			boost::shared_ptr<Exercise> exercise(new EuropeanExercise(_exerciseDates[i]));	
			boost::shared_ptr<Swaption> swaptionPtr = boost::shared_ptr<Swaption>(new		
					Swaption(swapPtr, exercise));
			bkSwaptions = swaptionPtr;
			bkSwaptions->setPricingEngine(boost::shared_ptr<PricingEngine>
				(new BlackSwaptionEngine(floatingTermStructure, swaptionVols[i])));
			bkPrice[i] = bkSwaptions->NPV();
			
			HWSwaptions = swaptionPtr;
			HWSwaptions->setPricingEngine(boost::shared_ptr<PricingEngine>(new
				JamshidianSwaptionEngine(boost::shared_ptr<HullWhite>(new HullWhite(
				floatingTermStructure, _meanReversion, _initialVol)))));
			hwPrice[i] = HWSwaptions->NPV();
			int loopCnt = 0;
			double error = hwPrice[i] - bkPrice[i];
			double oldParam = _initialVol;
			double newParam = 0;
			setVol(_initialVol);
			while (loopCnt <= maxloop & abs(error) > tolerance){
				setVol(_modelVol + pa);
				HWSwaptions->setPricingEngine(boost::shared_ptr<PricingEngine>(new
				JamshidianSwaptionEngine(boost::shared_ptr<HullWhite>(new HullWhite(
				floatingTermStructure, _meanReversion, _modelVol)))));
				double alpha = (HWSwaptions->NPV() - hwPrice[i]) / pa;
				if (alpha != 0){
					newParam = oldParam - error / alpha;
					if(newParam < 0){
						newParam = oldParam;
					}
					setVol(newParam);
					HWSwaptions->setPricingEngine(boost::shared_ptr<PricingEngine>(new
						JamshidianSwaptionEngine(boost::shared_ptr<HullWhite>(new HullWhite(
						floatingTermStructure, _meanReversion, _modelVol)))));
					error = (HWSwaptions->NPV()) - bkPrice[i];
					oldParam = newParam;
				} else {
					loopCnt = 2 * maxloop;
				}
				loopCnt++;
			}
			if(loopCnt < maxloop){
				spotVol[i] = newParam;
			}
			if(loopCnt == 0){
				spotVol[i] = _initialVol;
			}
			if(outloopcnt < loopCnt){
				outloopcnt = loopCnt;
			}
		}
		if(outloopcnt < maxloop){
			vol[0] = spotVol[0];
			for(int si = 1; si < swaptionN; si++){
				double tmp = (spotVol[si] * spotVol[si] * _swaptionInfo[si][0] -
					spotVol[si - 1] * spotVol[si - 1] * _swaptionInfo[si - 1][0])/
					(_swaptionInfo[si][0] - _swaptionInfo[si - 1][0]);
				if(tmp >0){
					vol[si] = sqrt(tmp);
				}else {
					vol[si] = _initialVol;
				}
			}
		} else {
			for(int i = 0 ; i < swaptionN; i++){
				vol[i] = _initialVol;
			}
		}
		std::vector<Volatility> calivol;
		calivol.reserve(swaptionN);
		for(int i = 0; i < swaptionN; i++){
			Tenor tenor = Tenor::Tenor(Period(_swaptionInfo[i][0], Years));
			calivol[i] = Volatility(tenor, vol[i]);
		}
		return calivol;
	}
}
