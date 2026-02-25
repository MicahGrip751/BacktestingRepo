#include "Equity.h"

		
Equity::Equity(const std::string& symbol, const std::string& clean_fn, const std::string& iso, ValidEquitiesBonds& veb) : 
	Asset{symbol, clean_fn, iso} 
{
	validate_(veb); 
}; 


int Equity::get_cik() const{
	return cik_; 
}

std::string Equity::get_entity() const{
	return entity_; 
}

void Equity::validate_(ValidEquitiesBonds& veb){
	if(!veb.valid_symbol(this->get_symbol())){
		throw std::invalid_argument("Equity Constructor: Invalid symbol."); 
	}
	//if the symbol is valid, set the CIK and Entity names 
	cik_ = veb.get_cik(this->get_symbol());
	entity_ = veb.get_entity(this->get_symbol()); 
}
