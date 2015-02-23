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

double QUANTOSAUROS_API __stdcall test1(SAFEARRAY** input){

	CString* new_today = quantoSauros::util().conversion(input);

	double a = 1;

	a = a + 1;


	return a;
}

double QUANTOSAUROS_API __stdcall bootstrapping(SAFEARRAY** in_today,
	//금리정보
	long in_rateN, double* in_ytmRates, double* in_discountRates, SAFEARRAY** in_spotRatesTenor,
	//변동성정보
	long in_volMaturityN, long in_volTenorN, double* in_volSurface, 
	SAFEARRAY** in_volSurfaceMaturities, SAFEARRAY** in_volSurfaceTenors, 
	//상품정보
	SAFEARRAY** in_ccyCd, long in_NotionalAmount, SAFEARRAY** in_dcf,
	//Range 정보
	long rangeN,
	double* in_inCouponRates, double* in_outCouponRates, 
	double* in_lowerBounds1, double* in_upperBounds1,
	SAFEARRAY** in_rangeStartDays, SAFEARRAY** in_rangeEndDays, 
	SAFEARRAY** in_callFlags, 
	//발행정보
	SAFEARRAY** in_issueDate, SAFEARRAY** in_maturityDate,
	//금리정보
	long referenceN, SAFEARRAY** in_referenceType, 
	SAFEARRAY** in_rateType, double* in_tenor, SAFEARRAY** in_swapCouponFrequency,
	//기타정보
	long in_monitorFrequency, long in_simulationNum,
	//결과
 	double* out_price){

	//util class를 이용하여 input data의 type을 변환
	#pragma region Convert the type of input data

		CString* new_today = quantoSauros::util().conversion(in_today);
		CString* new_spotRatesTenorT = quantoSauros::util().conversion(in_spotRatesTenor);
		CString* new_volSurfaceMaturitiesT = quantoSauros::util().conversion(in_volSurfaceMaturities);
		CString* new_volSurfaceTenorsT = quantoSauros::util().conversion(in_volSurfaceTenors);
		CString* new_ccyCd = quantoSauros::util().conversion(in_ccyCd);
		CString* new_dcf = quantoSauros::util().conversion(in_dcf);
		CString* new_rangeStartDays = quantoSauros::util().conversion(in_rangeStartDays);
		CString* new_rangeEndDays = quantoSauros::util().conversion(in_rangeEndDays);
		CString* new_callFlags = quantoSauros::util().conversion(in_callFlags);
		CString* new_issueDate = quantoSauros::util().conversion(in_issueDate);
		CString* new_maturityDate = quantoSauros::util().conversion(in_maturityDate);
		CString* new_referenceType = quantoSauros::util().conversion(in_referenceType);
		CString* new_rateType = quantoSauros::util().conversion(in_rateType);		
		CString* new_swapCouponFrequency = quantoSauros::util().conversion(in_swapCouponFrequency);

	#pragma endregion

		Date todayDate = quantoSauros::util().Date(new_today[0]);
		//////////////////////////////////////////////////////////////
		QuantLib::Settings::instance().evaluationDate() = todayDate;

	#pragma region generate an Interest Rate Curve
		std::vector<quantoSauros::InterestRate> interestRates;
		std::vector<quantoSauros::InterestRate> discountRates;

		int size = in_rateN;
		for (int i = 0; i < size; i++){
			quantoSauros::Tenor tenor = quantoSauros::Tenor(new_spotRatesTenorT[i]);
			interestRates.push_back(quantoSauros::InterestRate(tenor, in_ytmRates[i]));
			discountRates.push_back(quantoSauros::InterestRate(tenor, in_discountRates[i]));
		}		

		//이자율 커브 입력변수
		Calendar calendar = SouthKorea(SouthKorea::Settlement);
		Natural settlementDays = 1;
		DayCounter dcf = Actual365Fixed();
		Compounding compounding = Continuous;
		


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

		for (int tenorIndex = 0; tenorIndex < in_volTenorN; tenorIndex++){
		
			quantoSauros::Tenor volTenor = quantoSauros::Tenor(new_volSurfaceTenorsT[tenorIndex]);
			std::vector<quantoSauros::Volatility> vols;

			for (int maturityIndex = 0; maturityIndex < in_volMaturityN; maturityIndex++){	

				quantoSauros::Tenor volMaturity = quantoSauros::Tenor(new_volSurfaceMaturitiesT[maturityIndex]);
				Volatility vol = in_volSurface[maturityIndex + tenorIndex * in_volTenorN];
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
	#pragma region Range Accrual Note
		Currency currency = quantoSauros::util().Currency(new_ccyCd[0]);			
		QuantLib::Money notional(currency, in_NotionalAmount);
		QuantLib::DayCounter dcf1 = quantoSauros::util().DayCounter(new_dcf[0]);
		
		//Range 구간 데이터
		Date* exerciseDate = new Date[rangeN + 1];
		double* exercisePrice = new double[rangeN + 1];
		Date* rangeStartDates = new Date[rangeN + 1];
		Date* rangeEndDates = new Date[rangeN + 1];
		bool* exerciseFlag = new bool[rangeN];

		for (int i = 0; i < rangeN; i++){
			exerciseFlag[i] = quantoSauros::util().TrueOrFalse(new_callFlags[i]);
			rangeStartDates[i] = quantoSauros::util().Date(new_rangeStartDays[i]);
			rangeEndDates[i] = quantoSauros::util().Date(new_rangeEndDays[i]);
			if (exerciseFlag[i] == true){
				exerciseDate[i] = quantoSauros::util().Date(new_rangeEndDays[i]);				
				exercisePrice[i] = 1.0;
			}
		}
		
		QuantLib::Date issueDate = quantoSauros::util().Date(new_issueDate[0]);			
		QuantLib::Date maturityDate = quantoSauros::util().Date(new_maturityDate[0]);				

		std::vector<QuantLib::Rate> inCouponRates;
		std::vector<QuantLib::Rate> outCouponRates;
		std::vector<QuantLib::Rate> rangeUpperRates;
		std::vector<QuantLib::Rate> rangeLowerRates;
		std::vector<quantoSauros::Period> rangePeriods;

		for (int i = 0; i < rangeN; i++){
			
			inCouponRates.push_back(in_inCouponRates[i]);
			outCouponRates.push_back(in_outCouponRates[i]);
			if (exerciseFlag[i] == true){
				rangePeriods.push_back(
					quantoSauros::Period(rangeStartDates[i], rangeEndDates[i], 
						exerciseDate[i], exercisePrice[i]));
			} else {
				rangePeriods.push_back(
					quantoSauros::Period(rangeStartDates[i], rangeEndDates[i]));
			}
			
			rangeUpperRates.push_back(in_upperBounds1[i]);
			rangeLowerRates.push_back(in_lowerBounds1[i]);
			
		}

		//TODO 
		bool includePrincipal = true;
		QuantLib::Option::Type optionType = QuantLib::Option::Type::Call;		

		//TODO hull-white calibration
		Real meanReversion = 0.03;
		Real sigma = 0.01;		
		
		//test of LegInfo
		//TODO
		QuantLib::Frequency couponFrequency = Quarterly;
		quantoSauros::NoteLegScheduleInfo* scheduleInfo = new
			quantoSauros::NoteLegScheduleInfo(
				currency, rangePeriods, couponFrequency, dcf1, issueDate, maturityDate);

		quantoSauros::NoteLegAmortizationInfo* amortizationInfo = 
			new quantoSauros::NoteLegAmortizationInfo(notional, includePrincipal);

		std::vector<quantoSauros::RateType> rateTypes;
		std::vector<double> tenors;
		std::vector<QuantLib::Frequency> swapCouponFrequencies;
		std::vector<quantoSauros::NoteLegRangeCouponInfo*> couponInfos;
		for (int i = 0; i < referenceN; i++){
			rateTypes.push_back(quantoSauros::util::RateType(new_rateType[i]));
			tenors.push_back(in_tenor[i]);
			swapCouponFrequencies.push_back(quantoSauros::util::Frequency(new_swapCouponFrequency[i]));

			quantoSauros::ReferenceType referenceType = quantoSauros::util::ReferenceType(new_referenceType[i]);

			quantoSauros::NoteLegRangeCouponInfo* couponInfo;
			if (referenceType == quantoSauros::Spot){
				couponInfo = new quantoSauros::NoteLegSpotRangeCouponInfo(
					rateTypes[i], tenors[i], swapCouponFrequencies[i], irCurve, 
					inCouponRates, outCouponRates, 
					rangeUpperRates, rangeLowerRates);
			} else if (referenceType == quantoSauros::Spread){
				//TODO
				couponInfo = new quantoSauros::NoteLegSpotRangeCouponInfo(
					rateTypes[i], tenors[i], swapCouponFrequencies[i], irCurve, 
					inCouponRates, outCouponRates, 
					rangeUpperRates, rangeLowerRates);
			}

			couponInfos.push_back(couponInfo);
		}

		double executiveAccruedCoupon = 0;
		quantoSauros::NoteLegDataInfo* dataInfo = new
			quantoSauros::NoteLegDataInfo(executiveAccruedCoupon, in_monitorFrequency);

		quantoSauros::NoteLegOptionInfo* optionInfo = new
			quantoSauros::NoteLegOptionInfo(optionType);

		quantoSauros::HullWhiteParameters floatParam = 
			quantoSauros::HullWhiteParameters(meanReversion, sigma);
		
		quantoSauros::HullWhiteParameters discountParam = 
			quantoSauros::HullWhiteParameters(meanReversion, sigma);
		
		std::vector<quantoSauros::IRInfo> irInfos;
		for (int i = 0; i < referenceN; i++){
			irInfos.push_back(quantoSauros::IRInfo(irCurve, volSurfaces, floatParam, 
				rateTypes[i] ,tenors[i], swapCouponFrequencies[i]));
		}	
		
		quantoSauros::IRInfo discountInfo = 
			quantoSauros::IRInfo(discountCurve, volSurfaces, discountParam);

		QuantLib::Matrix correlationMatrix(2,2);		
		correlationMatrix[0][0] = 1.0;
		correlationMatrix[0][1] = 1.0;
		correlationMatrix[1][0] = 1.0;
		correlationMatrix[1][1] = 1.0;

		quantoSauros::CorrelationInfo correlationInfo = 
			quantoSauros::CorrelationInfo(correlationMatrix);

		std::vector<QuantLib::HullWhiteVolatility> hullWhiteVolatilities;
		QuantLib::HullWhiteVolatility discountHWVolatility;

		////////////////////////////////////////////////////////////////////
		std::vector<Volatility> vols;
		vols.push_back(0.012);
		vols.push_back(0.013);
		vols.push_back(0.016);
		vols.push_back(0.020);
		std::vector<Date> volDates;
		volDates.push_back(Date(Day(1), Month(1), Year(2014)));
		volDates.push_back(Date(Day(1), Month(7), Year(2014)));
		volDates.push_back(Date(Day(1), Month(1), Year(2015)));
		volDates.push_back(Date(Day(1), Month(7), Year(2015)));
		HullWhiteVolatility hwVols(todayDate, dcf, vols, volDates);
		////////////////////////////////////////////////////////////////////
				
		hullWhiteVolatilities.push_back(hwVols);

		discountHWVolatility = hwVols;


		//boost::function<Real (Time)> tmpVols = hwVols.vol();
		//double aatet = tmpVols(0.5);

		//Constructor of a Range Accrual Note
		quantoSauros::RangeAccrualNote raNote(
			scheduleInfo, amortizationInfo, 
			couponInfos, dataInfo, optionInfo,
			irInfos, discountInfo, correlationInfo, in_simulationNum);

		//getPrice Method of a Range Accrual Note		
		QuantLib::Money result = raNote.getPrice(todayDate, hullWhiteVolatilities, discountHWVolatility);
		//QuantLib::Money result = QuantLib::Money();

	#pragma endregion

		//MarketModelTest marketModelTest;
		//marketModelTest.testCallableSwapLS();


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
		
		return result.value();
}
