#include "Asset.h"

Asset::Asset(const std::string& symbol, const std::string& fn, const std::string& iso) : cs{CandleSeries()}, symbol_{symbol}, iso_{iso} {cs.read_clean(fn);};   

std::string Asset::get_symbol() const{
	return symbol_; 	
}
std::string Asset::get_iso() const{
	return iso_; 
}
