#pragma once
#include <ql/quantlib.hpp>
#include <ql/math/statistics/sequencestatistics.hpp>
#include <ql/math/matrixutilities/svd.hpp>

namespace quantoSauros {

	struct LSMCData {
		std::vector<double> values;
		double cumulatedCashFlows;
		double exerciseValue;
		double controlValue;
		bool isVaild;
	};

	double LongstaffSchwartz(
		std::vector<quantoSauros::LSMCData> inputData, 
		std::vector<double>& continuation
		);

}