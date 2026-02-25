#include "Bond.h"

Bond::Bond(const std::string& symbol, Datetime maturity, const std::string& clean_fn, const std::string& iso, ValidEquitiesBonds& veb) :
	Asset{symbol, clean_fn, iso}, maturity_{maturity} 
{
	validate_(veb); 
}; 


int Bond::get_cik() const{
	return cik_; 
}

int Bond::get_isin() const{
	if(isin_ == -1){
		std::cout << "isin_ has not been set yet" << std::endl; 
	}
	return isin_; 
}

std::string Bond::get_entity() const{
	return entity_;  
}

Datetime Bond::get_maturity() const{
	return maturity_; 
}

void Bond::validate_(ValidEquitiesBonds& veb){
	if(!veb.valid_symbol(this->get_symbol())){
		throw std::invalid_argument("Bond Constructor: Invalid symbol."); 
	}
	//if the symbol is valid, set the CIK and Entity names 
	cik_ = veb.get_cik(this->get_symbol());
	entity_ = veb.get_entity(this->get_symbol()); 
		
}
