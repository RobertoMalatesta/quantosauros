#pragma once
#include <ql/quantlib.hpp>
#include <cstring>;
#include "InterestRateCurve.h";
#include "VolatilitySurface.h";
#include "HullWhiteParameters.h";

using namespace QuantLib;
namespace quantoSauros {
	class HullWhiteVolatilityCalibration{
		public:
			quantoSauros::HullWhiteVolatilityCalibration(){};
			quantoSauros::HullWhiteVolatilityCalibration(Date asOfDate, Date maturityDate, 
				std::vector<QuantLib::Date> exerciseDates, bool optionFlag,
				quantoSauros::InterestRateCurve spotCurve, 
				quantoSauros::VolatilitySurface surface,
				quantoSauros::HullWhiteParameters params,
				DayCounter dcf, Currency currency
				){
				_asOfDate = asOfDate;
				_maturityDate = maturityDate;
				_exerciseDates = exerciseDates;
				_optionType = optionFlag;
				_spotCurve = spotCurve;
				_swaptionSurface = surface;
				_meanReversion = params.getVolatility1F();
				_initialVol = params.getVolatility1F();
				_dcf = dcf;
				_currency = currency;
				countryCode = currency.code;
				
				

				if (countryCode == "KRW"){
					calendar = SouthKorea(SouthKorea::Settlement);
					_businessDayConvention = ModifiedFollowing;
					_settlementDay = 1;
					_floatCouponFrequency = Quarterly;
					_fixedCouponFrequency = Quarterly;
					
				} else if (countryCode == "USD"){
					calendar = UnitedStates(UnitedStates::Settlement);
					_businessDayConvention = ModifiedFollowing;
					_settlementDay = 2;
					_floatCouponFrequency = Semiannual;
					_fixedCouponFrequency = Annual;
				} else if (countryCode == "EUR"){
					calendar = UnitedKingdom(UnitedKingdom::Settlement); 
					_businessDayConvention = ModifiedFollowing;
					_settlementDay = 2;
					_floatCouponFrequency = Semiannual;
					_fixedCouponFrequency = Annual;
				} else {
					calendar = SouthKorea(SouthKorea::Settlement);
					_businessDayConvention = ModifiedFollowing;
					_settlementDay = 1;
					_floatCouponFrequency = Quarterly;
					_fixedCouponFrequency = Quarterly;
				}
				
				Date _adjustedAsOfDate = calendar.adjust(_asOfDate, _businessDayConvention);
				Date _adjustedSettlementDate = calendar.adjust(_adjustedAsOfDate, _businessDayConvention);
				
				if(_optionType == false || _exerciseDates.size() == 0){
					_swaptionInfo.reserve(1);
					_swaptionInfo[0].reserve(2);
					_swaptionInfo[0][0] = _dcf.yearFraction(_adjustedAsOfDate, _adjustedSettlementDate);
					_swaptionInfo[0][1] = _dcf.yearFraction(_adjustedSettlementDate, _maturityDate);
				} else {
					_swaptionInfo.reserve(_exerciseDates.size());
					for(int i = 0; i < _exerciseDates.size(); i++){
						_swaptionInfo[i].reserve(2);
						for(int j = 0; j < 2; j++){
							if(j == 0){
								_swaptionInfo[i][j] = _dcf.yearFraction(_adjustedAsOfDate, _exerciseDates[i]);
							} else{
								_swaptionInfo[i][j] = _dcf.yearFraction(_exerciseDates[i], _maturityDate);
							}
						}
					}
				}
			}
			
			virtual std::vector<Volatility> calibration(); 
			virtual void setVol(double newVol){
				_modelVol = newVol;
			}
			~HullWhiteVolatilityCalibration(void);
		private:
			Date _asOfDate;
			Date _maturityDate;
			Calendar calendar;
			quantoSauros::InterestRateCurve _spotCurve;
			std::vector<QuantLib::Date> _exerciseDates;
			std::vector<std::vector<double>> _swaptionInfo;
			quantoSauros::VolatilitySurface _swaptionSurface;
			double _initialVol;
			double _modelVol;
			double _meanReversion;
			Frequency _floatCouponFrequency;
			Frequency _fixedCouponFrequency;

			int _settlementDay;
			DayCounter _dcf;
			bool _optionType;
			Currency _currency;
			BusinessDayConvention _businessDayConvention;
			CString countryCode 
	};
}
	//private Date _asOfDate;
	//private InterestRateCurve _spotCurve;	
	////[swaptionN][0: swaption Maturity, 1: swaption Tenor]
	//private double[][] _swaptionInfo;	
	//private VolatilitySurface _swaptionSurface;	
	//private double _initialVol;
	//private DayCountFraction _dcf;
	//private double _meanReversion;
	//private Frequency _couponFrequency;
	//private HullWhite _model;
	//private Calendar _calendar;
	//private BusinessDayConvention _businessDayConvention;
	//private int _settlementDay;
	//private DayCountFraction _dcf_swap;
	//private OptionTypeDef.Type _exerciseType;