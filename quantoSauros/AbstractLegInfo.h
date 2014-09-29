#pragma once

namespace quantoSauros{
	class AbstractLegInfo {
		public:
			AbstractLegInfo(){}
	};

	class AbstractLegCouponInfo : public AbstractLegInfo {
		public:
			AbstractLegCouponInfo(){}
	};

	class AbstractLegScheduleInfo : public AbstractLegInfo {
		public:
			AbstractLegScheduleInfo(){}
	};

	class AbstractLegDataInfo : public AbstractLegInfo {
		public:
			AbstractLegDataInfo(){}
	};

	class AbstractLegOptionInfo : public AbstractLegInfo {
		public:
			AbstractLegOptionInfo(){}
	};
	
	class AbstractLegAmortizationInfo : public AbstractLegInfo {
		public:
			AbstractLegAmortizationInfo(){}
	};
}