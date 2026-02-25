#pragma once 
#include <vector> 
#include "../Candle/CandleJson.h"
//struct to be populated when glaze reads in the file containing the cleaned candle series 
struct CleanCandleSeriesJson{
	int tf_; 
	double fidelity_;
	std::vector<CandleJson> candle_vec_; 
}; 
