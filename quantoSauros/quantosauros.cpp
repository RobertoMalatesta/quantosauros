#pragma once
#ifdef BOOST_MSVC
/* Uncomment the following lines to unmask floating-point
   exceptions. Warning: unpredictable results can arise...

   See http://www.wilmott.com/messageview.cfm?catid=10&threadid=9481
   Is there anyone with a definitive word about this?
*/
// #include <float.h>
// namespace { unsigned int u = _controlfp(_EM_INEXACT, _MCW_EM); }
#endif

#if defined(QL_ENABLE_SESSIONS)
namespace QuantLib {

    Integer sessionId() { return 0; }

}
#endif
#include "quantosauros.h";


int QUANTOSAUROS_API __stdcall bootstrapping(long today,
	long spotRateN, double* spotRates, SAFEARRAY** spotRatesTenor,
	long volMaturityN, long volTenorN, double* volSurface, 
	SAFEARRAY** volSurfaceMaturities, SAFEARRAY** volSurfaceTenors, 
 	double* OutZeroRates, long* OutDays){

		//util class를 이용하여 input data의 type을 변환
		#pragma region Convert the type of input data
		quantoSauros::util util = quantoSauros::util();
		LPCSTR* spotRatesTenorT = util.conversion(spotRatesTenor);
		LPCSTR* volSurfaceMaturitiesT = util.conversion(volSurfaceMaturities);
		LPCSTR* volSurfaceTenorsT = util.conversion(volSurfaceTenors);
		#pragma endregion

		#pragma region generate an Interest Rate Curve
		std::vector<quantoSauros::InterestRate> interestRates;

		int size = spotRateN;
		for (int i = 0; i < size; i++){
			quantoSauros::Tenor tenor = quantoSauros::Tenor(spotRatesTenorT[i]);
			interestRates.push_back(quantoSauros::InterestRate(tenor, spotRates[i]));
		}
		

		//이자율 커브 입력변수
		Calendar calendar = SouthKorea(SouthKorea::Settlement);
		Natural settlementDays = 0;
		DayCounter dcf = Actual365Fixed();
		Compounding compounding = Continuous;
		Date todayDate = Date(BigInteger(today));

		//이자율 커브 생성
		quantoSauros::InterestRateCurve irCurve(todayDate, interestRates,
			calendar, settlementDays, dcf, compounding);
		Period maturity(1, Years);
		Period tenor(3, Years);
		//double forwardRate = irCurve.getForwardRate(1, 1.25);
		//double df = irCurve.getDiscountFactor(1);
		//double swapRate = irCurve.getForwardSwapRate(maturity, tenor);		
		#pragma endregion

		#pragma region generate a Volatility Surface
		// Vol 셋팅		
		std::vector<quantoSauros::VolatilityCurve> volCurves;

		for (int tenorIndex = 0; tenorIndex < volTenorN; tenorIndex++){
		
			quantoSauros::Tenor volTenor = quantoSauros::Tenor(volSurfaceTenorsT[tenorIndex]);
			std::vector<quantoSauros::Volatility> vols;

			for (int maturityIndex = 0; maturityIndex < volMaturityN; maturityIndex++){	

				quantoSauros::Tenor volMaturity = quantoSauros::Tenor(volSurfaceMaturitiesT[maturityIndex]);
				double vol = volSurface[maturityIndex + tenorIndex * volTenorN];
				//Time t = 0;
				vols.push_back(quantoSauros::Volatility(volMaturity ,vol));
			}

			volCurves.push_back(quantoSauros::VolatilityCurve(todayDate, vols , dcf, volTenor));
		}		
		quantoSauros::VolatilitySurface volSurfaces(todayDate, volCurves, dcf);
		
		Volatility vol123 = volSurfaces.getVol(1.4,2.2);

		Volatility vol234 = volSurfaces.getVolatilityCurve(1).getVol(2.3);
		Volatility vo2345 = volSurfaces.getVolatilityCurve(1).getVol(Date(Day(1),Month(7),Year(2014)));
		#pragma endregion

		//Range Accrual Note
		Currency KRW = KRWCurrency();
		QuantLib::Money notional(KRW, 10000);
		
		Frequency swapCouponFrequency1 = Quarterly;
		double floatTenor = 0.25;
		std::vector<double> inCouponRates;
		inCouponRates.push_back(0.04);
		inCouponRates.push_back(0.05);
		inCouponRates.push_back(0.06);
		inCouponRates.push_back(0.06);
		std::vector<double> outCouponRates;
		outCouponRates.push_back(0.0);
		outCouponRates.push_back(0.0);
		outCouponRates.push_back(0.0);
		outCouponRates.push_back(0.0);

		std::vector<quantoSauros::Period> rangePeriods;
		rangePeriods.push_back(quantoSauros::Period(Date(Day(1), Month(5), Year(2015)), Date(Day(1), Month(5), Year(2016))));
		rangePeriods.push_back(quantoSauros::Period(Date(Day(1), Month(5), Year(2016)), Date(Day(1), Month(5), Year(2017))));
		rangePeriods.push_back(quantoSauros::Period(Date(Day(1), Month(5), Year(2017)), Date(Day(1), Month(5), Year(2018))));
		rangePeriods.push_back(quantoSauros::Period(Date(Day(1), Month(5), Year(2018)), Date(Day(1), Month(5), Year(2019))));

		std::vector<double> rangeUpperRates;
		rangeUpperRates.push_back(0.04);
		rangeUpperRates.push_back(0.05);
		rangeUpperRates.push_back(0.06);
		rangeUpperRates.push_back(0.06);

		std::vector<double> rangeLowerRates;
		rangeLowerRates.push_back(0.00);
		rangeLowerRates.push_back(0.00);
		rangeLowerRates.push_back(0.00);
		rangeLowerRates.push_back(0.00);

		std::vector<QuantLib::Date> exerciseDates;
		exerciseDates.push_back(QuantLib::Date(Date(Day(1), Month(5), Year(2015))));

		std::vector<double> exercisePrices;
		exercisePrices.push_back(1.0);
		
		QuantLib::DayCounter dcf1 = Actual360();
		bool includePrincipal = true;
		QuantLib::Option::Type optionType = QuantLib::Option::Type::Call;
		int monitorFrequency = 30;

		QuantLib::Date maturityDate = QuantLib::Date(Day(1), Month(5), Year(2019));

		quantoSauros::RangeAccrualNote raNote(notional, 
			todayDate, maturityDate, dcf1, includePrincipal, 
			floatTenor, swapCouponFrequency1,
			inCouponRates, outCouponRates,
			rangePeriods, rangeUpperRates, rangeLowerRates,
			exerciseDates, exercisePrices, optionType,
			monitorFrequency);

		Money asd = raNote.getNotional();
		BigNatural seed = 42;
		Real meanReversion = 0.01;
		Real sigma = 0.005;
		Date asOfDate = Date(Day(1), Month(8), Year(2017));
			
		raNote.getPrice(asOfDate, irCurve, volSurfaces, meanReversion, sigma, seed);
		//std::vector<quantoSauros::Volatility> asdasd = volCurve.getVolatilityCurve();
		//int index = volCurve.Length();
		

		//low discrepancysequence test

		//LowDiscrepancyTest testLowDiscrepancy;

		
		//testLowDiscrepancy.testSobol();

		//LiborMarketModelProcessTest test_process;

		//test_process.testInitialisation();
		//test_process.testMonteCarloCapletPricing();

		/*
		LiborMarketModelTest test;

		//test.suite();
		test.testCapletPricing();
		test.testSimpleCovarianceModels();
		test.testCalibration();
		*/
 	return 1;
}
