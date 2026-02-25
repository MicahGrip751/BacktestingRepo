#pragma once
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression.hpp>
#include <mlpack/methods/linear_regression/linear_regression_impl.hpp>
#include "../Utility/utility.h"
#include "../CandleSeries/CandleSeries.h" 
#include "../Timestamp/Timestamp.h"
#include <ranges>
#include <algorithm> 
#include <math.h>
#include <thread> 
#include <limits>
#include <deque>
#include <set> 

namespace tech_ind{
	/*
	Moving Averages
	*/

	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void sma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name = "SMA"); 	
	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, UnaryOp un_op, T init, const std::string& ts_name = "EMA"); 

	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, UnaryOp un_op, T init, const std::string& ts_name = "DEMA");
	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init, const std::string& ts_name = "TEMA");
	
	//this version of wma uses a fixed weight vector for all the windows ==> here weights are a hyperparameter which can be tuned
	template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp>
	void wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, UnaryOp un_op, T init, const std::string& ts_name = "WMA");
	//this version of wma creates a weight vector from the period k with values weighted by 1, 2, 3, ..., k.
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void wma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name = "WMA");
	
	//volume weighted moving average
	//un_op should take an object from the iterator range [first1, last1) & return the value to be weighted by volume
	//objects from the iterator range [first2, last2) need to have a .v() method which returns the volume
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp un_op, T init, const std::string& ts_name = "VWMA");

	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void wilders_ma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name = "WildersMA"); 
	
	//hull_ma setup 
	template <typename InputIt, typename T, typename UnaryOp> 
	void hull_ma_setup(InputIt first1, std::deque<T>& wma_diffs, T& wsum_k, T& wsum_k_2, T& wsum_sqrt_k, 
			std::vector<T>& weights_k, std::vector<T>& weights_k_2, std::vector<T>& weights_sqrt_k, int k, int k_2, int sqrt_k, T init, UnaryOp un_op);

	//single pass hull moving average 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void hull_ma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name = "HullMA"); 
	
	//single pass WMA of WMAs 
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void wma_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, UnaryOp un_op, T init, const std::string& ts_name);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void wma_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name);
	//single pass VWMA of VWMAs
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void vwma_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name);
	//assumes iterator range [first2, last2) elements have .v methods(); 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void vwma_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name); 

	/*
		Bollinger bands like indicators: Instead of returning the ma values & the band values themselves they return the place of the 
		the output of un_op relative to the bands. k1 is the period of the moving average, k2 is the number of periods used 
		in calculating the standard deviation. (The moving average value is used as the mean in the std calculation)
	*/

	//sma bands 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void sma_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init); 
	//ema bands (takes an extra parameter ts_name so that when called from wilders_bb the timestamp variable name is correct) 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T nstd, UnaryOp un_op, T init, const std::string& ts_name = "EMA Bollinger Bands"); 
	//dema bands  
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T nstd, UnaryOp un_op, T init); 
	//tema bands 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T nstd, UnaryOp un_op, T init); 
	//wma bands 
	template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp> 
	void wma_bb(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T nstd, UnaryOp un_op, T init);
	//wma bands overload which uses the wma version with weights constructed from the period. 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void wma_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init);
	//un_op takes elements from the range [first1, last1)
	//vwma bands (objects in [first2, last2) need to have a .v() method which returns the volume) 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void vwma_bb(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T nstd, UnaryOp un_op, T init); 
	//wilders moving average bands 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void wilders_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init); 
	//hull_ma bands 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void hull_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init); 

	/* 
		Bands Like Indicators
	*/
	//donchian channel
	//elements from the iterator range [first1, last1) must have .h and .l methods
	//ex is an example of type T so that the compiler can infer the type)
	//returns the position of the output of un_op relative to the upper and lower channels
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void donch_chan(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex); 

	//Moving Average Envelope Functions (returns the position of the output of un_op2 relative to the bands)
	//un_op1 is the unary operator used to compute the moving averages
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void sma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init);
	//EMA envelope function 
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void ema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//DEMA envelope function
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
	void dema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//TEMA envelope function 
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
	void tema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//WMA envelope (overload which takes w weights vector)
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename W, typename T>
	void wma_env(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//WMA envelope (overload which creates a weights vector from k)
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
	void wma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//VWMA envelope (overload which assumes that [first1, last1) contains elements with a .v method & un_op1 operates on this range)
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
	void vwma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//overload which assumes that the elements in the range [first2, last2) have .v methods & un_op1 and un_op2 operate on the range [first1, last2)
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
	void vwma_env(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 

	//Keltner Bands (k1 is the period of the moving average & k2 is the period of the ATR)
	//returns the output of un_op2 relative to the bands (which are computed with un_op1) 
	//version which uses an SMA
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//version which uses an EMA
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//version which uses a DEMA
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//version which uses a TEMA
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//version which uses a WMA	
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename W, typename T> 
	void k_bands_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	//version which uses a VWMA 
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void k_bands_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init); 
	
	/* 
		Moving Average Difference functions: Many of these are wrappers for functions in utility
	*/
	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void sma_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name = "SMA Difference");
	//Note: ema_diff is the same thind as the absolute price oscillator
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init, const std::string& ts_name = "EMA Difference"); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, UnaryOp un_op, T init, const std::string& ts_name = "DEMA Difference");
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, UnaryOp un_op, T init, const std::string& ts_name = "TEMA Difference"); 
	template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp> 
	void wma_diff(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights1, const std::vector<W>& weights2, UnaryOp un_op, T init, const std::string& ts_name = "WMA Difference"); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void wma_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name = "WMA Difference");
	//Note: un_op2 should take an element from the range [first2, last2) and return the volume
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void vwma_diff(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name = "VWMA Difference"); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void wilders_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name = "WildersMA Difference");
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void hull_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name = "HullMA Difference");
	
	/*
		Volatility Related Indicators	
	*/

	//moving standard deviation 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void mov_std(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 

	//rate of change 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void roc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 
	//rate of change which uses a the utility::percent_change version which takes an undefined value
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void roc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, T undef); 
	//average true range 
	//==> iterator range [first1, last1) must contain either Candles of CandlePtrs (or any type with .o(), .h(), .l(), and .c() methods)
	template <typename InputIt, typename OutputIt, typename T>
	void atr(InputIt first1, InputIt last1, OutputIt first2, int k,  T init);
	//average true range percent
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void atrp(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init);
	
	
	template <typename InputIt, typename OutputIt, typename T> 
	void donch_width(InputIt first1, InputIt last1, OutputIt first2, int k, T ex);
	//Chaikin Volatility
	//[first1, last1) must contain elements with .h and .l methods
	//k1 is the period of the moving average, k2 is the number of periods used in the roc
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_vol_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, T undef); 
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_vol_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T init, T undef); 
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_vol_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T init, T undef);  
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_vol_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T init, T undef);  
	template <typename InputIt, typename OutputIt, typename W, typename T> 
	void chaik_vol_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T init, T undef); 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T> 
	void chaik_vol_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init, T undef);  
	
	/* 
		Indices		 
	*/
	//Note: for cci version which do not take unary operators the typical price is used ==> elements from [first1, last1) must have .c, .l, .h methods
	//Commodity Channel Index Version using SMA
	//k1 is the number of periods used in the simple moving average of the typical prices & k2 is the number of periods 
	//used to compute the mean of |typical price - sma|; 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void cci_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, UnaryOp un_op, T init); 
	template <typename InputIt, typename OutputIt, typename T> 
	void cci_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, T init); 
	//CCI version using a k1 period ema with alpha alpha
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void cci_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T w, UnaryOp un_op, T init);
	template <typename InputIt, typename OutputIt, typename T> 
	void cci_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T w, T init);
	//CCI version which uses DEMA 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void cci_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T w, UnaryOp un_op, T init);
	template <typename InputIt, typename OutputIt, typename T> 
	void cci_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T w, T init);
	//CCI version which uses TEMA 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void cci_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T w, UnaryOp un_op, T init);
	template <typename InputIt, typename OutputIt, typename T> 
	void cci_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T w, T init);
	//CCI version which uses WMA
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void cci_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T w, UnaryOp un_op, T init);
	template <typename InputIt, typename OutputIt, typename W, typename T> 
	void cci_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T w, T init);
	//CCI version which uses VWMA (un_op works on [first1, last1) & elements from [first2, last2) are assumed to have .v methods) 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void cci_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T w, UnaryOp un_op, T init);
	//CCI version which uses a VWMA of typical price (assumes [first1, last1) range elements have .c, .h, .l and .v methods)
	template <typename InputIt, typename OutputIt, typename T> 
	void cci_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, T init);
	
	//Relative Vigor Index (version which uses Simple Moving Average)
	//[first1, last) elements must contain .c, .o, .h, .l methods
	//returns the difference between the RVI value and the signal line
	template <typename InputIt, typename OutputIt, typename T> 
	void rvigi_sma(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	//Relative Vigor Index (version which uses an EMA)
	template <typename InputIt, typename OutputIt, typename T>
	void rvigi_ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init); 
	//Relative Vigor Index (version which uses DEMAs)
	template <typename InputIt, typename OutputIt, typename T>
	void rvigi_dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init); 
	//Relative Vigor Index (version which uses TEMAs)
	template <typename InputIt, typename OutputIt, typename T>
	void rvigi_tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T init); 
	//Relative Vigor Index (version which uses WMAs)
	template <typename InputIt, typename OutputIt, typename W, typename T>
	void rvigi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init); 
	template <typename InputIt, typename OutputIt, typename T>
	void rvigi_wma(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	//Relative Vigor Index (version which uses VWMAs)
	template <typename InputIt, typename OutputIt, typename T> 
	void rvigi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 

	//Relative Strength Index (version which uses a Simple Moving Average)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void rsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 
	//RSI version which uses an EMA	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, UnaryOp un_op, T init); 
	//RSI version which uses a DEMA 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, UnaryOp un_op, T init);
	//RSI version which uses a TEMA 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init);
	//RSI version which uses a WMA & takes a weights vector  
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp>
	void rsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, UnaryOp un_op, T init); 
	//RSI version which uses a WMA & constructs a weights vector from k 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init);
	//RSI version which uses a volume weighted moving average & and assumes [first1, last1) range elements have a .v() method 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init);
	//RSI version which uses a vwma & assumes un_op operates on [first1, last1) and [first2, last2) elements have a .v() method	
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void rsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp un_op, T init); 

	//Relative Volatility index (version which uses rsi_sma)
	//k1 is the number of values used in the standard deviation calculations 
	//k2 is the period of the rsi
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init); 
	//Relative Volatility Index (version which uses rsi_ema)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha, UnaryOp un_op, T init); 
	//version which uses rsi_dema 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, UnaryOp un_op, T init); 
	//version which uses rsi_tema 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init); 
	//version which uses rsi_wma & takes a weights vector 
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void rvoli_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, const std::vector<W>& weights, UnaryOp un_op, T init);
	//this version constructs a weights vector from k2
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init); 
	//versions which use rsi_vwma
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init); 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void rvoli_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T ini); 

	//True Strength Index (Version which uses SMAs)
	//k1 is the period used for the sma of the differences & absolute differences
	//k2 is the period used for the sma of the smas
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 
	//TSI (Version which uses EMAs)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init, T undef);
	//TSI (version which uses DEMAs)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, 
			int k2, T alpha21, T alpha22, UnaryOp un_op, T init, T undef); 
	//TSI (version which uses TEMAs)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, 
			int k2, T alpha21, T alpha22, T alpha23, UnaryOp un_op, T init, T undef);
	//TSI (version which uses WMAs & takes two weight vectors)
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void tsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, UnaryOp un_op, T init, T undef); 	
	//TSI (version which uses WMAs & constructs weight vectors from k1, k2)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 	
	//TSI (version which uses VWMA's)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 
	//TSI (version which uses VWMA's and takes two iterator range)
	//un_op operates on [first1, last1) & [first2, last2) elements must have a .v method
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void tsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef); 

	//Directional Movement Index 
	//Elements of the range [first1, last1) must have .h, .l and .c methods
	template <typename InputIt, typename OutputIt, typename T> 
	void dmi(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	
	//Average Directional Index
	//k1 is the integer passed to dmi and k2 is the integer used to compute the wilders_ma of the dmi
	template <typename InputIt, typename OutputIt, typename T> 
	void adx(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init); 

	/* 
		Oscillators
	*/
	//momemtum oscillator (ex is an example of type T so that the compiler can infer the type) 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void mosc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex); 

	//Chande Momentum Oscillator 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void chande_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 

	//Chaiking Oscillator: Elements in [first1, last1) must have .c, .h, .l and .v methods
	//version which uses SMA difference
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init);
	//version which uses ema difference
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init);
	//version which uses dema difference 
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init);
	//version which uses tema difference 
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init);
	//version which uses wma difference 
	template <typename InputIt, typename OutputIt, typename W, typename T> 
	void chaik_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, T init);
	//version which uses wma difference	
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init);
	
	//assumes that range elements from [first1, last1) have .h and .l methods
	template <typename InputIt, typename OutputIt, typename T> 
	void aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, T ex); 
	//The following overload uses the same unary operator to construct the comparators used in the minmax
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex); 
	//This overload uses un_op1 to construct the min comparator & un_op2 to construct the max comparator 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2> 
	void aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex); 
	
	//Stochastic RSI version which uses rsi_sma 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 
	//Stochastic RSI version which uses rsi_ema
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, UnaryOp un_op, T init, T undef); 
	//Stochastic RSI version which calls rsi_dema 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, UnaryOp un_op, T init, T undef); 
	//Stochastic RSI version which calls rsi_tema
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, UnaryOp un_op, T init, T undef);
	//Stochastic RSI version which calls rsi_wma & takes a weights vector 
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void srsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, UnaryOp un_op, T init, T undef);
	//SRSI version which calls rsi_wma & constructs a weights vector from k1	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef);
	//SRSI versions which call rsi_vwma
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef);
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void srsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef);

	//Coppock Curve 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init);
	//Coppock Curve version which uses an EMA 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha, UnaryOp un_op, T init);
	//Coppock Curve version which uses a DEMA
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha1, T alpha2, UnaryOp un_op, T init);
	//Coppock Curve version which uses a TEMA
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init);
	//Coppock Curve (this version creates the weight vectors from k3) 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init); 
	//Coppock Curve (this version takes in a weight vector)
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void copp_curve_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& weights, UnaryOp un_op, T init); 
	//Coppock Curve version which uses a VWMA
	//we assume un_op operates on elements from the range [first1, last1) and these elements have a .v() method
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init);
	//we assume un_op operates on [first1, last1) and range elements from [first2, last2) have .v() methods
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void copp_curve_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, int k3, UnaryOp un_op, T init);

	//Moving Average Convergence Divergence  
	//k1 and k2 are the periods used in the ema difference 
	//k3 is the period used for the sma signal line	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void macd(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, int k3, UnaryOp un_op, T init); 
	//macd function which does not compute a signal line;  
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void macd(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init); 
	
	//Stochastic Oscillator (Just returns the oscillator value & not the signal)
	//just output of un_op minmax normed by min .l() and max .h() 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex); 
	//Stochastic Oscillator (SMA Signal Line) 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef);
	//Stochastic Oscillator (EMA Signal Line)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha, UnaryOp un_op, T init, T undef);
	//Stochastic Oscillator DEMA Signal Line 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void stoch_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, UnaryOp un_op, T init, T undef); 
	//Stochastic Oscillator TEMA Signal Line 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void stoch_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init, T undef); 
	//Stochastic Oscillator WMA Signal Line (takes in a weights vector)
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void stoch_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, const std::vector<W>& weights, UnaryOp un_op, T init, T undef); 
	//Stochastic Oscillator WMA Signal Line (constructs a weights vector from k2)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 
	//SOSC VWMA Signal Line (volumes taken from [first1, last1)) 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef); 
	//SOSC VWMA Signal Line (volumes taken from [first2, last2) & un_op works on [first1, last1)) 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void stoch_osc_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef); 
	
	//Klinger Oscillator (using sma_diff) 
	template <typename InputIt, typename OutputIt, typename T> 
	void klinger_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init);
	//Klinger Oscillator (using ema_diff)
	template <typename InputIt, typename OutputIt, typename T> 
	void klinger_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init);
	//Klinger Oscillator (using dema_diff)
	template <typename InputIt, typename OutputIt, typename T> 
	void klinger_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init); 
	//Klinger Oscillator (using tema_diff)
	template <typename InputIt, typename OutputIt, typename T> 
	void klinger_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init); 
	//Klinger Oscillator (using wma_diff)
	template <typename InputIt, typename OutputIt, typename T> 
	void klinger_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init); 
	template <typename InputIt, typename OutputIt, typename W, typename T> 
	void klinger_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, T init);

	//Ultimate Oscillator
	//elements from the range [first1, last1) must have .h(), .l() & .c() methods
	template <typename InputIt, typename OutputIt, typename T> 
	void ultimate_osc(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T init); 	
	
	//Detrended Price Oscillator
	//k1 is the period of the moving average 
	//k2 is the number of periods in the lag 
	//version which uses an SMA
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init); 
	//version which uses an EMA
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, UnaryOp un_op, T init); 
	//version which uses a DEMA
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, UnaryOp un_op, T init); 
	//version which uses a TEMA	
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, UnaryOp un_op, T init); 
	//version which uses a WMA & takes a weights vector 
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
	void dtp_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, UnaryOp un_op, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init);
	//version which uses a VWMA (assumes un_op works on [first1, last1) and these range elements have .v() methods)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init); 
	//version which uses a VWMA and assumes un_op works on [first1, last1) and range elements from [first2, last2) have .v() methods & the ranges start at the same datetime
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void dtp_osc_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init); 
	
	/* 
		Volume Related Indicators (Includes Money Flow Indicators)
	*/
	
	//Chaikin Money Flow
	//the iterator range [first1, last1) must point to elements with .o, .h, .l, .c, and .v methods
	template <typename InputIt, typename OutputIt, typename T> 
	void chaik_mf(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 

	//Accumulation Distribution line
	template <typename InputIt, typename OutputIt, typename T> 
	void acc_dist(InputIt first1, InputIt last1, OutputIt first2, T init); 

	//On Balance Volume
	//Assumes range [first1, last1) elements have a .v method and UnaryOp operates on elements from this range
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void on_bal_vol(InputIt first1, InputIt last1, OutputIt first2, UnaryOp un_op, T ex); 
	//Assumes range [first2, last2) elements have a .v method and UnaryOp operates on elements from the range [first1, last1)
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void on_bal_vol(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp un_op, T ex); 
	
	//Negative Volume Index 
	//Assumes range [first1, last1) elements have a .v method and UnaryOp operates on elements from this range 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void neg_vol_idx(InputIt first1, InputIt last1, OutputIt first2, UnaryOp un_op, T init); 
	//Assumes range [first2, last2) elements have a .v method and UnaryOp operates on elements from the range [first1, last1)
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
	void neg_vol_idx(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp un_op, T init); 
	
	//volume force indicator (used in the  klinger oscillator)
	//elements from range [first1, last1) must have .dt, .v, .l and .h methods
	template <typename InputIt, typename OutputIt, typename T>
	void vol_force(InputIt first1, InputIt last1, OutputIt first2, T ex); 

	/* 
		Pivots Related Functions and Indicators 
	*/

	//return the iterator to the the nearest up pivot to itc in the range [first1, last1) 
	//(previous up pivot if prev = true, next pivot if prev = false)
	//un_op must operate on elements of the range [first1, last1) and return values which are comparable 
	template <typename InputIt, typename UnaryOp> 
	InputIt upiv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev = true);
	//similar to upivit but for down pivots
	template <typename InputIt, typename UnaryOp> 
	InputIt dpiv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev = true);
	//returns iterator to the nearest pivot of either type
	template <typename InputIt, typename UnaryOp> 
	InputIt piv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev = true);
	//boolean for checking it the object pointed to by itc is a down pivot or up pivot  
	template <typename InputIt, typename UnaryOp> 
	bool is_up_pivot(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op); 
	template <typename InputIt, typename UnaryOp> 
	bool is_down_pivot(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op); 
	
	//Up and Down pivots indicators
	template <typename InputIt, typename OutputIt, typename UnaryOp> 
	void pivot_up(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op); 
	template <typename InputIt, typename OutputIt, typename UnaryOp> 
	void pivot_down(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op); 
	
	//True pivot up and true pivot down indicators 
	//a true pivot up is defined as a pivot up for which the next pivot down is higher than the pivot up & likewise for the true pivot down
	//un_op1 is used to determine the pivots and un_op2 is used to perform the price comparisons
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
	void true_pivot_up(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2); 	
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
	void true_pivot_down(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2); 	

	//distance to either the next or previous up pivot 
	template <typename InputIt, typename UnaryOp> 
	int bars_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev = true); 	
	//distance to either the next or previous down pivot 
	template <typename InputIt, typename UnaryOp> 
	int bars_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev = true); 	
	//distance to either the next or previous pivot of either type
	template <typename InputIt, typename UnaryOp> 
	int bars_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev = true); 	
	
	//percent return to either the next or previous up pivot
	//ex is an example of type T so that the compiler can infer the type
	template <typename InputIt, typename UnaryOp, typename T> 
	T perc_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true, bool scale = false); 	
	//percent return to either the next or previous down pivot 
	template <typename InputIt, typename UnaryOp, typename T> 
	T perc_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true, bool scale = false); 	
	//precent return to either the next or previous pivot of either type 
	template <typename InputIt, typename UnaryOp, typename T> 
	T perc_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true, bool scale = false); 
	//Log return to either the next or previous up pivot 
	template <typename InputIt, typename UnaryOp, typename T> 
	T log_ret_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true); 	
	//Log return to either the next or previous down pivot 
	template <typename InputIt, typename UnaryOp, typename T> 
	T log_ret_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true); 	
	//Log return to either the next or previous pivot of either type 
	template <typename InputIt, typename UnaryOp, typename T> 
	T log_ret_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev = true); 	
	
	//Note: Care is taken to assure that we don't use a pivot before we would actually know it exists
	//log return to the average since the previous pivot
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void log_ret_to_avg_since_prev_piv(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 
	//zscore since the last pivot 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void zscore_prev_piv(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 
	
	//Trendline Indicator 
	//k1 is the number of previous pivots to draw the trendline through 
	//k2 is the number of periods to use in computing the pivots & un_op1 is the unary used for computing the pivots 
	//up = true ==> trendline is drawn through the past k1 pivot ups (up = false ==> trendline is drawn through the past k1 pivot downs)
	//value returned is the log return from the trendline value to the output of un_op2 in the current period
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void trendline(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, bool up, T ex);

	//Trendline update function (if we know about a new pivot we update the deque of the iterators to the previoys k1 pivots & update the mlpack linear regression object)
	template <typename InputIt, typename UnaryOp, typename T> 
	void trendline_update(mlpack::LinearRegression<arma::mat>& lr, std::deque<InputIt>& dq_it_ppiv, 
			InputIt first, InputIt last, InputIt itc, int k1, int k2, UnaryOp un_op, bool up, T ex);
	
	//Trendline setup function 
	template <typename InputIt, typename UnaryOp> 
	void trendline_setup(InputIt first, InputIt last, std::deque<InputIt>& dq_it_ppiv, int k1, int k2, UnaryOp un_op, bool up);
	
	//function to make a linear model from dq_it_ppiv
	//un_op should be the the unary operator used to compute the pivots
	template <typename InputIt, typename UnaryOp> 
	void make_lr(mlpack::LinearRegression<arma::mat>& lr, const std::deque<InputIt>& dq_it_ppiv, UnaryOp un_op);

	//Trendline evaluate function (find the value of the trendline at the iterator position itc)
	template <typename InputIt, typename T> 
	T trendline_evaluate(const mlpack::LinearRegression<arma::mat>& lr, const std::deque<InputIt>& dq_it_ppiv, InputIt itc, T ex);


	//Trendline Channel Indicator
	//Draws a trendline throught the past k1 pivot ups (computed using un_op1) and through the past k1 pivot downs (computed using un_op1) 
	//and returns the output of un_op2 in the current period relative to upper and lower trendlines
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void trendline_channel(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex); 
	
	//This overload of trendline_channel writes the trendline channel value to the range staring first2, 
	//and writes the channel width to the range starting first3, and the slope difference to the range starting first4 
	template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
	void trendline_channel(InputIt first1, InputIt last1, OutputIt first2, OutputIt first3, OutputIt first4, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex); 

}
//Note: Make sure the UnaryOp works with the type contained in the range [first1, last1)
//==> could take a const Candle& and return type T when working with std::vector<Candle> iterators 
//==> could take a const CandlePtr& and return type T when working with std::vector<CandlePtr> iterators 
//==> coule take a const Timestamp& and return type T when working with std::vector<Timestamp<T>> iterators

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::sma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> sma_vals(std::distance(first1, last1) - k + 1);
	utility::roll_mean(first1, last1, sma_vals.begin(), k, un_op, init);
	utility::timestamp_zip(sma_vals.cbegin(), sma_vals.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> ema_vals(std::distance(first1, last1) - k);
	utility::ema(first1, last1, ema_vals.begin(), k, alpha, init, un_op);
	utility::timestamp_zip(ema_vals.cbegin(), ema_vals.cend(), std::next(first1, k), first2, ts_name, init); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> dema_vals(std::distance(first1, last1) - 2*k);
	utility::dema(first1, last1, dema_vals.begin(), k, alpha1, alpha2, init, un_op);
	utility::timestamp_zip(dema_vals.cbegin(), dema_vals.cend(), std::next(first1, 2*k), first2, ts_name, init); 

}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> tema_vals(std::distance(first1, last1) - 3*k);
	utility::tema(first1, last1, tema_vals.begin(), k, alpha1, alpha2, alpha3, init, un_op);
	utility::timestamp_zip(tema_vals.cbegin(), tema_vals.cend(), std::next(first1, 3*k), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp>
void tech_ind::wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, UnaryOp un_op, T init, const std::string& ts_name){
	int k = weights.size(); 
	std::vector<T> wma_vals(std::distance(first1, last1) - k + 1);
	utility::roll_wtd_mean(first1, last1, wma_vals.begin(), weights, init, un_op);
	utility::timestamp_zip(wma_vals.cbegin(), wma_vals.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::wma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> weights(k);
	//fill the weights vector
	for(int i = 0; i < k; i++){
		//the weight i + 1 is cast as type T
		weights[i] = i + 1; 
	}
	std::vector<T> wma_vals(std::distance(first1, last1) - k + 1);
	utility::roll_wtd_mean(first1, last1, wma_vals.begin(), weights, init, un_op);
	utility::timestamp_zip(wma_vals.cbegin(), wma_vals.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp un_op, T init, const std::string& ts_name){
	std::vector<T> vwma_vals(std::distance(first1, last1) - k + 1);
	utility::roll_wtd_mean(first1, last1, first2, last2, vwma_vals.begin(), k, init, un_op, [](const auto& x){return x.v();}); 
	utility::timestamp_zip(vwma_vals.cbegin(), vwma_vals.cend(), std::next(first1, k - 1), first3, ts_name, init); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::wilders_ma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name){
	//wilders moving average is just an ema with period 2*k - 1 and alpha 1 / k
	T alpha = 1.0 / k;  
	k = 2*k - 1; 
	std::vector<T> wma_vals(std::distance(first1, last1) - k);
	utility::ema(first1, last1, wma_vals.begin(), k, alpha, init, un_op);
	utility::timestamp_zip(wma_vals.cbegin(), wma_vals.cend(), std::next(first1, k), first2, ts_name, init); 
}
template <typename InputIt, typename T, typename UnaryOp> 
void tech_ind::hull_ma_setup(InputIt first1, std::deque<T>& wma_diffs, T& wsum_k, T& wsum_k_2, T& wsum_sqrt_k, 
		std::vector<T>& weights_k, std::vector<T>& weights_k_2, std::vector<T>& weights_sqrt_k, int k, int k_2, int sqrt_k, T init, UnaryOp un_op){
	for(int i = 0; i < k; i++){
		T w = i + 1; 
		if(i < k_2){
			weights_k_2[i] = w;
			wsum_k_2 += w; 
		}
		if(i < sqrt_k){
			weights_sqrt_k[i] = w;
			wsum_sqrt_k += w; 
		}
		weights_k[i] = w;
		wsum_k += w; 
	}
	//use a deque to hold the past sqrt(k) values 2*wma(floor(k/2)) - wma(k); 
	for(int i = 0; i < sqrt_k; i++){
		T wm_k = utility::wtd_mean(std::next(first1, i), std::next(first1, k + i), weights_k, un_op, wsum_k, init); 
		T wm_k_2 = utility::wtd_mean(std::next(first1, k - k_2 + i), std::next(first1, k + i), weights_k_2, un_op, wsum_k_2, init); 
		wma_diffs.push_back((2 * wm_k_2) - wm_k); 
	}
}
//single pass hull moving average 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::hull_ma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, const std::string& ts_name){
	//compute the other periods 
	int k_2 = k / 2; 
	int sqrt_k = std::floor(std::sqrt(k));
	//create the weight vectors for each period and compute the weight sums 
	T wsum_k = 0;
	T wsum_k_2 = 0;
	T wsum_sqrt_k = 0; 
	std::vector<T> weights_k(k); 
	std::vector<T> weights_k_2(k_2); 
	std::vector<T> weights_sqrt_k(sqrt_k);
	std::deque<T> wma_diffs;
	hull_ma_setup(first1, wma_diffs, wsum_k, wsum_k_2, wsum_sqrt_k, weights_k, weights_k_2, weights_sqrt_k, k, k_2, sqrt_k, init, un_op);
	//compute the first hull moving average value
	T hma = utility::wtd_mean(wma_diffs.begin(), wma_diffs.end(), weights_sqrt_k, wsum_sqrt_k, init);
	*first2 = Timestamp<T>{std::next(first1, k + sqrt_k - 2)->dt(), std::move(hma), ts_name};
	first2++;
	//compute the rest of the hull moving average values 
	for(auto it = std::next(first1, k + sqrt_k - 1); it != last1; it++){
		T wm_k = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights_k, un_op, wsum_k, init); 
		T wm_k_2 = utility::wtd_mean(std::prev(it, k_2 - 1), std::next(it), weights_k_2, un_op, wsum_k_2, init);
		//pop the last value from the deque & push a new wma difference onto the back 
		wma_diffs.pop_front(); 
		wma_diffs.push_back((2 * wm_k_2) - wm_k);
		hma = utility::wtd_mean(wma_diffs.begin(), wma_diffs.end(), weights_sqrt_k, wsum_sqrt_k, init); 
		*first2 = Timestamp<T>{it->dt(), std::move(hma), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::wma_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, UnaryOp un_op, T init, const std::string& ts_name){
	//get the sizes of the weights vectors
	int k1 = w1.size(); 
	int k2 = w2.size();
	//weight sums 
	T wsum1 = std::accumulate(w1.cbegin(), w1.cend(), init, [](T val, const W& w){return val + w;});
	T wsum2 = std::accumulate(w2.cbegin(), w2.cend(), init, [](T val, const W& w){return val + w;});
	//deque to store the most recent k2 k1 period wmas 
	std::deque<T> wma_k1_vals;
	//variable to store the most recent k1 period wma 
	T wma_k1; 
	for(auto it = std::next(first1, k1 - 1); it != last1; it++){
		//compute the k1 period weighted mean 
		wma_k1 = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), w1, un_op, wsum1, init); 
		if(wma_k1_vals.size() < k2){
			wma_k1_vals.push_back(wma_k1); 
			if(wma_k1_vals.size() == k2){
				*first2 = Timestamp<T>{it->dt(), utility::wtd_mean(wma_k1_vals.cbegin(), wma_k1_vals.cend(), w2, wsum2, init), ts_name};
				first2++; 
			}
		}else{
			//pop the oldes wma_k1 value & push the newest onto the deque 
			wma_k1_vals.push_back(wma_k1); 
			wma_k1_vals.pop_front(); 
			*first2 = Timestamp<T>{it->dt(), utility::wtd_mean(wma_k1_vals.cbegin(), wma_k1_vals.cend(), w2, wsum2, init), ts_name};
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::wma_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//construct the weights vectors from k1 and k2 & call the overload which takes two weights vectors 
	std::vector<T> w1(k1); 
	std::vector<T> w2(k2);
	for(int i = 0; i < std::max(k1, k2); i++){
		if(i < k1){
			w1[i] = i + 1; 
		}
		if(i < k2){
			w2[i] = i + 1; 
		}
	}
	tech_ind::wma_wma(first1, last1, first2, w1, w2, un_op, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::vwma_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//call the overload which takes two iterator ranges 
	tech_ind::vwma_vwma(first1, last1, first1, last1, first2, k1, k2, un_op, init, ts_name); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::vwma_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//deque to store the most recent k2 k1 period vwmas 
	std::deque<T> vwma_k1_vals;
	//variable to store the most recent k1 period vwma 
	T vwma_k1;
	//iterator to the element in the second iterator range corresponding to it
	auto it2 = first2;
	//iterator to the element in the second iterator range corresponding to the front element of vwma_k1_vals
	auto it22 = std::next(first2, k1 - 1); 
	for(auto it = std::next(first1, k1 - 1); it != last1; it++){
		//compute the k1 period weighted mean 
		vwma_k1 = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), it2, un_op, [](const auto& c){return c.v();}, init); 
		if(vwma_k1_vals.size() < k2){
			vwma_k1_vals.push_back(vwma_k1); 
			if(vwma_k1_vals.size() == k2){
				*first3 = Timestamp<T>{it->dt(), 
					utility::wtd_mean(vwma_k1_vals.cbegin(), vwma_k1_vals.cend(), it22, [](const auto& x){return x;}, [](const auto& c){return c.v();}, init), 
					ts_name};
				first3++;
				it22++; 
			}
		}else{
			//pop the oldes wma_k1 value & push the newest onto the deque 
			vwma_k1_vals.push_back(vwma_k1); 
			vwma_k1_vals.pop_front(); 
			*first3 = Timestamp<T>{it->dt(), 
				utility::wtd_mean(vwma_k1_vals.cbegin(), vwma_k1_vals.cend(), it22, [](const auto& x){return x;}, [](const auto& c){return c.v();}, init), 
				ts_name};
			first3++;
			it22++; 
		}
		it2++; 
	}
}

/*
	Bollinger Bands Indicators 
*/
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::sma_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "SMA Bollinger Bands";
	//find the starting iterator for computing the sma & iterator pointing to the first period for which we have sma_bb values 
	int k = std::max(k1, k2) - 1; 
	auto itf = std::next(first1, k); 
	//variables for the sum, sum of squares, mean and std
	T s, ss, m, std_val;
	//mean discard iterator and discard iterator for updating s and ss 
	auto itd_m = std::prev(itf, k1 - 1);
	auto itd_ss = std::prev(itf, k2 - 1);       
	for(auto it = itf; it != last1; it++){
		if(it == itf){
			//compute s, ss, m, from scratch on the first iteration
			m = utility::mean(itd_m, std::next(itf), init, un_op); 
			s = std::accumulate(itd_ss, std::next(itf), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(itd_ss, std::next(itf), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});      
			std_val = utility::std(s, ss, m, k2);  
		}else{
			//update the mean 
			utility::roll_mean_update(m, un_op(*itd_m), un_op(*it), k1); 	
			//update s, ss, & get the std_val 
			std_val = utility::roll_std_s_ss_update(s, ss, m, un_op(*itd_ss), un_op(*it), k2);
			itd_m++; 
			itd_ss++; 
		}
		//create & write the timestamp 
		utility::bb_timestamp(it, first2, m, std_val, nstd, ts_name, un_op);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::ema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T nstd, UnaryOp un_op, T init, const std::string& ts_name){
	auto ema_it = first1; 
	auto itp = std::next(first1, std::max(k1 + 1, k2) - 1);
	//call ema_setup
	T ema; 
	utility::ema_setup(ema, std::prev(itp, k1), k1, init, un_op);
	T s, ss, std; 	
	//iterator to discard value used in the sum of squares update and iterator to the mean update
	auto ss_itd = std::prev(itp, k2 - 1);
	//compute the rest of the ema_bb values 
	for(auto it = itp; it != last1; it++){
		//update the ema; 
		utility::ema_update(ema, un_op(*it), alpha); 
		if(it == itp){
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});
			std = utility::std(s, ss, ema, k2);
		}else{
			//update the sum and sum of squares & return the std 
			std = utility::roll_std_s_ss_update(s,ss, ema, un_op(*ss_itd), un_op(*it), k2);
			ss_itd++; 
		}
		//compute & write the sma_bb value
		utility::bb_timestamp(it, first2, ema, std, nstd, ts_name, un_op); 
		//increment the write iterator
		first2++; 
	}
}
//dema bands 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "DEMA Bollinger Bands"; 
	auto itp = std::next(first1, std::max(2*k1 + 1, k2) - 1);
	//variables for ema, ema_ema, s, ss, dema, and std
	T ema_ema, ema, s, ss, dema, std;   
	//call dema_setup
	utility::dema_setup(ema_ema, ema, std::prev(itp, 2*k1), k1, alpha1, init, un_op);
	//iterator to discard value used in the sum of squares update and iterator to the mean update
	auto ss_itd = std::prev(itp, k2 - 1);
	//compute the dema_bb values & write the timestamps
	for(auto it = itp; it != last1; it++){
		//update the dema; 
		dema = utility::dema_update(ema_ema, ema, un_op(*it), alpha1, alpha2); 
		if(it == itp){
			//on the first iteration compute s, ss, and std from scratch
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});
			std = utility::std(s, ss, dema, k2);
		}else{
			//update the sum and sum of squares & return the standard deviation
			std = utility::roll_std_s_ss_update(s,ss, dema, un_op(*ss_itd), un_op(*it), k2);
			ss_itd++; 
		}
		//compute & write the dema_bb timestamp
		utility::bb_timestamp(it, first2, dema, std, nstd, ts_name, un_op); 
		//increment the write iterator 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tema_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "TEMA Bollinger Bands"; 
	auto itp = std::next(first1, std::max(3*k1 + 1, k2) - 1);
	//variables for ema, ema_ema, ema_ema_ema, s, ss, tema, and std
	T ema_ema_ema, ema_ema, ema, s, ss, tema, std;   
	//call tema_setup
	utility::tema_setup(ema_ema_ema, ema_ema, ema, std::prev(itp, 3*k1), k1, alpha1, alpha2, init, un_op);
	//iterator to discard value used in the sum of squares update and iterator to the mean update
	auto ss_itd = std::prev(itp, k2 - 1);
	//compute the dema_bb values & write the timestamps
	for(auto it = itp; it != last1; it++){
		//update the tema 
		tema = utility::tema_update(ema_ema_ema, ema_ema, ema, un_op(*it), alpha1, alpha2, alpha3); 
		if(it == itp){
			//on the first iteration compute s, ss, and std from scratch
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});
			std = utility::std(s, ss, tema, k2);
		}else{
			//update the sum and sum of squares & return the standard deviation
			std = utility::roll_std_s_ss_update(s,ss, tema, un_op(*ss_itd), un_op(*it), k2);
			ss_itd++; 
		}
		//compute & write the dema_bb timestamp
		utility::bb_timestamp(it, first2, tema, std, nstd, ts_name, un_op); 
		//increment the write iterator 
		first2++; 
	}
}
//wma bands 
template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp> 
void tech_ind::wma_bb(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "WMA Bollinger Bands";
	int k1 = weights.size();
	//leading iterator
	auto itl = std::next(first1, std::max(k1, k2) - 1);
	//compute the weight sum 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	//variables for s, ss, wm, and std
	T s, ss, wm, std;
	//discard iterator
	auto ss_itd = std::prev(itl, k2 - 1);
	for(auto it = itl; it != last1; it++){
		//compute the weighted mean
		wm = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), weights, un_op, wsum, init);
		//compute the sum of squares & the first std or update them  
		if(it == itl){
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});           
			std = utility::std(s, ss, wm, k2); 
		}else{
			std = utility::roll_std_s_ss_update(s, ss, wm, un_op(*ss_itd), un_op(*it), k2);
			if(std::isnan(std)){
				std::cout << "nan std value" << std::endl; 
			}
			ss_itd++; 
		}
		//compute & write the first wma_bb timestamp 
		utility::bb_timestamp(it, first2, wm, std, nstd, ts_name, un_op);
		//increment the iterators 
		first2++; 
	}
}
//wma bands overload which uses a wma with weights constructed from k1
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::wma_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init){
	//create the weights vector
	std::vector<T> weights(k1); 
	for(int i = 0; i < k1; i++){
		weights[i] = i + 1; 	
	}
	//call the version of wma_bb which takes a weights vector 
	wma_bb(first1, last1, first2, weights, k2, nstd, un_op, init); 
}

//vwma bands
//Note: std calculation is performed on the non-volume weighted output of un_op 
//add an overload later which uses volume weighted values to compute the std
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::vwma_bb(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "VWMA Bollinger Bands";
	//leading iterator in the first iterator range
	auto itl1 = std::next(first1, std::max(k1, k2) - 1);
	//iterator to the first position in the second iterator range windows 
	auto itl2 = std::next(first2, std::max(k1, k2) - k1); 
	//variables for s, ss, vwm, and std
	T s, ss, vwm, std;
	//discard iterator
	auto ss_itd = std::prev(itl1, k2 - 1);
	for(auto it = itl1; it != last1; it++){
		//compute the volume weighted mean
		vwm = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), itl2, un_op, [](const auto& c){return c.v();}, init);
		//compute the sum of squares & the first std or update them  
		if(it == itl1){
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});           
			std = utility::std(s, ss, vwm, k2); 
		}else{
			std = utility::roll_std_s_ss_update(s, ss, vwm, un_op(*ss_itd), un_op(*it), k2); 	
			ss_itd++; 
		}
		//compute & write the vwma_bb timestamp 
		utility::bb_timestamp(it, first3, vwm, std, nstd, ts_name, un_op);
		//increment the iterators 
		first3++;
		itl2++; 
	}
}

//wilders moving average bollinger bands 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::wilders_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "Wilder's Moving Average Bollinger Bands"; 
	//just call ema_bb with a period of 2*k1 - 1 and an alpha of 2 / (2*k1 - 1) + 1
	k1 = 2*k1 - 1; 
	T alpha = 2.0 / (k1 + 1);
	ema_bb(first1, last1, first2, k1, alpha, k2, nstd, un_op, init, ts_name); 
}
//hull_ma bands 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::hull_bb(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T nstd, UnaryOp un_op, T init){
	std::string ts_name = "Hull Moving Average Bollinger Bands";
	//compute the other periods for the hull moving average 
	int k1_2 = k1 / 2; 
	int sqrt_k1 = std::floor(std::sqrt(k1));
	//compute the leading iterator 
	auto itl = std::next(first1, std::max(k2 - 1, k1 + sqrt_k1 - 1));
	//call hull_ma_setup 
	T wsum_k1 = 0;
	T wsum_k1_2 = 0;
	T wsum_sqrt_k1 = 0; 
	std::vector<T> weights_k1(k1); 
	std::vector<T> weights_k1_2(k1_2); 
	std::vector<T> weights_sqrt_k1(sqrt_k1);
	std::deque<T> wma_diffs_k1;
	hull_ma_setup(std::prev(itl, k1 + sqrt_k1 - 1), wma_diffs_k1, wsum_k1, wsum_k1_2, wsum_sqrt_k1, weights_k1, weights_k1_2, 
			weights_sqrt_k1, k1, k1_2, sqrt_k1, init, un_op);
	//variables for k1 & k1/2 period wmas 
	T wm_k1, wm_k1_2; 
	T hma = utility::wtd_mean(wma_diffs_k1.cbegin(), wma_diffs_k1.cend(), weights_sqrt_k1, wsum_sqrt_k1, init);  
	//variables for the sum, sum of squares and standard deviation 
	T s, ss, std; 
	//iterator to discard value used in the sum of squares update and iterator to the mean update
	auto ss_itd = std::prev(itl, k2 - 1);
	//compute the dema_bb values & write the timestamps
	for(auto it = itl; it != last1; it++){
		if(it == itl){
			//on the first iteration compute s, ss, and std from scratch
			s = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + un_op(x);});  
			ss = std::accumulate(std::prev(it, k2 - 1), std::next(it), init, [&un_op](T val, const auto& x){return val + std::pow(un_op(x), 2);});
			std = utility::std(s, ss, hma, k2);
			//note on the first iteration we dont compute a new hma value
		}else{
			//get the hma value 
			wm_k1 = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), weights_k1, un_op, wsum_k1, init); 
			wm_k1_2 = utility::wtd_mean(std::prev(it, k1_2 - 1), std::next(it), weights_k1_2, un_op, wsum_k1_2, init);
			//pop the last value from the deque & push a new wma difference onto the back 
			wma_diffs_k1.pop_front(); 
			wma_diffs_k1.push_back((2 * wm_k1_2) - wm_k1);
			hma = utility::wtd_mean(wma_diffs_k1.cbegin(), wma_diffs_k1.cend(), weights_sqrt_k1, wsum_sqrt_k1, init);  
			//update the sum and sum of squares & return the standard deviation
			std = utility::roll_std_s_ss_update(s,ss, hma, un_op(*ss_itd), un_op(*it), k2);
			ss_itd++; 
		}
		//compute & write the hull_bb timestamp
		utility::bb_timestamp(it, first2, hma, std, nstd, ts_name, un_op); 
		//increment the write iterator 
		first2++; 
	}
}

/* 
	Moving Average Difference functions
*/ 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::sma_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	int k = std::max(k1, k2); 
	std::vector<T> sma_diffs(std::distance(first1, last1) - k + 1); 
	//call utility::roll_diff_mean 
	utility::roll_diff_mean(first1, last1, sma_diffs.begin(), k1, k2, init, un_op);
	//call timestamp_zip
	utility::timestamp_zip(sma_diffs.cbegin(), sma_diffs.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
//ema_diff takes a ts_name parameter so that when it is called by wilders_diff the timestamp variable name is correct
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init, const std::string& ts_name){
	int k = std::max(k1 + 1, k2 + 1); 
	std::vector<T> ema_diffs(std::distance(first1, last1) - k + 1); 
	//call utility::roll_diff_mean 
	utility::ema_diff(first1, last1, ema_diffs.begin(), k1, k2, alpha1, alpha2, init, un_op);
	//call timestamp_zip
	utility::timestamp_zip(ema_diffs.cbegin(), ema_diffs.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, UnaryOp un_op, T init, const std::string& ts_name){
	int k = std::max(2*k1 + 1, 2*k2 + 1);
	std::vector<T> dema_diffs(std::distance(first1, last1) - k + 1);
	//compute the dema difference
	utility::dema_diff(first1, last1, dema_diffs.begin(), k1, k2, alpha11, alpha12, alpha21, alpha22, init, un_op); 
	//fill the timestamp vector by calling timestamp_zip 
	utility::timestamp_zip(dema_diffs.cbegin(), dema_diffs.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, UnaryOp un_op, T init, const std::string& ts_name){
	int k = std::max(3*k1 + 1, 3*k2 + 1);
	std::vector<T> tema_diffs(std::distance(first1, last1) - k + 1);
	//compute the tema differences 
	utility::tema_diff(first1, last1, tema_diffs.begin(), k1, k2, alpha11, alpha12, alpha13, alpha21, alpha22, alpha23, init, un_op);
	//fill the timestamp vector by calling timestamp_zip 
	utility::timestamp_zip(tema_diffs.cbegin(), tema_diffs.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}

template <typename InputIt, typename OutputIt, typename T, typename W, typename UnaryOp> 
void tech_ind::wma_diff(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights1, const std::vector<W>& weights2, UnaryOp un_op, T init, const std::string& ts_name){
	int k1 = weights1.size(); 
	int k2 = weights2.size(); 
	int k = std::max(k1, k2);
	std::vector<T> wma_diffs(std::distance(first1, last1) - k + 1);
	//compute the difference between the weighted moving averages
	if(k1 == k){
		utility::roll_diff_wtd_mean(first1, last1, wma_diffs.begin(), std::min(k1, k2), k, weights2, weights1, init, un_op); 
	}else{
		utility::roll_diff_wtd_mean(first1, last1, wma_diffs.begin(), std::min(k1, k2), k, weights1, weights2, init, un_op); 
	}
	//fill the timestamps vector 
	utility::timestamp_zip(wma_diffs.cbegin(), wma_diffs.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::wma_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//create the weights vectors from k1 and k2
	if(k1 > k2){
		std::swap(k1, k2); 
	}
	std::vector<T> w1(k1); 
	std::vector<T> w2(k2);
	for(int i = 0; i < std::max(k1, k2); i++){
		if(i < k1){
			w1[i] = i + 1; 
		}
		if(i < k2){
			w2[i] = i + 1; 
		}
	}
	//call the version on wma_diff which takes two weight vectors 
	tech_ind::wma_diff(first1, last1, first2, w1, w2, un_op, init, ts_name); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::vwma_diff(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	int k = std::max(k1, k2);
	std::vector<T> vwma_diffs(std::distance(first1, last1) - k + 1);
	//compute the differences between the volume weighted moving averages
	utility::roll_diff_wtd_mean(first1, last1, first2, last2, vwma_diffs.begin(), k1, k2, init, un_op, [](const auto& x){return x.v();});
	//fill the timestamps vector 
	utility::timestamp_zip(vwma_diffs.cbegin(), vwma_diffs.cend(), std::next(first1, k - 1), first3, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::wilders_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//compute the periods
	k1 = 2*k1 - 1; 
	T alpha1 = 2 / (k1 + 1);
	k2 = 2*k2 - 1; 
	T alpha2 = 2 / (k2 + 1);
	//call ema_diff  
	tech_ind::ema_diff(first1, last1, first2, k1, alpha1, k2, alpha2, un_op, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::hull_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, const std::string& ts_name){
	//ensure that k1 is less than k2 and if not, swap the k's 
	if(k1 > k2){
		std::swap(k1, k2); 
	}
	int k1_2 = k1 / 2;
	int k2_2 = k2 / 2; 
	int sqrt_k1 = std::floor(std::sqrt(k1)); 
	int sqrt_k2 = std::floor(std::sqrt(k2)); 
	//call hull_ma_setup for each period 
	T wsum_k1 = 0;
	T wsum_k1_2 = 0;
	T wsum_sqrt_k1 = 0; 
	std::vector<T> weights_k1(k1); 
	std::vector<T> weights_k1_2(k1_2); 
	std::vector<T> weights_sqrt_k1(sqrt_k1);
	std::deque<T> wma_diffs_k1;
	hull_ma_setup(std::next(first1, k2 - k1), wma_diffs_k1, wsum_k1, wsum_k1_2, wsum_sqrt_k1, weights_k1, weights_k1_2, 
			weights_sqrt_k1, k1, k1_2, sqrt_k1, init, un_op);
	T wsum_k2 = 0;
	T wsum_k2_2 = 0;
	T wsum_sqrt_k2 = 0; 
	std::vector<T> weights_k2(k2); 
	std::vector<T> weights_k2_2(k2_2); 
	std::vector<T> weights_sqrt_k2(sqrt_k2);
	std::deque<T> wma_diffs_k2; 
	hull_ma_setup(first1, wma_diffs_k2, wsum_k2, wsum_k2_2, wsum_sqrt_k2, weights_k2, weights_k2_2, 
			weights_sqrt_k2, k2, k2_2, sqrt_k2, init, un_op);
	//leading iterator 
	auto it = std::next(first1, k2 + sqrt_k2 - 2); 
	//compute and write the first hull_ma difference 
	T hma_k1 = utility::wtd_mean(wma_diffs_k1.begin(), wma_diffs_k1.end(), weights_sqrt_k1, wsum_sqrt_k1, init);
	T hma_k2 = utility::wtd_mean(wma_diffs_k2.begin(), wma_diffs_k2.end(), weights_sqrt_k2, wsum_sqrt_k2, init);
	*first2 = Timestamp<T>{it->dt(), std::move(hma_k2 - hma_k1), ts_name};
	first2++;
	it++; 
	//compute the hull moving average difference values 
	for(it; it != last1; it++){
		//compute the k1, k1/2, k2, and k2/2 period wtd_means
		T wm_k1 = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), weights_k1, un_op, wsum_k1, init); 
		T wm_k1_2 = utility::wtd_mean(std::prev(it, k1_2 - 1), std::next(it), weights_k1_2, un_op, wsum_k1_2, init);
		T wm_k2 = utility::wtd_mean(std::prev(it, k2 - 1), std::next(it), weights_k2, un_op, wsum_k2, init);
		T wm_k2_2 = utility::wtd_mean(std::prev(it, k2_2 - 1), std::next(it), weights_k2_2, un_op, wsum_k2_2, init);       
		//pop the last value from the deque & push a new wma difference onto the back 
		wma_diffs_k1.pop_front(); 
		wma_diffs_k1.push_back((2 * wm_k1_2) - wm_k1);
		wma_diffs_k2.pop_front(); 
		wma_diffs_k2.push_back((2 * wm_k2_2) - wm_k2);
		//compute the hull moving average values for the current period
		hma_k1 = utility::wtd_mean(wma_diffs_k1.begin(), wma_diffs_k1.end(), weights_sqrt_k1, wsum_sqrt_k1, init);
		hma_k2 = utility::wtd_mean(wma_diffs_k2.begin(), wma_diffs_k2.end(), weights_sqrt_k2, wsum_sqrt_k2, init);
		//create & write the hma difference timestamp
		*first2 = Timestamp<T>{it->dt(), std::move(hma_k2 - hma_k1), ts_name};
		first2++; 
	}
}


template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::mov_std(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Moving Standard Deviation";
	std::vector<T> mstd_vals(std::distance(first1, last1) - k + 1);
	utility::roll_std(first1, last1, mstd_vals.begin(), k, init, un_op);
	utility::timestamp_zip(mstd_vals.cbegin(), mstd_vals.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::roc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Rate of Change";
	std::vector<T> roc_vals(std::distance(first1, last1) - k + 1);
	utility::roll_simple_returns(first1, last1, roc_vals.begin(), k, un_op, init);
	utility::timestamp_zip(roc_vals.cbegin(), roc_vals.cend(), std::next(first1, k - 1), first2, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::roc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Rate of Change";  
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = Timestamp<T>{it->dt(), utility::percent_change(un_op(*it), un_op(*std::prev(it, k - 1)), undef, false), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::mosc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex){
	std::string ts_name = "Momentum Oscillator";
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		//100 * current val / val k periods ago
		T mosc = 100 * un_op(*it) / un_op(*std::prev(it, k - 1));
		//create & write the timestamp
		*first2 = Timestamp<T>{it->dt(), std::move(mosc), ts_name};
		//increment the write iterator
		first2++; 
	}
}

//[first1, last1) must contain objects with .o, .h, .l, .c methods
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::atr(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	std::string ts_name = "Average True Range";
	std::vector<T> atr_vals(std::distance(first1, last1) - k);
	//true range unary function (works with either candle's or CandlePtrs) 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	}; 
	utility::roll_mean(std::next(first1), last1, atr_vals.begin(), k, tr_op, init);
	utility::timestamp_zip(atr_vals.cbegin(), atr_vals.cend(), std::next(first1, k), first2, ts_name, init); 
}
//single pass algorithm for the average true range percent indicator 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::atrp(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Average True Range Percent";
	std::vector<T> atrp_vals(std::distance(first1, last1) - k);
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	}; 
	//compute the first k period average true range
	T atr_val = utility::mean(std::next(first1), std::next(first1, k + 1), init, tr_op);
	//get the iterator to the current object & the discard object 
	auto itp = std::next(first1, k);
	auto itd = std::next(first1); 
	//create & write the first timestamp for the average true range percent
	*first2 = Timestamp<T>{std::next(first1, k)->dt(), 100 / un_op(*itp) * atr_val, ts_name};
	first2++; 
	//increment the price iterator
	itp++; 
	//compute the rest of the average true range percents
	for(itp; itp != last1; itp++){
		utility::roll_mean_update(atr_val, tr_op(*itd), tr_op(*itp), k);
		//create & write the average true range percent timestamp
		*first2 = Timestamp<T>{itp->dt(), 100 / un_op(*itp) * atr_val, ts_name};
		first2++;
		itd++; 
	}
}
//elements from the range [first1, last1) need to have .o, .h, .l, .c, and .v methods
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::acc_dist(InputIt first1, InputIt last1, OutputIt first2, T init){
	std::string ts_name = "Accumulation Distribution Line"; 
	auto ad = [](const auto& c){
		if(c.h() == c.l()){
			return 0.0;  
		}
		return c.v()*(c.c() - c.l() - c.h() + c.c()) / (c.h() - c.l()); 
	};
	//create & write the first timestamp 
	*first2 = Timestamp<T>{first1->dt(), ad(*first1), ts_name};
	first2++;
	//create & write the rest of the adl timestamps
	for(auto it = std::next(first1); it != last1; it++){
		*first2 = Timestamp<T>{it->dt(), ad(*it) + std::prev(first2)->val(), ts_name};
		first2++;
	}
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::donch_chan(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex){
	std::string ts_name = "Donchian Channel"; 
	//comparators to be used in minmax
	auto h_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 
	};
	auto l_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	};
	//unary to return the high
	auto h_access = [](const auto& c){
		return c.h(); 
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	//create the deques 
	std::deque<int> dq_max_h;
	std::deque<int> dq_min_l;
	std::vector<std::pair<T, T>> minmax(1); 
	for(int i = 0; i < std::distance(first1, last1); i++){
		//iterator to the current element 
		auto it = std::next(first1, i); 
		utility::roll_minmax_update(first1, minmax.begin(), dq_min_l, dq_max_h, i, k, l_cmp, h_cmp, l_access, h_access);
		if(i >= k - 1){
			//compute & write the value of the donchian channel indicator (value - min) / (max - min));
			T dc_val;  
			if(std::get<0>(minmax[0]) != std::get<1>(minmax[0])){
				dc_val = (un_op(*std::next(first1, i)) - std::get<0>(minmax[0])) / (std::get<1>(minmax[0]) - std::get<0>(minmax[0])); 	
			}else{
				//undefined values are defined to be .5 as this corresponds to the middle of the channel 
				dc_val = 0.5;  
			}
			*first2 = Timestamp<T>{it->dt(), std::move(dc_val), ts_name};
			first2++; 
		}
	}
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::donch_width(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	std::string ts_name = "Donchian Width";
	auto h_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 
	};
	auto l_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	};
	//unary to return the high
	auto h_access = [](const auto& c){
		return c.h(); 
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	//create the deques 
	std::deque<int> dq_max_h;
	std::deque<int> dq_min_l;
	//call roll_minmax_update k - 1 times so that the next call produces a minmax tuple
	std::vector<std::pair<T, T>> minmax(1); 
	for(int i = 0; i < std::distance(first1, last1); i++){
		//iterator to the current element 
		auto it = std::next(first1, i);
		utility::roll_minmax_update(first1, minmax.begin(), dq_min_l, dq_max_h, i, k, l_cmp, h_cmp, l_access, h_access);
		if(i >= k - 1){
			//compute & write the value of the donchian channel indicator (min + (value - min) / (max - min));
			*first2 = Timestamp<T>{it->dt(), std::get<1>(minmax[0]) - std::get<0>(minmax[0]), ts_name};
			first2++; 
		}
	}
}
//single pass algorithm
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::sma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "SMA Envelope";
	//current and discard iterators 
	auto itc = std::next(first1, k);
	auto itd = first1; 
	//compute the first mean 
	T m = utility::mean(itd, itc, init, un_op1);
	//compute & write the first timestamp 
	*first2 = Timestamp<T>{std::prev(itc)->dt(), (un_op2(*std::prev(itc)) - m*(1 - p)) / (2*p*m), ts_name};
	first2++;
	//compute the rest of the moving average envelope values 
	for(itc; itc != last1; itc++){
		//update the mean 
		utility::roll_mean_update(m, un_op1(*itd), un_op1(*itc), k); 
		*first2 = Timestamp<T>{itc->dt(), (un_op2(*itc) - m*(1 - p)) / (2*p*m), ts_name};
		first2++;
		itd++; 
	}
}

template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::ema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "EMA Envelope";
	//call ema setup 
	T ema; 
	utility::ema_setup(ema, first1, k, init, un_op1);
	//create & write the ema envelope timestamps 
	for(auto it = std::next(first1, k); it != last1; it++){
		//call ema update 
		utility::ema_update(ema, un_op1(*it), alpha); 
		*first2 = Timestamp<T>{it->dt(), (un_op2(*it) - ema*(1 - p)) / (2*p*ema), ts_name};
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
void tech_ind::dema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "DEMA Envelope";
	//call dema setup 
	T ema_ema, ema;
	T dema; 
	utility::dema_setup(ema_ema, ema, first1, k, alpha1, init, un_op1); 
	//create & write the dema envelope timestamps 
	for(auto it = std::next(first1, 2*k); it != last1; it++){
		//call dema update 
		dema = utility::dema_update(ema_ema, ema, un_op1(*it), alpha1, alpha2); 
		*first2 = Timestamp<T>{it->dt(), (un_op2(*it) - dema*(1 - p)) / (2*p*dema), ts_name};
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
void tech_ind::tema_env(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "TEMA Envelope";
	//call tema_setup 
	T eeema, eema, ema, tema;
	utility::tema_setup(eeema, eema, ema, first1, k, alpha1, alpha2, init, un_op1);
	//create & write the tema envelope timestamps 
	for(auto it = std::next(first1, 3*k); it != last1; it++){
		//call tema update 
		tema = utility::tema_update(eeema, eema, ema, un_op1(*it), alpha1, alpha2, alpha3);
		*first2 = Timestamp<T>{it->dt(), (un_op2(*it) - tema*(1 - p)) / (2*p*tema), ts_name};
		first2++;
	}
}
//WMA envelope (overload which takes w weights vector)
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename W, typename T>
void tech_ind::wma_env(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "WMA Envelope";
	//number of weights 
	int k = weights.size(); 
	//compute the weight sum 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, W wt){return val + wt;});
	T wm; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		//create & write the timestamp
		wm = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights, un_op1, wsum, init);  
		*first2 = Timestamp<T>{it->dt(), (un_op2(*it) - wm*(1 - p)) / (2*p*wm), ts_name};
		first2++;
	}
}
//WMA envelope (overload which creates a weights vector from k)
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
void tech_ind::wma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	//create the weights vector
	std::vector<T> weights(k);
	for(int i = 0; i < k; i++){
		weights[i] = i + 1; 
	}
	//call the version of wma_env which takes a weights vector 
	tech_ind::wma_env(first1, last1, first2, weights, p, un_op1, un_op2, init); 
}
//VWMA envelope (overload which assumes that [first1, last1) contains elements with a .v method & un_op1 operates on this range)
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
void tech_ind::vwma_env(InputIt first1, InputIt last1, OutputIt first2, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	//call the overload which takes two iterator ranges 
	tech_ind::vwma_env(first1, last1, first1, last1, first2, k, p, un_op1, un_op2, init); 
}
//overload which assumes that the elements in the range [first2, last2) have .v methods & un_op1 and un_op2 operate on the range [first1, last2)
template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T>
void tech_ind::vwma_env(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T p, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "VWMA Envelope";
	T vwma_val;
	//iterator for the position in the second iterator range which corresponds to std::prev(it, k - 1)
	auto it2 = first2; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		//create & write the timestamp
		vwma_val = utility::wtd_mean(std::prev(it, k - 1), std::next(it), it2, un_op1, [](const auto& c){return c.v();}, init);
		*first3 = Timestamp<T>{it->dt(), (un_op2(*it) - vwma_val*(1 - p)) / (2*p*vwma_val), ts_name};
		first3++;
		it2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (SMA)";
	//leading iterator
	auto it = std::next(first1, std::max(k1, k2 + 1) - 1);
	//discard iterators 
	auto itd_k1 = std::prev(it, k1 - 1); 
	auto itd_k2 = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	//compute the first sma & atr values
	T atr = utility::mean(std::prev(it, k2 - 1), std::next(it), init, tr_op); 
	T m = utility::mean(std::prev(it, k1 - 1), std::next(it), init, un_op1);
	//create & write the first bands timestamp 
	utility::bb_timestamp(it, first2, m, atr, mult, ts_name, un_op2);
	first2++;
	it++; 
	//compute & write the rest of the timestamps by calling roll_mean_update 
	for(it; it != last1; it++){
		//update the atr & the sma 
		utility::roll_mean_update(m, un_op1(*itd_k1), un_op1(*it), k1); 
		utility::roll_mean_update(atr, tr_op(*itd_k2), tr_op(*it), k2);
		utility::bb_timestamp(it, first2, m, atr, mult, ts_name, un_op2);
		//increment the write & discard iterators
		first2++;
		itd_k1++;
		itd_k2++; 
	}
}

template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (EMA)";
	auto it = std::next(first1, std::max(k1 + 1, k2 + 1) - 1);
	//discard iterator for updating the ATR 
	auto itd = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	//call ema setup 
	T ema, atr;
	utility::ema_setup(ema, std::prev(it, k1), k1, init, un_op1);
	for(auto itt = it; itt != last1; itt++){
		//call ema update
		utility::ema_update(ema, un_op1(*itt), alpha); 
		if(itt == it){
			//if this is the first iteration compute the atr from scratch
			atr = utility::mean(std::prev(itt, k2 - 1), std::next(itt), init, tr_op);
		}else{
			//if it is not the first iteration call roll_mean_update 
			utility::roll_mean_update(atr, tr_op(*itd), tr_op(*itt), k2); 
			itd++; 
		}
		//make & write the timestamp
		utility::bb_timestamp(itt, first2, ema, atr, mult, ts_name, un_op2);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (DEMA)";
	auto it = std::next(first1, std::max(2*k1 + 1, k2 + 1) - 1);
	//discard iterator for updating the ATR 
	auto itd = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	//call dema setup 
	T eema, ema, dema, atr;
	utility::dema_setup(eema, ema, std::prev(it, 2*k1), k1, alpha1, init, un_op1);
	for(auto itt = it; itt != last1; itt++){
		//call dema update
		dema = utility::dema_update(eema, ema, un_op1(*itt), alpha1, alpha2); 
		if(itt == it){
			//if this is the first iteration compute the atr from scratch
			atr = utility::mean(std::prev(itt, k2 - 1), std::next(itt), init, tr_op); 
		}else{
			//if it is not the first iteration call roll_mean_update 
			utility::roll_mean_update(atr, tr_op(*itd), tr_op(*itt), k2); 
			itd++; 
		}
		//make & write the timestamp
		utility::bb_timestamp(itt, first2, dema, atr, mult, ts_name, un_op2);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (TEMA)"; 
	auto it = std::next(first1, std::max(3*k1 + 1, k2 + 1) - 1);
	//discard iterator for updating the ATR 
	auto itd = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	//call tema setup 
	T eeema, eema, ema, tema, atr;
	utility::tema_setup(eeema, eema, ema, std::prev(it, 3*k1), k1, alpha1, alpha2, init, un_op1);
	for(auto itt = it; itt != last1; itt++){
		//call tema update
		tema = utility::tema_update(eeema, eema, ema, un_op1(*itt), alpha1, alpha2, alpha3); 
		if(itt == it){
			//if this is the first iteration compute the atr from scratch
			atr = utility::mean(std::prev(itt, k2 - 1), std::next(itt), init, tr_op); 
		}else{
			//if it is not the first iteration call roll_mean_update 
			utility::roll_mean_update(atr, tr_op(*itd), tr_op(*itt), k2); 
			itd++; 
		}
		//make & write the timestamp
		utility::bb_timestamp(itt, first2, tema, atr, mult, ts_name, un_op2);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename W, typename T> 
void tech_ind::k_bands_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (WMA)";
	//get the size of the weights vector & sum the weights 
	int k1 = weights.size();
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	auto it = std::next(first1, std::max(k1, k2 + 1) - 1);
	//discard iterator for updating the ATR 
	auto itd = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	T wma, atr; 
	for(auto itt = it; itt != last1; itt++){
		//compute the weighted mean
		wma = utility::wtd_mean(std::prev(itt, k1 - 1), std::next(itt), weights, un_op1, wsum, init); 
		if(itt == it){
			//if this is the first iteration compute the atr from scratch
			atr = utility::mean(std::prev(itt, k2 - 1), std::next(itt), init, tr_op); 
		}else{
			//if it is not the first iteration call roll_mean_update 
			utility::roll_mean_update(atr, tr_op(*itd), tr_op(*itt), k2); 
			itd++; 
		}
		//make & write the timestamp
		utility::bb_timestamp(itt, first2, wma, atr, mult, ts_name, un_op2);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (WMA)"; 
	//construct the weights vector from k1 & call the overload of k_bands_wma which takes a weights vector 
	std::vector<T> weights(k1);
	for(int i = 0; i < k1; i++){
		weights[i] = i + 1; 
	}
	tech_ind::k_bands_wma(first1, last1, first2, weights, k2, mult, un_op1, un_op2, init); 
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::k_bands_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T mult, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	std::string ts_name = "Keltner Bands (VWMA)"; 
	//get the size of the weights vector & sum the weights 
	auto it = std::next(first1, std::max(k1, k2 + 1) - 1);
	//discard iterator for updating the ATR 
	auto itd = std::prev(it, k2 - 1); 
	auto tr_op = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c));
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	T vwma, atr; 
	for(auto itt = it; itt != last1; itt++){
		//compute the weighted mean
		vwma = utility::wtd_mean(std::prev(itt, k1 - 1), std::next(itt), std::prev(itt, k1 - 1), un_op1, [](const auto& c){return c.v();}, init); 
		if(itt == it){
			//if this is the first iteration compute the atr from scratch
			atr = utility::mean(std::prev(itt, k2 - 1), std::next(itt), init, tr_op); 
		}else{
			//if it is not the first iteration call roll_mean_update 
			utility::roll_mean_update(atr, tr_op(*itd), tr_op(*itt), k2); 
			itd++; 
		}
		//make & write the timestamp
		utility::bb_timestamp(itt, first2, vwma, atr, mult, ts_name, un_op2);
		first2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_mf(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	std::string ts_name = "Chaikin Money Flow";
	//Chaikin Money Flow is just a volume weighted moving average with the following unary 
	auto un_op = [](const auto& c){
		if(c.h() != c.l()){
			return (c.c() - c.l() - c.h() + c.c()) / (c.h() - c.l());  
		}else{
			return 0.0; 
		}
	};
	//call vwma with a non default ts_name parameter 
	vwma(first1, last1, first1, last1, first2, k, un_op, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_vol_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (SMA)"; 
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	};
	//deque to keep track of the past k2 k1 period sma values
	std::deque<T> dq_sma;
	//most recent sma value 
	T sma_val;
	//discard iterator 
	auto itd = first1; 
	for(auto it = std::next(first1, k1 - 1); it != last1; it++){
		if(it == std::next(first1, k1 - 1)){
			//compute the mean from scratch on the first iteration
			sma_val = utility::mean(std::prev(it, k1 - 1), std::next(it), init, hml);
			dq_sma.push_back(sma_val); 
		}else{
			//update the mean 
			utility::roll_mean_update(sma_val, hml(*itd), hml(*it), k1);
			itd++;
			dq_sma.push_back(sma_val);
			if(dq_sma.size() == k2){
				*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_sma.back(), dq_sma.front(), undef, false), ts_name};
				first2++; 
				dq_sma.pop_front(); 
			}
			/*
			if(dq_sma.size() < k2){
				dq_sma.push_back(sma_val);
				if(dq_sma.size() == k2){
					*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_sma.back(), dq_sma.front(), false), ts_name};
					first2++; 
				}
			}else{
				dq_sma.push_back(sma_val);
				//create & write the timestamp 
				*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_sma.back(), dq_sma.front(), false), ts_name};
				first2++; 
				dq_sma.pop_front(); 
			}
			*/
		}
	}
}
//single pass version of Chaikin Volatility which uses an ema
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_vol_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (EMA)";
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	}; 
	//call ema_setup with the h - l unary
	T ema; 
	utility::ema_setup(ema, first1, k1, init, hml);
	//deque of the past k2 ema values 
	std::deque<T> dq_ema;
	//fill the deque with the first k2 ema values
	int i = 0; 
	for(auto it = std::next(first1, k1); it != last1; it++){
		utility::ema_update(ema, hml(*it), alpha);
		dq_ema.push_back(ema);
		if(dq_ema.size() == k2){
			//create & write the Chaikin Volatility Timestamp (use the unscaled percentage change)
			*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_ema.back(), dq_ema.front(), undef, false), ts_name};
			first2++;
			//pop the oldest ema from the deque 
			dq_ema.pop_front(); 
		}
	} 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_vol_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (DEMA)";
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	}; 
	//call dema_setup with the h - l unary
	T eema, ema, dema; 
	utility::dema_setup(eema, ema, first1, k1, alpha1, init, hml);
	//deque of the past k2 dema values 
	std::deque<T> dq_dema;
	//fill the deque with the first k2 dema values
	for(auto it = std::next(first1, 2*k1); it != last1; it++){
		//call dema_update
		dema = utility::dema_update(eema, ema, hml(*it), alpha1, alpha2);
		dq_dema.push_back(dema); 
		if(dq_dema.size() == k2){
			//create & write the Chaikin Volatility Timestamp (use the unscaled percentage change)
			*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_dema.back(), dq_dema.front(), undef, false), ts_name};
			first2++;
			//pop the oldest dema from the deque  
			dq_dema.pop_front(); 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_vol_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (TEMA)";
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	}; 
	//call tema_setup with the h - l unary
	T eeema, eema, ema, tema; 
	utility::tema_setup(eeema, eema, ema, first1, k1, alpha1, alpha2, init, hml);
	//deque of the past k2 tema values 
	std::deque<T> dq_tema;
	for(auto it = std::next(first1, 3*k1); it != last1; it++){
		//call tema_update
		tema = utility::tema_update(eeema,eema, ema, hml(*it), alpha1, alpha2, alpha3); 
		dq_tema.push_back(tema);
		if(dq_tema.size() == k2){	
			//create & write the Chaikin Volatility Timestamp (use the unscaled percentage change)
			*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_tema.back(), dq_tema.front(), undef, false), ts_name};
			first2++;
			dq_tema.pop_front(); 
		}
	} 
}
template <typename InputIt, typename OutputIt, typename W, typename T> 
void tech_ind::chaik_vol_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (WMA)";
	int k1 = weights.size(); 
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	};
	//deque to keep track of the past k2 k1 period wma values
	std::deque<T> dq_wma;
	//most recent wma value 
	T wma_val;
	//sum the weights
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	for(auto it = std::next(first1, k1 - 1); it != last1; it++){
		//compute the weighted mean 
		wma_val = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), weights, hml, wsum, init);
		dq_wma.push_back(wma_val); 
		if(dq_wma.size() == k2){
			//create & write the timestamp 
			*first2 = Timestamp<T>{it->dt(), utility::percent_change(dq_wma.back(), dq_wma.front(), undef, false), ts_name};
			first2++; 
			dq_wma.pop_front(); 
		}
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T> 
void tech_ind::chaik_vol_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init, T undef){
	std::string ts_name = "Chaikin Volatility (VWMA)";
	auto hml = [](const auto& c){
		return c.h() - c.l(); 
	};
	//deque to keep track of the past k2 k1 period wma values
	std::deque<T> dq_vwma;
	//most recent vwma value 
	T vwma_val;
	//iterator to the correct position in [first2, last2)
	auto it2 = first2; 
	for(auto it = std::next(first1, k1 - 1); it != last1; it++){
		//compute the volume weighted mean of the high minus low
		vwma_val = utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), it2, hml, [](const auto& c){return c.v();}, init);
		dq_vwma.push_back(vwma_val); 
		if(dq_vwma.size() == k2){
			//create & write the timestamp 
			*first3 = Timestamp<T>{it->dt(), utility::percent_change(dq_vwma.back(), dq_vwma.front(), undef, false), ts_name};
			first3++; 
			//remove the oldest volume weighted mean from the deque
			dq_vwma.pop_front();
		}
		//increment the second range iterator
		it2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::cci_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (SMA)";
	int k = std::max(k1, k2);
	//iterator to the current object 
	auto it = std::next(first1, k - 1); 
	//discard iterator for the mean
	auto itd = std::prev(it, k1 - 1); 
	T m = utility::mean(std::prev(it, k1 - 1), std::next(it), init, un_op);
	//create & write the cci_timestamp 
	utility::cci_timestamp(it, first2, m, k2, w, un_op, init, ts_name); 
	first2++;
	it++; 
	//compute the rest of the CCI timestamps
	for(it; it != last1; it++){
		//update the mean
		utility::roll_mean_update(m, un_op(*itd), un_op(*it), k1);
		//create & write the cci timestamp
		utility::cci_timestamp(it, first2, m, k2, w, un_op, init, ts_name); 
		first2++; 
		itd++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::cci_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the overload which takes a unary op
	tech_ind::cci_sma(first1, last1, first2, k1, k2, w, tp, init); 	
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::cci_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (EMA)";
	int k = std::max(k1, k2 - 1);
	auto it = std::next(first1, k);
	//call ema_setup 
	T ema, denom; 
	utility::ema_setup(ema, std::prev(it, k1), k1, init, un_op);
	for(auto itc = it; itc != last1; itc++){
		//call ema update 
		utility::ema_update(ema, un_op(*itc), alpha);
		//create & write the cci timestamp
		utility::cci_timestamp(itc, first2, ema, k2, w, un_op, init, ts_name); 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::cci_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the overload which takes a unary op
	tech_ind::cci_ema(first1, last1, first2, k1, alpha, k2, w, tp, init); 	
}

//CCI version which uses DEMA 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::cci_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (DEMA)";
	int k = std::max(2*k1, k2 - 1);
	auto it = std::next(first1, k);
	//call dema_setup 
	T eema, ema, dema, denom; 
	utility::dema_setup(eema, ema, std::prev(it, 2*k1), k1, alpha1, init, un_op);
	for(auto itc = it; itc != last1; itc++){
		//call dema update 
		dema = utility::dema_update(eema, ema, un_op(*itc), alpha1, alpha2);
		//create & write the cci timestamp
		utility::cci_timestamp(itc, first2, dema, k2, w, un_op, init, ts_name); 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::cci_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the overload which takes a unary op
	tech_ind::cci_dema(first1, last1, first2, k1, alpha1, alpha2, k2, w, tp, init); 	
}
//CCI version which uses TEMA 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::cci_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (TEMA)";
	int k = std::max(3*k1, k2 - 1);
	auto it = std::next(first1, k);
	//call tema_setup 
	T eeema, eema, ema, tema, denom; 
	utility::tema_setup(eeema, eema, ema, std::prev(it, 3*k1), k1, alpha1, alpha2, init, un_op);
	for(auto itc = it; itc != last1; itc++){
		//call tema update 
		tema = utility::tema_update(eeema,eema, ema, un_op(*itc), alpha1, alpha2, alpha3);
		//create & write the cci timestamp
		utility::cci_timestamp(itc, first2, tema, k2, w, un_op, init, ts_name); 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::cci_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the overload which takes a unary op
	tech_ind::cci_tema(first1, last1, first2, k1, alpha1, alpha2, alpha3, k2, w, tp, init); 	
}
//CCI version which uses WMA
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::cci_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (WMA)";
	int k1 = weights.size(); 
	int k = std::max(k1 - 1, k2 - 1);
	auto it = std::next(first1, k);
	//variable to store the weighted mean & the denominator value 
	T wm, denom;
	//weight sum 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	for(auto itc = it; itc != last1; itc++){
		//compute the weighted mean
		wm = utility::wtd_mean(std::prev(itc, k1 - 1), std::next(itc), weights, un_op, wsum, init); 
		//create & write the cci timestamp
		utility::cci_timestamp(itc, first2, wm, k2, w, un_op, init, ts_name); 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename W, typename T> 
void tech_ind::cci_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the overload which takes a unary op 
	tech_ind::cci_wma(first1, last1, first2, weights, k2, w, tp, init); 
}
//CCI version which uses a VWMA 
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::cci_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T w, UnaryOp un_op, T init){
	std::string ts_name = "Commodity Channel Index (VWMA)";
	int k = std::max(k1 - 1, k2 - 1);
	auto it = std::next(first1, k);
	//variable to store the weighted mean & the denominator value 
	T vwm, denom;
	//iterator in the range [first2, last2) which corresponds to std::prev(itc, k - 1) 
	auto it2 = std::next(first2, k - k1 + 1); 
	for(auto itc = it; itc != last1; itc++){
		//compute the weighted mean
		vwm = utility::wtd_mean(std::prev(itc, k1 - 1), std::next(itc), it2, un_op, [](const auto& c){return c.v();}, init); 
		//create & write the cci timestamp
		utility::cci_timestamp(itc, first3, vwm, k2, w, un_op, init, ts_name); 
		first3++;
		it2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::cci_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T w, T init){
	auto tp = [](const auto& c){
		return (c.h() + c.l() + c.c()) / 3; 
	}; 
	//call the cci_vwma overload which takes two iterator ranges and a unary 
	tech_ind::cci_vwma(first1, last1, first1, last1, first2, k1, k2, w, tp, init); 
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::rvigi_sma(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	std::string ts_name = "Relative Vigor Index (SMA)";
	//discard iterator 
	auto itd = std::next(first1, 3); 
	//lambda to compute composite price #1 
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	std::deque<T> rvi_vals;
	T m1 = utility::mean(itd, std::next(itd, k), init, p1);
	T m2 = utility::mean(itd, std::next(itd, k), init, p2);
	if(m2 != 0){
		rvi_vals.push_back(m1 / m2); 
	}else{
		rvi_vals.push_back(0.0); 
	}
	//compute & write the rvigi timestamps
	for(auto it = std::next(first1, 3 + k); it != last1; it++){
		//update the means
		utility::roll_mean_update(m1, p1(*itd), p1(*it), k); 
		utility::roll_mean_update(m2, p2(*itd), p2(*it), k);
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
		if(m2 != 0){
			rvi_vals.push_back(m1 / m2); 
		}else{
			rvi_vals.push_back(0.0); 
		}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(m2 != 0){
				rvi_vals.push_back(m1 / m2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{std::next(itd, k)->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
		//increment the discard iterator 
		itd++; 
	}
}
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::rvigi_ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init){
	std::string ts_name = "Relative Vigor Index (EMA)";
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	std::deque<T> rvi_vals;
	T ema1, ema2;
	//call ema setup for both emas
	utility::ema_setup(ema1, std::next(first1, 3), k, init, p1);
	utility::ema_setup(ema2, std::next(first1, 3), k, init, p2); 
	//compute & write the rvigi timestamps
	for(auto it = std::next(first1, 3 + k); it != last1; it++){
		//update the emas
		utility::ema_update(ema1, p1(*it), alpha); 
		utility::ema_update(ema2, p2(*it), alpha); 
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
			if(ema2 != 0){
				rvi_vals.push_back(ema1 / ema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(ema2 != 0){
				rvi_vals.push_back(ema1 / ema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::rvigi_dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init){
	std::string ts_name = "Relative Vigor Index (DEMA)"; 
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	std::deque<T> rvi_vals;
	T eema1, ema1, eema2, ema2, dema1, dema2;
	//call dema setup for both demas
	utility::dema_setup(eema1, ema1, std::next(first1, 3), k, alpha1, init, p1);
	utility::dema_setup(eema2, ema2, std::next(first1, 3), k, alpha1, init, p2); 
	//compute & write the rvigi timestamps
	for(auto it = std::next(first1, 3 + 2*k); it != last1; it++){
		//update the demas
		dema1 = utility::dema_update(eema1, ema1, p1(*it), alpha1, alpha2); 
		dema2 = utility::dema_update(eema2, ema2, p2(*it), alpha1, alpha2); 
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
			if(dema2 != 0){
				rvi_vals.push_back(dema1 / dema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(dema2 != 0){
				rvi_vals.push_back(dema1 / dema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::rvigi_tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T init){
	std::string ts_name = "Relative Vigor Index (TEMA)"; 
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	std::deque<T> rvi_vals;
	T eeema1, eema1, ema1, eeema2, eema2, ema2, tema1, tema2;
	//call tema setup for both temas
	utility::tema_setup(eeema1, eema1, ema1, std::next(first1, 3), k, alpha1, alpha2, init, p1);
	utility::tema_setup(eeema2, eema2, ema2, std::next(first1, 3), k, alpha1, alpha2, init, p2); 
	//compute & write the rvigi timestamps
	for(auto it = std::next(first1, 3 + 3*k); it != last1; it++){
		//update the temas
		tema1 = utility::tema_update(eeema1, eema1, ema1, p1(*it), alpha1, alpha2, alpha3); 
		tema2 = utility::tema_update(eeema2, eema2, ema2, p2(*it), alpha1, alpha2, alpha3); 
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
			if(tema2 != 0){
				rvi_vals.push_back(tema1 / tema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(tema2 != 0){
				rvi_vals.push_back(tema1 / tema2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename W, typename T>
void tech_ind::rvigi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init){
	std::string ts_name = "Relative Vigor Index (WMA)";
	//get the size of the weights vector & sum the weights 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	int k = weights.size(); 
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	//deque to hold the past 4 rvi values
	std::deque<T> rvi_vals;
	T wma1, wma2; 
	for(auto it = std::next(first1, 2 + k); it != last1; it++){
		//compute the weighted moving averages 
		wma1 = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights, p1, wsum, init); 
		wma2 = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights, p2, wsum, init); 
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
			if(wma2 != 0){
				rvi_vals.push_back(wma1 / wma2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(wma2 != 0){
				rvi_vals.push_back(wma1 / wma2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::rvigi_wma(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	//create the weights vector & call the rvigi_wma overload which takes a weights vector 
	std::vector<T> weights(k);
	for(int i = 0; i < k; i++){
		weights[i] = i + 1; 
	}
	tech_ind::rvigi_wma(first1, last1, first2, weights, init); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::rvigi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	std::string ts_name = "Relative Vigor Index (VWMA)"; 
	auto p1 = [&first1](const auto& c){
		auto cmo = [](const auto& c1){
			return c1->c() - c1->o();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (cmo(it) + 2*cmo(itp) + 2*cmo(itp2) + cmo(itp3)) / 6; 
	}; 
	//lambda to compute composite price #1
	auto p2 = [&first1](const auto& c){
		auto hml = [](const auto& c1){
			return c1->h() - c1->l();  
		}; 
		//iterator to the current element c 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		auto itp = std::prev(it);
		auto itp2 = std::prev(it, 2);
		auto itp3 = std::prev(it, 3); 
		return (hml(it) + 2*hml(itp) + 2*hml(itp2) + hml(itp3)) / 6; 
	};
	//deque to hold the past 4 rvi values
	std::deque<T> rvi_vals;
	T vwma1, vwma2; 
	for(auto it = std::next(first1, 2 + k); it != last1; it++){
		//compute the weighted moving averages 
		vwma1 = utility::wtd_mean(std::prev(it, k - 1), std::next(it), std::prev(it, k - 1), p1, [](const auto& c){return c.v();}, init); 
		vwma2 = utility::wtd_mean(std::prev(it, k - 1), std::next(it), std::prev(it, k - 1), p2, [](const auto& c){return c.v();}, init); 
		if(rvi_vals.size() < 4){
			//push the new value onto the deque
			if(vwma2 != 0){
				rvi_vals.push_back(vwma1 / vwma2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			if(rvi_vals.size() == 4){
				T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
				//compute & write the difference between the rvi and the signal line
				*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
				first2++; 
			}
		}else{
			//pop the oldest rvigi value & push the new rvigi value onto the back 
			rvi_vals.pop_front(); 
			if(vwma2 != 0){
				rvi_vals.push_back(vwma1 / vwma2); 
			}else{
				rvi_vals.push_back(0.0); 
			}
			//compute signal line 
			T sig = (rvi_vals[0] + 2*rvi_vals[1] + 2*rvi_vals[2] + rvi_vals[3]) / 6; 
			//compute & write the difference between the rvi and the signal line
			*first2 = Timestamp<T>{it->dt(), rvi_vals.back() - sig, ts_name};
			first2++; 
		}
	}
}

//Note: We do not scale CMO values by 100
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::chande_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Chande Momentum Oscillator";
	//discard iterator
	auto itd = std::next(first1); 
	auto u = [&first1, &un_op](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		if(un_op(c) > un_op(*itp)){
			return un_op(c); 
		}else{
			return 0.0; 
		}
	};
	auto d = [&first1, &un_op](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		if(un_op(c) < un_op(*itp)){
			return un_op(c); 
		}else{
			return 0.0; 
		}
	};
	//variables for the usum and dsum
	T usum, dsum; 
	//compute the pieces of used in the first CMO value 
	for(auto it = std::next(first1, k); it != last1; it++){
		//if we havent computed the first sums
		if(it == std::next(first1, k)){
			usum = std::accumulate(std::next(first1), std::next(first1, k + 1), init, [&u](T val, const auto& x){return val + u(x);}); 
			dsum = std::accumulate(std::next(first1), std::next(first1, k + 1), init, [&d](T val, const auto& x){return val + d(x);});
		}else{
			//update the sums 
			usum = usum - u(*itd) + u(*it); 
			dsum = dsum - d(*itd) + d(*it);
			itd++; 
		}
		//create & write the first CMO value
		if(usum != 0.0 && dsum != 0.0){
			*first2 = Timestamp<T>{it->dt(), (usum - dsum) / (usum + dsum), ts_name};
		}else{
			*first2 = Timestamp<T>{it->dt(), 0.5, ts_name};
		}
		first2++;
	}
}

//two pass algorithm (single pass is possible)
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	std::string ts_name = "Chaikin Oscillator (SMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::sma_diff(adl_ts.cbegin(), adl_ts.cend(), first2, k1, k2, [](const auto& ts){return ts.val();}, init, ts_name); 
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init){
	std::string ts_name = "Chaikin Oscillator (EMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::ema_diff(adl_ts.cbegin(), adl_ts.cend(), first2, k1, alpha1, k2, alpha2, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init){
	std::string ts_name = "Chaikin Oscillator (DEMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::dema_diff(adl_ts.cbegin(), adl_ts.cend(), first2, k1, alpha11, alpha12, k2, alpha21, alpha22, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init){
	std::string ts_name = "Chaikin Oscillator (TEMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::tema_diff(adl_ts.cbegin(), adl_ts.cend(), first2, k1, alpha11, alpha12, alpha13, k2, alpha21, alpha22, alpha23, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename W, typename T> 
void tech_ind::chaik_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, T init){
	std::string ts_name = "Chaikin Oscillator (WMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::wma_diff(adl_ts.cbegin(), adl_ts.cend(), first2, w1, w2, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::chaik_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	std::string ts_name = "Chaikin Oscillator (WMA)";
	std::vector<Timestamp<T>> adl_ts(std::distance(first1, last1)); 
	tech_ind::acc_dist(first1, last1, adl_ts.begin(), init);
	tech_ind::wma_diff(adl_ts.cbegin(), adl_ts.cend(), first2, k1, k2, [](const auto& ts){return ts.val();}, init, ts_name); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::on_bal_vol(InputIt first1, InputIt last1, OutputIt first2, UnaryOp un_op, T init){
	std::string ts_name = "On Balance Volume";
	//create the first obv timestamp
	T obv = first1->v();
	*first2 = Timestamp<T>{first1->dt(), std::move(obv), ts_name};
	first2++;
	//iterator to the previous object
	auto itp = first1; 
	for(auto it = std::next(first1); it != last1; it++){
		T cur = un_op(*it);  
		T prev = un_op(*itp); 
		if(cur > prev){
			obv += it->v(); 
		}else if(cur < prev){
			obv -= it->v(); 
		}
		//create & write the on balance volume timestamp
		*first2 = Timestamp<T>{it->dt(), std::move(obv), ts_name};
		first2++;
		itp++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::on_bal_vol(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp un_op, T ex){
	std::string ts_name = "On Balance Volume";
	//set the first obv timestamp
	T obv = first2->v();
	*first3 = Timestamp<T>{first1->dt(), std::move(obv), ts_name};
	first3++;
	//iterator to the previous object
	auto itp = first1;
	//iterator used to access the current periods volume
	auto itv = std::next(first2); 
	for(auto it = std::next(first1); it != last1; it++){
		T cur = un_op(*it);  
		T prev = un_op(*itp); 
		if(cur > prev){
			obv += itv->v(); 
		}else if(cur < prev){
			obv -= itv->v(); 
		}
		//create & write the on balance volume timestamp
		*first3 = Timestamp<T>{it->dt(), std::move(obv), ts_name};
		first3++;
		itp++;
		itv++; 
	}
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::neg_vol_idx(InputIt first1, InputIt last1, OutputIt first2, UnaryOp un_op, T ex){
	std::string ts_name = "Negative Volume Index";
	//set the first nvi timestamp
	T nvi = first1->v();
	*first2 = Timestamp<T>{first1->dt(), std::move(nvi), ts_name};
	first2++;
	//iterator to the previous object
	auto itp = first1; 
	for(auto it = std::next(first1); it != last1; it++){
		if(it->v() < itp->v()){
			nvi += (nvi * ( (un_op(*it) - un_op(*itp)) / un_op(*itp)));  
		}
		//make & write the timestamp
		*first2 = Timestamp<T>{it->dt(), std::move(nvi), ts_name};
		first2++;
		itp++; 
	}
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::neg_vol_idx(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp un_op, T ex){
	std::string ts_name = "Negative Volume Index";
	//set the first nvi timestamp
	T nvi = first2->v();
	*first3 = Timestamp<T>{first1->dt(), std::move(nvi), ts_name};
	first3++;
	//iterator to the previous object
	auto itp = first1;
	//iterator to the current object with a .v method 
	auto itv = std::next(first2); 
	for(auto it = std::next(first1); it != last1; it++){
		if(itv->v() < std::prev(itv)->v()){
			nvi += (nvi * ( (un_op(*it) - un_op(*itp)) / un_op(*itp)));  
		}
		//make & write the timestamp
		*first2 = Timestamp<T>{it->dt(), std::move(nvi), ts_name};
		first2++;
		itv++;
		itp++; 
	}
}
template <typename InputIt, typename OutputIt, typename T>
void tech_ind::vol_force(InputIt first1, InputIt last1, OutputIt first2, T ex){
	std::string ts_name = "Volume Force"; 
	T dm = first1->h() - first1->l();
	T cm = dm;
	int t = 1; 
	auto vf = [&first1, &cm, &dm, &t, &ts_name](const auto& c){
		//get the iterator to previous object
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		//store the t value from the previous iteration in tp
		int tp = t; 
		dm = c.h() - c.l();
		//update t
		if(c.hlc3() > itp->hlc3()){
			t = 1; 
		}else{
			t = -1; 
		}
		//check if the trend has changed
		if(tp == t){
			cm += dm; 
		}else{
			cm = dm + (itp->h() - itp->l()); 
		}
		if(cm == 0){
			return Timestamp<T>{c.dt(), 200*c.v()*t, ts_name};  
		}else{
			return Timestamp<T>{c.dt(), 200*c.v()*t*((dm / cm) - 1), ts_name};	
		}
	};
	std::transform(std::next(first1), last1, first2, vf); 
}

//The versions of the aroon oscillator only compute the aroon line (which is not scaled by 100) and dont return the upper and lower aroon lines
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	//call the version of aroon osc which takes two unary operators 
	aroon_osc(first1, last1, first2, k, [](const auto& c){return c.l();}, [](const auto& c){return c.h();}, ex); 
}
//version of aroon oscillator which constructs the comparators from a single unary_op 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex){
	std::string ts_name = "Aroon Oscillator";
	auto min_cmp = [&un_op](const auto& z1, const auto& z2){
		return un_op(z1) >= un_op(z2);  
	};
	auto max_cmp = [&un_op](const auto& z1, const auto& z2){
		return un_op(z1) <= un_op(z2); 
	};
	//initialize the deques 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	for(int i = 0; i < std::distance(first1, last1); i++){
		//call roll_minmax_update
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, min_cmp, max_cmp, un_op); 
		if(i >= k - 1){
			//computing the aroon line & writing the timestamp
			*first2 = Timestamp<T>{std::next(first1, i)->dt(), (dq_max.front() - dq_min.front()) / (k + 0.0), ts_name}; 
			first2++; 
		}
	}
}
//version of aroon oscillator which constructs the comparators from two unary operators (un_op1 is used to construct the min comparator & un_op2 the max comparator)
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2> 
void tech_ind::aroon_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex){
	std::string ts_name = "Aroon Oscillator";
	auto min_cmp = [&un_op1](const auto& z1, const auto& z2){
		return un_op1(z1) >= un_op1(z2); 
	};
	auto max_cmp = [&un_op2](const auto& z1, const auto& z2){
		return un_op2(z1) <= un_op2(z2); 
	}; 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	for(int i = 0; i < std::distance(first1, last1); i++){
		//call roll_minmax_update
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, min_cmp, max_cmp, un_op1, un_op2); 
		if(i >= k - 1){
			//computing the aroon line & writing the timestamp (cast k to type T)
			*first2 = Timestamp<T>{std::next(first1, i)->dt(), (dq_max.front() - dq_min.front()) / (k + 0.0), ts_name}; 
			first2++; 
		}
	}
}

//single pass
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::rsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (SMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp);
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the difference is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//iterator to the discarded object 
	auto itd = std::next(first1); 
	//compute the first means
	T mu = utility::mean(itd, std::next(itd, k), init, u);
	T md = utility::mean(itd, std::next(itd, k), init, d);
	//create & write the first timestamp
	utility::rsi_timestamp(std::next(first1, k), first2, mu, md, ts_name); 
	first2++;
	//create & write the rest of the timestamps using roll_mean_update
	for(auto it = std::next(itd, k); it != last1; it++){
		//update the means 
		utility::roll_mean_update(mu, u(*itd), u(*it), k); 
		utility::roll_mean_update(md, d(*itd), d(*it), k);
		//create & write the first timestamp 
		utility::rsi_timestamp(it, first2, mu, md, ts_name); 
		first2++;
		//increment the discard iterator 
		itd++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (EMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the differnece is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//first iterator 
	auto itf = std::next(first1); 
	//call ema_setup 
	T ema_u, ema_d; 
	utility::ema_setup(ema_u, itf, k, init, u); 
	utility::ema_setup(ema_d, itf, k, init, d);
	//create & write the timestamps 
	for(auto it = std::next(itf, k); it != last1; it++){
		//update the emas 
		utility::ema_update(ema_u, u(*it), alpha); 
		utility::ema_update(ema_d, d(*it), alpha);
		utility::rsi_timestamp(it, first2, ema_u, ema_d, ts_name); 
		first2++;

	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (DEMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the differnece is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//first iterator 
	auto itf = std::next(first1); 
	//call dema_setup 
	T eema_u, ema_u, eema_d, ema_d, dema_u, dema_d; 
	utility::dema_setup(eema_u, ema_u, itf, k, alpha1, init, u); 
	utility::dema_setup(eema_d, ema_d, itf, k, alpha1, init, d);
	//create & write the timestamps 
	for(auto it = std::next(itf, 2*k); it != last1; it++){
		//update the ema's & compute the dema values by calling dema_update 
		dema_u = utility::dema_update(eema_u, ema_u, u(*it), alpha1, alpha2); 
		dema_d = utility::dema_update(eema_d, ema_d, d(*it), alpha1, alpha2); 
		//create & write the timestamp
		utility::rsi_timestamp(it, first2, dema_u, dema_d, ts_name); 
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (TEMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the differnece is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//first iterator 
	auto itf = std::next(first1); 
	//call tema_setup 
	T eeema_u, eema_u, ema_u, eeema_d, eema_d, ema_d, tema_u, tema_d; 
	utility::tema_setup(eeema_u, eema_u, ema_u, itf, k, alpha1, alpha2, init, u); 
	utility::tema_setup(eeema_d, eema_d, ema_d, itf, k, alpha1, alpha2, init, d);
	//create & write the timestamps 
	for(auto it = std::next(itf, 3*k); it != last1; it++){
		//update the ema's & compute the tema values by calling tema_update 
		tema_u = utility::tema_update(eeema_u, eema_u, ema_u, u(*it), alpha1, alpha2, alpha3); 
		tema_d = utility::tema_update(eeema_d, eema_d, ema_d, d(*it), alpha1, alpha2, alpha3); 
		//create & write the timestamp
		utility::rsi_timestamp(it, first2, tema_u, tema_d, ts_name); 
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp>
void tech_ind::rsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (WMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the differnece is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//get the size of the weights vector 
	int k = weights.size(); 
	//compute the weight sum 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	//first iterator
	auto itf = std::next(first1);
	//weighted mean variables 
	T wmu, wmd;  
	for(auto it = std::next(itf, k - 1); it != last1; it++){
		wmu = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights, u, wsum, init); 
		wmd = utility::wtd_mean(std::prev(it, k - 1), std::next(it), weights, d, wsum, init);
		//create & write the timestamp
		utility::rsi_timestamp(it, first2, wmu, wmd, ts_name); 
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	//construct a weights vector from k & call the rsi_wma version which takes a weights vector 
	std::vector<T> weights(k);
	for(int i = 0; i < k; i++){
		weights[i] = i + 1; 
	}
	tech_ind::rsi_wma(first1, last1, first2, weights, un_op, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	//call the overload which takes two iterator ranges 
	tech_ind::rsi_vwma(first1, last1, first1, last1, first2, k, un_op, init); 
}
//RSI version which uses a vwma & assumes un_op operates on [first1, last1) and [first2, last2) elements have a .v() method	
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp un_op, T init){
	std::string ts_name = "Relative Strength Index (VWMA)";
	auto u = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff > 0){
			return diff; 
		}else{
			return 0.0; 
		}
	}; 
	auto d = [&first1, &un_op](const auto& z){
		//get the iterator to object prior to z 
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//compute the difference
		T diff = un_op(z) - un_op(*itp); 
		if(diff < 0){
			//return the absolute value of the difference if the differnece is negative
			return -1*diff; 
		}else{
			return 0.0; 
		}
	};
	//first iterator
	auto itf = std::next(first1);
	//iterator to the position in the second iterator range which corresponds to itf
	auto it2 = std::next(first2); 	
	//volume weighted mean variables 
	T vwmu, vwmd;  
	for(auto it = std::next(itf, k - 1); it != last1; it++){
		vwmu = utility::wtd_mean(std::prev(it, k - 1), std::next(it), it2, u, [](const auto& c){return c.v();}, init); 
		vwmd = utility::wtd_mean(std::prev(it, k - 1), std::next(it), it2, d, [](const auto& c){return c.v();}, init); 
		//create & write the timestamp
		utility::rsi_timestamp(it, first3, vwmu, vwmd, ts_name); 
		first3++;
		it2++; 
	}
}

//two pass algorithm (one pass is possible but two pass is much simpler)
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (SMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_sma function 
	tech_ind::rsi_sma(std_vals.cbegin(), std_vals.cend(), first2, k2, [](const auto& ts){return ts.val();}, init); 	
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (EMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_ema function 
	tech_ind::rsi_ema(std_vals.cbegin(), std_vals.cend(), first2, k2, alpha, [](const auto& ts){return ts.val();}, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (DEMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_dema function 
	tech_ind::rsi_dema(std_vals.cbegin(), std_vals.cend(), first2, k2, alpha1, alpha2, [](const auto& ts){return ts.val();}, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (TEMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_tema function 
	tech_ind::rsi_tema(std_vals.cbegin(), std_vals.cend(), first2, k2, alpha1, alpha2, alpha3, [](const auto& ts){return ts.val();}, init); 
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::rvoli_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, const std::vector<W>& weights, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (WMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_wma function 
	tech_ind::rsi_wma(std_vals.cbegin(), std_vals.cend(), first2, weights, [](const auto& ts){return ts.val();}, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (WMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_wma function 
	tech_ind::rsi_wma(std_vals.cbegin(), std_vals.cend(), first2, k2, [](const auto& ts){return ts.val();}, init); 
}
//assumes that the iterator range [first1, last1) contains elements of a type with a .v method
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (VWMA)";
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_vwma function 
	tech_ind::rsi_vwma(std_vals.cbegin(), std_vals.cend(), std::next(first1, k1 - 1), last1, first2, k2, [](const auto& ts){return ts.val();}, init); 
}
//Note: The moving std is computed with the iterator range [first1, last1) and [first2, last2) is used as the second iterator range in the rsi_vwma calls
//we assume first1 and first2 have the same datetime 
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::rvoli_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Relative Volatility Index (VWMA)"; 
	std::vector<Timestamp<T>> std_vals(std::distance(first1, last1) - k1 + 1);
	//compute the k1 period rolling std 
	tech_ind::mov_std(first1, last1, std_vals.begin(), k1, un_op, init);
	//call the rsi_vwma function 
	tech_ind::rsi_vwma(std_vals.cbegin(), std_vals.cend(), std::next(first2, k1 - 1), last2, first3, k2, [](const auto& ts){return ts.val();}, init); 
}

//single pass algorithm is possible but two pass is much easier
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (SMA)"; 
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - k1);
	//call rsi_sma 
	tech_ind::rsi_sma(first1, last1, rsi_ts.begin(), k1, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (EMA)"; 
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - k1 - 1);
	//call rsi_ema 
	tech_ind::rsi_ema(first1, last1, rsi_ts.begin(), k1, alpha, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (DEMA)"; 
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - 2*k1 - 1);
	//call rsi_dema 
	tech_ind::rsi_dema(first1, last1, rsi_ts.begin(), k1, alpha1, alpha2, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (TEMA)"; 
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - 3*k1 - 1);
	//call rsi_tema 
	tech_ind::rsi_tema(first1, last1, rsi_ts.begin(), k1, alpha1, alpha2, alpha3, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::srsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (WMA)";
	int k = weights.size(); 
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - k);
	//call rsi_wma 
	tech_ind::rsi_wma(first1, last1, rsi_ts.begin(), weights, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (WMA)";
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - k1);
	//call rsi_wma 
	tech_ind::rsi_wma(first1, last1, rsi_ts.begin(), k1, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first2, k2, undef, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	tech_ind::srsi_vwma(first1, last1, first1, last1, first2, k1, k2, un_op, init, undef); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::srsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Relative Strength Index (VWMA)";
	std::vector<Timestamp<T>> rsi_ts(std::distance(first1, last1) - k1);
	//call rsi_vwma 
	tech_ind::rsi_vwma(first1, last1, first2, last2, rsi_ts.begin(), k1, un_op, init);
	//call the minmax norm version which outputs to a range containing timestamps
	utility::roll_minmax_norm(rsi_ts.cbegin(), rsi_ts.cend(), first3, k2, undef, ts_name); 
}
//Coppock Curve 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (SMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::sma 
	tech_ind::sma(std::next(first1, k2 - 1), last1, first2, k3, roc_sum, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (EMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes 
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::ema 
	tech_ind::ema(std::next(first1, k2 - 1), last1, first2, k3, alpha, roc_sum, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha1, T alpha2, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (DEMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes 
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::dema 
	tech_ind::dema(std::next(first1, k2 - 1), last1, first2, k3, alpha1, alpha2, roc_sum, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (TEMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes 
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::tema 
	tech_ind::tema(std::next(first1, k2 - 1), last1, first2, k3, alpha1, alpha2, alpha3, roc_sum, init, ts_name); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init){
	//create the weights vector from k3 & call the overload which takes in a weights vector 
	std::vector<T> weights(k3);
	for(int i = 0; i < k3; i++){
		weights[i] = i + 1; 
	}
	tech_ind::copp_curve_wma(first1, last1, first2, k1, k2, weights, un_op, init); 
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::copp_curve_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& weights, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (WMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes 
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::wma
	tech_ind::wma(std::next(first1, k2 - 1), last1, first2, weights, roc_sum, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, UnaryOp un_op, T init){
	//call the overload which takes two iterator ranges
	tech_ind::copp_curve_vwma(first1, last1, first1, last1, first2, k1, k2, k3, un_op, init); 
}
//assumes that first1 & first2 have the same datetime
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::copp_curve_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, int k3, UnaryOp un_op, T init){
	std::string ts_name = "Coppock Curve (VWMA)";
	if(k1 > k2){
		//if k1 is larger swap the k's 
		std::swap(k1, k2); 
	}
	auto roc_sum = [&first1, &k1, &k2, &init, &un_op](const auto& c){
		//get the iterator to the current element 
		auto it = std::next(first1, std::distance(&(*first1), &c));
		//compute & return the sum of the percent changes 
		return utility::percent_change_sum(it, k1, k2, un_op, init, false); 
	};
	//call tech_ind::vwma 
	tech_ind::vwma(std::next(first1, k2 - 1), last1, std::next(first2, k2 - 1), last2, first3, k3, roc_sum, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::macd(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init){
	std::string ts_name = "Moving Average Convergence Divergence"; 
	//macd is just an ema difference
	tech_ind::ema_diff(first1, last1, first2, k1, alpha1, k2, alpha2, un_op, init, ts_name); 	
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::macd(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, int k3, UnaryOp un_op, T init){
	std::string ts_name = "MACD & Signal Line (SMA)";
	if(k1 > k2){
		//if k1 is larger sway the k's and the alphas
		std::swap(k1, k2);
		std::swap(alpha1, alpha2); 
	}
	//compute the first k2 - k1 ema values of the k1 period ema 
	std::vector<T> ema_k1_vals(k2 - k1);
	utility::ema(first1, std::next(first1, k2), ema_k1_vals.begin(), k1, alpha1, init, un_op);
	//grab the second to last ema_k1 value
	T ema_k1 = ema_k1_vals.back(); 
	//compute the mean of the first k2 values 
	T ema_k2 = utility::mean(first1, std::next(first1, k2), init, un_op);
	//variable to hold the ema difference and the value of the signal line 
	T ema_diff, sig;
	//deque to hold the past k3 ema_diffs
	std::deque<T> ema_diffs; 
	for(auto it = std::next(first1, k2); it != last1; it++){
		//compute & assign the ema difference value
		ema_diff = utility::ema_diff_update(ema_k1, ema_k2, un_op(*it), alpha1, alpha2);
		if(ema_diffs.size() < k3){
			ema_diffs.push_back(ema_diff);
			if(ema_diffs.size() == k3){
				//if we have just added the k3'rd ema_difference compute the signal line & write the timestamp 
				sig = utility::mean(ema_diffs.cbegin(), ema_diffs.cend(), init);
				*first2 = Timestamp<std::pair<T, T>>{it->dt(), std::make_pair(ema_diff, sig), ts_name};
				first2++;
			}
		}else{
			//push the newest ema_diff onto the deque 
			ema_diffs.push_back(ema_diff);
			//update the signal line 
			utility::roll_mean_update(sig, ema_diffs.front(), ema_diffs.back(), k3); 
			//make & write the timestamp
			*first2 = Timestamp<std::pair<T, T>>{it->dt(), std::make_pair(ema_diff, sig), ts_name};
			first2++;
			//pop the oldest ema_diff from the deque
			ema_diffs.pop_front(); 
		}
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T undef){
	std::string ts_name = "Stochastic Oscillator";
	//min comparator 
	auto min_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	}; 
	//max comparator 
	auto max_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 	
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	auto h_access = [](const auto& c){
		return c.h(); 
	}; 
	//call roll_minmax_norm 
	utility::roll_minmax_norm(first1, last1, first2, k, undef, min_cmp, max_cmp, l_access, h_access, un_op, ts_name); 
}
//Stochastic Oscillator SMA signal line (two pass algorithm)
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator SMA Signal";
	std::vector<Timestamp<T>> so_vals(std::distance(first1, last1) - k1 + 1);
	//compute the stochastic oscillator
	tech_ind::stoch_osc(first1, last1, so_vals.begin(), k1, un_op, undef);
	//compute the sma of the minmax stoch_osc
	tech_ind::sma(so_vals.cbegin(), so_vals.cend(), first2, k2, [](const auto& t){return t.val();}, init, ts_name); 	
}
//single pass algorithm
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator EMA Signal";
	auto min_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	}; 
	//max comparator 
	auto max_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 	
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	auto h_access = [](const auto& c){
		return c.h(); 
	}; 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	//call roll_minmax_update k1 - 1 times so that the next call returns a minmax pair
	for(int i = 0; i < k1 - 1; i++){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access); 
	}
	int i = k1 - 1; 
	auto op = [undef, first1, &dq_min, &dq_max, &min_cmp, &max_cmp, &un_op, &minmax, &i, k1, &l_access, &h_access](const auto& z){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access);
		i++; 
		if(std::get<0>(minmax[0]) != std::get<1>(minmax[0])){
			return (un_op(z) - std::get<0>(minmax[0])) / (std::get<1>(minmax[0]) - std::get<0>(minmax[0])); 
		}else{
			return undef;  
		}
	};
	tech_ind::ema(std::next(first1, k1 - 1), last1, first2, k2, alpha, op, init, ts_name);
}
//single pass algorithm
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::stoch_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator DEMA Signal";
	auto min_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	}; 
	//max comparator 
	auto max_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 	
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	auto h_access = [](const auto& c){
		return c.h(); 
	}; 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	//call roll_minmax_update k1 - 1 times so that the next call returns a minmax pair
	for(int i = 0; i < k1 - 1; i++){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access); 
	}
	int i = k1 - 1; 
	auto op = [undef, first1, &dq_min, &dq_max, &min_cmp, &max_cmp, &un_op, &minmax, &i, k1, &l_access, &h_access](const auto& z){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access);
		i++; 
		if(std::get<0>(minmax[0]) != std::get<1>(minmax[0])){
			return (un_op(z) - std::get<0>(minmax[0])) / (std::get<1>(minmax[0]) - std::get<0>(minmax[0])); 
		}else{
			return undef;  
		}
	};
	tech_ind::dema(std::next(first1, k1 - 1), last1, first2, k2, alpha1, alpha2, op, init, ts_name);
}
//Stochastic Oscillator TEMA Signal Line (single pass algorithm)
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void tech_ind::stoch_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T alpha3, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator TEMA Signal";
	auto min_cmp = [](const auto& c1, const auto& c2){
		return c1.l() >= c2.l(); 
	}; 
	//max comparator 
	auto max_cmp = [](const auto& c1, const auto& c2){
		return c1.h() <= c2.h(); 	
	};
	auto l_access = [](const auto& c){
		return c.l(); 
	}; 
	auto h_access = [](const auto& c){
		return c.h(); 
	}; 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	//call roll_minmax_update k1 - 1 times so that the next call returns a minmax pair
	for(int i = 0; i < k1 - 1; i++){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access); 
	}
	int i = k1 - 1; 
	auto op = [undef, first1, &dq_min, &dq_max, &min_cmp, &max_cmp, &un_op, &minmax, &i, k1, &l_access, &h_access](const auto& z){
		utility::roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k1, min_cmp, max_cmp, l_access, h_access);
		i++; 
		if(std::get<0>(minmax[0]) != std::get<1>(minmax[0])){
			return (un_op(z) - std::get<0>(minmax[0])) / (std::get<1>(minmax[0]) - std::get<0>(minmax[0])); 
		}else{
			return undef;  
		}
	};
	tech_ind::tema(std::next(first1, k1 - 1), last1, first2, k2, alpha1, alpha2, alpha3, op, init, ts_name);
}
//Stochastic Oscillator WMA Signal Line (takes in a weights vector)
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, const std::vector<W>& weights, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator WMA Signal";
	//compute the stoch_osc 
	std::vector<Timestamp<T>> so_vals(std::distance(first1, last1) - k1 + 1);
	tech_ind::stoch_osc(first1, last1, so_vals.begin(), k1, un_op, undef); 
	//compute the wma of the stoch_osc
	tech_ind::wma(so_vals.cbegin(), so_vals.cend(), first2, weights, [](const auto& t){return t.val();}, init, ts_name);
}
//Stochastic Oscillator WMA Signal Line (constructs a weights vector from k2)
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator WMA Signal";
	//compute the stoch_osc 
	std::vector<Timestamp<T>> so_vals(std::distance(first1, last1) - k1 + 1);
	tech_ind::stoch_osc(first1, last1, so_vals.begin(), k1, un_op, undef); 
	//compute the wma of the stoch_osc
	tech_ind::wma(so_vals.cbegin(), so_vals.cend(), first2, k2, [](const auto& t){return t.val();}, init, ts_name);
}
//SOSC VWMA Signal Line (volumes taken from [first1, last1)) 
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator VWMA Signal";
	//compute the stoch_osc 
	std::vector<Timestamp<T>> so_vals(std::distance(first1, last1) - k1 + 1);
	tech_ind::stoch_osc(first1, last1, so_vals.begin(), k1, un_op, undef); 
	//compute the wma of the stoch_osc
	tech_ind::vwma(so_vals.cbegin(), so_vals.cend(), std::next(first1, k1 - 1), last1, first2, k2, [](const auto& t){return t.val();}, init, ts_name);
}
//SOSC VWMA Signal Line (volumes taken from [first2, last2) & un_op works on [first1, last1)) 
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::stoch_osc_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "Stochastic Oscillator VWMA Signal";
	//compute the stoch_osc 
	std::vector<Timestamp<T>> so_vals(std::distance(first1, last1) - k1 + 1);
	tech_ind::stoch_osc(first1, last1, so_vals.begin(), k1, un_op, undef); 
	//compute the wma of the stoch_osc
	tech_ind::vwma(so_vals.cbegin(), so_vals.cend(), std::next(first2, k1 - 1), last2, first3, k2, [](const auto& t){return t.val();}, init, ts_name);
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::klinger_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	std::string ts_name = "Klinger Oscillator (SMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::sma_diff(vf_vals.cbegin(), vf_vals.cend(), first2, k1, k2, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::klinger_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init){
	std::string ts_name = "Klinger Oscillator (EMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::ema_diff(vf_vals.cbegin(), vf_vals.cend(), first2, k1, alpha1, k2, alpha2, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::klinger_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init){
	std::string ts_name = "Klinger Oscillator (DEMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::dema_diff(vf_vals.cbegin(), vf_vals.cend(), first2, k1, alpha11, alpha12, k2, alpha21, alpha22, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::klinger_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init){
	std::string ts_name = "Klinger Oscillator (TEMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::tema_diff(vf_vals.cbegin(), vf_vals.cend(), first2, k1, alpha11, alpha12, alpha13, k2, alpha21, alpha22, alpha23, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::klinger_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	std::string ts_name = "Klinger Oscillator (WMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::wma_diff(vf_vals.cbegin(), vf_vals.cend(), first2, k1, k2, [](const auto& ts){return ts.val();}, init, ts_name); 
}
template <typename InputIt, typename OutputIt, typename W, typename T> 
void tech_ind::klinger_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, T init){
	std::string ts_name = "Klinger Oscillator (WMA Difference)"; 
	std::vector<Timestamp<T>> vf_vals(std::distance(first1, last1) - 1);
	tech_ind::vol_force(first1, last1, vf_vals.begin(), init);
	tech_ind::wma_diff(vf_vals.cbegin(), vf_vals.cend(), first2, w1, w2, [](const auto& ts){return ts.val();}, init, ts_name); 
}

//Note: Values are not scaled by 100
template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::ultimate_osc(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, int k3, T init){
	std::string ts_name = "Ultimate Oscillator";
	//order the ks
	std::vector<int> kvec{k1, k2, k3};
	std::sort(kvec.begin(), kvec.end());
	k1 = kvec[0];
	k2 = kvec[1]; 
	k3 = kvec[2];
	//high minus low unary 
	auto tr = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	//buying pressure unary
	auto bp = [&first1](const auto& c){
		//iterator to the previous element 
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		return c.c() - std::min(c.l(), itp->c()); 
	}; 
	//iterator to the current element 
	auto it = std::next(first1, k3);
	//discard iterators to update the sums
	auto itd_k1 = std::prev(it, k1 - 1);
	auto itd_k2 = std::prev(it, k2 - 1);
	auto itd_k3 = std::next(first1);
	T bpsum_k1, bpsum_k2, bpsum_k3, trsum_k1, trsum_k2, trsum_k3; 
	T denom = 1 + (k2 / (k3 + 0.0)) + (k1 / (k3 + 0.0)); 
	for(it; it != last1; it++){
		if(it == std::next(first1, k3)){
			//if this it the first iteration compute the sums from scratch
			bpsum_k1 = std::accumulate(itd_k1, std::next(it), init, [&bp](T val, const auto& c){return val + bp(c);}); 
			bpsum_k2 = std::accumulate(itd_k2, std::next(it), init, [&bp](T val, const auto& c){return val + bp(c);}); 
			bpsum_k3 = std::accumulate(itd_k3, std::next(it), init, [&bp](T val, const auto& c){return val + bp(c);}); 
			trsum_k1 = std::accumulate(itd_k1, std::next(it), init, [&tr](T val, const auto& c){return val + tr(c);}); 
			trsum_k2 = std::accumulate(itd_k2, std::next(it), init, [&tr](T val, const auto& c){return val + tr(c);}); 
			trsum_k3 = std::accumulate(itd_k3, std::next(it), init, [&tr](T val, const auto& c){return val + tr(c);});
		}else{
			//if this is not the first iteration update the sums 
			bpsum_k1 = bpsum_k1 + bp(*it) - bp(*itd_k1); 	
			bpsum_k2 = bpsum_k2 + bp(*it) - bp(*itd_k2);  
			bpsum_k3 = bpsum_k3 + bp(*it) - bp(*itd_k3);  
			trsum_k1 = trsum_k1 + tr(*it) - tr(*itd_k1); 
			trsum_k2 = trsum_k2 + tr(*it) - tr(*itd_k2);
			trsum_k3 = trsum_k3 + tr(*it) - tr(*itd_k3);
			itd_k1++;  
			itd_k2++; 
			itd_k3++; 
		}
		//create & write the ultimate oscillator timestamps
		if(trsum_k1 == 0.0 || trsum_k2 == 0.0 || trsum_k2 == 0.0){
			*first2 = Timestamp<T>{it->dt(), 0.5, ts_name}; 
		}else{
			*first2 = Timestamp<T>{it->dt(), ( ((k1 / (k3 + 0.0))*(bpsum_k1 / trsum_k1)) + ((k2/(k3 + 0.0)) * (bpsum_k2 / trsum_k2)) + (bpsum_k3 / trsum_k3)) / denom, ts_name}; 
		}
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (SMA)";
	//leading iterator 
	auto itf = std::next(first1, std::max(k1, k2) - 1); 
	//variable for the sma 
	T sma_val; 
	//compute the dtp_osc values & write the Timestamps 
	for(auto it = itf; it != last1; it++){
		if(it == itf){
			//compute the mean from scratch on the first iteration
			sma_val = utility::mean(std::prev(it, k1 - 1), std::next(it), init, un_op);
		}else{
			//call roll mean update 
			utility::roll_mean_update(sma_val, un_op(*std::prev(it, k1)), un_op(*it), k1); 
		}
		*first2 = Timestamp<T>{it->dt(), un_op(*std::prev(it, k2 - 1)) - sma_val, ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (EMA)"; 
	//leading iterator 
	auto itf = std::next(first1, std::max(k1 + 1, k2) - 1); 
	//call ema_setup 
	T ema_val; 
	utility::ema_setup(ema_val, std::prev(itf, k1), k1, init, un_op); 
	//compute the dtp_osc values & write the Timestamps 
	for(auto it = itf; it != last1; it++){
		//call ema_update 
		utility::ema_update(ema_val, un_op(*it), alpha);
		*first2 = Timestamp<T>{it->dt(), un_op(*std::prev(it, k2 - 1)) - ema_val, ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (DEMA)"; 
	//leading iterator 
	auto itf = std::next(first1, std::max(2*k1 + 1, k2) - 1); 
	//call dema_setup 
	T eema_val, ema_val, dema_val; 
	utility::dema_setup(eema_val, ema_val, std::prev(itf, 2*k1), k1, alpha1, init, un_op); 
	//compute the dtp_osc values & write the Timestamps 
	for(auto it = itf; it != last1; it++){
		//call dema_update 
		dema_val = utility::dema_update(eema_val, ema_val, un_op(*it), alpha1, alpha2); 
		*first2 = Timestamp<T>{it->dt(), un_op(*std::prev(it, k2 - 1)) - dema_val, ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, T alpha2, T alpha3, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (TEMA)"; 
	//leading iterator 
	auto itf = std::next(first1, std::max(3*k1 + 1, k2) - 1); 
	//call tema_setup 
	T eeema_val, eema_val, ema_val, tema_val; 
	utility::tema_setup(eeema_val, eema_val, ema_val, std::prev(itf, 3*k1), k1, alpha1, alpha2, init, un_op); 
	//compute the dtp_osc values & write the Timestamps 
	for(auto it = itf; it != last1; it++){
		//call tema_update 
		tema_val = utility::tema_update(eeema_val,eema_val, ema_val, un_op(*it), alpha1, alpha2, alpha3); 
		*first2 = Timestamp<T>{it->dt(), un_op(*std::prev(it, k2 - 1)) - tema_val, ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (WMA)";
	//compute the weights size & sum the weights 
	int k1 = weights.size(); 
	T wsum = std::accumulate(weights.cbegin(), weights.cend(), init, [](T val, const W& w){return val + w;});
	//leading iterator 
	auto itf = std::next(first1, std::max(k1, k2) - 1);
	//compute & write the timestamps 
	for(auto it = itf; it != last1; it++){
		*first2 = Timestamp<T>{it->dt(), 
			un_op(*std::prev(it, k2 - 1)) - utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), weights, un_op, wsum, init), 
			ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	//construct the weights vector & call the dtp_osc_wma overload which takes a weights vector 
	std::vector<T> weights(k1);
	for(int i = 0; i < k1; i++){
		weights[i] = i + 1; 
	}
	tech_ind::dtp_osc_wma(first1, last1, first2, weights, k2, un_op, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init){
	//call the overload which takes two iterator ranges 
	tech_ind::dtp_osc_vwma(first1, last1, first1, last1, first2, k1, k2, un_op, init); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::dtp_osc_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init){
	std::string ts_name = "Detrended Price Oscillator (VWMA)";
	int k = std::max(k1, k2) - 1; 
	//leading iterator
	auto itf = std::next(first1, k);
	//iterator in the second iterator range corresponding to std::prev(it, k1 - 1) 
	auto it2 = std::next(first2, k - (k1 - 1));  
	//compute & write the timestamps 
	for(auto it = itf; it != last1; it++){
		*first3 = Timestamp<T>{it->dt(), 
			un_op(*std::prev(it, k2 - 1)) - utility::wtd_mean(std::prev(it, k1 - 1), std::next(it), it2, un_op, [](const auto& c){return c.v();}, init), 
			ts_name};
		first3++;
		it2++; 
	}
}

//Not a single pass algorithm (single pass is possible) 
//Note: values are not scaled by 100
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_sma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (SMA)";
	//unary operators (not really unaries but they take in one argument) to access differences & absolute differences 
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int n = std::distance(first1, last1) - (k1 + k2) - 3; 
	std::vector<T> mm_diff(n); 
	std::vector<T> mm_abs_diff(n); 
	auto f1 = [&diff, k1, k2, init, &mm_diff, first1, last1](){
		utility::roll_meanmean(std::next(first1), last1, mm_diff.begin(), k1, k2, init, diff); 
	}; 
	auto f2 = [&abs_diff, k1, k2, init, &mm_abs_diff, first1, last1](){
		utility::roll_meanmean(std::next(first1), last1, mm_abs_diff.begin(), k1, k2, init, abs_diff); 
	};
	//compute each mean of the mean on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const T& t1, const T& t2){
		if(t2 != 0.0){
			return t1 / t2; 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(mm_diff.cbegin(), mm_diff.cend(), mm_abs_diff.cbegin(), mm_abs_diff.cend(), std::next(first1, k1 + k2 - 1), first2, bin_op, ts_name, init); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_ema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (EMA)";
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int n = std::distance(first1, last1) - (k1 + k2); 
	std::vector<T> eema_diff(n); 
	std::vector<T> eema_abs_diff(n); 
	auto f1 = [&diff, k1, k2, init, &eema_diff, first1, last1, alpha1, alpha2](){
		utility::eema(std::next(first1), last1, eema_diff.begin(), k1, alpha1, k2, alpha2, init, diff); 
	}; 
	auto f2 = [&abs_diff, k1, k2, init, &eema_abs_diff, first1, last1, alpha1, alpha2](){
		utility::eema(std::next(first1), last1, eema_abs_diff.begin(), k1, alpha1, k2, alpha2, init, abs_diff); 
	};
	//compute each ema of the ema on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const T& t1, const T& t2){
		if(t2 != 0.0){
			return t1 / t2; 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(eema_diff.cbegin(), eema_diff.cend(), eema_abs_diff.cbegin(), eema_abs_diff.cend(), std::next(first1, k1 + k2 + 1), first2, bin_op, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_dema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (DEMA)";
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int k = 2*k1 + 2*k2; 
	int n = std::distance(first1, last1) - k; 
	std::vector<T> ddema_diff(n); 
	std::vector<T> ddema_abs_diff(n); 
	auto f1 = [&diff, k1, k2, init, &ddema_diff, first1, last1, alpha11, alpha12, alpha21, alpha22](){
		utility::ddema(std::next(first1), last1, ddema_diff.begin(), k1, alpha11, alpha12, k2, alpha21, alpha22, init, diff); 
	}; 
	auto f2 = [&abs_diff, k1, k2, init, &ddema_abs_diff, first1, last1, alpha11, alpha12, alpha21, alpha22](){
		utility::ddema(std::next(first1), last1, ddema_abs_diff.begin(), k1, alpha11, alpha12, k2, alpha21, alpha22, init, abs_diff); 
	};
	//compute each dema of the dema on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const T& t1, const T& t2){
		if(t2 != 0.0){
			return t1 / t2; 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(ddema_diff.cbegin(), ddema_diff.cend(), ddema_abs_diff.cbegin(), ddema_abs_diff.cend(), std::next(first1, k + 1), first2, bin_op, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_tema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (TEMA)";
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int k = (3*k1) + (3*k2); 
	int n = std::distance(first1, last1) - k; 
	std::vector<T> ttema_diff(n); 
	std::vector<T> ttema_abs_diff(n); 
	auto f1 = [&diff, k1, k2, init, &ttema_diff, first1, last1, alpha11, alpha12, alpha13, alpha21, alpha22, alpha23](){
		utility::ttema(std::next(first1), last1, ttema_diff.begin(), k1, alpha11, alpha12, alpha13, k2, alpha21, alpha22, alpha23, init, diff); 
	}; 
	auto f2 = [&abs_diff, k1, k2, init, &ttema_abs_diff, first1, last1, alpha11, alpha12, alpha13, alpha21, alpha22, alpha23](){
		utility::ttema(std::next(first1), last1, ttema_abs_diff.begin(), k1, alpha11, alpha12, alpha13, k2, alpha21, alpha22, alpha23, init, abs_diff); 
	};
	//compute each tema of the tema on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const T& t1, const T& t2){
		if(t2 != 0.0){
			return t1 / t2; 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(ttema_diff.cbegin(), ttema_diff.cend(), ttema_abs_diff.cbegin(), ttema_abs_diff.cend(), std::next(first1, k + 1), first2, bin_op, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp> 
void tech_ind::tsi_wma(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& w1, const std::vector<W>& w2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (WMA)";
	int k1 = w1.size(); 
	int k2 = w2.size(); 
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int n = std::distance(first1, last1) - (k1 + k2) + 1; 
	std::vector<Timestamp<T>> wma_wma_diff(n); 
	std::vector<Timestamp<T>> wma_wma_abs_diff(n);
	auto f1 = [first1, last1, init, &wma_wma_diff, &diff, &w1, &w2](){
		tech_ind::wma_wma(std::next(first1), last1, wma_wma_diff.begin(), w1, w2, diff, init, ""); 	
	}; 
	auto f2 = [first1, last1, init, &wma_wma_abs_diff, &abs_diff, &w1, &w2](){
		tech_ind::wma_wma(std::next(first1), last1, wma_wma_abs_diff.begin(), w1, w2, abs_diff, init, ""); 	
	}; 
	//compute each tema of the tema on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const auto& t1, const auto& t2){
		if(t2.val() != 0.0){
			return t1.val() / t2.val(); 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(wma_wma_diff.cbegin(), wma_wma_diff.cend(), wma_wma_abs_diff.cbegin(), wma_wma_abs_diff.cend(), wma_wma_diff.begin(), first2, bin_op, ts_name, init); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_wma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	//construct the weight vectors
	std::vector<T> w1(k1); 
	std::vector<T> w2(k2);
	for(int i = 0; i < std::max(k1, k2); i++){
		if(i < k1){
			w1[i] = i + 1; 
		}
		if(i < k2){
			w2[i] = i + 1; 
		}
	}
	//call the overload of tsi_wma which takes two weight vectors 
	tech_ind::tsi_wma(first1, last1, first2, w1, w2, un_op, init, undef); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_vwma(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T init, T undef){
	//call the overload which takes two iterator ranges 
	tech_ind::tsi_vwma(first1, last1, first1, last1, first2, k1, k2, un_op, init, undef); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp> 
void tech_ind::tsi_vwma(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, UnaryOp un_op, T init, T undef){
	std::string ts_name = "True Strength Index (VWMA)";
	auto diff = [&first1, &un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return un_op(z) - un_op(*itp); 
	};
	auto abs_diff = [&first1, & un_op](const auto& z){
		//get the iterator to the previous object
		auto itp = std::next(first1, std::distance(&(*first1), &z) - 1);
		//return the difference 
		return std::abs(un_op(z) - un_op(*itp)); 
	};
	int n = std::distance(first1, last1) - (k1 + k2) + 1; 
	std::vector<Timestamp<T>> vvwma_diff(n); 
	std::vector<Timestamp<T>> vvwma_abs_diff(n);
	auto f1 = [first1, last1, first2, last2, init, k1, k2, &vvwma_diff, &diff](){
		tech_ind::vwma_vwma(std::next(first1), last1, first2, last2, vvwma_diff.begin(), k1, k2, diff, init, ""); 	
	}; 
	auto f2 = [first1, last1, first2, last2, init, k1, k2, &vvwma_abs_diff, &abs_diff](){
		tech_ind::vwma_vwma(std::next(first1), last1, first2, last2, vvwma_abs_diff.begin(), k1, k2, abs_diff, init, "");
	}; 
	//compute each tema of the tema on separate threads 
	std::thread t1(f1);
	std::thread t2(f2); 
	t1.join(); 
	t2.join();
	auto bin_op = [undef](const auto& t1, const auto& t2){
		if(t2.val() != 0.0){
			return t1.val() / t2.val(); 
		}else{
			return undef; 
		}
	}; 
	//call timestamp_zip overload which takes two iterator ranges & a unary op 
	utility::timestamp_zip(vvwma_diff.cbegin(), vvwma_diff.cend(), vvwma_abs_diff.cbegin(), vvwma_abs_diff.cend(), vvwma_diff.begin(), first3, bin_op, ts_name, init); 
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::dmi(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	std::string ts_name = "Directional Movement Index";
	auto dmp = [&first1](const auto& c){
		//iterator to the previous object 
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		if(c.h() > itp->h()){
			return c.h() - itp->h();  
		}else{
			return 0.0; 
		}
	};
	auto dmn = [&first1](const auto& c){
		//iterator to the previous object 
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		if(c.l() < itp->l()){
			return itp->l() - c.l();
		}else{
			return 0.0; 
		}

	};
	auto tr = [&first1](const auto& c){
		auto itp = std::next(first1, std::distance(&(*first1), &c) - 1);
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	T alpha = 1.0 / k;
	//smoothed DM+, DM- and TR
	T sdmp, sdmn, smtr;
	T dip, din; 
	//call ema_setup 
	utility::ema_setup(sdmp, std::next(first1), k, init, dmp); 
	utility::ema_setup(sdmn, std::next(first1), k, init, dmn); 
	utility::ema_setup(smtr, std::next(first1), k, init, tr); 
	for(auto it = std::next(first1, k + 1); it != last1; it++){
		//call ema update 
		utility::ema_update(sdmp, dmp(*it), alpha); 
		utility::ema_update(sdmn, dmn(*it), alpha); 
		utility::ema_update(smtr, tr(*it), alpha);
		if(smtr != 0.0){
			dip = (sdmp / smtr);
			din = (sdmn / smtr);
		}else{
			dip = .5;
			din = .5; 	
		}
		//compute the directional movement index & make the timestamp
		*first2 = Timestamp<T>{it->dt(), 100.0 * std::abs(dip - din) / (dip + din), ts_name};
		first2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T> 
void tech_ind::adx(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	std::string ts_name = "Average Directional Index";
	//call dmi 
	std::vector<Timestamp<T>> dmi_vals(std::distance(first1, last1) - k1 - 1);
	tech_ind::dmi(first1, last1, dmi_vals.begin(), k1, init);
	tech_ind::wilders_ma(dmi_vals.cbegin(), dmi_vals.cend(), first2, k2, [](const auto& ts){return ts.val();}, init, ts_name); 
}


template <typename InputIt, typename UnaryOp> 
InputIt tech_ind::upiv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev){
	if(std::distance(first, itc) < k || std::distance(itc, last) < k){
		throw std::invalid_argument("upiv_it: itc must be at least k positions away from first and last"); 	
	}
	bool is_upiv; 
	while(!is_upiv && !(itc == std::next(first, k - 1) || itc == std::prev(last, k - 1))){
		is_upiv = tech_ind::is_up_pivot(first, last, itc, k, un_op);
		if(is_upiv){
			break; 
		}
		if(prev){
			itc--; 
		}else{
			itc++; 
		}
	}
	return itc; 
}
template <typename InputIt, typename UnaryOp> 
InputIt tech_ind::dpiv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev){
	if(std::distance(first, itc) < k || std::distance(itc, last) < k){
		throw std::invalid_argument("dpiv_it: itc must be at least k positions away from first and last"); 	
	}
	bool is_dpiv = false; 
	while(!is_dpiv && !(itc == std::next(first, k - 1) || itc == std::prev(last, k - 1))){
		is_dpiv = tech_ind::is_down_pivot(first, last, itc, k, un_op);
		if(is_dpiv){
			break; 
		}
		if(prev){
			itc--; 
		}else{
			itc++; 
		}
	}
	return itc; 
}
template <typename InputIt, typename UnaryOp> 
InputIt tech_ind::piv_it(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op, bool prev){
	bool is_upiv = false; 
	bool is_dpiv = false; 
	while(!is_dpiv && !is_upiv && !(itc == std::next(first, k - 1) || itc == std::prev(last, k - 1))){
		is_dpiv = tech_ind::is_down_pivot(first, last, itc, k, un_op);
		is_upiv = tech_ind::is_up_pivot(first, last, itc, k, un_op);
		if(is_upiv || is_dpiv){
			break; 
		}
		if(prev){
			itc--; 
		}else{
			itc++; 
		}
	}
	return itc; 
}
template <typename InputIt, typename UnaryOp> 
bool tech_ind::is_up_pivot(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op){
	if(std::distance(first, itc) < k || std::distance(itc, last) < k){
		return false; 
	}
	bool is_up = true; 
	auto itr = itc; 
	auto itl = itc;
	for(int i = 0; i < k; i++){
		if((un_op(*itl) >= un_op(*std::prev(itl))) || (un_op(*itr) >= un_op(*std::next(itr)))){
			//if we are not an up pivot
			is_up = false; 
			break; 
		}
		itl--; 
		itr++; 
	}
	return is_up; 
}
template <typename InputIt, typename UnaryOp> 
bool tech_ind::is_down_pivot(InputIt first, InputIt last, InputIt itc, int k, UnaryOp un_op){
	if(std::distance(first, itc) < k || std::distance(itc, last) < k){
		return false; 
	}
	bool is_dp = true; 
	auto itr = itc; 
	auto itl = itc;
	for(int i = 0; i < k; i++){
		if((un_op(*itl) <= un_op(*std::prev(itl))) || (un_op(*itr) <= un_op(*std::next(itr)))){
			//if we are not a down pivot
			is_dp = false; 
			break; 
		}
		itl--; 
		itr++; 
	}
	return is_dp; 
}

template <typename InputIt, typename OutputIt, typename UnaryOp> 
void tech_ind::pivot_up(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op){
	std::string ts_name = "Pivot Up"; 
	for(auto it = std::next(first1, k); it != std::prev(last1, k); it++){
		*first2 = Timestamp<bool>{it->dt(), tech_ind::is_up_pivot(first1, last1, it, k, un_op), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp> 
void tech_ind::pivot_down(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op){
	std::string ts_name = "Pivot Down"; 
	for(auto it = std::next(first1, k); it != std::prev(last1, k); it++){
		*first2 = Timestamp<bool>{it->dt(), tech_ind::is_down_pivot(first1, last1, it, k, un_op), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
void tech_ind::true_pivot_up(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2){
	std::string ts_name = "True Pivot Up"; 
	for(auto it = std::next(first1, k); it != std::prev(last1, k); it++){
		//if the current position in the candle vector is a pivot up
		if(tech_ind::is_up_pivot(first1, last1, it, k, un_op1)){
			//check if the next pivot (of either type) is higher than the up pivot 
			auto itnp = tech_ind::piv_it(first1, last1, std::next(it), k, un_op1, false);
			if(un_op2(*itnp) > un_op2(*it)){
				*first2 = Timestamp<bool>{it->dt(), true, ts_name};
			}else{
				*first2 = Timestamp<bool>{it->dt(), false, ts_name};
			}
		}else{
			*first2 = Timestamp<bool>{it->dt(), false, ts_name};
		}
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
void tech_ind::true_pivot_down(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp1 un_op1, UnaryOp2 un_op2){
	std::string ts_name = "True Pivot Down"; 
	for(auto it = std::next(first1, k); it != std::prev(last1, k); it++){
		//if the current position in the candle vector is a pivot down 
		if(tech_ind::is_down_pivot(first1, last1, it, k, un_op1)){
			//check if the next pivot (of either type) is lower than the down pivot 
			auto itnp = tech_ind::piv_it(first1, last1, std::next(it), k, un_op1, false);
			if(un_op2(*itnp) < un_op2(*it)){
				*first2 = Timestamp<bool>{it->dt(), true, ts_name};
			}else{
				*first2 = Timestamp<bool>{it->dt(), false, ts_name};
			}
		}else{
			*first2 = Timestamp<bool>{it->dt(), false, ts_name};
		}
		first2++; 
	}
}

template <typename InputIt, typename UnaryOp> 
int tech_ind::bars_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev){
	auto itpiv = tech_ind::upiv_it(first1, last1, itc, k, un_op, prev);
	return std::abs(std::distance(itc, itpiv)); 
}
template <typename InputIt, typename UnaryOp> 
int tech_ind::bars_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev){
	auto itpiv = tech_ind::dpiv_it(first1, last1, itc, k, un_op, prev);
	return std::abs(std::distance(itc, itpiv)); 
}
template <typename InputIt, typename UnaryOp> 
int tech_ind::bars_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, bool prev){
	auto itpiv = tech_ind::piv_it(first1, last1, itc, k, un_op, prev);
	return std::abs(std::distance(itc, itpiv)); 
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::perc_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev, bool scale){
	auto itpiv = tech_ind::upiv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::percent_change(un_op(*itc), un_op(*itpiv), scale); 
	}else{
		return utility::percent_change(un_op(*itpiv), un_op(*itc), scale); 
	}
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::perc_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev, bool scale){
	auto itpiv = tech_ind::dpiv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::percent_change(un_op(*itc), un_op(*itpiv), scale); 
	}else{
		return utility::percent_change(un_op(*itpiv), un_op(*itc), scale); 
	}
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::perc_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev, bool scale){
	auto itpiv = tech_ind::piv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::percent_change(un_op(*itc), un_op(*itpiv), scale); 
	}else{
		return utility::percent_change(un_op(*itpiv), un_op(*itc), scale); 
	}
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::log_ret_to_upiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev){
	auto itpiv = tech_ind::upiv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::log_return(un_op(*itc), un_op(*itpiv)); 
	}else{
		return utility::log_return(un_op(*itpiv), un_op(*itc)); 
	}
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::log_ret_to_dpiv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev){
	auto itpiv = tech_ind::dpiv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::log_return(un_op(*itc), un_op(*itpiv)); 
	}else{
		return utility::log_return(un_op(*itpiv), un_op(*itc)); 
	}
}
template <typename InputIt, typename UnaryOp, typename T> 
T tech_ind::log_ret_to_piv(InputIt first1, InputIt last1, InputIt itc, int k, UnaryOp un_op, T ex, bool prev){
	auto itpiv = tech_ind::piv_it(first1, last1, itc, k, un_op, prev);
	if(prev){
		return utility::log_return(un_op(*itc), un_op(*itpiv)); 
	}else{
		return utility::log_return(un_op(*itpiv), un_op(*itc)); 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void tech_ind::log_ret_to_avg_since_prev_piv(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Log Return to Average Since Previous Pivot";
	//get the iterator to the first pivot
	auto itppiv = tech_ind::piv_it(first1, last1, std::next(first1, k - 1), k, un_op, false);
	//variable for the mean since the previous pivot
	T m; 
	for(auto it = std::next(itppiv, k); it != last1; it++){
		m = utility::mean(itppiv, std::next(it), init, un_op);
		//compute & write the timestamp for the log return to the average since the previous pivot 
		*first2 = Timestamp<T>{it->dt(), utility::log_return(un_op(*it), m), ts_name};
		first2++;
		
		if(std::distance(first1, it) < 25){
			std::cout << "current candle" << std::endl; 
			it->display(); 
			std::cout << "previous pivot" << std::endl; 
			itppiv->display();
			std::cout << "mean since previous pivot" << m << std::endl; 
		}
		//update the previous pivot 
		itppiv = tech_ind::piv_it(first1, last1, std::prev(it, k - 1), k, un_op, true); 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void tech_ind::zscore_prev_piv(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	std::string ts_name = "Z-Score Since Previous Pivot"; 
	//get the iterator to the first pivot
	auto itppiv = tech_ind::piv_it(first1, last1, std::next(first1, k - 1), k, un_op, false);
	//variable for the zscore since the previous pivot
	T zscore; 
	for(auto it = std::next(itppiv, k); it != last1; it++){
		//compute & write the timestamp for the log return to the average since the previous pivot 
		*first2 = Timestamp<T>{it->dt(), utility::zscore(itppiv, std::next(it), un_op(*it), init, un_op), ts_name};
		first2++; 
		//update the previous pivot 
		itppiv = tech_ind::piv_it(first1, last1, std::prev(it, k - 1), k, un_op, true); 
	}
}
template <typename InputIt, typename UnaryOp> 
void tech_ind::trendline_setup(InputIt first, InputIt last, std::deque<InputIt>& dq_it_ppiv, int k1, int k2, UnaryOp un_op, bool up){
	//the iterator at which to start looking for pivots
	auto it = std::next(first, k2);
	while(dq_it_ppiv.size() < k1){
		if(up){
			if(tech_ind::is_up_pivot(first, last, it, k2, un_op)){
				dq_it_ppiv.push_back(it); 	
			}
		}else{
			if(tech_ind::is_down_pivot(first, last, it, k2, un_op)){
				dq_it_ppiv.push_back(it); 	
			}
		}
		it++; 
	}
}
template <typename InputIt, typename UnaryOp> 
void tech_ind::make_lr(mlpack::LinearRegression<arma::mat>& lr, const std::deque<InputIt>& dq_it_ppiv, UnaryOp un_op){
	int k = dq_it_ppiv.size(); 			
	//generate the arma matrices from the deque of iterators 
	arma::mat m(1, k);
	arma::rowvec r(k);
	for(int i = 0; i < k; i++){
		m[i] = std::distance(dq_it_ppiv.front(), dq_it_ppiv[i]);
		r[i] = un_op(*dq_it_ppiv[i]); 
	}
	//make the linear regression object 
	lr = mlpack::LinearRegression<arma::mat>(m, r); 
}

template <typename InputIt, typename UnaryOp, typename T> 
void tech_ind::trendline_update(mlpack::LinearRegression<arma::mat>& lr, std::deque<InputIt>& dq_it_ppiv, 
		InputIt first, InputIt last, InputIt itc, int k1, int k2, UnaryOp un_op, bool up, T ex){
	//check for a new pivot k2 periods prior to itc since we wouldn't know about any pivots after this 
	if(up && tech_ind::is_up_pivot(first, last, std::prev(itc, k2), k2, un_op) || 
			!up && tech_ind::is_down_pivot(first, last, std::prev(itc, k2), k2, un_op)){
		dq_it_ppiv.pop_front(); 
		dq_it_ppiv.push_back(std::prev(itc, k2));
	}else{
		return; 
	}
	//call make_lr
	tech_ind::make_lr(lr, dq_it_ppiv, un_op);
	/*
	if(std::distance(first, itc) < 250){
		std::cout << std::endl << "up: " << up << std::endl; 
		arma::vec coefs = lr.Parameters(); 
		std::cout << "slope of the linear model: " << coefs.at(1) << std::endl;
		std::cout << "intercept of the linear model: " << coefs.at(0) << std::endl;
		std::cout << "current candle and its distance" << std::endl; 
		itc->display();
		std::cout << "distance: " << std::distance(dq_it_ppiv.front(), itc) << std::endl; 
		std::cout << "displaying the previous pivots and their distances" << std::endl;
		for(auto it : dq_it_ppiv){
			it->display();
			std::cout << "Distance: " << std::distance(dq_it_ppiv.front(), it) << std::endl; 
		}
	}
	*/
}

template <typename InputIt, typename T> 
T tech_ind::trendline_evaluate(const mlpack::LinearRegression<arma::mat>& lr, const std::deque<InputIt>& dq_it_ppiv, InputIt itc, T ex){
	arma::vec cur = arma::vec(1).ones();
	cur.at(0) = std::distance(dq_it_ppiv.front(), itc) + 1;
	return lr.Predict(cur); 
}

//Note: The value of the trendline is taken to be the old value in the log return call 
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::trendline(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, bool up, T ex){
	std::string ts_name;
	if(up){
		ts_name = "Log Return to Trendline Through Pivot Ups";
	}else{
		ts_name = "Log Return to Trendline Through Pivot Downs"; 
	}
	//deque to hold the iterators to the past k1 pivots 
	std::deque<InputIt> dq_it_ppiv;
	//call trendline setup 
	tech_ind::trendline_setup(first1, last1, dq_it_ppiv, k1, k2, un_op1, up);
	//create the first linear model 
	mlpack::LinearRegression<arma::mat> lr;
	tech_ind::make_lr(lr, dq_it_ppiv, un_op1); 
	//Note: first iterator is k2 after the iterator of the back of the deque (why?) this is the first time we would know about the first k1 pivots 
	for(auto it = std::next(dq_it_ppiv.back(), k2); it != last1; it++){
		//dont call update on the first iteration (why it is possible the last pivot is k2 periods prior to it ==> would place it in the deque twice)
		if(it != std::next(dq_it_ppiv.back(), k2)){
			//call trendline_update 
			tech_ind::trendline_update(lr, dq_it_ppiv, first1, last1, it, k1, k2, un_op1, up, ex);
		}
		*first2 = Timestamp<T>{it->dt(), utility::log_return(un_op2(*it), tech_ind::trendline_evaluate(lr, dq_it_ppiv, it, ex)), ts_name};
		first2++; 
	}
}
//Trendline channel indicator
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::trendline_channel(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex){
	std::string ts_name = "Trendline Channel";
	//deques to hold the iterators to the past k1 up and dowm pivots
	std::deque<InputIt> dq_upiv; 
	std::deque<InputIt> dq_dpiv;
	//call trendline_setup for the pivots of each type 
	tech_ind::trendline_setup(first1, last1, dq_upiv, k1, k2, un_op1, true);  
	tech_ind::trendline_setup(first1, last1, dq_dpiv, k1, k2, un_op1, false); 
	//starting iterator
	auto it = first1;
	//if the k1st down pivot occurs after the k1st up pivot
	if(std::distance(first1, dq_upiv.back()) < std::distance(first1, dq_dpiv.back())){
		it = dq_dpiv.back();
		//get the most recent k1 up pivots prior to it
		auto next_upiv = tech_ind::upiv_it(first1, last1, std::next(dq_upiv.back()), k2, un_op1, false);  
		while(std::distance(next_upiv, it) >= 0){
			dq_upiv.pop_front(); 
			dq_upiv.push_back(next_upiv);
			next_upiv = tech_ind::upiv_it(first1, last1, std::next(dq_upiv.back()), k2, un_op1, false);  
		}
	}else{
		//if the k1st up pivot occurs after the k1st down pivot
		it = dq_upiv.back(); 
		//get the most recent k1 down pivots prior to it
		auto next_dpiv = tech_ind::dpiv_it(first1, last1, std::next(dq_dpiv.back()), k2, un_op1, false);
		while(std::distance(next_dpiv, it) >= 0){
			dq_dpiv.pop_front(); 
			dq_dpiv.push_back(next_dpiv);
			next_dpiv = tech_ind::dpiv_it(first1, last1, std::next(dq_dpiv.back()), k2, un_op1, false);  
		}
	}
	//make the linear regressions 
	mlpack::LinearRegression<arma::mat> lr_up; 
	mlpack::LinearRegression<arma::mat> lr_down; 
	//call make_lr for both the trendline channels 
	tech_ind::make_lr(lr_up, dq_upiv, un_op1); 
	tech_ind::make_lr(lr_down, dq_dpiv, un_op1); 
	for(auto itt = std::next(it, k2); itt != last1; itt++){
		//dont call update on the first iteration
		if(itt != std::next(it, k2)){
			//update the trendlines 
			tech_ind::trendline_update(lr_up, dq_upiv, first1, last1, itt, k1, k2, un_op1, true, ex); 
			tech_ind::trendline_update(lr_down, dq_dpiv, first1, last1, itt, k1, k2, un_op1, false, ex); 
		}
		//evaluate the up and down trendlines 
		T tlv_up = tech_ind::trendline_evaluate(lr_up, dq_upiv, itt, ex); 
		T tlv_down = tech_ind::trendline_evaluate(lr_down, dq_dpiv, itt, ex); 
		if(tlv_up != tlv_down){
			//Note: lr_up is the lower channel (trendline through up pivots) & lr_down is the upper channel
			*first2 = Timestamp<T>{itt->dt(), (un_op2(*itt) - tlv_up) / (tlv_down - tlv_up), ts_name};
		}else{
			//if the trendlines are equal just use .5 as the value which corresponds to in the middle of the trendline channel 
			*first2 = Timestamp<T>{itt->dt(), .5, ts_name};
		}
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp1, typename UnaryOp2, typename T> 
void tech_ind::trendline_channel(InputIt first1, InputIt last1, OutputIt first2, OutputIt first3, OutputIt first4, int k1, int k2, UnaryOp1 un_op1, UnaryOp2 un_op2, T ex){ 
	std::string ts_name1 = "Trendline Channel";
	std::string ts_name2 = "Trendline Channel Width";
	std::string ts_name3 = "Trendline Channel Slope Difference";
	//deques to hold the iterators to the past k1 up and dowm pivots
	std::deque<InputIt> dq_upiv; 
	std::deque<InputIt> dq_dpiv;
	//call trendline_setup for the pivots of each type 
	tech_ind::trendline_setup(first1, last1, dq_upiv, k1, k2, un_op1, true);  
	tech_ind::trendline_setup(first1, last1, dq_dpiv, k1, k2, un_op1, false); 
	//starting iterator
	auto it = first1;
	//if the k1st down pivot occurs after the k1st up pivot
	if(std::distance(first1, dq_upiv.back()) < std::distance(first1, dq_dpiv.back())){
		it = dq_dpiv.back();
		//get the most recent k1 up pivots prior to it
		auto next_upiv = tech_ind::upiv_it(first1, last1, std::next(dq_upiv.back()), k2, un_op1, false);  
		while(std::distance(next_upiv, it) >= 0){
			dq_upiv.pop_front(); 
			dq_upiv.push_back(next_upiv);
			next_upiv = tech_ind::upiv_it(first1, last1, std::next(dq_upiv.back()), k2, un_op1, false);  
		}
	}else{
		//if the k1st up pivot occurs after the k1st down pivot
		it = dq_upiv.back(); 
		//get the most recent k1 down pivots prior to it
		auto next_dpiv = tech_ind::dpiv_it(first1, last1, std::next(dq_dpiv.back()), k2, un_op1, false);
		while(std::distance(next_dpiv, it) >= 0){
			dq_dpiv.pop_front(); 
			dq_dpiv.push_back(next_dpiv);
			next_dpiv = tech_ind::dpiv_it(first1, last1, std::next(dq_dpiv.back()), k2, un_op1, false);  
		}
	}
	//make the linear regressions 
	mlpack::LinearRegression<arma::mat> lr_up; 
	mlpack::LinearRegression<arma::mat> lr_down; 
	//call make_lr for both the trendline channels 
	tech_ind::make_lr(lr_up, dq_upiv, un_op1); 
	tech_ind::make_lr(lr_down, dq_dpiv, un_op1); 
	for(auto itt = std::next(it, k2); itt != last1; itt++){
		//dont call update on the first iteration
		if(itt != std::next(it, k2)){
			//update the trendlines 
			tech_ind::trendline_update(lr_up, dq_upiv, first1, last1, itt, k1, k2, un_op1, true, ex); 
			tech_ind::trendline_update(lr_down, dq_dpiv, first1, last1, itt, k1, k2, un_op1, false, ex); 
		}
		//evaluate the up and down trendlines 
		T tlv_up = tech_ind::trendline_evaluate(lr_up, dq_upiv, itt, ex); 
		T tlv_down = tech_ind::trendline_evaluate(lr_down, dq_dpiv, itt, ex);
		arma::vec up_coefs = lr_up.Parameters(); 
		arma::vec down_coefs = lr_down.Parameters(); 
		if(tlv_up != tlv_down){
			//Note: lr_up is the lower channel (trendline through up pivots) & lr_down is the upper channel
			*first2 = Timestamp<T>{itt->dt(), (un_op2(*itt) - tlv_up) / (tlv_down - tlv_up), ts_name1};
		}else{
			//if the trendlines are equal just use .5 as the value which corresponds to in the middle of the trendline channel 
			*first2 = Timestamp<T>{itt->dt(), .5, ts_name1};
		}
		*first3 = Timestamp<T>{itt->dt(), tlv_down - tlv_up, ts_name2};
		*first4 = Timestamp<T>{itt->dt(), down_coefs.at(1) - up_coefs.at(1), ts_name3};
		first2++;
		first3++; 
		first4++; 
	}
}


