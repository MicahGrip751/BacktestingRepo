#pragma once
#include "../CandleSeries/CandleSeries.h"
#include "../Utility/utility.h" 

class Asset{
	public:
		Asset(const std::string& symbol, const std::string& fn, const std::string& iso); 
		std::string get_symbol() const;
		std::string get_iso() const;
		CandleSeries cs; 
	protected:
		 //String to hold the asset symbol
		std::string symbol_;
		//iso code for what currency the asset is denominated in (for forex this is the base currency)
		std::string iso_; 
}; 
