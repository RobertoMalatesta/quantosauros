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
		util();
		LPCSTR* conversion(SAFEARRAY** safeArray);

		~util(void);
	};	
}
struct RateData {
		enum RateType
		{
			SwapRate, DepositRate,
		};
		quantoSauros::Tenor tenor;
		//Integer n;
		//TimeUnit units;
		Rate rate;
		RateType type;
};
/*
template<typename T>
int ArraySize(T*& arr){
	return _msize(arr);
}
*/


