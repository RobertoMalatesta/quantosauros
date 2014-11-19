#include "LongstaffSchwartz.h";

namespace quantoSauros {

	double LongstaffSchwartz(
		std::vector<quantoSauros::LSMCData> inputData,
		std::vector<double>& continuation) {
			
			/*std::vector<double> basisCoefficients;*/
			// 1) find the covariance matrix of basis function values and
            //    deflated cash-flows

			int N = inputData.front().values.size();
			std::vector<double> temp(N + 1);
			QuantLib::SequenceStatistics stats(N + 1);
			
			for (int i = 0; i < inputData.size(); i++) {
				if (inputData[i].isVaild){
					std::copy(inputData[i].values.begin(),
						inputData[i].values.end(),
						temp.begin());		
					//TODO
					temp.back() = inputData[i].cumulatedCashFlows 
						- inputData[i].controlValue;
					stats.add(temp);
				}				
			}

			std::vector<double> means = stats.mean();
			QuantLib::Matrix covariance = stats.covariance();

			QuantLib::Matrix C(N, N);
			QuantLib::Array target(N);

			for (int i = 0; i < N; ++i){
				target[i] = covariance[i][N] + means[i] * means[N];
				for (int j = 0; j <= i; ++j){
					C[i][j] = C[j][i] = covariance[i][j] + means[i] * means[j];
				}
			}

			// 2) solve for least square regression			
			QuantLib::Array alphas = QuantLib::SVD(C).solveFor(target);
            /*basisCoefficients.resize(N);*/
            /*std::copy(alphas.begin(), alphas.end(),
                      basisCoefficients.begin());*/
			
			// 3) use exercise strategy to divide paths into exercise and
            //    non-exercise domains
			for (int i = 0; i < inputData.size(); i++) {
				if (inputData[i].isVaild){

					double estimatedContinuationValue = 
						std::inner_product(inputData[i].values.begin(),
                                 inputData[i].values.end(),
                                 alphas.begin(),
                                 inputData[i].controlValue);
					// for exercise paths, add deflated rebate to
                    // deflated cash-flows at previous time frame;
                    // for non-exercise paths, add deflated cash-flows to
                    // deflated cash-flows at previous time frame
					continuation[i] = estimatedContinuationValue;
				}
			}

			return 0;
	}
}