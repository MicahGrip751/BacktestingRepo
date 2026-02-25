#include "ValidForex.h"

ValidForex::ValidForex(const std::string& iso4217_data_path){
	std::ifstream file(iso4217_data_path); 
	if(!file.is_open()){
		throw std::runtime_error("File was not opened"); 
	}
	//first line has header info (no need to store it)
	std::string line; 
	std::getline(file, line);

	while(getline(file, line)){
		int pos = line.find(",");
		line.erase(0, pos + 1); //erase up to and including the first "," since the second column is the currency name
		pos = line.find(','); 
		std::string currency = line.substr(0, pos);
		line.erase(0, pos + 1);
		pos = line.find(','); //second column is the ISO code 
		std::string iso_code = line.substr(0, pos);  
		um_.insert({iso_code, currency}); 	
	}
}

bool ValidForex::valid_symbol(std::string symbol) const{
	if(um_.find(symbol) != um_.end()){
		return true; 
	}else{
		return false; 
	}
}

std::string ValidForex::get_cur(std::string symbol){
	return um_[symbol]; 
}
