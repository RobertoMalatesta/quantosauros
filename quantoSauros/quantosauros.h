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
//#include "test_libormarketmodel.h";
//#include "test_libormarketmodelprocess.h";
//#include "test_lowdiscrepancysequence.h";
//#include "dtoRateMarketData.h";

using namespace QuantLib;

int QUANTOSAUROS_API __stdcall bootstrapping(long today,
	long rateN, double* market_ytmRates, double* market_discountRates, SAFEARRAY** spotRatesTenor,
	long volMaturityN, long volTenorN, double* market_volSurface, 
	SAFEARRAY** volSurfaceMaturities, SAFEARRAY** volSurfaceTenors,
	double* OutZeroRates, long* OutDays);


/*
20140416
[TODO] 
2. swaption 변동성 클래스 만들기

[DONE]
1. 이자율 커브 클래스 만들기

*/