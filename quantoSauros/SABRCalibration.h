// Chapter 12 SABR Model Calibration
// Letian Wang (www.letianwang.net)
// The model is presented in Chapter 12
 
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <fstream>
#include <ql/quantlib.hpp>
using namespace QuantLib;
 
/////////////////////////////////// Header ///////////////////////////////////////////////
// QuantLib calibrates sigma0 (alpha) together with other parameters.
// For example, see the call of SABRInterpolation in
//              SwaptionVolCube1::sabrCalibration(const Cube& marketVolCube)
// Here it follows our discussion and estimate sigma0 by solving the cubic equation
//              This has to be done for each calibration iteration.
 
// In theory we only need to change the process of how sigma0 is determined
//              in practice we have to alter all the classes related to class SABRInterpolationImpl
//              because they are private
// So except for sigma0 part, other parts are adapted from class SABRInterpolationImpl
 
// 1. class SABRInterpolatkionImplAdj
// 1.1 class SabrParametersTransformation
// 1.2 class SABRError
// It is advised to locate ATM volatility according to I1 and I2.
//              for convenience it is pointed out explicitly
template <class I1, class I2>
class SABRInterpolationImplAdj : public Interpolation::templateImpl<I1,I2>,
        public detail::SABRCoeffHolder {
public:
        SABRInterpolationImplAdj(
                const I1& xBegin, const I1& xEnd,
                const I2& yBegin,
                Time t,
                const Real& forward,
                Real alpha, Real beta, Real nu, Real rho,
                Real atmVol,                                    // Designate the ATM vol
                bool alphaIsFixed,
                bool betaIsFixed,
                bool nuIsFixed,
                bool rhoIsFixed,
                bool vegaWeighted,
                const boost::shared_ptr<EndCriteria>& endCriteria,
                const boost::shared_ptr<OptimizationMethod>& optMethod)
                : Interpolation::templateImpl<I1,I2>(xBegin, xEnd, yBegin),
                SABRCoeffHolder(t, forward, alpha, beta, nu, rho,
                alphaIsFixed,betaIsFixed,nuIsFixed,rhoIsFixed),
                endCriteria_(endCriteria), optMethod_(optMethod),
                forward_(forward),
                vegaWeighted_(vegaWeighted),
                atmVol_(atmVol)
        {
                // if no optimization method or endCriteria is provided, we provide one
                if (!optMethod_)
                        //optMethod_ = boost::shared_ptr<OptimizationMethod>(new
                        //    LevenbergMarquardt(1e-8, 1e-8, 1e-8));
                        optMethod_ = boost::shared_ptr<OptimizationMethod>(new
                        Simplex(0.01));
                if (!endCriteria_) {
                        endCriteria_ = boost::shared_ptr<EndCriteria>(new
                                EndCriteria(60000, 100, 1e-8, 1e-8, 1e-8));
                }
                weights_ = std::vector<Real>(xEnd-xBegin, 1.0/(xEnd-xBegin));
        }
       
        void update() {
                // forward_ might have changed
                QL_REQUIRE(forward_>0.0, "at the money forward rate must be "
                        "positive: " << io::rate(forward_) << " not allowed");
               
                // we should also check that y contains positive values only
               
                // we must update weights if it is vegaWeighted
                if (vegaWeighted_) {
                        std::vector<Real>::const_iterator x = this->xBegin_;
                        std::vector<Real>::const_iterator y = this->yBegin_;
                        //std::vector<Real>::iterator w = weights_.begin();
                        weights_.clear();
                        Real weightsSum = 0.0;
                        for ( ; x!=this->xEnd_; ++x, ++y) {
                                Real stdDev = std::sqrt((*y)*(*y)*t_);
                                weights_.push_back(
                                        blackFormulaStdDevDerivative(*x, forward_, stdDev));
                                weightsSum += weights_.back();
                        }
                        // weight normalization
                        std::vector<Real>::iterator w = weights_.begin();
                        for ( ; w!=weights_.end(); ++w)
                                *w /= weightsSum;
                }
               
                // there is nothing to optimize
                if (alphaIsFixed_ && betaIsFixed_ && nuIsFixed_ && rhoIsFixed_) {
                        error_ = interpolationError();
                        maxError_ = interpolationMaxError();
                        SABREndCriteria_ = EndCriteria::None;
                        return;
                       
                } else {
               
                        SABRError costFunction(this);
                        transformation_ = boost::shared_ptr<ParametersTransformation>(new
                                SabrParametersTransformation);
                       
                        Array guess(4);
                        guess[0] = alpha_;
                        guess[1] = beta_;
                        guess[2] = nu_;
                        guess[3] = rho_;
                       
                        std::vector<bool> parameterAreFixed(4);
                        parameterAreFixed[0] = alphaIsFixed_;
                        parameterAreFixed[1] = betaIsFixed_;
                        parameterAreFixed[2] = nuIsFixed_;
                        parameterAreFixed[3] = rhoIsFixed_;
                       
                        Array inversedTransformatedGuess(transformation_->inverse(guess));
                       
                        ProjectedCostFunction constrainedSABRError(costFunction,
                                inversedTransformatedGuess, parameterAreFixed);
                       
                        Array projectedGuess
                                (constrainedSABRError.project(inversedTransformatedGuess));
                       
                        NoConstraint constraint;
                        Problem problem(constrainedSABRError, constraint, projectedGuess);
                        SABREndCriteria_ = optMethod_->minimize(problem, *endCriteria_);
                        Array projectedResult(problem.currentValue());
                        Array transfResult(constrainedSABRError.include(projectedResult));
 
 
                        Array result = transformation_->direct(transfResult);
                        //alpha_ = result[0];
                        beta_  = result[1];
                        nu_    = result[2];
                        rho_   = result[3];
                        alpha_ = findAlpha(beta_, rho_,nu_, t_, forward_,atmVol_);
                                               
                }
                error_ = interpolationError();
                maxError_ = interpolationMaxError();
               
        }
 
        Real value(Real x) const {
                QL_REQUIRE(x>0.0, "strike must be positive: " <<
                        io::rate(x) << " not allowed");
                return sabrVolatility(x, forward_, t_,
                        alpha_, beta_, nu_, rho_);
        }
       
        Real primitive(Real) const {
                QL_FAIL("SABR primitive not implemented");
        }
       
        Real derivative(Real) const {
                QL_FAIL("SABR derivative not implemented");
        }
       
        Real secondDerivative(Real) const {
                QL_FAIL("SABR secondDerivative not implemented");
        }
        // calculate total squared weighted difference (L2 norm)
        Real interpolationSquaredError() const {
                Real error, totalError = 0.0;
                std::vector<Real>::const_iterator x = this->xBegin_;
                std::vector<Real>::const_iterator y = this->yBegin_;
                std::vector<Real>::const_iterator w = weights_.begin();
                for (; x != this->xEnd_; ++x, ++y, ++w) {
                        error = (value(*x) - *y);
                        totalError += error*error * (*w);
                }
                return totalError;
        }
        // calculate weighted differences
        Disposable<Array> interpolationErrors(const Array&) const {
                Array results(this->xEnd_ - this->xBegin_);
                std::vector<Real>::const_iterator x = this->xBegin_;
                Array::iterator r = results.begin();
                std::vector<Real>::const_iterator y = this->yBegin_;
                std::vector<Real>::const_iterator w = weights_.begin();
                for (; x != this->xEnd_; ++x, ++r, ++w, ++y) {
                        *r = (value(*x) - *y)* std::sqrt(*w);
                }
                return results;
        }
 
       
        Real interpolationError() const {
                Size n = this->xEnd_-this->xBegin_;
                Real squaredError = interpolationSquaredError();
                return std::sqrt(n*squaredError/(n-1));
        }
       
        Real interpolationMaxError() const {
                Real error, maxError = QL_MIN_REAL;
                I1 i = this->xBegin_;
                I2 j = this->yBegin_;
                for (; i != this->xEnd_; ++i, ++j) {
                        error = std::fabs(value(*i) - *j);
                        maxError = std::max(maxError, error);
                }
                return maxError;
        }
 
private:
        // 1.1 class SabrParametersTransformation
        class SabrParametersTransformation :
                public ParametersTransformation {
                        mutable Array y_;
                        const Real eps1_, eps2_, dilationFactor_ ;
        public:
                SabrParametersTransformation() : y_(Array(4)),
                        eps1_(.0000001),
                        eps2_(.9999),
                        dilationFactor_(0.001){
                }
               
                Array direct(const Array& x) const {
                        y_[0] = x[0]*x[0] + eps1_;
                        //y_[1] = std::atan(dilationFactor_*x[1])/M_PI + 0.5;
                        y_[1] = std::exp(-(x[1]*x[1]));
                        y_[2] = x[2]*x[2] + eps1_;
                        y_[3] = eps2_ * std::sin(x[3]);
                        return y_;
                }
 
                Array inverse(const Array& x) const {
                        y_[0] = std::sqrt(x[0] - eps1_);
                        //y_[1] = std::tan(M_PI*(x[1] - 0.5))/dilationFactor_;
                        y_[1] = std::sqrt(-std::log(x[1]));
                        y_[2] = std::sqrt(x[2] - eps1_);
                        y_[3] = std::asin(x[3]/eps2_);
                       
                        return y_;
                }
        };
 
        // 1.2 class SABRError
        class SABRError : public CostFunction {
        public:
                SABRError(SABRInterpolationImplAdj* sabr)
                        : sabr_(sabr) {}
               
                Real value(const Array& x) const {
                        const Array y = sabr_->transformation_->direct(x);
                        sabr_->alpha_ = y[0];
                        sabr_->beta_  = y[1];
                        sabr_->nu_    = y[2];
                        sabr_->rho_   = y[3];
                        sabr_->alpha_ = sabr_->findAlpha(sabr_->beta_, sabr_->rho_,
                                sabr_->nu_, sabr_->t_, sabr_->forward_,sabr_->atmVol_);
                        return sabr_->interpolationSquaredError();
                }
 
                Disposable<Array> values(const Array& x) const{
                        const Array y = sabr_->transformation_->direct(x);
                        sabr_->alpha_ = y[0];
                        sabr_->beta_  = y[1];
                        sabr_->nu_    = y[2];
                        sabr_->rho_   = y[3];
                        sabr_->alpha_ = sabr_->findAlpha(sabr_->beta_, sabr_->rho_,
                                sabr_->nu_, sabr_->t_, sabr_->forward_, sabr_->atmVol_);
                        return sabr_->interpolationErrors(x);
                }
               
        private:
                SABRInterpolationImplAdj* sabr_;
        };
 
        boost::shared_ptr<EndCriteria> endCriteria_;
        boost::shared_ptr<OptimizationMethod> optMethod_;
        bool vegaWeighted_;
        boost::shared_ptr<ParametersTransformation> transformation_;
        NoConstraint constraint_;
 
public:
        const Real& forward_;
        const Real atmVol_;             // ATM volatility
        // solver
        double findAlpha(double beta,double rho, double v, double T,
                double f0, double atm) const;
};      // End of class SABRInterpolationImplAdj
 
// step 2: find alpha
// chapter 5.6 numerical c++
template <class I1, class I2>
double SABRInterpolationImplAdj<I1,I2>::findAlpha
(double beta, double rho, double v, double T, double f0,double atm) const
{
        const double epsilon = 1.0e-8;
        // prepare for the coefficients
        // a0 x^3 + b0 x^2 + c0 x + d0 = 0
        double a, b, c;
        double a0, b0, c0, d0;
        double A, Q, R, theta;
        double x, x1, x2, x3;
        // a0 can not be zero
        a0 = (1.0-beta)*(1.0-beta)*T / 24.0 / std::pow(f0, 2.0-2.0*beta);
        b0 = rho*beta*v*T / 4.0 / std::pow(f0, 1-beta);
        c0 = 1.0 + (2.0-3.0*rho*rho)*v*v*T/24.0;
        d0 = -atm * std::pow(f0, 1-beta);
 
        a = b0/a0; b = c0/a0; c = d0/a0;
        Q = (a*a -3.0*b) / 9.0;
        R = (2.0*a*a*a - 9.0*a*b + 27.0*c) / 54.0;
       
        if (R*R < Q*Q*Q) {              // three real roots
                theta = std::acos(R / std::sqrt(Q*Q*Q));
                x1 = -2.0*std::sqrt(Q)*std::cos(theta/3.0)-a/3.0;
                x2 = -2.0*std::sqrt(Q)*std::cos( (theta+2.0*M_PI)/3.0 )-a/3.0;
                x3 = -2.0*std::sqrt(Q)*std::cos( (theta-2.0*M_PI)/3.0 )-a/3.0;
               
                x = std::max(x1, std::max(x2,x3));
                if (x1 > 0.0)
                        x = std::min(x,x1);
                if (x2 > 0.0)
                        x = std::min(x,x2);
                if (x3 > 0.0)
                        x = std::min(x,x3);
        } else {                // one real root
                double sgn;
                sgn = R > 0.0 ? 1.0 : -1.0;
                A = -sgn*std::pow(std::abs(R)+std::sqrt(R*R-Q*Q*Q),1.0/3.0);
                double B;
                if (std::abs(A) < epsilon)
                        B = 0.0;
                else
                        B = Q/A;
                x = (A+B)-a/3.0;
        }
 
        return std::max(x,0.0);
}
 
 
// 2. class SABRInterpolationAdj
//! %SABR smile interpolation between discrete volatility points.
class SABRInterpolationAdj : public Interpolation {
public:
        template <class I1, class I2>
        SABRInterpolationAdj(const I1& xBegin,  // x = strikes
                const I1& xEnd,
                const I2& yBegin,  // y = volatilities
                Time t,            // option expiry
                const Real& forward,
                Real alpha,
                Real beta,
                Real nu,
                Real rho,
                Real atmVol,
                bool alphaIsFixed,
                bool betaIsFixed,
                bool nuIsFixed,
                bool rhoIsFixed,
                bool vegaWeighted = true,
                const boost::shared_ptr<EndCriteria>& endCriteria
                = boost::shared_ptr<EndCriteria>(),
                const boost::shared_ptr<OptimizationMethod>& optMethod
                = boost::shared_ptr<OptimizationMethod>()) {
                       
                        impl_ = boost::shared_ptr<Interpolation::Impl>(new
                                SABRInterpolationImplAdj<I1,I2>(xBegin, xEnd, yBegin,
                                t, forward,
                                alpha, beta, nu, rho,
                                atmVol,
                                alphaIsFixed, betaIsFixed,
                                nuIsFixed, rhoIsFixed,
                                vegaWeighted,
                                endCriteria,
                                optMethod));
                        coeffs_ =
                                boost::dynamic_pointer_cast<detail::SABRCoeffHolder>(
                                impl_);
        }
        Real expiry()  const { return coeffs_->t_; }
        Real forward() const { return coeffs_->forward_; }
        Real alpha()   const { return coeffs_->alpha_; }
        Real beta()    const { return coeffs_->beta_; }
        Real nu()      const { return coeffs_->nu_; }
        Real rho()     const { return coeffs_->rho_; }
        Real rmsError() const { return coeffs_->error_; }
        Real maxError() const { return coeffs_->maxError_; }
        const std::vector<Real>& interpolationWeights() const {
                return coeffs_->weights_; }
        EndCriteria::Type endCriteria(){ return coeffs_->SABREndCriteria_; }
       
private:
        boost::shared_ptr<detail::SABRCoeffHolder> coeffs_;
};
 
// class 3: SABRAdj
//! %SABR interpolation factory and traits
class SABRAdj {
public:
        SABRAdj(Time t, Real forward,
                Real alpha, Real beta, Real nu, Real rho,
                Real atmVol,
                bool alphaIsFixed, bool betaIsFixed,
                bool nuIsFixed, bool rhoIsFixed,
                bool vegaWeighted = false,
                const boost::shared_ptr<EndCriteria> endCriteria
                = boost::shared_ptr<EndCriteria>(),
                const boost::shared_ptr<OptimizationMethod> optMethod
                = boost::shared_ptr<OptimizationMethod>())
                : t_(t), forward_(forward),
                alpha_(alpha), beta_(beta), nu_(nu), rho_(rho),
                atmVol_(atmVol),
                alphaIsFixed_(alphaIsFixed), betaIsFixed_(betaIsFixed),
                nuIsFixed_(nuIsFixed), rhoIsFixed_(rhoIsFixed),
                vegaWeighted_(vegaWeighted),
                endCriteria_(endCriteria),
                optMethod_(optMethod) {}
        template <class I1, class I2>
        Interpolation interpolate(const I1& xBegin, const I1& xEnd,
                const I2& yBegin) const {
                        return SABRInterpolationAdj(xBegin, xEnd, yBegin,
                                t_,  forward_,
                                alpha_, beta_, nu_, rho_,
                                atmVol_,
                                alphaIsFixed_, betaIsFixed_,
                                nuIsFixed_, rhoIsFixed_,
                                vegaWeighted_,
                                endCriteria_, optMethod_);
        }
        static const bool global = true;
private:
        Time t_;
        Real forward_;
        Real alpha_, beta_, nu_, rho_;
        Real atmVol_;
        bool alphaIsFixed_, betaIsFixed_, nuIsFixed_, rhoIsFixed_;
        bool vegaWeighted_;
        const boost::shared_ptr<EndCriteria> endCriteria_;
        const boost::shared_ptr<OptimizationMethod> optMethod_;
};
 
// 4: class BlackNormalImpliedStdDevHelper
class BlackNormalImpliedStdDevHelper
{
public:
        BlackNormalImpliedStdDevHelper(Option::Type optionType,
                Real strike,
                Real forward,
                Real discount,
                Real blackPrice)
                : type_(optionType),blackPrice_(blackPrice),
                strike_(strike), forward_(forward), discount_(discount)
        {}
 
        double operator() (double stdDev) const {
                Real result;
                Real h = (forward_-strike_)*type_, d = h/stdDev;
        if (stdDev==0.0)
                        result = 0.0;
                else {
                        //CumulativeNormalDistribution phi;
                        //result = discount_*stdDev*(phi.derivative(d*type_) + d*phi(d));
                        result = bachelierBlackFormula(type_,strike_, forward_,
                                stdDev, discount_);
                }
 
        return result - blackPrice_;
        }
private:
        Option::Type type_;
        Real strike_, forward_,discount_, blackPrice_;
};
 
// 5: function blackImpliedNormalStdDev
double blackImpliedNormalStdDev(
        Option::Type optionType,
        Real strike,
        Real forward,
        Real blackPrice,
        Real discount,
        Real guess,
        Real accuracy = 1.0e-8,
        Natural maxIterations = 2000)
{
        BlackNormalImpliedStdDevHelper f(optionType, strike, forward,
                discount, blackPrice);
        Brent solver;
        solver.setMaxEvaluations(maxIterations);
        Real minSdtDev = 0.0, maxStdDev = 3.0;
        Real stdDev = solver.solve(f, accuracy, guess, minSdtDev, maxStdDev);
        QL_ENSURE(stdDev>=0.0,
                "stdDev (" << stdDev << ") must be non-negative");
        return stdDev;
}

class SABRTest {
public:
		static void run(){
			SavedSettings oldSettings;
			///////////////////////////// Input Data //////////////////////////////////
			double discountFactor = 0.999356;               // T: Monday, Sep 19, 2011
			Natural nDays = 109;                                    // T = 109 days
			double F0 = 99.665;                                             // today's futures
			struct PnK {
					double P;
					double K;
			};
 
			PnK marketQuotes[] = {
					{0.0025, 98.625},
					{0.0050, 98.750},
					{0.0075, 98.875},
					{0.0100, 99.000},
					{0.0125, 99.125},
					{0.0150, 99.250},
					{0.0200, 99.375},
					{0.0275, 99.500},
					{0.0450, 99.625},               // atm 99.665
					{0.0950, 99.750},               // atm 99.665
					{0.2100, 99.875},
					{0.3350, 100.00},
					{0.4600, 100.125},
					{0.5850, 100.250},
					{0.7100, 100.375},
					{0.8350, 100.500},
					{0.9600, 100.625},
					{1.0850, 100.750},
					{1.2100, 100.875},
					{1.3350, 101.000},
					{1.4600, 101.125},
					{1.5850, 101.250},
					{1.8350, 101.50},
					{2.0850, 101.750},
					{2.3350, 102.000},
					{2.5850, 102.250},
					{2.835, 102.50},
					{3.085, 102.750},
					{3.335, 103.00}
			};
 
			///////////////////////////// Settings //////////////////////////////////
			Date today(2, June, 2011);
			Date maturityDate(21, Sep, 2011);
			Integer fixingDays = 2;
 
			Calendar calendar = UnitedStates(UnitedStates::Settlement);
			DayCounter dayCounter = Actual360();
			Date settlementDate = calendar.advance(today, fixingDays,Days);
			Settings::instance().evaluationDate() = settlementDate;
			BusinessDayConvention convention = ModifiedFollowing;
			bool endofMonth = true;
 
			// calculate implied volatilities
			std::vector<double> K;
			std::vector<double> P;
			std::vector<double> IV;
			double F;
			double tao;
 
			F = (100.0-F0) / 100.0;
			tao = dayCounter.yearFraction(settlementDate, maturityDate);
			for (Size i = 0; i < sizeof(marketQuotes)/sizeof(marketQuotes[0]);i++)
			{
					double iv, k, p,f;
					p = (marketQuotes[i].P * 2500.0) / (1000000.0) * 4.0;
					k = (100.0 - marketQuotes[i].K) / 100.0;
 
					iv = blackImpliedNormalStdDev(Option::Call, k, F, p, discountFactor, 0.01);
					//iv = blackFormulaImpliedStdDev(Option::Call, k,
					//      F, p, discountFactor);
					iv = iv / std::sqrt(tao);
 
					// K.push_back(k);
					// We have to use original K and F for interpolation
					//              because they can not be negative
					K.push_back(marketQuotes[i].K);
					P.push_back(p);
					IV.push_back(iv);
			}
 
 
			//////////////////////////////////////////////////////////
			// beta is fixed to 0.5
			// while the rest three parameters are free to change
			double error1, error2;
			Real beta = 0.5;
			Real initialAlpha = 0.3;
			Real initialNu = 0.02;
			Real initialRho = 0.01;
 
			//boost::shared_ptr<OptimizationMethod> method (
			//      new LevenbergMarquardt(1e-8,1e-8,1e-8) );
			boost::shared_ptr<OptimizationMethod> method (
					new Simplex(0.001) );
			boost::shared_ptr<EndCriteria> endCriteria(new
						EndCriteria(10000000, 1000, 1e-8, 1e-8, 1e-8));
 
			// alpha, rho, v simultaneously
			SABRInterpolation sabrInterpolation1(K.begin(), K.end(),
					IV.begin(), tao, F0,
					initialAlpha, beta, initialNu, initialRho,
					false, true,
					false, false,
					false,                  // not vega weighted
					endCriteria, method);
			sabrInterpolation1.update();
 
			Real calibratedBeta1 = sabrInterpolation1.beta();
			Real calibratedAlpha1 = sabrInterpolation1.alpha();
			Real calibratedNu1 = sabrInterpolation1.nu();
			Real calibratedRho1 = sabrInterpolation1.rho();
			error1 = sabrInterpolation1.rmsError();
 
 
			// alpha is calibrated to ATM vol
			double atmVol = (4.0*IV[8]+IV[9])/5.0;
			SABRInterpolationAdj sabrInterpolation2(K.begin(), K.end(),
					IV.begin(), tao, F0,
					initialAlpha, beta, initialNu, initialRho,
					atmVol,
					false, true,
					false, false,
					false,
					endCriteria, method);
			sabrInterpolation2.update();
 
			Real calibratedBeta2 = sabrInterpolation2.beta();
			Real calibratedAlpha2 = sabrInterpolation2.alpha();
			Real calibratedNu2 = sabrInterpolation2.nu();
			Real calibratedRho2 = sabrInterpolation2.rho();
			error2 = sabrInterpolation2.rmsError();
 
			///////////////////////// output /////////////////////////
			std::ofstream fout("D:\\output.csv");
			for (Size i = 0; i < K.size(); i++)
			{
					double v1, v2;
					v1 = sabrVolatility(K[i], F0, tao,
							calibratedAlpha1, calibratedBeta1,
							calibratedNu1, calibratedRho1);
					v2 = sabrVolatility(K[i], F0, tao,
							calibratedAlpha2, calibratedBeta2,
							calibratedNu2, calibratedRho2);
					fout << marketQuotes[i].K << ","
							<< K[i] << ","
							<< IV[i] << ","
							<< v1 << ","
							<< v2 << ","
							<< std::endl;
			}
 
			fout.close();
			std::cout << "SABR calibration ends." << std::endl;
			std::cout << "SSE of quantlib method: " << error1 << std::endl;
			std::cout << "SSE of cubic equation method: " << error2 << std::endl;
			std::cout << "Check output.csv for volatilities." << std::endl;
 
			system("pause");
		}
};
