#pragma once
#include "Asset.h"
#include "ValidForex.h"

class Forex : public Asset{
	public:
		//Note: Forex Constructors call Read Clean
		//this constructor is used for currency pairs in which USD is either the base or the quote
		Forex(const std::string symbol, const std::string& fn, ValidForex& vfx); 	
		
		//This constructor is used for cross pairs
		//symbol_1 is the forex cross pair, symbol_2 is either USD/Base or Base/USD and symbol_3 is either USD/Quote or Quote/USD
		Forex(const std::string symbol_1, const std::string& symbol_2, const std::string symbol_3, const std::string fn_1, const std::string fn_2, const std::string fn_3, ValidForex& vfx); 	
		
		//accessors	
		std::string get_asset_c() const;
		int get_n_trading_days() const; 
		int get_unit_multiplier() const; 
		std::string get_base() const; 
		std::string get_quote() const; 
		std::string get_base_iso() const;
		std::string get_quote_iso() const;

		//returns the (bid + ask) / 2 exchange rate for usd/base at cs_ iterator it
		double usd_base(std::vector<Candle>::const_iterator it) const; 	
		//returns the (bid + ask) / 2 exchange rate for quote/usd at cs_ iterator position it 
		double quote_usd(std::vector<Candle>::const_iterator it) const; 		
		//given an amount of of usd compute the position size in lots 
		double position_size_units(double pos_size_in_usd, std::vector<Candle>::const_iterator it); 

	private:
		const std::string asset_c_ = "FX";
		const int n_trading_days_ = 313; //closed on sundays 
		const int unit_multiplier_ = 100000; //100000 units of the base currency in a lot 
		std::string base_cur_ = ""; //Base currency (name of first currency in the symbol) 
		std::string quote_cur_ = ""; //Quote Currency (name of second currency in the symbol); 
		std::string base_iso_; //ISO code of the base currency  
		std::string quote_iso_; //ISO code of the quote currency.
		//symbols of the other exchange rates
		//either base/USD or USD/base
		std::string usd_b_or_b_usd_;
		//either quote/USD or USD/quote
		std::string usd_q_or_q_usd_;
		//candleseries for the other exchange rates (only set in the case of cross pairs)
		CandleSeries usd_b_or_b_usd_cs_; 
		CandleSeries usd_q_or_q_usd_cs_;
		//unsynced cbegin & cend iterator pairs for the symbol candle series, the usd_b_or_b_usd and the usd_q_or_q_usd candle series 
		std::tuple<std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>, 
				   std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>,	
				   std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>> unsynced_it_pairs;  
		//synced cbegin & cend iterators 
		std::tuple<std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>, 
				   std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>,	
				   std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>> synced_it_pairs;  
		void validate_(ValidForex& vfx); 
		void validate_(ValidForex& vfx, std::string symbol2, std::string symbol3); 
};	 
