#pragma once
#include "../CandleSeries/CandleSeries.h" 
#include "../Timestamp/Timestamp.h"
#include "../Utility/utility.h"
#include <ranges>
#include <algorithm> 
#include <math.h>
#include <thread> 

//Note: methods are templated so that they work on Candle or CandlePtr objects
//Objects must have .o, .c, .h, .l, .hml methods
namespace cand_pat{
	/*
	 Single Candle Candlestick Patterns
	*/
	template <typename C>
	bool bull(const C& c);
	template <typename C> 
	bool bear(const C& c);
	//return the size of the body, bottom wick & top_wick
	template <typename C, typename T> 
	T body(const C& c, T ex); 
	template <typename C, typename T> 
	T body_bottom(const C& c, T ex);
	template <typename C, typename T> 
	T body_top(const C& c, T ex);
	//returns candle bottom + p*(body size)
	template <typename C, typename T> 
	T body_percentile(const C& c, T p); 
	template <typename C, typename T> 
	T bottom_wick(const C& c, T ex);
	//serves as a selector for different size types of a candle
	template <typename C, typename T> 
	T size_measure(const C& c, T ex, std::string type);
	template <typename C, typename T> 
	T top_wick(const C& c, T ex); 	
	template <typename C, typename T> 
	bool small_body(const C& c, T f = .3); 
	template <typename C, typename T> 
	bool large_body(const C& c, T f = .7);
	template <typename C, typename T> 
	bool small_top_wick(const C& c, T f = .3); 
	template <typename C, typename T> 
	bool large_top_wick(const C& c, T f = .5); 
	template <typename C, typename T> 
	bool small_bot_wick(const C& c, T f = .3); 
	template <typename C, typename T> 
	bool large_bot_wick(const C& c, T f = .5);

	//f1 is the value passed to small body, f2 is passed to small_top_wick and f3 is passed to large_bot_wick 
	template <typename C, typename T = double>
	bool hammer(const C& c, T f1 = .3, T f2 = .3, T f3 = .5);
	//applies the previous hammer overload to the iterator range [first1, last1)
	template <typename InputIt, typename OutputIt, typename T = double> 
	void hammer(InputIt first1, InputIt last1, OutputIt first2, T f1 = .3, T f2 = .3, T f3 = .5);

	//f1 is passed to small body, f2 is passed to small_bot_wick, and f3 is passed to large_top_wick
	template <typename C, typename T = double> 
	bool inv_hammer(const C& c, T f1 = .3, T f2 = .3, T f3 = .5);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void inv_hammer(InputIt first1, InputIt last1, OutputIt first2, T f1 = .3, T f2 = .3, T f3 = .5);

	//true if the body of the candle is > f*hml
	template <typename C, typename T = double>
	bool marubozu(const C& c, T f1 = .85); 
	template <typename C, typename T = double>
	bool bull_marubozu(const C& c, T f = .85);
	template <typename InputIt, typename OutputIt, typename T = double>
	void bull_marubozu(InputIt first1, InputIt last1, OutputIt first2, T f = .85);

	template <typename C, typename T = double> 
	bool bear_marubozu(const C& c, T f = .85); 
	template <typename InputIt, typename OutputIt, typename T = double> 
	void bear_marubozu(InputIt first1, InputIt last1, OutputIt first2, T f = .85);

	//f1 is passed to small body f2 determines whether the body is centered
	template <typename C, typename T = double> 
	bool spinning_top(const C& c, T f1 = .3, T f2 = .3);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void spinning_top(InputIt first1, InputIt last1, OutputIt first2, T f1 = .3, T f2 = .3);

	//very small top wick
	template <typename C, typename T = double> 
	bool shaven_head(const C& c, T f = .075); 
	template <typename InputIt, typename OutputIt, typename T = double> 
	void shaven_head(InputIt first1, InputIt last1, OutputIt first2, T f = .075); 

	template <typename C, typename T = double> 
	bool shaven_bottom(const C& c, T f = .075);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void shaven_bottom(InputIt first1, InputIt last1, OutputIt first2, T f = .075); 
	
	//f1 is passed to small_body, f2 defines the proportion of l + f2(hml) and or or h - f2(hml) the top and or bottom of the candle must be above
	template <typename C, typename T = double> 
	bool doji(const C& c, T f1 = .1, T f2 = .4);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void doji(InputIt first1, InputIt last1, OutputIt first2, T f1 = .1, T f2 = .4);

	template <typename C, typename T = double> 
	bool gravestone_doji(const C& c, T f1 = .1, T f2 = .3); 
	template <typename InputIt, typename OutputIt, typename T = double> 
	void gravestone_doji(InputIt first1, InputIt last1, OutputIt first2, T f1 = .1, T f2 = .3);

	template <typename C, typename T = double> 
	bool dragonfly_doji(const C& c, T f1 = .1, T f2 = .3); 
	template <typename InputIt, typename OutputIt, typename T = double>  
	void dragonfly_doji(InputIt first1, InputIt last1, OutputIt first2, T f1 = .1, T f2 = .3);
	
	//returns a weighted average of the {0, 1} values from inv_hammer, spinning_top, shaven_bottom, doji & gravestone_doji
	template <typename InputIt, typename OutputIt, typename W, typename T = double> 
	void bull_scsp(InputIt first1, InputIt last1, OutputIt first2, std::vector<W> w, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T f8, T f9, T f10); 
	//returns a weighted average of the {0, 1} values from hammer, spinning_top, shaven_bottom, doji & dragonfly_doji
	template <typename InputIt, typename OutputIt, typename W, typename T = double> 
	void bear_scsp(InputIt first1, InputIt last1, OutputIt first2, std::vector<W> w, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T f8, T f9, T f10); 

	/* 
		Double Candlestick Patterns	
	*/
	
	//Note:: The approach for determining if a candle is large or not is to compute the zscore for some measure of candle size with a window of the 
	//past k candles (candle size measures are specified using strings see size_measure above) 
	
	//returns is the Candle of CandlePtr pointed to by it1 engulfs the object it2 points to
	template <typename InputIt, typename T = double> 
	bool engulfs(InputIt it1, InputIt it2, T ex = 0.0);
	template <typename InputIt> 
	bool gap_up(InputIt it);
	template <typename InputIt> 
	bool gap_down(InputIt it); 
	
	//c_large is a boolean specifying if the current engulfing candle is large 
	template <typename InputIt> 
	bool bull_engulf(InputIt it, bool c_large);
	//k is the number of candles to use in computing the zscore, p is the the cutoff zscore for determining if a candle is large, meas is the string passed to size_measure
	//Note: init is used as the example type when computing the size measure
	template <typename InputIt, typename OutputIt, typename T = double>
	void bull_engulf(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0);  

	template <typename InputIt> 
	bool bear_engulf(InputIt it, bool c_large);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void bear_engulf(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0);

	//f1 is passed to bull marubozu, f2, f3 are passed to doji, f4, f5 are passed to dragonfly_doji
	//p_large specifies whether the previous candle was large
	template <typename InputIt, typename T = double> 
	bool bull_doji_star(InputIt it, bool p_large, T f1 = .85, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3); 
	template <typename InputIt, typename OutputIt, typename T = double> 
	void bull_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init, T f1 = .85, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3);

	//f1 is passed to bear marubozu, f2, f3 are passed to doji, f4, f5 are passed to gravestone_doji
	//p_large specifies whether the previous candle was large
	template <typename InputIt, typename T = double> 
	bool bear_doji_star(InputIt it, bool p_large, T f1 = .85, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3); 
	template <typename InputIt, typename OutputIt, typename T = double> 
	void bear_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init, T f1 = .85, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3);
	
	template <typename InputIt> 
	bool piercing_line(InputIt it, bool c_large, bool p_large);
	template <typename InputIt, typename OutputIt, typename T> 
	void piercing_line(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 

	template <typename InputIt> 
	bool dark_cloud_cover(InputIt it, bool c_large, bool p_large);
	template <typename InputIt, typename OutputIt, typename T> 
	void dark_cloud_cover(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 

	//f1 is passed to large body and f2 is passed to small body 
	//c_small is a bool specifying if the current candle is small, p_large specifies if the previous candle was large
	template <typename InputIt, typename T> 
	bool bull_harami(InputIt it, bool c_small, bool p_large, T f1 = .7, T f2 = .3);
	template <typename InputIt, typename OutputIt, typename T> 
	void bull_harami(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1 = .7, T f2 = .3, T init = 0.0);

	template <typename InputIt, typename T> 
	bool bear_harami(InputIt it, bool c_small, bool p_large, T f1 = .7, T f2 = .3); 
	template <typename InputIt, typename OutputIt, typename T> 
	void bear_harami(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1 = .7, T f2 = .3, T init = 0.0);
	
	//f1 is passed to large body, f2 & f3 are passed to doji, f4 & f5 are passed to gravestone_doji	
	template <typename InputIt, typename T> 
	bool bull_harami_doji(InputIt it, bool p_large, T f1 = .7, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3); 
	template <typename InputIt, typename OutputIt, typename T> 
	bool bull_harami_doji(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1 = .7, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3, T init = 0.0); 
	
	//f1 is passed to large body, f2 & f3 are passed to doji, f4 & f5 are passed to dragonfly_doji	
	template <typename InputIt, typename T> 
	bool bear_harami_doji(InputIt it, bool p_large, T f1 = .7, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3); 
	template <typename InputIt, typename OutputIt, typename T> 
	bool bear_harami_doji(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1 = .7, T f2 = .1, T f3 = .4, T f4 = .1, T f5 = .3, T init = 0.0); 
	
	//Note: On Neck and In Neck patterns are combined
	template <typename InputIt> 
	bool bull_neck(InputIt it);
	template <typename InputIt, typename OutputIt> 
	void bull_neck(InputIt first1, InputIt last1, OutputIt first2); 

	template <typename InputIt> 
	bool bear_neck(InputIt it); 
	template <typename InputIt, typename OutputIt> 
	void bear_neck(InputIt first1, InputIt last1, OutputIt first2); 
	
	//f is passed to marubozu
	template <typename InputIt, typename T = double> 
	bool rising_window(InputIt it, bool c_large, bool p_large, T f = .75);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void rising_window(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T f = .75, T init = 0.0); 

	template <typename InputIt, typename T = double> 
	bool falling_window(InputIt it, bool c_large, bool p_large, T f = .75);
	template <typename InputIt, typename OutputIt, typename T = double> 
	void falling_window(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T f = .75, T init = 0.0); 
	
	//Note: normally the counter attack line patterns require the current candle to be small but a large candle is a stronger signal ==> we only check if the previous candle was large
	template <typename InputIt> 
	bool bull_counter_attack(InputIt it, bool p_large); 
	template <typename InputIt, typename OutputIt, typename T> 
	void bull_counter_attack(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 
	
	template <typename InputIt> 
	bool bear_counter_attack(InputIt it, bool p_large);  
	template <typename InputIt, typename OutputIt, typename T> 
	void bear_counter_attack(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 

	/*
		Triple Candle Candlestick Patterns 
	*/
	
	//p2_large specifies is the candle 2 periods ago was large, p1_small specifies is the previous candle was small
	//f is passed to marubozu
	template <typename InputIt, typename T> 
	bool morning_star(InputIt it, bool p2_large, bool p1_small, T f = .85);
	template <typename InputIt, typename OutputIt, typename T> 
	void morning_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f = .85, T init = 0.0);
	
	template <typename InputIt, typename T> 
	bool evening_star(InputIt it, bool p2_large, bool p1_small, T f = .85);
	template <typename InputIt, typename OutputIt, typename T> 
	void evening_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f = .85, T init = 0.0);
	
	//f1, f2 are passed to doji, f3 & f4 are passed to gravestone_doji, f5 & f6 are passed to dragonfly_doji, f7 is passed to marubozu	
	template <typename InputIt, typename T> 
	bool morning_doji_star(InputIt it, bool p2_large, bool p1_small, T f1 = .1, T f2 = .4, T f3 = .1, T f4 = .3, T f5 = .1, T f6 = .3, T f7 = .85);
	template <typename InputIt, typename OutputIt, typename T> 
	void morning_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type,
			T f1 = .1, T f2 = .4, T f3 = .1, T f4 = .3, T f5 = .1, T f6 = .3, T f7 = .85, T init = 0.0);

	template <typename InputIt, typename T> 
	bool evening_doji_star(InputIt it, bool p2_large, bool p1_small, T f1 = .1, T f2 = .4, T f3 = .1, T f4 = .3, T f5 = .1, T f6 = .3, T f7 = .85);
	template <typename InputIt, typename OutputIt, typename T> 
	void evening_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, 
			T f1 = .1, T f2 = .4, T f3 = .1, T f4 = .3, T f5 = .1, T f6 = .3, T f7 = .85, T init = 0.0); 	

	//f defines the body percentile of the previous candle that the open of each candle must be above
	template <typename InputIt, typename T> 
	bool three_white_soldiers(InputIt it, bool c_large, bool p1_large, bool p2_large, T p = .3); 
	template <typename InputIt, typename OutputIt, typename T> 
	void three_white_soldiers(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T p = .3, T init = 0.0);

	//f defines the body percentile of the previous candle that the open of each candle must be below 
	template <typename InputIt, typename T> 
	bool three_black_crows(InputIt it, bool c_large, bool p1_large, bool p2_large, T p = .7); 
	template <typename InputIt, typename OutputIt, typename T> 
	void three_black_crows(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T p = .7, T init = 0.0);
	
	template <typename InputIt> 
	bool three_inside_up(InputIt it); 
	template <typename InputIt, typename OutputIt> 
	void three_inside_up(InputIt first1, InputIt last1, OutputIt first2);

	template <typename InputIt> 
	bool three_inside_down(InputIt it);
	template <typename InputIt, typename OutputIt> 
	void three_inside_down(InputIt first1, InputIt last1, OutputIt first2); 
	
	template <typename InputIt> 
	bool three_outside_up(InputIt it);
	template <typename InputIt, typename OutputIt> 
	void three_outside_up(InputIt first1, InputIt last1, OutputIt first2);

	template <typename InputIt> 
	bool three_outside_down(InputIt it);
	template <typename InputIt, typename OutputIt> 
	void three_outside_down(InputIt first1, InputIt last1, OutputIt first2);

	template <typename InputIt> 
	bool up_tasuki_gap(InputIt it, bool c_large, bool p1_large);
	template <typename InputIt, typename OutputIt, typename T> 
	void up_tasuki_gap(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 	

	template <typename InputIt> 
	bool down_tasuki_gap(InputIt it, bool c_large, bool p1_large);
	template <typename InputIt, typename OutputIt, typename T> 
	void down_tasuki_gap(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0);
	
	template <typename InputIt> 
	bool down_gap_two_soldiers(InputIt it, bool p2_large);
	template <typename InputIt, typename OutputIt, typename T> 
	void down_gap_two_soldiers(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 

	template <typename InputIt> 
	bool up_gap_two_crows(InputIt it, bool p2_large); 
	template <typename InputIt, typename OutputIt, typename T> 
	void up_gap_two_crows(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init = 0.0); 
}

/*
	Single Candle Candlestick Pattern Implementations 
*/
template <typename C>
bool cand_pat::bull(const C& c){
	return c.c() > c.o(); 	
}
template <typename C> 
bool cand_pat::bear(const C& c){
	return c.c() < c.o(); 
}
template <typename C, typename T> 
T cand_pat::body(const C& c, T ex){
	return std::abs(c.c() - c.o()); 
}
template <typename C, typename T> 
T cand_pat::body_percentile(const C& c, T p){
	return body_bottom(c, p) + (p*(body_top(c, p) - body_bottom(c, p))); 	
}
template <typename C, typename T> 
T cand_pat::body_bottom(const C& c, T ex){
	return std::min(c.c(), c.o()); 
}
template <typename C, typename T> 
T cand_pat::body_top(const C& c, T ex){
	return std::max(c.c(), c.o()); 
}
template <typename C, typename T> 
T cand_pat::bottom_wick(const C& c, T ex){
	return (body_bottom(c, ex) - c.l());  
}
template <typename C, typename T> 
T cand_pat::size_measure(const C& c, T ex, std::string type){
	if(type == "high minus low"){
		return c.h() - c.l(); 	
	}else if(type == "body"){
		return body(c, ex); 
	}else if(type == "max wick to body"){
		return std::max(c.h() - body_bottom(c, ex), body_top(c, ex) - c.l()); 	
	}else if(type == "mean wick to body"){
		return (1.0/2)*(c.h() - body_bottom(c, ex) + body_top(c, ex) - c.l()); 
	}else if(type == "mean of body and wick to body"){
		return (1.0/3)*(c.h() - body_bottom(c, ex) + body_top(c, ex) - c.l() + body(c, ex)); 
	}else{
		throw std::invalid_argument("size_measure: Enter a valid string for type"); 
	}
}
template <typename C, typename T> 
T cand_pat::top_wick(const C& c, T ex){
	return (c.h() - body_top(c, ex)); 
}
template <typename C, typename T> 
bool cand_pat::small_body(const C& c, T f){
	return (std::abs(c.o() - c.c())) < f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::large_body(const C& c, T f){
	return (std::abs(c.o() - c.c())) > f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::small_top_wick(const C& c, T f){
	return top_wick(c, f) < f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::large_top_wick(const C& c, T f){
	return top_wick(c, f) > f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::small_bot_wick(const C& c, T f){
	return bottom_wick(c, f) < f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::large_bot_wick(const C& c, T f){
	return bottom_wick(c, f) > f*(c.hml()); 
}
template <typename C, typename T> 
bool cand_pat::hammer(const C& c, T f1, T f2, T f3){
	return ((small_body(c, f1) && small_top_wick(c, f2)) && large_bot_wick(c, f3)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::hammer(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2, T f3){
	std::string ts_name = "Hammer";
	auto fcn = [f1, f2, f3, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::hammer(c, f1, f2, f3), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::inv_hammer(const C& c, T f1, T f2, T f3){
	return ((small_body(c, f1) && large_top_wick(c, f3)) && small_bot_wick(c, f2)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::inv_hammer(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2, T f3){
	std::string ts_name = "Inverted Hammer";
	auto fcn = [f1, f2, f3, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::inv_hammer(c, f1, f2, f3), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::marubozu(const C& c, T f){
	//use f as the example of the T type
	return (body(c, f) > (f*c.hml())); 
}
template <typename C, typename T> 
bool cand_pat::bull_marubozu(const C& c, T f){
	return bull(c) && marubozu(c, f);  
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bull_marubozu(InputIt first1, InputIt last1, OutputIt first2, T f){
	std::string ts_name = "Bullish Marubozu";
	auto fcn = [f, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::bull_marubozu(c, f), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::bear_marubozu(const C& c, T f){
	return bear(c) && marubozu(c, f); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bear_marubozu(InputIt first1, InputIt last1, OutputIt first2, T f){
	std::string ts_name = "Bearish Marubozu";
	auto fcn = [f, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::bear_marubozu(c, f), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::spinning_top(const C& c, T f1, T f2){ 
	T fhml = f2 * c.hml(); 	
	return (small_body(c, f1) && (body_top(c, f2) < (c.h() - fhml))) && (body_bottom(c, f2) > (c.l() + fhml)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::spinning_top(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2){
	std::string ts_name = "Spinning Top";
	auto fcn = [f1, f2, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::spinning_top(c, f1, f2), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::shaven_head(const C& c, T f){
	return small_top_wick(c, f); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::shaven_head(InputIt first1, InputIt last1, OutputIt first2, T f){
	std::string ts_name = "Shaven Head";
	auto fcn = [f, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::shaven_head(c, f), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::shaven_bottom(const C& c, T f){
	return small_bot_wick(c, f); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::shaven_bottom(InputIt first1, InputIt last1, OutputIt first2, T f){
	std::string ts_name = "Shaven Bottom";
	auto fcn = [f, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::shaven_bottom(c, f), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::doji(const C& c, T f1, T f2){
	return (small_body(c, f1) && (body_top(c, f1) < (c.h() - f2*c.hml()))) && (body_bottom(c, f1) > (c.l() + f2*c.hml()));
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::doji(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2){
	std::string ts_name = "Doji";
	auto fcn = [f1, f2, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::doji(c, f1, f2), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::gravestone_doji(const C& c, T f1, T f2){
	//must have a small body and the top of the body must be below l + f2(hml)
	return small_body(c, f1) && (body_top(c, f1) < (c.l() + f2*c.hml()));  
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::gravestone_doji(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2){
	std::string ts_name = "Gravestone Doji";
	auto fcn = [f1, f2, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::gravestone_doji(c, f1, f2), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename C, typename T> 
bool cand_pat::dragonfly_doji(const C& c, T f1, T f2){
	//must have a small body and the bottom of the body must be above h - f2(hml)
	return small_body(c, f1) && (body_bottom(c, f1) > (c.h() - f2*c.hml()));
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::dragonfly_doji(InputIt first1, InputIt last1, OutputIt first2, T f1, T f2){
	std::string ts_name = "Dragonfly Doji";
	auto fcn = [f1, f2, &ts_name](const auto& c){
		return Timestamp<bool>{c.dt(), cand_pat::dragonfly_doji(c, f1, f2), ts_name}; 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
template <typename InputIt, typename OutputIt, typename W, typename T> 
void cand_pat::bull_scsp(InputIt first1, InputIt last1, OutputIt first2, std::vector<W> w, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T f8, T f9, T f10){
	std::string ts_name = "Bull Single Candlestick Patterns"; 
	auto fcn = [&](const auto& c){
		std::vector<T> vals(5);
		if(cand_pat::inv_hammer(c, f1, f2, f3)){
			vals[0] = 1.0; 
		}else{
			vals[0] = 0.0; 
		}
		if(cand_pat::spinning_top(c, f4, f5)){
			vals[1] = 1.0; 
		}else{
			vals[1] = 0.0; 
		}
		if(cand_pat::shaven_bottom(c, f6)){
			vals[2] = 1.0; 
		}else{
			vals[2] = 0.0; 
		}
		if(cand_pat::doji(c, f7, f8)){
			vals[3] = 1.0; 
		}else{
			vals[3] = 0.0; 
		}
		if(cand_pat::gravestone_doji(c, f9, f10)){
			vals[4] = 1.0; 
		}else{
			vals[5] = 0.0; 
		}
		return Timestamp<T>{c.dt(), utility::wtd_mean(vals.begin(), vals.end(), w, 0.0), ts_name}; 
	}; 
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}
//returns a weighted average of the {0, 1} values from hammer, spinning_top, shaven_bottom, doji & dragonfly_doji
template <typename InputIt, typename OutputIt, typename W, typename T> 
void cand_pat::bear_scsp(InputIt first1, InputIt last1, OutputIt first2, std::vector<W> w, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T f8, T f9, T f10){
	std::string ts_name = "Bear Single Candlestick Patterns"; 
	auto fcn = [&](const auto& c){
		std::vector<T> vals(5);
		if(cand_pat::hammer(c, f1, f2, f3)){
			vals[0] = 1.0; 
		}else{
			vals[0] = 0.0; 
		}
		if(cand_pat::spinning_top(c, f4, f5)){
			vals[1] = 1.0; 
		}else{
			vals[1] = 0.0; 
		}
		if(cand_pat::shaven_head(c, f6)){
			vals[2] = 1.0; 
		}else{
			vals[2] = 0.0; 
		}
		if(cand_pat::doji(c, f7, f8)){
			vals[3] = 1.0; 
		}else{
			vals[3] = 0.0; 
		}
		if(cand_pat::dragonfly_doji(c, f9, f10)){
			vals[4] = 1.0; 
		}else{
			vals[4] = 0.0; 
		}
		return Timestamp<T>{c.dt(), utility::wtd_mean(vals.begin(), vals.end(), w, 0.0), ts_name}; 
	}; 
	std::transform(std::execution::par_unseq, first1, last1, first2, fcn); 
}

/*
	Double Candle Candlestick Pattern Implementations 
*/

template <typename InputIt, typename T> 
bool cand_pat::engulfs(InputIt it1, InputIt it2, T ex){
	return (body_top(*it1, ex) > body_top(*it2, ex)) && (body_bottom(*it1, ex) < body_bottom(*it2, ex)); 
}
template <typename InputIt> 
bool cand_pat::gap_up(InputIt it){
	auto itp = std::prev(it);
	return (it->o() > itp->c()); 
}
template <typename InputIt> 
bool cand_pat::gap_down(InputIt it){
	auto itp = std::prev(it);
	return (it->o() < itp->c()); 
}
template <typename InputIt> 
bool cand_pat::bull_engulf(InputIt it, bool c_large){
	auto itp = std::prev(it);
	return (bull(*it) && bear(*itp) && c_large && engulfs(it, itp)); 
}
template <typename InputIt, typename OutputIt, typename T>
void cand_pat::bull_engulf(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Bullish Engulfing";
	if(k <= 2){
		throw std::invalid_argument("bull_engulf: The parameter k must be larger than 2"); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator 
	auto itd = first1; 
	T m, std, zs;
	bool c_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
			zs = utility::zscore(size_unary(*it), m, std); 
		}else{
			//update the zscore 
			zs = utility::roll_zscore_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		c_large = (zs > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bull_engulf(it, c_large), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::bear_engulf(InputIt it, bool c_large){
	auto itp = std::prev(it);
	return (bear(*it) && bull(*itp) && c_large && engulfs(it, itp)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bear_engulf(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Bearish Engulfing";
	if(k <= 2){
		throw std::invalid_argument("bear_engulf: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator 
	auto itd = first1; 
	//variables for the mean, standard deviation and zscore
	T m, std, zs;
	bool c_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
			zs = utility::zscore(size_unary(*it), m, std); 
		}else{
			//update the zscore 
			zs = utility::roll_zscore_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		c_large = (zs > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bear_engulf(it, c_large), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bull_doji_star(InputIt it, bool p_large, T f1, T f2, T f3, T f4, T f5){
	auto itp = std::prev(it); 
	return (bear_marubozu(*itp, f1) && p_large && (doji(*it, f2, f3) || dragonfly_doji(*it, f4, f5)) && (body_top(*it, f1) < itp->c()));
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bull_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init, T f1, T f2, T f3, T f4, T f5){
	std::string ts_name = "Bullish Doji Star";
	if(k <= 2){
		throw std::invalid_argument("bull_doji_star: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score
	T m, std, zs;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the previous candle
		zs = utility::zscore(size_unary(*std::prev(it)), m, std); 
		p_large = (zs > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bull_doji_star(it, p_large, f1, f2, f3, f4, f5), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bear_doji_star(InputIt it, bool p_large, T f1, T f2, T f3, T f4, T f5){
	auto itp = std::prev(it); 
	return (bull_marubozu(*itp, f1) && p_large && (doji(*it, f2, f3) || gravestone_doji(*it, f4, f5)) && (body_bottom(*it, f1) > itp->c()));
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bear_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init, T f1, T f2, T f3, T f4, T f5){
	std::string ts_name = "Bearish Doji Star";
	if(k <= 2){
		throw std::invalid_argument("bear_doji_star: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score
	T m, std, zs;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the previous candle
		zs = utility::zscore(size_unary(*std::prev(it)), m, std); 
		p_large = (zs > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bear_doji_star(it, p_large, f1, f2, f3, f4, f5), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::piercing_line(InputIt it, bool c_large, bool p_large){
	auto itp = std::prev(it); 
	return (bull(*it) && bear(*itp) && c_large && p_large && (it->c() > body_percentile(*itp, .5)));  
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::piercing_line(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Piercing Line";
	if(k <= 2){
		throw std::invalid_argument("piercing_line: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::piercing_line(it, c_large, p_large), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::dark_cloud_cover(InputIt it, bool c_large, bool p_large){
	auto itp = std::prev(it); 
	return (bear(*it) && bull(*itp) && c_large && p_large && (it->c() < body_percentile(*itp, .5))); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::dark_cloud_cover(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Dark Cloud Cover";
	if(k <= 2){
		throw std::invalid_argument("dark_cloud_cover: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::dark_cloud_cover(it, c_large, p_large), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bull_harami(InputIt it, bool c_small, bool p_large, T f1, T f2){
	auto itp = std::prev(it);
	return (bull(*it) && bear(*itp) && c_small && p_large && small_body(*it, f2) && large_body(*itp, f1) && engulfs(itp, it)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bull_harami(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T init){
	std::string ts_name = "Bullish Harami";
	if(k <= 2){
		throw std::invalid_argument("bull_harami: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_small, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current candle is small and the previous candle was large 
		c_small = (zs_c < zs_small_cut);
		p_large = (zs_p > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bull_harami(it, c_small, p_large, f1, f2), ts_name};
		first2++;
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bear_harami(InputIt it, bool c_small, bool p_large, T f1, T f2){
	auto itp = std::prev(it);
	return (bear(*it) && bull(*itp) && c_small && p_large && small_body(*it, f2) && large_body(*itp, f1) && engulfs(itp, it)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bear_harami(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T init){
	std::string ts_name = "Bearish Harami";
	if(k <= 2){
		throw std::invalid_argument("bear_harami: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_small, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current candle is small and the previous candle was large 
		c_small = (zs_c < zs_small_cut);
		p_large = (zs_p > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bear_harami(it, c_small, p_large, f1, f2), ts_name};
		first2++;
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bull_harami_doji(InputIt it, bool p_large, T f1, T f2, T f3, T f4, T f5){
	auto itp = std::prev(it); 
	return (bull(*it) && bear(*itp) && p_large && large_body(*itp, f1) && engulfs(*itp, *it) && (doji(*it, f2, f3) || gravestone_doji(*it, f4, f5))); 
}
template <typename InputIt, typename OutputIt, typename T> 
bool cand_pat::bull_harami_doji(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T f3, T f4, T f5, T init){
	std::string ts_name = "Bullish Harami Doji";
	if(k <= 2){
		throw std::invalid_argument("bull_harami_doji: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current candle is small and the previous candle was large 
		p_large = (zs_p > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bull_harami_doji(it, p_large, f1, f2, f3, f4, f5), ts_name};
		first2++;
	}
}
template <typename InputIt, typename T> 
bool cand_pat::bear_harami_doji(InputIt it, bool p_large, T f1, T f2, T f3, T f4, T f5){
	auto itp = std::prev(it); 
	return (bear(*it) && bull(*itp) && p_large && large_body(*itp, f1) && engulfs(*itp, *it) && (doji(*it, f2, f3) || dragonfly_doji(*it, f4, f5))); 
}
template <typename InputIt, typename OutputIt, typename T> 
bool cand_pat::bear_harami_doji(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T f3, T f4, T f5, T init){
	std::string ts_name = "Bearish Harami Doji";
	if(k <= 2){
		throw std::invalid_argument("bear_harami_doji: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size boolean
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		p_large = (zs_p > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bear_harami_doji(it, p_large, f1, f2, f3, f4, f5), ts_name};
		first2++;
	}
}
template <typename InputIt> 
bool cand_pat::bull_neck(InputIt it){
	auto itp = std::prev(it);
	return (bull(*it) && bear(*itp) && (it->c() < itp->c())); 
}
template <typename InputIt, typename OutputIt> 
void cand_pat::bull_neck(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Bullish On/In Neck";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::bull_neck(it), ts_name}; 
	};
	std::transform(std::execution::par_unseq, std::next(first1), last1, first2, fcn); 
}
template <typename InputIt> 
bool cand_pat::bear_neck(InputIt it){
	auto itp = std::prev(it);
	return (bear(*it) && bull(*itp) && (it->c() > itp->c())); 
}
template <typename InputIt, typename OutputIt> 
void cand_pat::bear_neck(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Bearish On/In Neck";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::bear_neck(it), ts_name}; 
	};
	std::transform(std::execution::par_unseq, std::next(first1), last1, first2, fcn); 
}
template <typename InputIt, typename T> 
bool cand_pat::rising_window(InputIt it, bool c_large, bool p_large, T f){
	auto itp = std::prev(it);
	return (bull_marubozu(*it, f) && bull_marubozu(*itp, f) && c_large && p_large && gap_up(it));
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::rising_window(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T f, T init){
	std::string ts_name = "Rising Window"; 
	if(k <= 2){
		throw std::invalid_argument("rising_window: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::rising_window(it, c_large, p_large, f), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::falling_window(InputIt it, bool c_large, bool p_large, T f){
	auto itp = std::prev(it);
	return (bear_marubozu(*it, f) && bear_marubozu(*itp, f) && c_large && p_large && gap_down(it)); 	
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::falling_window(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T f, T init){
	std::string ts_name = "Falling Window"; 
	if(k <= 2){
		throw std::invalid_argument("falling_window: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::falling_window(it, c_large, p_large, f), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::bull_counter_attack(InputIt it, bool p_large){
	auto itp = std::prev(it); 
	return (bull(*it) && bear(*itp) && p_large && gap_down(it)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bull_counter_attack(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Bullish Counter Attack Line";
	if(k <= 2){
		throw std::invalid_argument("bull_counter_attack: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size boolean
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bull_counter_attack(it, p_large), ts_name};
		first2++;
	}
}
template <typename InputIt> 
bool cand_pat::bear_counter_attack(InputIt it, bool p_large){
	auto itp = std::prev(it); 
	return (bear(*it) && bull(*itp) && p_large && gap_up(it)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::bear_counter_attack(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Bearish Counter Attack Line";
	if(k <= 2){
		throw std::invalid_argument("bear_counter_attack: The parameter k must be larger than 2."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p;
	bool p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size boolean
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::bear_counter_attack(it, p_large), ts_name};
		first2++;
	}
}
/*
	Triple Candle Candlestick Pattern Implementations 
*/
template <typename InputIt, typename T> 
bool cand_pat::morning_star(InputIt it, bool p2_large, bool p1_small, T f){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	double ex = 0.0; 
	return (bear_marubozu(*itp2, f) && p2_large && p1_small && (body_top(*itp, ex) < itp2->c()) && bull(*it) && (it->c() >= body_percentile(*itp2, .5))); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::morning_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f, T init){
	std::string ts_name = "Morning Star";
	if(k <= 3){
		throw std::invalid_argument("morning_star: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p1, zs_p2;
	bool p1_small, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size booleans
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		p1_small = (zs_p1 < zs_small_cut);
		p2_large = (zs_p2 > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::morning_star(it, p2_large, p1_small, f), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::evening_star(InputIt it, bool p2_large, bool p1_small, T f){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	double ex = 0.0; 
	return (bull_marubozu(*itp2, f) && p2_large && p1_small && (body_bottom(*itp, ex) > itp2->c()) && bear(*it) && (it->c() <= body_percentile(*itp2, .5))); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::evening_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f, T init){
	std::string ts_name = "Evening Star";
	if(k <= 3){
		throw std::invalid_argument("evening_star: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p1, zs_p2;
	bool p1_small, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size booleans
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		p1_small = (zs_p1 < zs_small_cut);
		p2_large = (zs_p2 > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::evening_star(it, p2_large, p1_small, f), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::morning_doji_star(InputIt it, bool p2_large, bool p1_small, T f1, T f2, T f3, T f4, T f5, T f6, T f7){
	return (morning_star(it, p2_large, p1_small, f7) && (doji(*it, f1, f2) || gravestone_doji(*it, f3, f4) || dragonfly_doji(*it, f5, f6))); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::morning_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T init){
	std::string ts_name = "Morning Doji Star";
	if(k <= 3){
		throw std::invalid_argument("morning_doji_star: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p1, zs_p2;
	bool p1_small, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscores & size booleans
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		p1_small = (zs_p1 < zs_small_cut);
		p2_large = (zs_p2 > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::morning_doji_star(it, p2_large, p1_small, f1, f2, f3, f4, f5, f6, f7), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::evening_doji_star(InputIt it, bool p2_large, bool p1_small, T f1, T f2, T f3, T f4, T f5, T f6, T f7){
	return (evening_star(it, p2_large, p1_small, f7) && (doji(*it, f1, f2) || gravestone_doji(*it, f3, f4) || dragonfly_doji(*it, f5, f6))); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::evening_doji_star(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_small_cut, T zs_large_cut, std::string type, T f1, T f2, T f3, T f4, T f5, T f6, T f7, T init){
	std::string ts_name = "Evening Doji Star";
	if(k <= 3){
		throw std::invalid_argument("evening_doji_star: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p1, zs_p2;
	bool p1_small, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the previous 
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		//compute the zscore of the candle two periods ago
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		//booleans for if the current and previous candles were large
		p1_small = (zs_p1 < zs_small_cut);
		p2_large = (zs_p2 > zs_large_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::evening_doji_star(it, p2_large, p1_small, f1, f2, f3, f4, f5, f6, f7), ts_name};
		first2++; 
	}
}
template <typename InputIt, typename T> 
bool cand_pat::three_white_soldiers(InputIt it, bool c_large, bool p1_large, bool p2_large, T p){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	return (bull(*it) && bull(*itp) && bull(*itp2) && c_large && p1_large && p2_large && (itp->o() >= body_percentile(*itp2, p)) && 
			(it->o() >= body_percentile(*itp, p)) && (itp->c() > itp2->c()) && (it->c() > itp->c())); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::three_white_soldiers(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T p, T init){
	std::string ts_name = "Three White Soldiers"; 	
	if(k <= 3){
		throw std::invalid_argument("three_white_soldiers: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p1, zs_p2;
	bool c_large, p1_large, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		//compute the zscore of the candle two periods ago
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std);
		c_large = (zs_p1 > zs_cut); 
		p1_large = (zs_p1 > zs_cut);
		p2_large = (zs_p2 > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::three_white_soldiers(it, c_large, p1_large, p2_large, p), ts_name};
		first2++; 
	}
}
//f defines the body percentile of the previous candle that the open of each candle must be below 
template <typename InputIt, typename T> 
bool cand_pat::three_black_crows(InputIt it, bool c_large, bool p1_large, bool p2_large, T p){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	return (bear(*it) && bear(*itp) && bear(*itp2) && c_large && p1_large && p2_large && (itp->o() <= body_percentile(*itp2, p)) && 
			(it->o() <= body_percentile(*itp, p)) && (itp->c() < itp2->c()) && (it->c() < itp->c())); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::three_black_crows(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T p, T init){
	std::string ts_name = "Three Black Crows"; 	
	if(k <= 3){
		throw std::invalid_argument("three_black_crows: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p1, zs_p2;
	bool c_large, p1_large, p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p1 = utility::zscore(size_unary(*std::prev(it)), m, std);
		//compute the zscore of the candle two periods ago
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std);
		c_large = (zs_p1 > zs_cut); 
		p1_large = (zs_p1 > zs_cut);
		p2_large = (zs_p2 > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::three_black_crows(it, c_large, p1_large, p2_large, p), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::three_inside_up(InputIt it){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bear(*itp2) && bull(*itp) && bull(*it) && (itp->c() > body_percentile(*itp2, .5)) && (it->c() > itp2->o())); 
}
template <typename InputIt, typename OutputIt> 
void cand_pat::three_inside_up(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Three Inside Up";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::three_inside_up(it), ts_name}; 
	}; 
	std::transform(std::execution::par_unseq, std::next(first1, 2), last1, first2, fcn); 
}
template <typename InputIt> 
bool cand_pat::three_inside_down(InputIt it){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bull(*itp2) && bear(*itp) && bear(*it) && (itp->c() < body_percentile(*itp2, .5)) && (it->c() < itp2->o())); 	
}
template <typename InputIt, typename OutputIt> 
void cand_pat::three_inside_down(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Three Inside Down";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::three_inside_down(it), ts_name}; 
	}; 
	std::transform(std::execution::par_unseq, std::next(first1, 2), last1, first2, fcn); 
}
template <typename InputIt> 
bool cand_pat::three_outside_up(InputIt it){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bear(*itp2) && bull(*itp) && bull(*it) && engulfs(itp, itp2) && (it->c() > itp->c())); 
}
template <typename InputIt, typename OutputIt> 
void cand_pat::three_outside_up(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Three Outside Up";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::three_outside_up(it), ts_name}; 
	}; 
	std::transform(std::execution::par_unseq, std::next(first1, 2), last1, first2, fcn); 
}
template <typename InputIt> 
bool cand_pat::three_outside_down(InputIt it){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bull(*itp2) && bear(*itp) && bear(*it) && engulfs(itp, itp2) && (it->c() < itp->c())); 
}
template <typename InputIt, typename OutputIt> 
void cand_pat::three_outside_down(InputIt first1, InputIt last1, OutputIt first2){
	std::string ts_name = "Three Outside Down";
	auto fcn = [first1, &ts_name](const auto& c){
		auto it = std::next(first1, std::distance(&(*first1), &c));
		return Timestamp<bool>{c.dt(), cand_pat::three_outside_down(it), ts_name}; 
	};
	std::transform(std::execution::par_unseq, std::next(first1, 2), last1, first2, fcn); 
}
template <typename InputIt> 
bool cand_pat::up_tasuki_gap(InputIt it, bool c_large, bool p1_large){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bull(*itp2) && bull(*itp) && bear(*it) && c_large && p1_large && gap_up(itp) && (it->c() > itp2->c()) && (it->c() < itp->o())); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::up_tasuki_gap(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Up Taskuki Gap"; 
	if(k <= 3){
		throw std::invalid_argument("up_tasuki_gap: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::up_tasuki_gap(it, c_large, p_large), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::down_tasuki_gap(InputIt it, bool c_large, bool p1_large){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp); 
	return (bear(*itp2) && bear(*itp) && bull(*it) && c_large && p1_large && gap_down(itp) && (it->c() < itp2->c()) && (it->c() > itp->o())); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::down_tasuki_gap(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Down Taskuki Gap"; 
	if(k <= 3){
		throw std::invalid_argument("down_tasuki_gap: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_c, zs_p;
	bool c_large, p_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore of the current candle 
		zs_c = utility::zscore(size_unary(*it), m, std);
		//compute the zscore of the previous candle
		zs_p = utility::zscore(size_unary(*std::prev(it)), m, std); 
		//booleans for if the current and previous candles were large
		c_large = (zs_c > zs_cut);
		p_large = (zs_p > zs_cut); 
		*first2 = Timestamp<bool>{it->dt(), cand_pat::down_tasuki_gap(it, c_large, p_large), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::down_gap_two_soldiers(InputIt it, bool p2_large){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	return (bear(*itp2) && bull(*itp) && bull(*it) && p2_large && gap_down(itp) && engulfs(it, itp)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::down_gap_two_soldiers(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Down Gap Two Soldiers";
	if(k <= 3){
		throw std::invalid_argument("down_gap_two_soldiers: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p2;
	bool p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore & the size boolean
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		p2_large = (zs_p2 > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::down_gap_two_soldiers(it, p2_large), ts_name};
		first2++; 
	}
}
template <typename InputIt> 
bool cand_pat::up_gap_two_crows(InputIt it, bool p2_large){
	auto itp = std::prev(it);
	auto itp2 = std::prev(itp);
	return (bull(*itp2) && bear(*itp) && bear(*it) && p2_large && gap_up(itp) && engulfs(it, itp)); 
}
template <typename InputIt, typename OutputIt, typename T> 
void cand_pat::up_gap_two_crows(InputIt first1, InputIt last1, OutputIt first2, int k, T zs_cut, std::string type, T init){
	std::string ts_name = "Up Gap Two Crows";
	if(k <= 3){
		throw std::invalid_argument("up_gap_two_crows: The parameter k must be larger than 3."); 
	}
	auto size_unary = [&type, init](const auto& c){
		return cand_pat::size_measure(c, init, type); 
	};
	//discard iterator
	auto itd = first1;
	//variables for the mean, std and z-score for the current and previous candle sizes
	T m, std, zs_p2;
	bool p2_large; 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		if(it == std::next(first1, k - 1)){
			std::pair<T, T> mv = utility::mean_var(first1, std::next(it), init, size_unary);
			m = mv.first; 
			std = std::sqrt(mv.second); 
		}else{
			//update the standard deviation 
			utility::roll_std_update(std, m, size_unary(*itd), size_unary(*it), k); 
			itd++; 
		}
		//compute the zscore & the size boolean
		zs_p2 = utility::zscore(size_unary(*std::prev(it, 2)), m, std); 
		p2_large = (zs_p2 > zs_cut);
		*first2 = Timestamp<bool>{it->dt(), cand_pat::up_gap_two_crows(it, p2_large), ts_name};
		first2++; 
	}
}

