#include "ValidEquitiesBonds.h"

ValidEquitiesBonds::ValidEquitiesBonds(const std::string& ticker_mappings_path){
	std::ifstream f(ticker_mappings_path); 
	if(!f.is_open()){
		throw std::runtime_error("File was not opened"); 
	}
	nlohmann::json data = nlohmann::json::parse(f);
	for(auto it = data.begin(); it != data.end(); it++){
		//std::cout << it.value() << std::endl; 
		std::string symbol = it.value()["ticker"]; 
		std::string entity = it.value()["title"]; 
		int cik = it.value()["cik_str"];
		std::tuple<int, std::string> tup{cik, entity}; 
		um_.insert({symbol, tup}); 
	}
}

bool ValidEquitiesBonds::valid_symbol(std::string symbol) const{
	if(um_.find(symbol) == um_.end()){
		return false; 
	}
	return true; 
}

int ValidEquitiesBonds::get_cik(std::string symbol){
	if(valid_symbol(symbol)){
		return std::get<0>(um_[symbol]); 
	}else{
		throw std::invalid_argument("get_cik: Enter a valid symbol."); 
	}
}

std::string ValidEquitiesBonds::get_entity(std::string symbol){
	return std::get<1>(um_[symbol]); 
}
