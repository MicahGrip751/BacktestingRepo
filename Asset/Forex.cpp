#include "Forex.h"

//assumes symbols are ISO 4217 codes separated by a character
//Takes the base currency ISO as the Asset iso 
Forex::Forex(const std::string symbol, const std::string& fn, ValidForex& vfx) : 
	Asset{symbol, fn, symbol.substr(0, 3)}, base_iso_{symbol.substr(0, 3)}, quote_iso_{symbol.substr(4, 3)} 
{
	validate_(vfx); 
}; 

Forex::Forex(const std::string symbol, const std::string& usd_b_or_b_usd, const std::string usd_q_or_q_usd, const std::string fn_1, const std::string fn_2, const std::string fn_3, ValidForex& vfx) : 
	Asset{symbol, fn_1, symbol.substr(0, 3)},  base_iso_{symbol.substr(0, 3)}, quote_iso_{symbol.substr(4, 3)}, 
	usd_b_or_b_usd_{usd_b_or_b_usd}, usd_q_or_q_usd_{usd_q_or_q_usd}, usd_b_or_b_usd_cs_{CandleSeries()}, usd_q_or_q_usd_cs_{CandleSeries()} 
{
	//call read clean on the usd/base and usd/quote files 
	usd_b_or_b_usd_cs_.read_clean(fn_2); 
	usd_q_or_q_usd_cs_.read_clean(fn_3);
	//check if the timeframes are the same 
	if(usd_b_or_b_usd_cs_.tf() != usd_q_or_q_usd_cs_.tf()){
		if(usd_b_or_b_usd_cs_.tf() != this->cs.tf()){
			throw std::invalid_argument("Forex Constructor: The timeframes of the three CandleSeries must be the same."); 
		}
	}
	//Call Validate
	validate_(vfx);
	validate_(vfx, usd_b_or_b_usd, usd_q_or_q_usd);
	//sync the .cbegin() & .cend() iterator pairs for all 3 CandleSeries 
	unsynced_it_pairs = std::make_tuple(std::make_pair(cs.cs_it_b(), cs.cs_it_e()), 
									  std::make_pair(usd_b_or_b_usd_cs_.cs_it_b(), usd_b_or_b_usd_cs_.cs_it_e()), 
									  std::make_pair(usd_q_or_q_usd_cs_.cs_it_b(), usd_q_or_q_usd_cs_.cs_it_e()));
	synced_it_pairs = unsynced_it_pairs; 
	utility::sync_iterators(synced_it_pairs);

}; 	
std::string Forex::get_asset_c() const{
	return asset_c_; 
}
int Forex::get_n_trading_days() const{
	return n_trading_days_; 
}
int Forex::get_unit_multiplier() const{
	return unit_multiplier_; 
}
std::string Forex::get_base() const{
	return base_cur_; 
}
std::string Forex::get_quote() const{
	return quote_cur_; 
}

std::string Forex::get_base_iso() const{
	return base_iso_; 
}
std::string Forex::get_quote_iso() const{
	return quote_iso_; 
}

double Forex::usd_base(std::vector<Candle>::const_iterator it) const{
	if(base_iso_ == "USD"){
		//the exchange rate of USD/USD is 1
		return 1.0; 
	}else if(quote_iso_ == "USD"){
		//if usd is quote currency then exchange rate usd_base is 1 / (base / usd ex rate). We take the midpoint of bid and ask for simplicity
		return 2.0 / (it->b() + it->a());  
	}else{
		//if neither base or quote is usd we must use the usd/base and usd/quote candleseries
		//check if it corresponds to a datetime in usd_b_or_b_usd CandleSeries	
		if((it->dt() < usd_b_or_b_usd_cs_.cs_it_b()->dt()) || (it->dt() > usd_b_or_b_usd_cs_.cs_it_rb()->dt())){
			throw std::runtime_error("get_usd_base: The iterator it corresponds to a datetime not in usd_b_or_b_usd's CandleSeries"); 
		}
		//distance from the synced first iterator in symbol's cs to the input iterator
		auto n = std::distance(std::get<0>(synced_it_pairs).first, it);                    
		//get the usd_b_or_b_usd_cs_ iterator which corresponds to the same datetime as it.
		auto usd_base_it = std::next(std::get<1>(synced_it_pairs).first, n); 
		std::string base = usd_b_or_b_usd_.substr(0, 3);  
		std::string quote = usd_b_or_b_usd_.substr(4, 3);
		if(base == "USD"){
			//if the symbol is usd/base
			return (usd_base_it->b() + usd_base_it->a()) / 2; 
		}else if(quote == "USD"){
			//if the symbol is base/usd return 1 / ((b + a) /2)
			return 2.0 / (usd_base_it->b() + usd_base_it->a()); 
		}else{
			throw std::runtime_error("usd_base: The symbol usd_b_or_b_usd_ should either be USD/base or base/USD"); 
		}
	}
}

double Forex::quote_usd(std::vector<Candle>::const_iterator it) const{
	if(base_iso_ == "USD"){
		//if USD is base currency then quote/usd is 1 / (usd/quote)
		return 2.0 / (it->b() + it->a());  
	}else if(quote_iso_ == "USD"){
		//if USD is quote currency then quote/USD is USD/USD exchange rate
		return 1.0; 
	}else{
		//if neither base or quote is usd we must use the usd/base and usd/quote candleseries
		//check if it corresponds to a datetime in usd_q_or_q_usd CandleSeries	
		if((it->dt() < usd_q_or_q_usd_cs_.cs_it_b()->dt()) || (it->dt() > usd_q_or_q_usd_cs_.cs_it_rb()->dt())){
			throw std::runtime_error("quote_usd: The iterator it corresponds to a datetime not in usd_q_or_q_usd's CandleSeries"); 
		}
		//distance from the synced first iterator in symbol's cs to the input iterator
		auto n = std::distance(std::get<0>(synced_it_pairs).first, it);                    
		//get the usd_q_or_q_usd_cs_ iterator which corresponds to the same datetime as it.
		auto usd_quote_it = std::next(std::get<2>(synced_it_pairs).first, n); 
		std::string base = usd_b_or_b_usd_.substr(0, 3);  
		std::string quote = usd_b_or_b_usd_.substr(4, 3);
		if(base == "USD"){
			//if the symbol is usd/quote the the exchange rate for quote/usd is the reciprocal of the bid ask midpoint
			return 2.0 / (usd_quote_it->b() + usd_quote_it->a()); 
		}else if(quote == "USD"){
			//if the symbol is quote/usd return ((b + a) /2)
			return (usd_quote_it->b() + usd_quote_it->a()) / 2; 
		}else{
			throw std::runtime_error("quote_usd: The symbol usd_q_or_q_usd should be either USD/quote or quote/USD"); 
		}
	}
}

double Forex::position_size_units(double pos_size_in_usd, std::vector<Candle>::const_iterator it){
	//compute the position size in units of the base currency & then convert to lots;  
	return (pos_size_in_usd * usd_base(it)) / unit_multiplier_; 
}

void Forex::validate_(ValidForex& vfx){
	if(!vfx.valid_symbol(base_iso_) || !vfx.valid_symbol(quote_iso_)){
		throw std::invalid_argument("Forex Constructor: Invalid Symbol. One of the currencies is not a valid ISO 4217 code"); 
	}
	//if the ISO codes are valid set the currency names
	base_cur_ = vfx.get_cur(base_iso_);  
	quote_cur_ = vfx.get_cur(quote_iso_);  
}

void Forex::validate_(ValidForex& vfx, std::string symbol2, std::string symbol3){
	std::string s2_base = symbol2.substr(0, 3);  
	std::string s2_quote = symbol2.substr(4, 3);
	std::string s3_base = symbol3.substr(0, 3);  
	std::string s3_quote = symbol3.substr(4, 3);
	if(!vfx.valid_symbol(s2_base) || !vfx.valid_symbol(s2_quote)){
		throw std::invalid_argument("Forex Constructor: Invalid symbol2. One of the currencies is not a valid ISO 4217 code"); 
	}
	if(!vfx.valid_symbol(s3_base) || !vfx.valid_symbol(s3_quote)){
		throw std::invalid_argument("Forex Constructor: Invalid symbol3. One of the currencies is not a valid ISO 4217 code"); 
	}

}

