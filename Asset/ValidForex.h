#pragma once
#include <unordered_map>
#include <fstream>

class ValidForex{
	public:
		ValidForex(const std::string& iso4217_data_path); 
		bool valid_symbol(std::string symbol) const; //check if symbol is in um_
		std::string get_cur(std::string symbol); 
	private:
		std::unordered_map<std::string, std::string> um_; 
}; 

