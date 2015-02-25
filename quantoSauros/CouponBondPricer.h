#pragma once
#include "AbstractPricer.h";
#include "RangeAccrualArguments.h";
#include "LongstaffSchwartz.h";
#include "CouponBondData.h"

namespace quantoSauros{
	class CouponBondPricer : public AbstractCouponBondPricer {
		public:
			CouponBondPricer(quantoSauros::RangeAccrualArguments args) : 
				AbstractCouponBondPricer(args.getAsOfDate() args.getSimulationNum(),
				args.getIRInfos().size(), args.getPeriods().size()){

					m_args = args;
			};
		private:
			quantoSauros::RangeAccrualArguments m_args;

	};
}