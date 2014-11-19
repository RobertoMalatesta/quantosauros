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
#include "LegAmortizationInfo.h";
#include "LegCouponInfo.h";
#include "LegDataInfo.h";
#include "LegOptionInfo.h";
#include "LegScheduleInfo.h";
#include "IRInfo.h";
#include "CorrelationInfo.h";
#include "marketmodel.h"
//#include "test_libormarketmodel.h";
//#include "test_libormarketmodelprocess.h";
//#include "test_lowdiscrepancysequence.h";
//#include "dtoRateMarketData.h";

using namespace QuantLib;

double QUANTOSAUROS_API __stdcall test1(SAFEARRAY** input);

double QUANTOSAUROS_API __stdcall bootstrapping(SAFEARRAY** today,
	//�ݸ�����
	long rateN, double* market_ytmRates, double* market_discountRates, SAFEARRAY** spotRatesTenor,
	//����������
	long volMaturityN, long volTenorN, double* market_volSurface, 
	SAFEARRAY** volSurfaceMaturities, SAFEARRAY** volSurfaceTenors,
	//��ǰ����
	SAFEARRAY** in_ccyCd, long in_NotionalAmount, SAFEARRAY** in_dcf,
	long rangeN,
	double* inCouponRates1, double* outCouponRates1, 
	double* lowerBounds1, double* upperBounds1,
	SAFEARRAY** rangeStartDays, SAFEARRAY** rangeEndDays, 
	SAFEARRAY** callFlags, 
	SAFEARRAY** in_issueDate, SAFEARRAY** in_maturityDate,
	//�ݸ�����
	long referenceN, SAFEARRAY** in_referenceType, 
	SAFEARRAY** in_rateType, double* in_tenor, SAFEARRAY** swapCouponFrequency,
	//��Ÿ����
	long in_monitorFrequency, long in_simulationNum,
	//���
	double* price);


/*
20140416
[TODO] 
2. swaption ������ Ŭ���� �����

[DONE]
1. ������ Ŀ�� Ŭ���� �����

*/