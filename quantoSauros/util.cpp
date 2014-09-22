#include "util.h";

namespace quantoSauros{

	util::util()
	{
	}
	/*
	LPCSTR* util::conversion(SAFEARRAY** safeArray)
	{
		long lElements; // number of elements in the array
		LPCSTR* result;
		// checking if it is an one-dimensional array
		if ( (*safeArray)->cDims != 1 ) {
			HRESULT lResult=SafeArrayLock(*safeArray);
			result =(LPCSTR*) (*safeArray)->pvData;
			return result;
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

		return result;
	}
	*/
	CString* util::conversion(SAFEARRAY** safeArray)
	{
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

	util::~util(void)
	{
	}
}