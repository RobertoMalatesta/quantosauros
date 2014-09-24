#pragma once
#include <ql/quantlib.hpp>
#include <atlstr.h>
#include <OleAuto.h>

#include "Tenor.h";

using namespace QuantLib;

namespace quantoSauros{

	class util
	{
	public:		
		static CString* util::conversion(SAFEARRAY** safeArray);
		static QuantLib::Currency util::Currency(CString str);
		static QuantLib::DayCounter util::DayCounter(CString str);

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
			currency = USDCurrency();
		} else if (str == "EUR"){
			currency == EURCurrency();
		} else if (str == "GBP") {
			currency == GBPCurrency();		
		//asia
		} else if (str == "KRW"){
			currency = KRWCurrency();
		} else if (str == "HKD"){
			currency == HKDCurrency();
		} else if (str == "JPY"){
			currency == JPYCurrency();
		} else if (str == "CNY"){
			currency == CNYCurrency();
		} else if (str == "SGD"){
			currency == SGDCurrency();
		}

		return currency;
	}

	inline QuantLib::DayCounter util::DayCounter(CString str){
		QuantLib::DayCounter dcf;
		if (str == "ACT365"){
			dcf = Actual365Fixed();
		} else if (str == "ACT360"){
			dcf = Actual360();
		} else if (str == "ACTACT"){
			dcf = ActualActual();
		} else {
			dcf = Actual360();
		}

		return dcf;
	}

}


