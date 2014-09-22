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
		//LPCSTR* conversion(SAFEARRAY** safeArray);
		CString* util::conversion(SAFEARRAY** safeArray);

		~util(void);
	};	
}


