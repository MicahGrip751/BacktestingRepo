#pragma once
#include "Asset.h"
#include "ValidEquitiesBonds.h"

class Equity : public Asset{
	public:
		Equity(const std::string& symbol, const std::string& clean_fn, const std::string& iso, ValidEquitiesBonds& veb); 
		int get_cik() const;
		std::string get_entity() const; 
		
	private:
		int cik_ = -1; //default is an invalid CIK
		std::string entity_;
		void validate_(ValidEquitiesBonds& veb); 
}; 
