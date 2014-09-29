#pragma once

#include "AbstractLegInfo.h"
#include <ql/quantlib.hpp>

namespace quantoSauros{
	class NoteLegOptionInfo : public AbstractLegOptionInfo {
		public:
			NoteLegOptionInfo(){}
			NoteLegOptionInfo(QuantLib::Option::Type optionType){
				m_optionType = optionType;
			}
			QuantLib::Option::Type getOptionType();
		protected:
			QuantLib::Option::Type m_optionType;
	};

	inline QuantLib::Option::Type NoteLegOptionInfo::getOptionType(){
		return m_optionType;
	}

	class SwapLegOptionInfo : public AbstractLegOptionInfo {
		public:

	};

}
