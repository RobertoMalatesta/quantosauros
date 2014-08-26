/*
#pragma once
#include <ql\quantlib.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <list>
#include <iostream>
#include <algorithm>
using namespace QuantLib;
using namespace std;

class RateMarketData
{
public:
	RateMarketData(){
		m_depositLength = 0;
		m_swapLength = 0;
	}
	void ADD(RateData rD){
		if (rD.type == RateData::DepositRate){
			m_depositLength++;
		} else {
			m_swapLength++;
		}
		rateDataList.push_back(rD);
	}
	RateData* getSwapRates(){		
		list<RateData> tmpList;
		list<RateData>::iterator found = rateDataList.begin(), end = rateDataList.end();		
		while(true){
			found = find_if(found, rateDataList.end(), Predicate(RateData::SwapRate));
			if (found == end)
				break;
			tmpList.push_back(*found);
			found++;
		}		
		RateData* result = new RateData[tmpList.size()];
		list<RateData>::iterator begin = tmpList.begin();
		for (int i = 0; i < tmpList.size(); i++){
			result[i] = *begin;
			begin++;
		}			
		return result;
	}	
	RateData* getDepositRates(){
		list<RateData> tmpList;
		list<RateData>::iterator found = rateDataList.begin(), end = rateDataList.end();		
		while(true){
			found = find_if(found, rateDataList.end(), Predicate(RateData::DepositRate));
			if (found == end)
				break;
			tmpList.push_back(*found);
			found++;
		}		
		RateData* result = new RateData[tmpList.size()];
		list<RateData>::iterator begin = tmpList.begin();
		for (int i = 0; i < tmpList.size(); i++){
			result[i] = *begin;
			begin++;
		}			
		return result;
	}
	int LENGTH(){
		return m_swapLength + m_depositLength;
	}
	int LENGTH_SWAP(){
		return m_swapLength;
	}
	int LENGTH_DEPOSIT(){
		return m_depositLength;
	}
	
	~RateMarketData(){
		
	}

private:
	list<RateData> rateDataList;
	int m_swapLength;
	int m_depositLength;
	class Predicate
	{
		public:
			Predicate(RateData::RateType x) : x(x) {}
			bool operator() (const RateData &rD) const { return rD.type == x; }
		private:
			const RateData::RateType x;
		
	}; 	
};
*/
