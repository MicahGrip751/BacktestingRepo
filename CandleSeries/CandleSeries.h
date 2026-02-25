#pragma once
#include "../Candle/Candle.h" 
#include "../Candle/CandlePtr.h"
#include "CleanCandleSeriesJson.h"
#include "../Timestamp/Timestamp.h"
#include <nlohmann/json.hpp>
#include <glaze/glaze.hpp>
#include <fstream>
#include <map>
#include <numeric> 
#include <algorithm>
#include <ranges> 
#include <execution> 

class CandleSeries{
	public:
		//default constructor is used when we wish to perform cleaning & reading separately
		CandleSeries() = default;
		//cleaning function (writes to a json file to be read later)
		void clean(std::string f_in, std::string f_out, std::string tf, std::string asset_c, std::string tmz_i);
		//file name is the cleaned candleseries 
		void make_clean_htf(std::string clean_ltf_fn, std::string clean_htf_fn, std::string htf, Datetime st); 
		//reading cleaning data function 
		void read_clean(std::string fn); 
		void read_clean2(std::string fn); 

		//accessors to the begin and end iterators for cs_
		std::vector<Candle>::const_iterator cs_it_b() const; 
		std::vector<Candle>::const_iterator cs_it_e() const; 
		//accessors to the begin and end iterators for htf
		std::vector<CandlePtr>::const_iterator htf_it_b() const;
		std::vector<CandlePtr>::const_iterator htf_it_e() const; 
		//accessors to the begin and end reverse iterators for cs_
		std::vector<Candle>::const_reverse_iterator cs_it_rb() const; 
		std::vector<Candle>::const_reverse_iterator cs_it_re() const; 
		//accessors to the begin and end reverse iterators for htf
		std::vector<CandlePtr>::const_reverse_iterator htf_it_rb() const;
		std::vector<CandlePtr>::const_reverse_iterator htf_it_re() const; 
		//accessors for the timeframes in mins 
		unsigned short int tf() const; 
		unsigned short int htf() const; 
		//extract a series of CandlePtrs from the base timeframe 
		void extract_c_ptrs(std::vector<CandlePtr>& c_ptr_v); 
		//accessor to extract a range view (pt = price type) (tf = timeframe to use) (st = start date is using a higher tf) 
		void extract_ts(std::vector<Timestamp<double>>& ts, const std::string& pt, std::string tf, Datetime dt = Datetime()); 
		//Compute the higher tf starting from st
		void comp_htf(std::string htf, Datetime st);
		void comp_htf(std::string htf, Datetime st, std::string x);
		//comp_htf helper takes in tf string and if tf is the base timeframe it returns true, if not it computes htf and returns false
		bool comp_htf_help(std::string tf, Datetime st); 
		//return the number of candles in the base timeframe 
		int cs_size() const; 
		//return the number of candles in the higher timeframe 
		int htf_cs_size() const; 
		//display the candle series
		void display_cs() const; 	
		//display the htf series
		void display_htf_cs() const; 
		//display head of cs_ and htf_cs_ (first n candles); 
		void cs_head(int n = 12) const; 
		void htf_cs_head(int n = 12) const;
		//display the fidelity of the data
		void display_fid() const;
		//Convert a timeframe string into a number of minutes
		unsigned short int tf_min(std::string tf_) const;
		//generate the trading hours 
		void gen_trading_hours(std::string asset_c, int tf_in_min, std::string tmz_o, 
			std::vector<std::pair<std::chrono::hh_mm_ss<std::chrono::seconds>, std::chrono::hh_mm_ss<std::chrono::seconds>>>& v); 

		//Generate valid datetimes 
		void gen_valid_dt(std::string asset_c, std::string tf, Datetime st, Datetime end, std::vector<Datetime>& valid_dt); 
	private:
		//store the timeframe and higher timeframe in minutes 
		unsigned short int tf_;
		unsigned short int htf_ = 0;
		//fidelity is the percentage of real data 
		double fidelity_ = 1;
		//candlestick series
		std::vector<Candle> cs_;
		//where to store the higher timeframe candle series
		std::vector<CandlePtr> htf_cs_; 
};
