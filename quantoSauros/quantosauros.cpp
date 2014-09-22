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


int QUANTOSAUROS_API __stdcall bootstrapping(SAFEARRAY** today,
	//금리정보
	long rateN, double* market_ytmRates, double* market_discountRates, SAFEARRAY** spotRatesTenor,
	//변동성정보
	long volMaturityN, long volTenorN, double* market_volSurface, 
	SAFEARRAY** volSurfaceMaturities, SAFEARRAY** volSurfaceTenors, 
	//상품정보
	SAFEARRAY** in_ccyCd, long in_NotionalAmount, SAFEARRAY** in_dcf,
	long rangeN,
	double* inCouponRates1, double* outCouponRates1, 
	double* lowerBounds1, double* upperBounds1,
	SAFEARRAY** rangeStartDays, SAFEARRAY** rangeEndDays, 
	SAFEARRAY** callFlags, 
	SAFEARRAY** in_issueDate, SAFEARRAY** in_maturityDate,
	//결과
 	double* OutZeroRates, long* OutDays){

		//util class를 이용하여 input data의 type을 변환
		#pragma region Convert the type of input data
		quantoSauros::util util = quantoSauros::util();

		CString* new_today = util.conversion(today);
		CString* spotRatesTenorT = util.conversion(spotRatesTenor);
		CString* volSurfaceMaturitiesT = util.conversion(volSurfaceMaturities);
		CString* volSurfaceTenorsT = util.conversion(volSurfaceTenors);
		CString* new_ccyCd = util.conversion(in_ccyCd);
		CString* new_dcf = util.conversion(in_dcf);
		CString* new_rangeStartDays = util.conversion(rangeStartDays);
		CString* new_rangeEndDays = util.conversion(rangeEndDays);
		CString* new_callFlags = util.conversion(callFlags);
		CString* new_issueDate = util.conversion(in_issueDate);
		CString* new_maturityDate = util.conversion(in_maturityDate);

		#pragma endregion

		#pragma region generate an Interest Rate Curve
		std::vector<quantoSauros::InterestRate> interestRates;
		std::vector<quantoSauros::InterestRate> discountRates;

		int size = rateN;
		for (int i = 0; i < size; i++){
			quantoSauros::Tenor tenor = quantoSauros::Tenor(spotRatesTenorT[i]);
			interestRates.push_back(quantoSauros::InterestRate(tenor, market_ytmRates[i]));
			discountRates.push_back(quantoSauros::InterestRate(tenor, market_discountRates[i]));
		}		

		//이자율 커브 입력변수
		Calendar calendar = SouthKorea(SouthKorea::Settlement);
		Natural settlementDays = 1;
		DayCounter dcf = Actual365Fixed();
		Compounding compounding = Continuous;
		Date todayDate = QuantLib::Date(Day(atoi(new_today[0].Right(2))), 
				Month(atoi(new_today[0].Mid(5,2))), 
				Year(atoi(new_today[0].Left(4))));

		//이자율 커브 생성
		quantoSauros::InterestRateCurve irCurve(todayDate, interestRates,
			calendar, settlementDays, dcf, compounding);
		quantoSauros::InterestRateCurve discountCurve(todayDate, discountRates,
			calendar, settlementDays, dcf, compounding);

		//InterestRateCurve test
		//Period maturity(1, Years);
		//Period tenor(3, Years);
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
				double vol = market_volSurface[maturityIndex + tenorIndex * volTenorN];
				//Time t = 0;
				vols.push_back(quantoSauros::Volatility(volMaturity ,vol));
			}

			volCurves.push_back(quantoSauros::VolatilityCurve(todayDate, vols , dcf, volTenor));
		}		
		quantoSauros::VolatilitySurface volSurfaces(todayDate, volCurves, dcf);
		
		//VolatilitySurface Test
		//Volatility vol123 = volSurfaces.getVol(1.4,2.2);
		//Volatility vol234 = volSurfaces.getVolatilityCurve(1).getVol(2.3);
		//Volatility vo2345 = volSurfaces.getVolatilityCurve(1).getVol(Date(Day(1),Month(7),Year(2014)));
		#pragma endregion

		//Range Accrual Note		
		Currency currency;		
		//TODO Currency 클래스 생성
		if (new_ccyCd[0] == "KRW"){
			currency = KRWCurrency();
		} else {
			currency = KRWCurrency();
		}
		QuantLib::Money notional(currency, in_NotionalAmount);

		QuantLib::DayCounter dcf1;
		if (new_dcf[0] == "ACT/365"){
			dcf1 = Actual365Fixed();
		} else {
			dcf1 = Actual360();
		}
		//Range 구간 데이터
		int* EndDay = new int[rangeN];
		int* EndMonth = new int[rangeN];
		int* EndYear = new int[rangeN];
		int* StartDay = new int[rangeN];
		int* StartMonth = new int[rangeN];
		int* StartYear = new int[rangeN];
		bool* exerciseFlag = new bool[rangeN];

		for (int i = 0; i < rangeN; i++){
			EndYear[i] = atoi(new_rangeEndDays[i].Left(4));
			EndMonth[i] = atoi(new_rangeEndDays[i].Mid(5,2));
			EndDay[i] = atoi(new_rangeEndDays[i].Right(2));			
			StartYear[i] = atoi(new_rangeStartDays[i].Left(4));
			StartMonth[i] = atoi(new_rangeStartDays[i].Mid(5,2));
			StartDay[i] = atoi(new_rangeStartDays[i].Right(2));
			exerciseFlag[i] = (new_callFlags[i] == "True") ? true: false;
		}

		Date* exerciseDate = new Date[rangeN + 1];
		double* exercisePrice = new double[rangeN + 1];
		Date* rangeStartDates = new Date[rangeN + 1];
		Date* rangeEndDates = new Date[rangeN + 1];
		for (int i = 0; i < rangeN; i++){
			rangeStartDates[i] = Date(Day(StartDay[i]), Month(StartMonth[i]), Year(StartYear[i])); 
			rangeEndDates[i] = Date(Day(EndDay[i]), Month(EndMonth[i]), Year(EndYear[i]));
			if (exerciseFlag[i] == true){
				exerciseDate[i] = Date(Day(EndDay[i]), Month(EndMonth[i]), Year(EndYear[i]));
				exercisePrice[i] = 1.0;
			}
		}
		
		QuantLib::Date issueDate = 
			QuantLib::Date(Day(atoi(new_issueDate[0].Right(2))), 
				Month(atoi(new_issueDate[0].Mid(5,2))), 
				Year(atoi(new_issueDate[0].Left(4))));
		QuantLib::Date maturityDate = 
					QuantLib::Date(Day(atoi(new_maturityDate[0].Right(2))), 
				Month(atoi(new_maturityDate[0].Mid(5,2))), 
				Year(atoi(new_maturityDate[0].Left(4))));

		Frequency swapCouponFrequency1 = Quarterly;
		double floatTenor = 0.25;

		std::vector<double> inCouponRates;
		std::vector<double> outCouponRates;
		std::vector<double> rangeUpperRates;
		std::vector<double> rangeLowerRates;
		std::vector<quantoSauros::Period> rangePeriods;

		for (int i = 0; i < rangeN; i++){
			
			inCouponRates.push_back(inCouponRates1[i]);
			outCouponRates.push_back(outCouponRates1[i]);
			if (exerciseFlag[i] == true){
				rangePeriods.push_back(
					quantoSauros::Period(rangeStartDates[i], rangeEndDates[i], 
						exerciseDate[i], exercisePrice[i]));
			} else {
				rangePeriods.push_back(
					quantoSauros::Period(rangeStartDates[i], rangeEndDates[i]));
			}
			
			rangeUpperRates.push_back(upperBounds1[i]);
			rangeLowerRates.push_back(lowerBounds1[i]);
			
		}


		bool includePrincipal = true;
		QuantLib::Option::Type optionType = QuantLib::Option::Type::Call;
		int monitorFrequency = 10;

		quantoSauros::RangeAccrualNote raNote(notional, 
			issueDate, maturityDate, dcf1, includePrincipal, 
			floatTenor, swapCouponFrequency1,
			inCouponRates, outCouponRates,
			rangePeriods, rangeUpperRates, rangeLowerRates,
			optionType,
			monitorFrequency);

		int simulationNum = 100;
		Real meanReversion = 0.01;
		Real sigma = 0.001;		
		
		raNote.getPrice(todayDate, irCurve, volSurfaces, meanReversion, sigma, 
			discountCurve, volSurfaces, meanReversion, sigma,
			simulationNum);
		

		//MCLongstaffSchwartzEngineTest LONGSTAFFTEST;
		//LONGSTAFFTEST.testAmericanMaxOption();

		//SABRTest().run();

		//PathGeneratorTest tester1;

		//tester1.testMultiPathGenerator();

		//ShortRateModelTest tester2;
		//tester2.testHullWhite2F(irCurve);


		//tester2.testCachedHullWhite(irCurve);

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
