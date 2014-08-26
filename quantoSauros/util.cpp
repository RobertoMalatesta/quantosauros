#include "util.h";

namespace quantoSauros{

	util::util()
	{
	}
	LPCSTR* util::conversion(SAFEARRAY** safeArray)
	{
		long lElements; // number of elements in the array
		LPCSTR* result;
		// checking if it is an one-dimensional array
		if ( (*safeArray)->cDims != 1 ) return NULL;

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

	util::~util(void)
	{
	}
}