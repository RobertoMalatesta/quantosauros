#pragma once

#include "AbstractLegInfo.h"

namespace quantoSauros{

#pragma region NoteLegScheduleInfo

	class NoteLegScheduleInfo : public AbstractLegScheduleInfo {
		public:
			//Constructor1
			NoteLegScheduleInfo(
				QuantLib::Currency currency,
				std::vector<quantoSauros::Period> periods,
				QuantLib::Frequency couponFrequency,
				QuantLib::DayCounter dayCounter)
					: AbstractLegScheduleInfo(currency, 
						periods, couponFrequency, dayCounter, 
						periods[0].getStartDate(), periods[periods.size()].getEndDate()){					
			}
			//Constructor2
			NoteLegScheduleInfo(
				QuantLib::Currency currency,
				std::vector<quantoSauros::Period> periods,
				QuantLib::Frequency couponFrequency,
				QuantLib::DayCounter dayCounter,
				QuantLib::Date issueDate,
				QuantLib::Date maturityDate)
					: AbstractLegScheduleInfo(currency, 
						periods, couponFrequency, dayCounter, issueDate, maturityDate) {					
			}

	};

#pragma endregion



#pragma region SwapLegScheduleInfo
	class SwapLegScheduleInfo : public AbstractLegScheduleInfo {
		public:

	};

#pragma endregion
}
