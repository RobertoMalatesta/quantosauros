#pragma once
#ifdef QUANTOSAUROS_EXPORTS
#define QUANTOSAUROS_API __declspec(dllexport)
#else
#define QUANTOSAUROS_API   __declspec(dllimport)
#endif
#include <Windows.h>
#include <OleAuto.h>
#include <ql\quantlib.hpp>
#include <atlstr.h>

#include "InterestRateCurve.h";
#include "VolatilityCurve.h";
#include "VolatilitySurface.h";
#include "util.h";
#include "Tenor.h";
#include "InterestRate.h";
#include "RangeAccrualNote.h";
#include "pathgenerator.h";
#include "shortratemodels.h";
#include "SABRCalibration.h";
#include "mclongstaffschwartzengine.h";
//#include "test_libormarketmodel.h";
//#include "test_libormarketmodelprocess.h";
//#include "test_lowdiscrepancysequence.h";
//#include "dtoRateMarketData.h";

using namespace QuantLib;

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
	double* OutZeroRates, long* OutDays);


/*
20140416
[TODO] 
2. swaption 변동성 클래스 만들기

[DONE]
1. 이자율 커브 클래스 만들기

*/