#pragma once
#include <ql/quantlib.hpp>
#include <atlstr.h>
#include <OleAuto.h>

#include "InterestRate.h";
#include "Tenor.h";

//using namespace QuantLib;

namespace quantoSauros{

	enum ReferenceType
	{
		Spot, Spread
	};

	class util
	{
	public:		
		static CString* util::conversion(SAFEARRAY** safeArray);
		static QuantLib::Currency util::Currency(CString str);
		static QuantLib::DayCounter util::DayCounter(CString str);
		static bool util::TrueOrFalse(CString str);
		static QuantLib::Date util::Date(CString str);
		static quantoSauros::RateType util::RateType(CString str);
		static QuantLib::Frequency util::Frequency(CString str);
		static quantoSauros::ReferenceType util::ReferenceType(CString str);
	};	

	inline CString* util::conversion(SAFEARRAY** safeArray){
		long lElements; // number of elements in the array
		LPCSTR* result;
		// checking if it is an one-dimensional array
		if ( (*safeArray)->cDims != 1 ) {
			return NULL;
		}
		// checking if it is an array of longs
		if ( (*safeArray)->cbElements != 4 ) return NULL;

		// how many elements are there in the array
		lElements=(*safeArray)->rgsabound[0].cElements;

		// locking the array before using its elements
		HRESULT lResult=SafeArrayLock(*safeArray);
		if(lResult)return NULL;

		// using the array
		result =(LPCSTR*) (*safeArray)->pvData;
      
		// releasing the array
		lResult=SafeArrayUnlock(*safeArray);
		if (lResult) return NULL;

		//convert LPCSTR* to CString
		USES_CONVERSION;		
		CString* cStringResult;
		cStringResult = new CString[(int)lElements];

		for (int i = 0; i < lElements; i++){
			cStringResult[i] = A2CT(result[i]);
		}
		return cStringResult;		
	}
	
	inline QuantLib::Currency util::Currency(CString str){
		QuantLib::Currency currency;
		//western
		if (str == "USD"){
			currency = QuantLib::USDCurrency();
		} else if (str == "EUR"){
			currency == QuantLib::EURCurrency();
		} else if (str == "GBP") {
			currency == QuantLib::GBPCurrency();		
		//asia
		} else if (str == "KRW"){
			currency = QuantLib::KRWCurrency();
		} else if (str == "HKD"){
			currency == QuantLib::HKDCurrency();
		} else if (str == "JPY"){
			currency == QuantLib::JPYCurrency();
		} else if (str == "CNY"){
			currency == QuantLib::CNYCurrency();
		} else if (str == "SGD"){
			currency == QuantLib::SGDCurrency();
		}

		return currency;
	}

	inline QuantLib::DayCounter util::DayCounter(CString str){
		QuantLib::DayCounter dcf;
		if (str == "ACT365"){
			dcf = QuantLib::Actual365Fixed();
		} else if (str == "ACT360"){
			dcf = QuantLib::Actual360();
		} else if (str == "ACTACT"){
			dcf = QuantLib::ActualActual();
		} else {
			dcf = QuantLib::Actual360();
		}

		return dcf;
	}
	
	inline bool util::TrueOrFalse(CString str){
		bool result;
		if (str == "True"){
			result = true;
		} else if (str == "False"){
			result = false;
		} else {
			result = NULL;
		}
		return result;
	}
	
	inline QuantLib::Date util::Date(CString str){
		int StartYear = atoi(str.Left(4));
		int StartMonth = atoi(str.Mid(5,2));
		int StartDay = atoi(str.Right(2));

		return QuantLib::Date(
			QuantLib::Day(StartDay), QuantLib::Month(StartMonth), 
			QuantLib::Year(StartYear));
	}
	
	inline quantoSauros::RateType util::RateType(CString str){

		quantoSauros::RateType result;
		if (str == "Forward" || str == "Deposit"){
			result = quantoSauros::DepositRate;
		} else if (str == "Swap"){
			result = quantoSauros::SwapRate;
		} else {
			result = quantoSauros::None;
		}
		return result;
	}

	inline QuantLib::Frequency util::Frequency(CString str){
		QuantLib::Frequency result;
		if (str =="Quarterly") {
			result = Quarterly;
		} else if (str == "Annually") {
			result = Annual;
		} else if (str == "Monthly") {
			result = Monthly;
		} else if (str == "Semianuual") {
			result = Semiannual;
		} else {
			result = NoFrequency;
		}
		return result;
	}

	inline quantoSauros::ReferenceType util::ReferenceType(CString str){

		quantoSauros::ReferenceType result;

		if (str == "Spot"){
			result = quantoSauros::Spot;
		} else if (str == "Spread"){
			result = quantoSauros::Spread;
		}
		return result;
	}
}


