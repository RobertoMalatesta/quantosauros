#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{
	class NoteLegOptionInfo : public AbstractLegOptionInfo {
		public:
			NoteLegOptionInfo(){

			};
			NoteLegOptionInfo(QuantLib::Option::Type optionType)
				: AbstractLegOptionInfo(optionType){
				
			};		
	};

	class SwapLegOptionInfo : public AbstractLegOptionInfo {
		public:

	};

}
