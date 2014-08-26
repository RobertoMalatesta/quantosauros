#include "VolatilitySurface.h"

namespace quantoSauros {
	VolatilitySurface::VolatilitySurface(Date today, 
		std::vector<quantoSauros::VolatilityCurve> &volCurves, DayCounter dcf)
	{
		m_dcf = dcf;
		m_today = today;
		m_volCurves = volCurves;
	}
	Date VolatilitySurface::getDate()
	{
		return m_today;
	}
	//X : Maturity, Y: Tenor
	QuantLib::Volatility VolatilitySurface::getVol(Time maturity, Time tenor)
	{
		double val;
		int idx, idy, i, j;
		int Nx = m_volCurves[0].Length();
		int Ny = m_volCurves.size();

		for(i = 0; i < Nx; i++) {
			if(maturity < m_volCurves[0].getVol(i).getTime()) {				
				idx = i - 1;
				break;
			}
		}
		if(maturity < m_volCurves[0].getVol(0).getTime()) {idx = 0; maturity = m_volCurves[0].getVol(0).getTime();}
		if(maturity >= m_volCurves[0].getVol(Nx - 1).getTime()) {idx = Nx - 2; maturity = m_volCurves[0].getVol(Nx - 1).getTime();}

		for(j = 0; j < Ny; j++) {
			if(tenor < m_volCurves[j].getTenor().getTenorInTime()) {
				idy = j - 1;
				break;
			}
		}
		if(tenor < m_volCurves[0].getTenor().getTenorInTime()) {
			idy = 0; 
			tenor = m_volCurves[0].getTenor().getTenorInTime();
		}
		if(tenor >= m_volCurves[Ny - 1].getTenor().getTenorInTime()) {
			idy = Ny - 2; 
			tenor = m_volCurves[Ny - 1].getTenor().getTenorInTime();
		}
			
		double Dxiplus1 = m_volCurves[0].getVol(idx + 1).getTime();
		double Dxi = m_volCurves[0].getVol(idx).getTime();
		double Dyiplus1 = m_volCurves[idy + 1].getTenor().getTenorInTime();
		double Dyi = m_volCurves[idy].getTenor().getTenorInTime();
		double volii = m_volCurves[idx].getVol(idy).getVolatility();
		double voliplus1i = m_volCurves[idx + 1].getVol(idy).getVolatility();
		double voliiplus1 = m_volCurves[idx].getVol(idy + 1).getVolatility();
		double voliplus1iplus1 = m_volCurves[idx + 1].getVol(idy + 1).getVolatility();

		val = 1.0 * (maturity - Dxiplus1) * (tenor - Dyiplus1) /
			((Dxi - Dxiplus1) * (Dyi - Dyiplus1)) * volii;	
		val = val + 1.0 * (maturity - Dxi) * (tenor - Dyiplus1) /
			((Dxiplus1 - Dxi) * (Dyi - Dyiplus1)) * 
			voliplus1i;	
		val = val + 1.0 * (maturity - Dxiplus1) * (tenor - Dyi) / 
			((Dxi - Dxiplus1) * (Dyiplus1 - Dyi)) *
			voliiplus1;	
		val = val + 1.0 * (maturity - Dxi) * (tenor - Dyi) / 
			((Dxiplus1 - Dxi) * (Dyiplus1 - Dyi)) *
			voliplus1iplus1;
	 
		return val;
	}
	int VolatilitySurface::Length()
	{
		return m_volCurves.size();
	}
	quantoSauros::VolatilityCurve VolatilitySurface::getVolatilityCurve(int index)
	{
		return m_volCurves[index];
	}
	std::vector<quantoSauros::VolatilityCurve> VolatilitySurface::getVolatilitySurface()
	{
		return m_volCurves;
	}

	VolatilitySurface::~VolatilitySurface(void)
	{
	}
}