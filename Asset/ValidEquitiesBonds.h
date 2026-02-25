#pragma once 
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

class ValidEquitiesBonds{
	public:
		ValidEquitiesBonds(const std::string& ticker_mappings_path); 
		bool valid_symbol(std::string symbol) const; //check if symbol is in um_
		int get_cik(std::string symbol);
		std::string get_entity(std::string symbol); 
	private:
		std::unordered_map<std::string, std::tuple<int, std::string>> um_; 
}; 
