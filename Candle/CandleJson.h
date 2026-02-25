#pragma once
#include <string>
//this struct is used to when reading json files with glaze since glaze can automatically create structs from json objects 
//and automatically create json objects from structs

struct CandleJson{
	std::string Datetime;
	double Open;
	double High;
	double Low;
	double Close;
	double Volume;
	double Bid;
	double Ask;  
}; 
