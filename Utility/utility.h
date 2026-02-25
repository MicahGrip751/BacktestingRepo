#pragma once
#include <armadillo>
#include <algorithm> 
#include <iostream>
#include <fstream> 
#include <vector>
#include <deque> 
#include <execution>
#include <numeric> 
#include <functional> 
#include <math.h> 
#include <thread>
#include <filesystem>
#include <stack> 
#include <stdio.h>
#include <pugixml.hpp> 
#include <boost/regex.hpp> 
#include "../Timestamp/Timestamp.h"
#include "../Candle/CandlePtr.h" 
#include "../Candle/Candle.h" 

namespace utility{
	/*
		Templated Methods	
	*/

	//T must be an integer like type which can be passed to next (returns the substring from std::next(s.begin(), indices.first)) to std::next(s.begin(), indices.second)
	template <typename T>
	std::string substr(const std::string& input, const std::pair<T, T>& indices);
	//check if two iterator ranges are the same size (throw if they are not)
	template <typename It1, typename It2> 
	void itr_size_check(It1 first1, It1 last1, It2 first2, It2 last2); 
	//compute the mean absolute difference between the values of two vectors of timestamps 
	template <typename InputIt, typename T> 
	void timestamp_mad(InputIt first1, InputIt last1, InputIt first2, T init); 
	//call .display() on the first k objects from a vector of Timestamps 
	template <typename T> 
	void timestamp_head(const std::vector<T>& tsv, int k = 12); 
	template <typename t> 
	void timestamp_tail(const std::vector<t>& tsv, int k = 12); 
	//percent change can be used for computing simple returns
	//return the percent change between two values (scale = true ==> value is truly a percentage (i.e. it is multiplied by 100). Not multiplied by 100 if scale = false) 
	template <typename T> 
	T percent_change(T new_val, T old_val, bool scale = true);
	//used if new & old value are different types T1, T2 (need to have - and / operators defined for them) (returns percent change cast as type T1)
	template <typename T1, typename T2> 
	T1 percent_change(T1 new_val, T2 old_val, bool scale = true);
	//percent_change overload which is used when percent changes can be undefined (i.e. the old value is zero) 
	template <typename T> 
	T percent_change(T new_val, T old_val, T undef, bool scale = true); 
	//used return the difference in k2 period percent change & k1 period percent change 
	template <typename InputIt, typename T> 
	T percent_change_diff(InputIt cit, int k1, int k2, T ex, bool scale = true);
	template <typename InputIt, typename UnaryOp, typename T> 
	T percent_change_diff(InputIt cit, int k1, int k2, UnaryOp un_op, T ex, bool scale = true);
	//used to return the sum of the k1 period and k2 period percent change 
	template <typename InputIt, typename T> 
	T percent_change_sum(InputIt cit, int k1, int k2, T ex, bool scale = true); 
	template <typename InputIt, typename UnaryOp, typename T> 
	T percent_change_sum(InputIt cit, int k1, int k2, UnaryOp un_op, T ex, bool scale = true); 

	//computes the log return (ln(new_val / old_val))
	template <typename T> 
	T log_return(T new_val, T old_val);
	template <typename T1, typename T2> 
	T1 log_return(T1 new_val, T2 old_val);
	
	//Converts a log return to a simple return (percentage specifies whether to multiply by 100)
	template <typename T> 
	T log_to_simple_return(T log_return, bool percentage = false); 
	//Converts a simple return to a log return (scaled specifies if the simple return is expressed as a percentage) 
	template <typename T> 
	T simple_to_log_return(T simple_return, bool percentage = false); 
	
	//Given a win return, loss return, win probability & loss probability compute the expected return 
	template <typename T> 
	T expected_return(T win_return, T loss_return, T win_prob, T loss_prob); 

	//given an iterator range containing values we compute the log returns for each successive pair & return the average log return 
	template <typename InputIt, typename T>
	T average_log_return(InputIt first, InputIt last, T ex); 
	//given an iterator range containing values we compute the simple return for each successive pair & return the average simple return 
	template <typename InputIt, typename T>
	T average_simple_return(InputIt first, InputIt last, T ex); 

	//function to compute the final value after a series of incremental percent changes 
	//if scale = true we assume the percentage returns in [first1, last1) are scaled by 100; 
	template <typename InputIt, typename T> 
	T accumulate_percent_changes(InputIt first1, InputIt last1, T start_value, bool scale = true); 
	template <typename InputIt, typename T> 
	T accumulate_log_returns(InputIt first1, InputIt last1, T start_value);

	//functions for computing the mean of [first, last) iterator range
	template <typename T, typename InputIt> 
	T mean(InputIt first, InputIt last, T init);
	//un_op is used in the operator passed to accumulate
	template <typename T, typename InputIt, typename UnaryOp> 
	T mean(InputIt first, InputIt last, T init, UnaryOp un_op); 
	//functions for computing the variance of [first, last) iterator range
	template <typename T, typename InputIt> 
	T var(InputIt first, InputIt last, T init);
	//sq_op is the operator which computes the squares 
	//s_op is the operator passed to mean 
	template <typename T, typename InputIt, typename SQOp, typename UnaryOp>
	T var(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op);
	//un_op is used in the operators passed to mean and transform_reduce
	template <typename T, typename InputIt, typename UnaryOp>
	T var(InputIt first, InputIt last, T init, UnaryOp un_op); 
	//these versions of var take a mean as the parameter m & uses the iterator range [last - k + 1, last]; use the variance shortcut formula 
	template <typename InputIt, typename T> 
	T var(InputIt last, T m, int k, T init); 
	template <typename InputIt, typename T, typename UnaryOp> 
	T var(InputIt last, T m, int k, T init, UnaryOp un_op);
	//overload of std which takes sum of variables, sum of squares, mean & k & uses the formula one step before the variance shortcut formula 
	template <typename T> 
	T var(const T& s, const T& ss, const T& m, int k);

	//function to return the mean and variance as a pair (useful in z_score calculation since the mean is calculated along the way of computing the variance)
	template <typename T, typename InputIt> 
	std::pair<T, T> mean_var(InputIt first, InputIt last, T init); 
	template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
	std::pair<T, T> mean_var(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op); 
	template <typename T, typename InputIt, typename UnaryOp> 
	std::pair<T, T> mean_var(InputIt first, InputIt last, T init, UnaryOp un_op); 

	//functions for computing the standard deviation 
	template <typename T, typename InputIt> 
	T std(InputIt first, InputIt last, T init); 
	template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
	T std(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op); 
	template <typename T, typename InputIt, typename UnaryOp>
	T std(InputIt first, InputIt last, T init, UnaryOp un_op);
	//overloads of std which take the mean as a parameter an use the iterator range [last - k + 1, last];
	template <typename InputIt, typename T> 
	T std(InputIt last, T m, int k, T init); 
	template <typename InputIt, typename T, typename UnaryOp> 
	T std(InputIt last, T m, int k, T init, UnaryOp un_op);
	//overload of std which takes sum of variables, sum of squares, mean & k 
	template <typename T> 
	T std(const T& s, const T& ss, const T& m, int k);

	//functions for computing the z-score
	//used if we know the mean, value and standard deviation 
	template <typename T> 
	T zscore(T value, T mean, T std);
	//used if we have to compute the mean and standard deviation
	template <typename T, typename InputIt> 
	T zscore(InputIt first, InputIt last, T value, T init);
	template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
	T zscore(InputIt first, InputIt last, T value, T init, SQOp sq_op, UnaryOp un_op);
	template <typename T, typename InputIt, typename UnaryOp> 
	T zscore(InputIt first, InputIt last, T value, T init, UnaryOp un_op);
	
	//mean absolute difference between the values in the iterator range [last - k + 1, last] and T val 
	template <typename InputIt, typename T> 
	T mean_abs_diff(InputIt last, const T& val, int k, T init); 
	template <typename InputIt, typename T, typename UnaryOp> 
	T mean_abs_diff(InputIt last, const T& val, int k, UnaryOp un_op, T init); 

	//weighted mean (W is the type of the weights)  
	template <typename T, typename InputIt, typename W>
	T wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, T init); 
	template <typename T, typename InputIt, typename W, typename UnaryOp>
	T wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, UnaryOp un_op, T init);
	//versions of wtd_mean which take in a weight sum 
	template <typename InputIt, typename W, typename T> 
	T wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, T w_sum, T init);
	template <typename InputIt, typename W, typename T, typename UnaryOp> 
	T wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, UnaryOp un_op, T w_sum, T init);
	//version of wtd_mean which takes two iterator ranges (un_op1 applied to [first1, last1) and likewise for un_op2)
	template <typename InputIt1, typename InputIt2, typename T, typename UnaryOp1, typename UnaryOp2>
	T wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, UnaryOp1 un_op1, UnaryOp2 un_op2, T init);

	
	//Note: These are not actually percentiles they just find compute min + p*(max - min)
	//which is the percentile for a uniform distribution but not for other distributions 
	
	//percentile (returns the pth percentile of the objects in [first, last)) 
	template <typename T, typename InputIt> 
	T percentile(InputIt first, InputIt last, int p, T ex);
	//percentile function with comparator comp and unary un_op
	//ex is an example of type T so the compiler can infer the type T
	template <typename T, typename InputIt, typename Comp, typename UnaryOp> 
	T percentile(InputIt first, InputIt last, int p, Comp comp, UnaryOp un_op, T ex); 
	//given a value and an iterator range [first, last) return which percentile the value is
	template <typename T, typename InputIt, typename In> 
	T percentile(InputIt first, InputIt last, In value, T ex);
	template <typename T, typename InputIt, typename In, typename Comp, typename UnaryOp> 
	T percentile(InputIt first, InputIt last, In value, Comp comp, UnaryOp un_op, T ex);
	//compute the percentile of the In value given the min and the max 
	template <typename T> 
	T percentile(T min, T max, T val); 
	
	//Note: The quartiles functions, for the same reason as the percentiles ones are not actually quartiles
	//return Q1, median & Q3
	template <typename InputIt, typename T>
	std::tuple<T, T, T> quartiles(InputIt first, InputIt last, T ex); 
	template<typename InputIt, typename T, typename Comp, typename UnaryOp> 
	std::tuple<T, T, T> quartiles(InputIt first, InputIt last, Comp comp, UnaryOp un_op, T ex); 

	//given two iterator ranges [first1, last1) and [first2, last2) we write pairs x /pm y where x and y are from each iterator range 
	template <typename InputIt1, typename InputIt2, typename OutputIt> 
	void plus_minus(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3);
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
	void plus_minus(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp1 un_op1, UnaryOp2 un_op2);
	//given value (& non scaled percentage) we return (1 - p)*value and (1 + p)*value in a pair
	template <typename T> 
	std::pair<T, T> plus_minus_percentage(T value, T p);
	template <typename T, typename In, typename UnaryOp> 
	std::pair<T, T> plus_minus_percentage(In value, T p, UnaryOp un_op);
	//given an iterator range [first, last) we compute values (1 /pm p)x where x is from the iterator range
	template <typename InputIt, typename OutputIt> 
	void plus_minus_percentage(InputIt first1, InputIt last1, OutputIt first2, double p);
	template <typename InputIt, typename OutputIt, typename UnaryOp> 
	void plus_minus_percentage(InputIt first1, InputIt last1, OutputIt first2, double p, UnaryOp un_op);

	//given two iterator ranges we compute the pairwise differences between them
	template <typename InputIt1, typename InputIt2, typename OutputIt> 
	void difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3);
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
	void difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp1 un_op1, UnaryOp2 un_op2);
	//lagged difference between two iterator ranges (kth ele of [first1, last1) - first2) and so on); 
	template <typename InputIt1, typename InputIt2, typename OutputIt> 
	void lagged_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k);
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
	void lagged_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp1 un_op1, UnaryOp2 un_op2);
	
	//Note: k is the number of periods back including the current period 
	//rolling simple returns (doesn't scale by 100) 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_simple_returns(InputIt first1, InputIt last1, OutputIt first2, int k, T ex);
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void roll_simple_returns(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex);
	//rolling log returns
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_log_returns(InputIt first1, InputIt last1, OutputIt first2, int k, T ex);
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void roll_log_returns(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex);

	//rolling update functions 
	template <typename T>
	void roll_mean_update(T& mean, const T& old_val, const T& new_val, int k);
	template <typename T>
	double roll_diff_mean_update(T& mean1, T& mean2, const T& old_val1, const T& old_val2, const T& new_val, int k1, int k2);
	template <typename T> 
	void roll_var_update(T& var, T& mean, const T& old_val, const T& new_val, int k); 
	//this version of roll_var_update takes in a sum of squares & a mean 
	template <typename T> 
	T roll_var_ss_update(T& ss, const T& m, const T& old_val, const T& new_val, int k);
	// takes a sum of square, a sum, and a mean (used for cases where the variance shortcut doesn't work (i.e. when we are not using a true mean))
	template <typename T> 
	T roll_var_s_ss_update(T& s, T& ss, const T& m, const T& old_val, const T& new_val, int k);
	template <typename T> 
	void roll_std_update(T& std, T& mean, const T& old_val, const T& new_val, int k); 
	template <typename T> 
	T roll_std_ss_update(T& ss, const T& m, const T& old_val, const T& new_val, int k); 
	//this version is used to update the std when the mean used isn't a true mean (formula used is one step prior to the final variance shortcut formula)
	//s is the sum of the variables to be updated & ss is the sum of the squares of the variables to be updated
	template <typename T> 
	T roll_std_s_ss_update(T& s, T& ss, const T& m, const T& old_val, const T& new_val, int k);
	template <typename T> 
	T roll_zscore_update(T& std, T& mean, const T& old_val, const T& new_val, int k); 
	template <typename InputIt, typename OutputIt> 
	void roll_max_update(InputIt first1, OutputIt out, std::deque<int>& dq_max, int i, int k); 
	template <typename InputIt, typename OutputIt> 
	void roll_min_update(InputIt first1, OutputIt out, std::deque<int>& dq_max, int i, int k); 
	//for roll_min update use Comp = true if arg1 >= arg2 for roll_max use comp = true if arg1 <= arg2  
	template <typename InputIt, typename OutputIt, typename Comp, typename UnaryOp>
	void roll_min_or_max_update(InputIt first1, OutputIt out, std::deque<int>& dq, int i, int k, Comp comp, UnaryOp un_op); 
	//rolling minmax update
	template <typename InputIt, typename OutputIt>
	void roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k);
	//comp1 is comparator used for min comp2 is comparator used for max
	template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2,typename UnaryOp>
	void roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op);
	//un_op1 is applied to the min elements & un_op2 is applied to the max elements 
	template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2> 
	void roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2); 
	template <typename T> 
	void ema_update(T& ema, const T& new_val, const T& alpha); 
	template <typename T> 
	T dema_update(T& ema_ema, T& ema, const T& new_val, const T& alpha1, const T& alpha2); 
	template <typename T> 
	T tema_update(T& ema_ema_ema, T& ema_ema, T& ema, const T& new_val, const T& alpha1, const T& alpha2, const T& alpha3);
	//exponential moving average difference update functions
	//alpha1 is the alpha for ema1 and alpha2 is the alpha for ema2
	template <typename T>
	T ema_diff_update(T& ema1, T& ema2, const T& new_val, const T& alpha1, const T& alpha2);
	//alpha11 and 12 are alphas for ema_ema1 and alpha21 & 22 are alphas for ema_ema2
	template <typename T> 
	T dema_diff_update(T& ema_ema1, T& ema_ema2, T& ema1, T& ema2, const T& new_val, const T& alpha11, const T& alpha12, const T& alpha21, const T& alpha22);
	//alpha11, 12 and 13 are alphas for ema_ema1 and alpha21, 22 and 23 are alphas for ema_ema_ema2
	template <typename T> 
	T tema_diff_update(T& ema_ema_ema1, T& ema_ema1, T& ema1, T& ema_ema_ema2, T& ema_ema2, T& ema2, const T& new_val, 
			const T& alpha11, const T& alpha12, const T& alpha13, const T& alpha21, const T& alpha22, const T& alpha23);

	//update functions for computing rolling quartiles (these call roll_minmax_update & then write the tuple of Q1, Q2, and Q3 to out) 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_quartiles_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, T ex);
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>
	void roll_quartiles_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex); 
	
	//update functions for rolling percentile 
	template <typename InputIt, typename OutputIt, typename T>
	void roll_percentile_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, T ex);
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>
	void roll_percentile_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex);
	

	//rolling mean
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_mean(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T>	
	void roll_mean(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init); 

	//rolling weighted mean  
	template <typename InputIt, typename OutputIt, typename W, typename T> 
	void roll_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init); 
	template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp>	
	void roll_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init, UnaryOp un_op); 
	//rolling mean of the pairwise products of the iterator ranges [first1, last1) and [first2, last2) output is written starting at first3
	//values in the second iterator range are used as weights in a weighted mean of k values
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T>
	void roll_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T init);
	//un_op1 is the unary applied to [first1, last1), un_op2 is the unary applied to [first2, last2) 
	//it is assumed un_op1 & un_op2 take elements and return type T 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2>
	void roll_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T init, UnaryOp1 un_op1, UnaryOp2 un_op2); 
	//setup functions for ema (they setup the parameters passed to ema_update) 
	template <typename InputIt, typename T> 
	void ema_setup(T& ema, InputIt first, int k, T init);
	template <typename InputIt, typename T, typename UnaryOp> 
	void ema_setup(T& ema, InputIt first, int k, T init, UnaryOp un_op);
	//setup functions for dema 
	template <typename InputIt, typename T> 
	void dema_setup(T& ema_ema, T& ema, InputIt first, int k, T alpha1, T init);
	template <typename InputIt, typename T, typename UnaryOp> 
	void dema_setup(T& ema_ema, T& ema, InputIt first, int k, T alpha1, T init, UnaryOp un_op);
	//setup functions for tema 
	template <typename InputIt, typename T> 
	void tema_setup(T& ema_ema_ema, T& ema_ema, T& ema, InputIt first, int k, T alpha1, T alpha2, T init); 
	template <typename InputIt, typename T, typename UnaryOp> 
	void tema_setup(T& ema_ema_ema, T& ema_ema, T& ema, InputIt first, int k, T alpha1, T alpha2, T init, UnaryOp un_op); 

	//exponential moving average k is the period alpha is the smoothing factor 
	template <typename InputIt, typename OutputIt, typename T>
	void ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init, UnaryOp un_op);
	
	//double exponential moving average (single pass)
	//alpha1 is the alpha used in computing ema alpha2 is the alpha used in computing ema(ema)
	template <typename InputIt, typename OutputIt, typename T> 
	void dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init, UnaryOp un_op); 

	//triple exponential moving average (single pass)
	//alpha1 is the alpha used in computing ema, alpha2 is the alpha used in computing ema(ema)
	//alpha3 is the alpha used in computing ema(ema(ema))
	template <typename InputIt, typename OutputIt, typename T> 
	void tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T alpha2, T alpha3, T init, UnaryOp un_op); 
	
	//roll mean of roll mean 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_meanmean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void roll_meanmean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, UnaryOp un_op); 
	//ema of ema 
	template <typename InputIt, typename OutputIt, typename T> 
	void eema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void eema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init, UnaryOp un_op);
	//dema of dema 
	template <typename InputIt, typename OutputIt, typename T> 
	void ddema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ddema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init, UnaryOp un_op);
	//tema of tema 
	template <typename InputIt, typename OutputIt, typename T> 
	void ttema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void ttema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init, UnaryOp un_op);
	
	//difference between two rolling percent changes 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_diff_percent_change(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T ex);
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
	void roll_diff_percent_change(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T ex);

	//rolling difference between two rolling means
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_diff_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename T>	
	void roll_diff_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, UnaryOp un_op); 
	
	//rolling difference between two weighted rolling means with fixed weight vectors
	//w1 is the weights vector of size k1 used to compute the k1 period weighted mean 
	//w2 is the weights vector of size k2 used to compute the k2 period weighted mean 
	template <typename InputIt, typename OutputIt, typename W,  typename T> 
	void roll_diff_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& w1, const std::vector<W>& w2, T init); 
	template <typename InputIt, typename OutputIt, typename UnaryOp, typename W, typename T>	
	void roll_diff_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& w1, const std::vector<W>& w2, T init, UnaryOp un_op); 

	//rolling difference bewtween two weighed rolling means where the weights come from a second iterator range [first2, last2);
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T> 
	void roll_diff_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init); 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2>	
	void roll_diff_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init, UnaryOp1 un_op1, UnaryOp2 un_op2); 

	//difference between two emas (larger k ema - smaller k ema)
	template <typename InputIt, typename OutputIt, typename T>
	void ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T init);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T init, UnaryOp un_op);
	
	//difference between two demas (larger k dema - smaller k dema)
	//alpha11 & alpha12 are the alphas used in the k1 period dema 
	//alpha21 & alpha22 are the alphas used in the k2 period dema
	template <typename InputIt, typename OutputIt, typename T>
	void dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha11, T alpha12, T alpha21, T alpha22, T init);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha11, T alpha12, T alpha21, T alpha22, T init, UnaryOp un_op);
	
	//difference between two temas (larger k tema - smaller k tema)
	//alpha11, alpha12, & alpha13 are the alphas used in the k1 period tema 
	//alpha21, alpha22, & alpha23 are the alphas used in the k2 period tema
	template <typename InputIt, typename OutputIt, typename T>
	void tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, 
			T alpha11, T alpha12, T alpha13, T alpha21, T alpha22, T alpha23, T init);
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
	void tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, 
			T alpha11, T alpha12, T alpha13, T alpha21, T alpha22, T alpha23, T init, UnaryOp un_op);

	//rolling variance 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_var(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
	void roll_var(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op); 

	//rolling standard deviation 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_std(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
	void roll_std(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op); 
	
	
	//rolling minimum 
	template <typename InputIt, typename OutputIt> 
	void roll_min(InputIt first1, InputIt last1, OutputIt first2, int k);
	//rolling maximum
	template <typename InputIt, typename OutputIt> 
	void roll_max(InputIt first1, InputIt last1, OutputIt first2, int k);
	//rolling min or max depending on comp (see comments on update funcion roll_min_or_max_update)
	template <typename InputIt, typename OutputIt, typename Comp, typename UnaryOp>	
	void roll_min_or_max(InputIt first1, InputIt last1, OutputIt first2, int k, Comp comp, UnaryOp un_op);

	//rolling minmax (first2 should be an iterator to a container containing pairs) 
	template <typename InputIt, typename OutputIt> 
	void roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k);
	//comp1 is true if arg 1 >= arg 2, comp2 is true if arg1 <= arg2
	template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp> 
	void roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op);
	//this version calls the roll_minmax_update overload which takes two unary operators
	template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2> 
	void roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2);

	//rolling percentile
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_percentile(InputIt first1, InputIt last1, OutputIt first2, int k, T ex); 
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>	
	void roll_percentile(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex);
	//rolling inter quartile range 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_iqr(InputIt first1, InputIt last1, OutputIt first2, int k, T ex); 
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>	
	void roll_iqr(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex);
		
	//rolling scalers 
	//rolling z-score 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_zscore(InputIt first1, InputIt last1, OutputIt first2, int k, T init); 
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
	void roll_zscore(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op); 

	//rolling min-max normalization 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef);
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
	void roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, Comp1 comp1, Comp2 comp2, UnaryOp un_op);

	//roll minmax norm which operates which uses un_op3 for the value, un_op1 for operating on the min according to comp1, & un_op2 for operating on the max according to comp_2
	//assumes [first1, last1 elements have .dt() methods & writes timestamps to range starting at first2)
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2, typename UnaryOp3> 
	void roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2, UnaryOp3 un_op3, const std::string& ts_name); 
	//rolling min-max norm which operates on a iterator range containing timestamps 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, const std::string& ts_name);

	//rolling mean normalization 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_mean_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T init, T undef); 
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
	void roll_mean_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T init, T undef, Comp1 comp1, Comp2 comp2, UnaryOp un_op);

	//rolling robust scaling 
	template <typename InputIt, typename OutputIt, typename T> 
	void roll_robust_scale(InputIt first1, InputIt last1, OutputIt first2, int k, T ex);
	template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
	void roll_robust_scale(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex);
	
	//function to fill a vector of timestamps from an iterator range given a starting iterator to an object which has a .dt() method
	//name is the name of the variable held in the timestamps
	template <typename DtStartIt, typename InputIt, typename OutputIt, typename T> 
	void timestamp_zip(InputIt first1, InputIt last1, DtStartIt dt_first, OutputIt first2, const std::string& name, T ex);
	//Timestamp zip which takes two iterator ranges & a binary operation used to make the timestamps 
	template <typename DtStartIt, typename InputIt1, typename InputIt2, typename OutputIt, typename BinOp, typename T> 
	void timestamp_zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, DtStartIt dt_first, OutputIt first3, BinOp bin_op, const std::string& name, T ex);
	//timestamp_zip which takes two iterator ranges & a binary operation used to make the timestamps
	//assumes [first1, last1) elements have a .dt method which accesses datetime used to make the timestamps 
	template <typename InputIt1, typename InputIt2, typename OutputIt, typename BinOp, typename T> 
	void timestamp_zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, BinOp bin_op, const std::string& name, T ex); 

	//function to make & write a bollinger band timestamp
	//itc is the iterator to the current object with a .dt() method, write is the iterator we write to
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void bb_timestamp(InputIt itc, OutputIt write, T m, T std, T nstd, const std::string& ts_name, UnaryOp un_op); 
	//function to make & write an rsi timestamp from mu & md (it is the iterator to the current object with a .dt() methods and write is the iterator we write to)
	//Note: checks to avert divide by zero (will occur is mu & md are both zero)
	template <typename InputIt, typename OutputIt, typename T> 
	void rsi_timestamp(InputIt it, OutputIt write, T mu, T md, const std::string& ts_name); 
	//function to make & write a cci timestamp (calls mean_abs_diff with k)
	template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
	void cci_timestamp(InputIt it, OutputIt write, T m, int k, T w, UnaryOp un_op, T init, const std::string& ts_name); 
	//methods for string processing
	//the type F can be a character or a string (it is passed to string.find())
	template <typename F> 
	void find_positions(const std::string& s, F f, std::vector<int>& positions);



	template <typename F> 
	std::string float_format(F f, int k); 
	template <typename I> 
	std::string integral_format(I i);
	//helper function for formatting a row
	template <typename... Types1, typename... Types2>
	void row_print(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, const std::vector<int>& col_widths, char col_sep = '|');  
	template <size_t... Is, typename... Types1, typename... Types2> 
	void row_print_impl(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, std::index_sequence<Is...>, const std::vector<int>& col_widths, char col_sep = '|');
	//print a line separating rows (corner character is displayed at each of the cell corners) 
	void row_separator_print(const std::vector<int>& col_widths, char row_sep = '-', char corner = '+'); 		
	//printing tables to the console 
	template <typename... Types1, typename... Types2, typename... Types3, typename... Types4>
	void table_print(const std::tuple<Types1...> headers, const std::tuple<Types2...> header_fcns, const std::vector<std::tuple<Types3...>>& rows, const std::tuple<Types4...> row_fcns,  
			const std::vector<int>& col_widths, char head_sep = '=', char row_sep = '-', char corner = '+', char col_sep = '|');

	//float_cell and integral_cell are useful is wrapped in a lambda & passed to table_latex in a tuple
	template <typename F> 
	std::string float_cell(F f, int k); 
	template <typename I> 
	std::string integral_cell(I i);
	//generating the latex for a row of a table
	//elements in fcn_tup should format a cell element to a string (if numeric enclose it in $$)
	template <typename... Types1, typename... Types2>
	std::string row_latex(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup);
	template <size_t... Is, typename... Types1, typename... Types2>
	std::string row_latex_impl(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, std::index_sequence<Is...> indices);  
	//generating the latex for tables  
	template <typename... Types1, typename... Types2, typename... Types3, typename... Types4> 
	std::string table_latex(const std::tuple<Types1...> headers, const std::tuple<Types2...> header_fcns, const std::vector<std::tuple<Types3...>>& rows, const std::tuple<Types4...> row_fcns,
			const std::string& table_alignment, const std::string& col_alignment);


	//S is any type which can be passed to std::filesystem::path
	template <typename S> 
	void ensure_exists(S path); 
	//S is any type which can be passed to FileStream's .open() method (throws an exception in the file was not opened)
	template <typename FileStream, typename S> 
	void fio_open(FileStream& fs, S path);
	//opens a file which will be written to (calls ensure_exists prior to opening the file)
	template <typename S> 
	void fout_open(std::ofstream& ofs, S path); 

	
	
	//given an iterator first we find the iterator in the range [it, last) whose .dt() method equals dt
	//uses ++ if forwards = true and -- if false ==> InputIt must be a bidirectional iterator which points to objects with a .dt() method
	template <typename InputIt> 
	InputIt find_iterator_to_dt(InputIt it, InputIt last, const Datetime& dt, bool forwards = true);  
	//assumes the objects pointed to by the iterators in the ItPairs have .dt() methods
	//assumes the second iterator in the pairs is an ending iterator
	//returns the starting and ending pairs which have the same start datetime and end datetime
	template <typename... ItPairs> 
	void sync_iterators(std::tuple<ItPairs...>& iterator_pairs);
	
	void sync_iterators(std::pair<std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>, 
			std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>>& csitp_tup); 
	void sync_iterators(std::pair<std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>, 
			std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>>& csitpp); 

	template <size_t... Is, typename... ItPairs> 
	void sync_iterators_impl(std::tuple<ItPairs...>& iterator_pairs, std::index_sequence<Is...> indices); 
	//generate an arma::Mat<T> from a collection of (first, last) iterators to timestamp objects (calls sync_iterators)
	//the functors in fcns should take elements from the corresponding iterator range & return type T
	//each iterator pair will correspond to a row in the matrix
	template <typename T, typename... ItPairs, typename... Fcns> 
	void generate_matrix(const std::tuple<ItPairs...>& iterator_pairs, std::tuple<Fcns...>& fcns, arma::Mat<T>& matrix); 	
	template <size_t... Is, typename T, typename... ItPairs, typename... Fcns> 
	void generate_matrix_impl(const std::tuple<ItPairs...>& iterator_pairs, std::tuple<Fcns...>& fcns, std::index_sequence<Is...> indices, arma::Mat<T>& matrix); 	
	//generate an arma::Row<L> from an iterator range containing [first, last) Timestamp<bool> objects 
	template <typename InputIt, typename L> 
	void generate_labels(const std::pair<InputIt, InputIt>& it_pair, arma::Row<L>& labels);
	//un_op should take an element from the range [first, last) and return type L
	template <typename InputIt, typename L, typename UnaryOp> 
	void generate_labels(const std::pair<InputIt, InputIt>& it_pair, arma::Row<L>& labels, UnaryOp un_op);
	//sync the iterators to be passed to generate_matrix and generate_labels 
	template <typename LabelItPair, typename... ItPairs> 
	void sync_matrix_labels_iterators(std::tuple<ItPairs...>& mat_it_pairs,  LabelItPair& lab_it_pair); 	
	
	template <typename... ItPairs> 
	void sync_matrix_cs_iterators(std::tuple<ItPairs...>& mat_it_pairs, std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>& csitp); 
	template <typename... ItPairs> 
	void sync_matrix_cs_iterators(std::tuple<ItPairs...>& mat_it_pairs, std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>& csitp); 

	template <typename T, typename It>
	void split_matrix(const arma::Mat<T>& mat, std::vector<arma::Mat<T>>& mat_splits, const std::pair<It, It> flc_its, std::vector<std::pair<It, It>>& flc_its_splits, int k); 
	template <typename T, typename It> 
	void split_matrix(const arma::Mat<T>& mat, std::vector<arma::Mat<T>>& mat_splits, const std::pair<It, It> unsynced_flc_its, const std::pair<It, It>& synced_flc_its, 
			std::vector<std::pair<It, It>>& flc_its_splits, int k); 
	
	template <std::size_t ...Is1, std::size_t ...Is2, typename T, typename ...Its> 
	void split_matrices(const std::vector<arma::Mat<T>>& mats, std::vector<std::vector<arma::Mat<T>>>& mats_splits, const std::tuple<std::pair<Its, Its>...>& flc_its, 
			std::vector<std::tuple<std::pair<Its, Its>...>>& flc_its_splits, std::index_sequence<Is1...> idx1, std::index_sequence<Is2...> idx2);


	//Convert an object which supports range based for loops to a string
	template <typename T> 
	void range_based_to_string(const T& input, std::string& out, char left = '(', char sep = ',', char right = ')');  
	//Tuple methods
	//tuple print method 
	template <typename TupleT> 
	void tuple_print(TupleT tup, char left = '(', char sep = ',', char right = ')');
	//convert a tuple to a string (like tuple_print but returns the string instead of printing)
	template <typename TupleT> 
	std::string tuple_string(TupleT tup, char left = '(', char sep = ',', char right = ')'); 
	//used for offsetting index sequences
	template <std::size_t N, std::size_t... Is> 
	std::index_sequence<(N + Is)...> offset(std::index_sequence<Is...>); 
	template <std::size_t Offset, std::size_t N> 
	auto make_index_sequence_with_offset(); 
	template <typename Tuple, std::size_t... Is> 
	auto tuple_select(Tuple tup, std::index_sequence<Is...>); 
	//separate the input tuple into two tuples (first k elements of input tuple are assigned to the first tuple and the remaining elements to the second)
	template <typename... Types1, typename... Types2, typename... Types3> 
	void tuple_split(const std::tuple<Types1...>& tup_in, std::tuple<Types2...>& tup_out1, std::tuple<Types3...>& tup_out2); 
	//return a tuple of the sub tuples of tup consisting of element sets {1} , {1, 2}, ... {1, 2, ...N} and so on
	template <typename ...Types> 
	auto sub_tuples_1_to_n(const std::tuple<Types...>& tup); 
	template <std::size_t... Is, typename Tuple> 
	auto sub_tuples_1_to_n_impl(Tuple tup, std::index_sequence<Is...> indices); 
	//Given a vector of tuples & a function which takes a tuple & returns a boolean we remove the tuples for which the function returns false 
	//useful for constructing hyperparameter tuples in which the parameters have restrictions (i.e. one hyperparameter must be less than the other etc.)
	template <typename ...Types> 
	std::vector<std::tuple<Types...>> validate_tuples(const std::vector<std::tuple<Types...>>& vec_of_tuples, std::function<bool (const std::tuple<Types...>&)> validator);
	template <typename ...Types, typename V> 
	std::vector<std::tuple<Types...>> validate_tuples(const std::vector<std::tuple<Types...>>& vec_of_tuples, V validator, int n);

	//only works for tuples which have positive numeric elements (sums the tuple elements & divides each element by the sum)
	template <typename ...Types> 
	void normalize_tuple(std::tuple<Types...>& tup); 
	//normalize the sub tuple elements with indices in the index sequence indices
	template <std::size_t ...Is, typename ...Types> 
	void normalize_sub_tuple(std::tuple<Types...>& tup, std::index_sequence<Is...>& indices);
	//parameter types in Types... must be convertible to T 
	template <typename T, typename ...Types> 
	void tuple_to_vector(const std::tuple<Types...>& tuple, std::vector<T>& out);
	template <typename T, size_t ...Is, typename ...Types> 
	void sub_tuple_to_vector(const std::tuple<Types...>& tuple, std::vector<T>& out, const std::index_sequence<Is...>& indices); 

	template <typename ...Types> 
	auto triple_first_elements(const std::tuple<std::tuple<Types, Types, Types>...>& tup); 
	template <std::size_t... Is, typename Tuple> 
	auto triple_first_elements_impl(Tuple tup, std::index_sequence<Is...> indices); 
	
	//elements in the output tuple are vectors of the types in the tuple
	template <typename ...Types> 
	std::tuple<std::vector<Types>...> vectors_from_tuples(const std::tuple<Types...>& tup); 
	//This version is used in discretization	
	template <typename ...Types> 
	std::tuple<std::vector<Types>...> vectors_from_tuple_of_triples(const std::tuple<std::tuple<Types, Types, Types>...>& tup); 

	//types in the PointTypes parameter pack should match corresponding types in the BoundDiscTup pack of tuples
	template <size_t... Is, typename ...PointTypes> 
	void discretization_helper(auto& tuple_of_vecs, const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...> indices); 
	//Methods for constructing grids to be used in the GridSearch hyperparameter tuner
	template <typename T>
	std::vector<T> discretization(const std::tuple<T, T, T>& bnd_disc_tup); 
	template <typename T1, typename T2> 
	std::vector<std::tuple<T1, T2>> discretization(const std::tuple<T1, T1, T1>& bd_tup1, const std::tuple<T2, T2, T2>& bd_tup2);
	template <typename ...PointTypes, typename T>
	std::vector<std::tuple<PointTypes..., T>> discretization(std::vector<std::tuple<PointTypes...>>& points, const std::tuple<T, T, T>& bound_disc_tuple); 
	//bound_disc_tuples should be tuples consisting of a lower bound, upperbound and discretization step.
	template <typename ...PointTypes> 
	std::vector<std::tuple<PointTypes...>> discretization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples);
	//randomly sample n points from a grid defined by bound_disc_tuples 
	template <typename ...PointTypes> 
	std::vector<std::tuple<PointTypes...>> discretization_sample(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, int n); 
	template <std::size_t ...Is, typename ...PointTypes> 
	std::vector<std::tuple<PointTypes...>> discretization_sample_impl(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...>& indices, int n); 
	

	//cartesian product (version where points1 and points2 are collections of 1 dimensional points)
	template <typename T1, typename T2> 
	std::vector<std::tuple<T1, T2>> cartesian_product(const std::vector<T1>& points1, const std::vector<T2>& points2); 
	//cartesian product (version where points2 consists of one dimensional points)
	template <typename ...Types1, typename T> 
	std::vector<std::tuple<Types1..., T>> cartesian_product(const std::vector<std::tuple<Types1...>>& points1, const std::vector<T>& points2); 
	//cartesian product (concatenates every tuple in points1 with every tuple in points2 and writes the output to the tuples in ouput)
	template <typename ...Types1, typename ...Types2> 
	std::vector<std::tuple<Types1..., Types2...>> cartesian_product(const std::vector<std::tuple<Types1...>>& points1, const std::vector<std::tuple<Types2...>>& points2); 
			
	//Methods for Genetic Hyperparameter Tuning
		
	//Initial Population of size n (calls discretization_sample)
	template <typename ...PointTypes> 
	std::vector<std::tuple<PointTypes...>> initialization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, int n);
	//this version of initialization calls normalization
	template <std::size_t ...Is, typename ...PointTypes> 
	std::vector<std::tuple<PointTypes...>> initialization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...>& indices, int n); 
	//Discrete Recombination where alleles are selected from parent 1 with probability p and from parent 2 with probability (1 - p)
	template <typename ...PointTypes> 
	std::tuple<PointTypes...> discrete_recombination(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double p);
	template <size_t ...Is, typename ...PointTypes> 
	std::tuple<PointTypes...> discrete_recombination_impl(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double p, std::index_sequence<Is...> indices); 
	//Intermediate Recombination
	template <typename ...PointTypes> 
	std::tuple<PointTypes...> intermediate_recombination(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double beta);
	template <size_t ...Is, typename ...PointTypes> 
	std::tuple<PointTypes...> intermediate_recombination_impl(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double beta, std::index_sequence<Is...> indices);
	//Select n pairs from k highest performing individuals in a generation (elements in generation are an individual and its score) 
	//fit_comp compares the scores in the generation vector (it is passed to sort & assumes that it sorts the scores so that after sorting the best individuals are at the front of the vector)
	//the double in the output tuples is the beta to be used in intermediate_recombination or the p to be used in discrete_recombination
	template <typename FitComp, typename ...PointTypes> 
	std::vector<std::tuple<std::tuple<PointTypes...>, std::tuple<PointTypes...>, double>> selection(
			std::vector<std::pair<std::tuple<PointTypes...>, double>>& generation, FitComp fit_comp, double scale = .75);
	//fit_fcn is a function which takes an individual and evaluates its fitness
	//n_threads is the number of threads to use when evaluating the fitness of each individual in the generation 
	template <typename FitFcn, typename ...PointTypes> 
	std::vector<std::pair<std::tuple<PointTypes...>, double>> generation_fitness(const std::vector<std::tuple<PointTypes...>>& generation, FitFcn fit_fcn, int n_threads = 12); 
	
	template <typename T> 
	struct vec_hash{
		std::size_t operator()(const std::vector<T>& v) const; 
	};
	//fixed duplicate columns of a matrix by adding perturbations to the duplicated columns (generates randu vector from -pert to pert)
	template <typename T> 
	void fix_duplicates(arma::Mat<T>& matrix, T pert); 

	/*
		Non-Templated Methods & Classes	
	*/
	//Compute and return the number of days between datetimes 
	int n_days_between(Datetime dt1, Datetime dt2);
	//convert a boolean to a True or False string 
	std::string to_string(bool bool_in); 	
	//Hash function for pairs of positive integers
	struct PairHash{
		std::size_t operator()(const std::pair<unsigned int, unsigned int>& p) const;
	}; 
	


	std::string command_output(const std::string& command); 

	//Curl Class for making simple curl requests to download webpages
	class Curl{
		public:
			Curl() = default; 
			//url, header are assumed to not be enclosed in quotes
			Curl(const std::string& url, const std::string& header); 
			Curl(const std::string& url, const std::string& header, const std::string& download_path);
			bool url_exists(const std::string& url); 
			void display_request();
			void send_request() const; 	
		private:
			const std::string header_ = "user-agent: Micah Grip micahjamesgrip@gmail.com"; 
			std::string request_ = ""; 
	};
	
	//Functions for generating and writing latex (surrounds body in a document and include statements)
	//first overload does not have a document title and the second does
	void generate_latex_doc(std::string& body);
	void generate_latex_doc(std::string& body, const std::string& title);
	void write_latex(const std::string& path, std::string& body); 	
	void write_latex(const std::string& path, std::string& body, const std::string& title); 	
	
	/*
		Sub-Namespace dedicated to parsing 
	*/
	namespace parse{
		//find the position of either the next or previous opening tag with tagname name starting at pos
		std::pair<int, int> open_tag(const std::string& input, const std::string& name, int pos, bool prev = false);
		//find the position of the next or previous open tag with anyname starting at pos
		std::pair<int, int> open_tag(const std::string& input, int pos, bool prev = false); 
		std::pair<int, int> closing_tag(const std::string& input, const std::string& name, int pos, bool prev = false); 
		std::pair<int, int> closing_tag(const std::string& input, int pos, bool prev = false); 
		std::pair<int, int> find_enclosing(const std::string& input, boost::regex e, const std::string& name);
		//find enclosing lazy differs from find enclosing in that it cannot handle nested tables & only tries to match the partial tags <name and name/>
		std::pair<int, int> find_enclosing_lazy(const std::string& input, boost::regex re, const std::string& name);
		std::pair<int, int> find_enclosed_after(const std::string& input, boost::regex re, const std::string& name); 
		//delete self closing tags (HTML or XML style) from an input 
		void remove_self_closing_tags(std::string& input);
		//delete styling tags from an input 
		void remove_styling_tags(std::string& input);
		//delete tags with a particular name
		void remove_tags(std::string& input, const std::string& name);
		void remove_attributes(std::string& input);
		void remove_empty_tds(std::string& input);
		void remove_non_table_tags(std::string& input);
		void fix_parentheses(std::string& input);
		//we assume that if the title is in the table it is not in a td element 
		void extract_balance_sheet_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers); 
		void extract_income_statement_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers);
		void extract_cash_flow_statement_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers);
	
		double find_currency_multiple(const std::string& input);
		double find_share_multiple(const std::string& input); 
		double convert_cell(const std::string& cell); 
		std::string format_column_header(const std::string& column_header); 
		std::string format_row_header(const std::string& row_header); 
		
		//finding a balance sheet table
		std::pair<int, int> find_balance_sheet(const std::string& input, std::pair<double, double>& multipilers, bool has_title); 
		//finding an income statement table 
		std::pair<int, int> find_income_statement(const std::string& input, std::pair<double, double>& multipilers, bool has_title); 
		//finding a cash flow statement 
		std::pair<int, int> find_cash_flow_statement(const std::string& input, std::pair<double, double>& multipliers, bool has_title);
		
		std::pair<int, int> table_dimensions(const pugi::xml_document& doc); 
		void construct_headers(const pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers); 

		//parse a table to an unordered_map of unordered_maps (first set of keys are the column headers & second set are the row labels)
		//assumes that the table has one header row and one header column and entries in the table are numberic (will be converted to doubles)
		void parse_table(const std::string& table_input, std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table);
		


		void clean_table(std::string& table_input); 
		void parse_table_data(const pugi::xml_document& doc, const std::string& first_col_header, const std::string& second_col_header, 
				std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table); 
		
		void parse_table_data(const pugi::xml_document& doc, const std::vector<std::string>& col_headers, std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table); 
	}

};
template <typename T>
std::string utility::substr(const std::string& input, const std::pair<T, T>& indices){
	return std::string(std::next(input.cbegin(), indices.first), std::next(input.cbegin(), indices.second)); 
}

template <typename It1, typename It2> 
void utility::itr_size_check(It1 first1, It1 last1, It2 first2, It2 last2){
	if(std::distance(first1, last1) != std::distance(first2, last2)){
		throw("itr_size_check: Iterator ranges [first1, last1) and [first2, last2) are not the same size."); 
	}
}
template <typename InputIt, typename T> 
void utility::timestamp_mad(InputIt first1, InputIt last1, InputIt first2, T init){
	T mad = init; 
	int i = 0; 
	for(auto it = first1; it != last1; it++){
		if(it->dt() != first2->dt()){
			throw std::runtime_error("utility::timestamp_mad : Mismatched datetimes."); 
		}
		if(it->val() != first2->val()){
			i++; 
			T ad = std::abs(it->val() - first2->val());
			mad += ad; 
		}
		first2++;
	}
	std::cout << "There are " << i << " Timestamps with unequal values." << std::endl; 
	std::cout << "The sum of the absolute differences between the values is : " << mad << std::endl; 
	std::cout << "The mean absolute difference between the values in the vectors of Timestamps is: " << mad / (std::distance(first1, last1) + 0.0) << std::endl; 
}
template <typename T> 
void utility::timestamp_head(const std::vector<T>& tsv, int k){
	auto it = tsv.cbegin(); 
	for(int i = 0; i < k; i++){
		it->display(); 
		it++; 
	}
}
template <typename t> 
void utility::timestamp_tail(const std::vector<t>& tsv, int k){
	auto it = std::prev(tsv.end(), k);
	for(int i = 0; i < k; i++){
		it->display(); 
		it++; 
	}
}

//percent change & log of percent change functions 
template <typename T> 
T utility::percent_change(T new_val, T old_val, bool scale){
	if(scale == false){
		return (new_val - old_val) / old_val; 
	}else{
		return 100.0 * (new_val - old_val) / old_val;
	}
}
template <typename T1, typename T2> 
T1 utility::percent_change(T1 new_val, T2 old_val, bool scale){
	if(scale == false){
		return (new_val - old_val) / old_val; 
	}else{
		return 100.0 * (new_val - old_val) / old_val;
	}
}
template <typename T> 
T utility::percent_change(T new_val, T old_val, T undef, bool scale){
	//use undefined value for any very small old_val (not just zero)
	if(std::abs(old_val) < (.5*std::pow(10, -9))){
		return undef; 
	}else{
		if(scale){
			return 100.0 * (new_val - old_val) / old_val; 
		}else{
			return (new_val - old_val) / old_val;
		}
	}
}
template <typename InputIt, typename T> 
T utility::percent_change_diff(InputIt cit, int k1, int k2, T ex, bool scale){
	return utility::percent_change(*cit, *std::prev(cit, k2 - 1), scale) - utility::percent_change(*cit, *std::prev(cit, k1 - 1), scale);  
}
template <typename InputIt, typename UnaryOp, typename T> 
T utility::percent_change_diff(InputIt cit, int k1, int k2, UnaryOp un_op, T ex, bool scale){
	return utility::percent_change(un_op(*cit), un_op(*std::prev(cit, k2 - 1), scale)) - 
		utility::percent_change(un_op(*cit), un_op(*std::prev(cit, k1 - 1)), scale);  
}
template <typename InputIt, typename T> 
T utility::percent_change_sum(InputIt cit, int k1, int k2, T ex, bool scale){
	return utility::percent_change(*cit, *std::prev(cit, k2 - 1), scale) + utility::percent_change(*cit, *std::prev(cit, k1 - 1), scale);  
}
template <typename InputIt, typename UnaryOp, typename T> 
T utility::percent_change_sum(InputIt cit, int k1, int k2, UnaryOp un_op, T ex, bool scale){
	return utility::percent_change(un_op(*cit), un_op(*std::prev(cit, k2 - 1)), scale) + 
		utility::percent_change(un_op(*cit), un_op(*std::prev(cit, k1 - 1)), scale);  
}

template <typename T> 
T utility::log_return(T new_val, T old_val){
	return std::log(new_val / old_val);
}
template <typename T1, typename T2> 
T1 utility::log_return(T1 new_val, T2 old_val){
	return std::log(new_val / old_val); 
}
//Converts a log return to a simple return 
template <typename T> 
T utility::log_to_simple_return(T log_return, bool percentage){
	if(percentage){
		return (std::exp(log_return) - 1) * 100;  
	}else{
		return (std::exp(log_return) - 1); 
	}
}
//Converts a simple return to a log return 
template <typename T> 
T utility::simple_to_log_return(T simple_return, bool percentage){
	if(percentage){
		return std::log((simple_return / 100) + 1.0); 
	}else{
		return std::log(simple_return + 1.0); 
	}
}
//compute the expected return
template <typename T> 
T utility::expected_return(T win_return, T loss_return, T win_prob, T loss_prob){
	if((win_prob <= 0 || win_prob >= 1) || (loss_prob <= 0 || loss_prob >= 1)){
		throw std::invalid_argument("expected_return: win_prob and loss_prob must be in [0, 1]."); 
	}
	return ((win_prob * win_return) - (loss_prob * loss_return)); 
}

//given an iterator range containing values we compute the log returns for each successive pair & return the average log return 
template <typename InputIt, typename T>
T utility::average_log_return(InputIt first, InputIt last, T ex){
	T alr = 0.0;
	for(auto it = std::next(first); it != last; it++){
		alr += std::log(*it / *std::prev(it)); 
	}
	return alr / (std::distance(first, last) - 1); 
}
//given an iterator range containing values we compute the simple return for each successive pair & return the average simple return 
template <typename InputIt, typename T>
T utility::average_simple_return(InputIt first, InputIt last, T ex){
	T alr = 1.0;
	for(auto it = std::next(first); it != last; it++){
		alr *= (1.0 + percent_change(*it, *std::prev(it), false)); 
	}
	return std::pow(alr, 1.0 / (std::distance(first, last) - 1)) - 1.0; 
}

template <typename InputIt, typename T> 
T utility::accumulate_percent_changes(InputIt first1, InputIt last1, T start_value, bool scale){
	auto acc = [scale](T val, const auto& x){
		if(scale){
			//convert scaled percentage to the decimal equivalent & multiply
			return val*(1 + (x / 100)); 
		}else{
			return val*(1 + x); 
		}
	};
	//get the decimal equivalent of the total percentage change
	T tpc = std::accumulate(first1, last1, 1.0, acc);
	//compute & return the final value
	return start_value * tpc; 
}
template <typename InputIt, typename T> 
T utility::accumulate_log_returns(InputIt first1, InputIt last1, T start_value){
	//sum the log returns to get the total log return 
	T lrs = std::accumulate(first1, last1, 0.0); 
	//compute & return the final value
	return start_value * std::exp(lrs);  
}

//computing the mean
template <typename T, typename InputIt> 
T utility::mean(InputIt first, InputIt last, T init){	
	return (1.0 / std::distance(first, last)) * std::accumulate(first, last, init); 
}
template <typename T, typename InputIt, typename UnaryOp> 
T utility::mean(InputIt first, InputIt last, T init, UnaryOp un_op){
	auto s_op = [&un_op](T val, const auto& x){
		return un_op(x) + val; 
	}; 
	return (1.0 / std::distance(first, last)) * std::accumulate(first, last, init, s_op); 
}
//computing the variance (we use the variance shortcut formula becuase it saves std::distance(first, last) operations)

template <typename T, typename InputIt> 
T utility::var(InputIt first, InputIt last, T init){
	//compute the sum of the squares of each element in range [first, last)
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), std::multiplies<>()); 
	T m = mean(first, last, init);
	int n = std::distance(first, last); 	
	//E[X^2] - E[X}^2
	return (ss - (n * std::pow(m, 2)) ) / (n - 1);  
}

template <typename T, typename InputIt, typename SQOp, typename UnaryOp>
T utility::var(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op){ 
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), sq_op);
	T m = mean(first, last, init, un_op);
	int n = std::distance(first, last); 
	return (ss - (n * std::pow(m, 2)) ) / (n - 1);  
}
template <typename T, typename InputIt, typename UnaryOp>
T utility::var(InputIt first, InputIt last, T init, UnaryOp un_op){
	auto m_op = [&un_op](const auto& x1, const auto& x2){
		return un_op(x1) * un_op(x2); 
	};
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), m_op);
	T m = mean(first, last, init, un_op);
	int n = std::distance(first, last); 
	return (ss - (n * std::pow(m, 2)) ) / (n - 1);  
}
template <typename InputIt, typename T> 
T utility::var(InputIt last, T m, int k, T init){
	T ss = std::transform_reduce(std::prev(last, k - 1), std::next(last), std::prev(last, k - 1), init, std::plus<>(), std::multiplies<>()); 
	//E[X^2] - E[X}^2
	return (ss - (k * std::pow(m, 2)) ) / (k - 1);  
}
template <typename InputIt, typename T, typename UnaryOp> 
T utility::var(InputIt last, T m, int k, T init, UnaryOp un_op){
	auto m_op = [&un_op](const auto& x1, const auto& x2){
		return un_op(x1) * un_op(x2); 
	};
	T ss = std::transform_reduce(std::prev(last, k - 1), std::next(last), std::prev(last, k - 1), init, std::plus<>(), m_op); 
	return (ss - (k * std::pow(m, 2)) ) / (k - 1);  
}
template <typename T> 
T utility::var(const T& s, const T& ss, const T& m, int k){
	return (ss - (2*m*s) + (k * std::pow(m, 2))) / (k - 1);  
}

//computing the mean and the variance as a pair 
template <typename T, typename InputIt> 
std::pair<T, T> utility::mean_var(InputIt first, InputIt last, T init){
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), std::multiplies<>()); 
	T m = mean(first, last, init);
	int n = std::distance(first, last); 	
	//E[X^2] - E[X}^2
	return std::make_pair(m, (ss - (n * std::pow(m, 2)) ) / (n - 1));  
}
template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
std::pair<T, T> utility::mean_var(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op){
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), sq_op);
	T m = mean(first, last, init, un_op);
	int n = std::distance(first, last); 
	return std::make_pair(m, (ss - (n * std::pow(m, 2)) ) / (n - 1));  
}
template <typename T, typename InputIt, typename UnaryOp> 
std::pair<T, T> utility::mean_var(InputIt first, InputIt last, T init, UnaryOp un_op){
	auto m_op = [&un_op](const auto& x1, const auto& x2){
		return un_op(x1) * un_op(x2); 
	};
	T ss = std::transform_reduce(first, last, first, init, std::plus<>(), m_op);
	T m = mean(first, last, init, un_op);
	int n = std::distance(first, last); 
	return std::make_pair(m, (ss - (n * std::pow(m, 2)) ) / (n - 1));  
}

//computing the standard deviation (we just compute the variance & take a square root)
template <typename T, typename InputIt> 
T utility::std(InputIt first, InputIt last, T init){
	//return the standard deviation
	return std::sqrt(var(first, last, init)); 
}
template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
T utility::std(InputIt first, InputIt last, T init, SQOp sq_op, UnaryOp un_op){
	//return the standard deviation
	return std::sqrt(var(first, last, init, sq_op, un_op)); 
}
template <typename T, typename InputIt, typename UnaryOp>
T utility::std(InputIt first, InputIt last, T init, UnaryOp un_op){
	//return the standard deviation
	return std::sqrt(var(first, last, un_op)); 
}
//overloads of std which take the mean as a parameter an use the iterator range [last - k + 1, last];
template <typename InputIt, typename T> 
T utility::std(InputIt last, T m, int k, T init){
	return std::sqrt(var(last, m, k, init)); 
}
template <typename InputIt, typename T, typename UnaryOp> 
T utility::std(InputIt last, T m, int k, T init, UnaryOp un_op){
	return std::sqrt(var(last, m, k, init, un_op)); 
}
template <typename T> 
T utility::std(const T& s, const T& ss, const T& m, int k){
	return std::sqrt(var(s, ss, m, k)); 	
}

//computing the z-score
//In cases where the standard deviation is zero we return 0 for the zscore since the only way for this to occur is if all the values are equal to the mean
template <typename T> 
T utility::zscore(T value, T mean, T std){
	if(std != 0){
		//if the zscore is defined return it
		return (value - mean) / std; 
	}else{
		return 0.0; 
	}
}
template <typename T, typename InputIt> 
T utility::zscore(InputIt first, InputIt last, T value, T init){
	//compute the mean and the variance
	auto mv = mean_var(first, last, init);
	if(mv.second != 0){
		//if the zscore is defined return it
		return (value - mv.first) / std::sqrt(mv.second); 
	}else{
		return 0.0; 
	}
}
template <typename T, typename InputIt, typename SQOp, typename UnaryOp> 
T utility::zscore(InputIt first, InputIt last, T value, T init, SQOp sq_op, UnaryOp un_op){
	//compute the mean and the variance
	auto mv = mean_var(first, last, init, sq_op, un_op);  
	if(mv.second != 0){
		//if the zscore is defined return it
		return (value - mv.first) / std::sqrt(mv.second); 
	}else{
		return 0.0; 
	}
}
template <typename T, typename InputIt, typename UnaryOp> 
T utility::zscore(InputIt first, InputIt last, T value, T init, UnaryOp un_op){
	//compute the mean and the variance
	auto mv = mean_var(first, last, init, un_op); 
	if(mv.second != 0){
		//if the zscore is defined return it
		return (value - mv.first) / std::sqrt(mv.second); 
	}else{
		return 0.0; 
	}
}
template <typename InputIt, typename T> 
T utility::mean_abs_diff(InputIt last, const T& val, int k, T init){
	auto mad = [&val](const auto& x){
		return std::abs(val - x); 
	};
	return utility::mean(std::prev(last, k - 1), std::next(last), init, mad); 
}
template <typename InputIt, typename T, typename UnaryOp> 
T utility::mean_abs_diff(InputIt last, const T& val, int k, UnaryOp un_op, T init){
	auto mad = [&un_op, &val](const auto& x){
		return std::abs(val - un_op(x)); 
	};
	return utility::mean(std::prev(last, k - 1), std::next(last), init, mad); 
}

template <typename T>
void utility::roll_mean_update(T& mean, const T& old_val, const T& new_val, int k){
	mean = (((mean * k) - old_val) + new_val) / k; 	
}

template <typename T> 
double utility::roll_diff_mean_update(T& mean1, T& mean2, const T& old_val1, const T& old_val2, const T& new_val, int k1, int k2){
	roll_mean_update(mean1, old_val1, new_val, k1); 
	roll_mean_update(mean2, old_val2, new_val, k2);
	return mean2 - mean1;  
}

template <typename T, typename InputIt, typename W> 
T utility::wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, T init){
	int n = weights.size();
	//sum the weights 
	auto comp_ws = [](T val, const auto& w){
		return val + w; 	
	}; 
	T w_sum = std::accumulate(weights.cbegin(), weights.cend(), init, comp_ws); 
	if(std::distance(first, last) != n){
		throw std::invalid_argument("utility::wtd_mean : weights must be the same size as the iterator range [first, last)"); 
	}

	return (1 / w_sum) * std::inner_product(first, last, weights.cbegin(), init); 
}
		
template <typename T, typename InputIt, typename W, typename UnaryOp>
T utility::wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, UnaryOp un_op, T init){
	int n = weights.size(); 
	if(std::distance(first, last) != n){
		throw std::invalid_argument("utility::wtd_mean : weights must be the same size as the iterator range [first, last)"); 
	}
	auto comp_ws = [](T val, const auto& w){
		return val + w; 	
	}; 
	T w_sum = std::accumulate(weights.cbegin(), weights.cend(), init, comp_ws); 
	auto mult_op = [&un_op](const auto& x1, const W w){
		return un_op(x1) * w; 
	}; 
	return (1 / w_sum) * std::inner_product(first, last, weights.cbegin(), init, std::plus<>(), mult_op); 
}
//Note: These do not perform size checks on the weights & iterator range [first, last) 
template <typename InputIt, typename W, typename T> 
T utility::wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, T w_sum, T init){
	return (1 / w_sum) * std::inner_product(first, last, weights.cbegin(), init, std::plus<>(), std::multiplies<>()); 
}
template <typename InputIt, typename W, typename T, typename UnaryOp> 
T utility::wtd_mean(InputIt first, InputIt last, const std::vector<W>& weights, UnaryOp un_op, T w_sum, T init){
	auto mult_op = [&un_op](const auto& x1, W w){
		return un_op(x1) * w; 
	}; 
	return (1 / w_sum) * std::inner_product(first, last, weights.cbegin(), init, std::plus<>(), mult_op); 
}
//the weights are taken from the iterator range beginning with first2
template <typename InputIt1, typename InputIt2, typename T, typename UnaryOp1, typename UnaryOp2>
T utility::wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, UnaryOp1 un_op1, UnaryOp2 un_op2, T init){
	int k = std::distance(first1, last1); 
	auto comp_ws = [&un_op2](T val, const auto& y){
		return val + un_op2(y); 
	};
	//compute the weight sum 
	T w_sum = std::accumulate(first2, std::next(first2, k), init, comp_ws); 
	auto mult_op = [&un_op1, &un_op2](const auto& x, const auto& y){
		return un_op1(x) * un_op2(y); 
	};
	return (1 / w_sum) * std::inner_product(first1, last1, first2, init, std::plus<>(), mult_op); 
}


//functions to return the value of the pth percentile
template <typename T, typename InputIt> 
T utility::percentile(InputIt first, InputIt last, int p, T ex){
	if(p < 0 || p > 100){
		throw std::invalid_argument("utility::percentile : p must be an integer in [0, 100]"); 
	}
	auto min_max = std::minmax_element(first, last); 
	return *(min_max.first) + (p * (*(min_max.second) - *(min_max.first)) / 100.0); 
}
template <typename T, typename InputIt, typename Comp, typename UnaryOp> 
T utility::percentile(InputIt first, InputIt last, int p, Comp comp, UnaryOp un_op, T ex){
	if(p < 0 || p > 100){
		throw std::invalid_argument("utility::percentile : p must be an integer in [0, 100]"); 
	}
	auto min_max = std::minmax_element(first, last, comp);
	return un_op(*(min_max.first)) + (p * (un_op(*(min_max.second)) - un_op(*(min_max.first))) / 100.0); 
}
//compute the percentile of a given value 
template <typename T, typename InputIt, typename In> 
T utility::percentile(InputIt first, InputIt last, In value, T ex){
	//compute the min and the max 
	auto min_max = std::minmax_element(first, last);
	//compute and return what percentile value is
	if(*(min_max.second) != *(min_max.first)){
		return 100.0 * ((value - min_max.first) / (*(min_max.second) - *(min_max.first))); 
	}else{
		//if the min = max just say we are the 50th percentile
		return 50.0; 
	}
}
template <typename T, typename InputIt, typename In, typename Comp, typename UnaryOp> 
T utility::percentile(InputIt first, InputIt last, In value, Comp comp, UnaryOp un_op, T ex){
	//compute the min and the max using the comparator comp 
	auto min_max = std::minmax_element(first, last, comp);
	if(*(min_max.second) != *(min_max.first)){
		return 100 * ((un_op(value) - un_op(*min_max.first)) / (un_op(*(min_max.second)) - un_op(*(min_max.first)))); 
	}else{
		return 50.0; 
	}
}
template <typename T> 
T utility::percentile(T min, T max, T val){
	if(max != min){
		return 100.0 * ((val - min) / (max - min)); 
	}else{
		return 50.0; 
	}
}
//return x +- y where x, y are corresponding elements from iterator ranges
template <typename InputIt1, typename InputIt2, typename OutputIt> 
void utility::plus_minus(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3){
	//checking the ranges are the same size
	if(std::distance(first1, last1) != std::distance(first2, last2)){
		throw std::invalid_argument("utility::plus_minus: Iterator ranges [first1, last1) and [first2, last2) are not the same size."); 
	}
	auto it2 = first2;  
	for(auto it1 = first1; it1 != last1; it1++){
		*first3 = std::make_pair(*it1 - *it2, *it1 + *it2);
		it2++;
		first3++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
void utility::plus_minus(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp1 un_op1, UnaryOp2 un_op2){
	//checking the ranges are the same size
	if(std::distance(first1, last1) != std::distance(first2, last2)){
		throw std::invalid_argument("utility::plus_minus: Iterator ranges [first1, last1) and [first2, last2) are not the same size."); 
	}
	auto it2 = first2;  
	for(auto it1 = first1; it1 != last1; it1++){
		*first3 = std::make_pair(un_op1(*it1) - un_op2(*it2), un_op1(*it1) + un_op2(*it2));
		it2++;
		first3++; 
	}
}

template <typename T> 
std::pair<T, T> utility::plus_minus_percentage(T value, T p){
	return std::make_pair((1 - p)*value, (1 + p)*value); 
}
template <typename T, typename In, typename UnaryOp> 
std::pair<T, T> utility::plus_minus_percentage(In value, T p, UnaryOp un_op){
	return std::make_pair((1 - p)*un_op(value), (1 + p)*un_op(value)); 
}
//given an iterator range [first, last) we compute values (1 /pm p)x where x is from the iterator range
template <typename InputIt, typename OutputIt> 
void utility::plus_minus_percentage(InputIt first1, InputIt last1, OutputIt first2, double p){
	for(auto it = first1; it != last1; it++){
		*first2 = plus_minus_percentage(*it, p);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp> 
void utility::plus_minus_percentage(InputIt first1, InputIt last1, OutputIt first2, double p, UnaryOp un_op){
	for(auto it = first1; it != last1; it++){
		*first2 = plus_minus_percentage(*it, p, un_op);
		first2++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt> 
void utility::difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3){
	itr_size_check(first1, last1, first2, last2);
	std::transform(first1, last1, first2, first3, std::minus<>()); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
void utility::difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, UnaryOp1 un_op1, UnaryOp2 un_op2){
	itr_size_check(first1, last1, first2, last2);
	auto bin_op = [&un_op1, &un_op2](const auto& x, const auto& y){
		return un_op1(x) - un_op2(y); 	
	}; 
	std::transform(first1, last1, first2, first3, bin_op); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt> 
void utility::lagged_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k){
	difference(std::next(first1, k - 1), last1, first2, std::prev(last2, k - 1), first3); 	
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOp1, typename UnaryOp2> 
void utility::lagged_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, UnaryOp1 un_op1, UnaryOp2 un_op2){
	difference(std::next(first1, k - 1), last1, first2, std::prev(last2, k - 1), first3, un_op1, un_op2); 	
}

template <typename InputIt, typename T>
std::tuple<T, T, T> utility::quartiles(InputIt first, InputIt last, T ex){
	auto min_max = std::minmax_element(first, last); 
	T q1 = *(min_max.first) + (25 * (*(min_max.second) - *(min_max.first)) / 100.0); 
	T q2 = *(min_max.first) + (50 * (*(min_max.second) - *(min_max.first)) / 100.0); 
	T q3 = *(min_max.first) + (75 * (*(min_max.second) - *(min_max.first)) / 100.0);
	return std::tuple<T, T, T>{q1, q2, q3}; 
}
template<typename InputIt, typename T, typename Comp, typename UnaryOp> 
std::tuple<T, T, T> utility::quartiles(InputIt first, InputIt last, Comp comp, UnaryOp un_op, T ex){
	auto min_max = std::minmax_element(first, last, comp); 
	T q1 = un_op(*(min_max.first)) + (25 * (un_op(*(min_max.second)) - un_op(*(min_max.first))) / 100.0); 
	T q2 = un_op(*(min_max.first)) + (50 * (un_op(*(min_max.second)) - un_op(*(min_max.first))) / 100.0); 
	T q3 = un_op(*(min_max.first)) + (75 * (un_op(*(min_max.second)) - un_op(*(min_max.first))) / 100.0);
	return std::tuple<T, T, T>{q1, q2, q3}; 
}
//note: k is the number of periods back inclusive of the current period
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_simple_returns(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = percent_change(*it, *std::prev(it, k - 1), false);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void utility::roll_simple_returns(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex){
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = percent_change(un_op(*it), un_op(*std::prev(it, k - 1)), false);
		first2++; 
	}
}
//rolling log returns
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_log_returns(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = log_return(*it, *std::prev(it, k - 1));
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void utility::roll_log_returns(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T ex){
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = log_return(un_op(*it), un_op(*std::prev(it, k - 1)));
		first2++; 
	}
}

template <typename T> 
void utility::roll_var_update(T& var, T& mean, const T& old_val, const T& new_val, int k){
	//copy the old mean
	T old_mean = mean;
	//compute the updated mean 
	roll_mean_update(mean, old_val, new_val, k);
	var = (var + ((new_val - old_val)*(new_val - mean + old_val - old_mean)/(k - 1))); 
	//var = (((var*(k - 1)) + (new_val - old_val)*(new_val - mean + old_val - old_mean))/(k - 1)); 
}
template <typename T> 
T utility::roll_var_ss_update(T& ss, const T& m, const T& old_val, const T& new_val, int k){
	//update the sum of squares 
	ss = (ss - std::pow(old_val, 2) + std::pow(new_val, 2));
	//return the variance
	return (ss - (k * std::pow(m, 2)) ) / (k - 1.0);  
}
template <typename T> 
T utility::roll_var_s_ss_update(T& s, T& ss, const T& m, const T& old_val, const T& new_val, int k){
	//update sum of the variables 
	s = (s - old_val + new_val); 	
	//update the sum of the squares of the variables 
	ss = (ss - std::pow(old_val, 2) + std::pow(new_val, 2));
	//return the variance 
	return std::sqrt(std::abs(ss - (2*m*s) + (k * std::pow(m, 2))) / (k - 1));  
}
template <typename T> 
void utility::roll_std_update(T& std, T& mean, const T& old_val, const T& new_val, int k){
	//square the standard deviation to get the variance 
	T var = std::pow(std, 2);
	//update the variance 
	roll_var_update(var, mean, old_val, new_val, k);
	//take the square root of the variance
	if(var < 0){
		//not exactly sure why we hit this case but we do
		std = 0.0; 	
	}else{
		std = std::sqrt(var);  
	}
}
template <typename T> 
T utility::roll_std_ss_update(T& ss, const T& m, const T& old_val, const T& new_val, int k){
	//update the sum of squares 
	ss = (ss - std::pow(old_val, 2) + std::pow(new_val, 2));
	//assumes we have a true mean
	return std::sqrt((ss - (k * std::pow(m, 2))) / (k - 1));  
}
template <typename T> 
T utility::roll_std_s_ss_update(T& s, T& ss, const T& m, const T& old_val, const T& new_val, int k){
	//update sum of the variables 
	s = (s - old_val + new_val); 	
	//update the sum of the squares of the variables 
	ss = (ss - std::pow(old_val, 2) + std::pow(new_val, 2));
	//return the std (since we may not be using a true mean take the absolute value to ensure we don't get a negative in the square root)
	return std::sqrt(std::abs(ss - (2*m*s) + (k * std::pow(m, 2))) / (k - 1));  
}

template <typename T> 
T utility::roll_zscore_update(T& std, T& mean, const T& old_val, const T& new_val, int k){
	//update the mean and the standard deviation
	roll_std_update(std, mean, old_val, new_val, k);
	//return the z-score
	return (new_val - mean) / std; 
}
template <typename InputIt, typename OutputIt>
void utility::roll_max_update(InputIt first1, OutputIt out, std::deque<int>& dq_max, int i, int k){
	while(!dq_max.empty() && dq_max.front() <= i - k){
		dq_max.pop_front(); 
	}
	//pop the indices whose values are <= the current value
	while(!dq_max.empty() && *std::next(first1, dq_max.back()) <= *std::next(first1, i)){
		dq_max.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq_max.push_back(i);
	if(i >= k - 1){
		//assign the max
		*out = *std::next(first1, dq_max.front());
	}
}
template <typename InputIt, typename OutputIt> 
void utility::roll_min_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, int i, int k){
	while(!dq_min.empty() && dq_min.front() <= i - k){
		dq_min.pop_front(); 
	}
	//pop the indices whose values are >= the current value
	while(!dq_min.empty() && *std::next(first1, dq_min.back()) >= *std::next(first1, i)){
		dq_min.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq_min.push_back(i);
	if(i >= k - 1){
		//assign the max
		*out = *std::next(first1, dq_min.front());
	}
}
template <typename InputIt, typename OutputIt, typename Comp, typename UnaryOp>
void utility::roll_min_or_max_update(InputIt first1, OutputIt out, std::deque<int>& dq, int i, int k, Comp comp, UnaryOp un_op){
	while(!dq.empty() && dq.front() <= i - k){
		dq.pop_front(); 
	}
	//pop the indices whose values are <= the current value
	while(!dq.empty() && comp(*std::next(first1, dq.back()), *std::next(first1, i))){
		dq.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq.push_back(i);
	if(i >= k - 1){
		//assign the max
		*out = un_op(*std::next(first1, dq.front()));
	}
}
template <typename InputIt, typename OutputIt>
void utility::roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k){
	while(!dq_min.empty() && dq_min.front() <= i - k){
		dq_min.pop_front(); 
	}
	while(!dq_max.empty() && dq_max.front() <= i - k){
		dq_max.pop_front(); 
	}
	//pop the indices whose values are >= the current value
	while(!dq_min.empty() && *std::next(first1, dq_min.back()) >= *std::next(first1, i)){
		dq_min.pop_back(); 
	}
	//pop the indices whose values are <= the current value
	while(!dq_max.empty() && *std::next(first1, dq_max.back()) <= *std::next(first1, i)){
		dq_max.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq_min.push_back(i);
	dq_max.push_back(i);

	if(i >= k - 1){
		//assign the max
		*out = std::make_pair(*std::next(first1, dq_min.front()), *std::next(first1, dq_max.front()));
	}
}
template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp>
void utility::roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op){
	while(!dq_min.empty() && dq_min.front() <= i - k){
		dq_min.pop_front(); 
	}
	while(!dq_max.empty() && dq_max.front() <= i - k){
		dq_max.pop_front(); 
	}
	//pop the indices whose values are >= the current value
	while(!dq_min.empty() && comp1(*std::next(first1, dq_min.back()),  *std::next(first1, i))){
		dq_min.pop_back(); 
	}
	//pop the indices whose values are <= the current value
	while(!dq_max.empty() && comp2(*std::next(first1, dq_max.back()), *std::next(first1, i))){
		dq_max.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq_min.push_back(i);
	dq_max.push_back(i);

	if(i >= k - 1){
		//assign the max
		*out = std::make_pair(un_op(*std::next(first1, dq_min.front())), un_op(*std::next(first1, dq_max.front())));
	}
}
template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2> 
void utility::roll_minmax_update(InputIt first1, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2){
	while(!dq_min.empty() && dq_min.front() <= i - k){
		dq_min.pop_front(); 
	}
	while(!dq_max.empty() && dq_max.front() <= i - k){
		dq_max.pop_front(); 
	}
	//pop the indices whose values are >= the current value
	while(!dq_min.empty() && comp1(*std::next(first1, dq_min.back()),  *std::next(first1, i))){
		dq_min.pop_back(); 
	}
	//pop the indices whose values are <= the current value
	while(!dq_max.empty() && comp2(*std::next(first1, dq_max.back()), *std::next(first1, i))){
		dq_max.pop_back(); 
	}
	//push the current index onto the back of the dq
	dq_min.push_back(i);
	dq_max.push_back(i);

	if(i >= k - 1){
		//assign the max
		*out = std::make_pair(un_op1(*std::next(first1, dq_min.front())), un_op2(*std::next(first1, dq_max.front())));
	}
}

//exponential moving average update functions
template <typename T> 
void utility::ema_update(T& ema, const T& new_val, const T& alpha){
	ema = (alpha * new_val) + ((1 - alpha) * ema); 
}
template <typename T> 
T utility::dema_update(T& ema_ema, T& ema, const T& new_val, const T& alpha1, const T& alpha2){
	//update the ema value
	ema_update(ema, new_val, alpha1);
	//update the ema(ema) value 
	ema_update(ema_ema, ema, alpha2);
	//compute and return the dema value 
	return (2 * ema) - ema_ema;
}
template <typename T> 
T utility::tema_update(T& ema_ema_ema, T& ema_ema, T& ema, const T& new_val, const T& alpha1, const T& alpha2, const T& alpha3){
	//update the ema value 
	ema_update(ema, new_val, alpha1); 
	//update the ema(ema) value 
	ema_update(ema_ema, ema, alpha2);
	//update the ema(ema(ema)) value
	ema_update(ema_ema_ema, ema_ema, alpha3);
	return (3 * ema) - (3 * ema_ema) + ema_ema_ema; 
}

//update functions for the differences between two exponential moving averages
template <typename T>
T utility::ema_diff_update(T& ema1, T& ema2, const T& new_val, const T& alpha1, const T& alpha2){
	//update ema1 and ema2 
	ema_update(ema1, new_val, alpha1); 
	ema_update(ema2, new_val, alpha2);
	return ema2 - ema1;
}
//alpha11 and 12 are alphas for ema_ema1 and alpha21 & 22 are alphas for ema_ema2
template <typename T> 
T utility::dema_diff_update(T& ema_ema1, T& ema_ema2, T& ema1, T& ema2, const T& new_val, const T& alpha11, const T& alpha12, const T& alpha21, const T& alpha22){
	//update ema_ema1, ema1, ema_ema2 & ema2 
	T dema1 = dema_update(ema_ema1, ema1, new_val, alpha11, alpha12); 	
	T dema2 = dema_update(ema_ema2, ema2, new_val, alpha21, alpha22); 
	return dema2 - dema1;
}
//alpha11, 12 and 13 are alphas for ema_ema1 and alpha21, 22 and 23 are alphas for ema_ema_ema2
template <typename T> 
T utility::tema_diff_update(T& ema_ema_ema1, T& ema_ema1, T& ema1, T& ema_ema_ema2, T& ema_ema2, T& ema2, const T& new_val, 
		const T& alpha11, const T& alpha12, const T& alpha13, const T& alpha21, const T& alpha22, const T& alpha23){
	//update ema_ema_ema1, ema_ema1, ema1, ema_ema_ema2, ema_ema2 & ema2 
	T tema1 = tema_update(ema_ema_ema1, ema_ema1, ema1, new_val, alpha11, alpha12, alpha13); 
	T tema2 = tema_update(ema_ema_ema2, ema_ema2, ema2, new_val, alpha21, alpha22, alpha23);
	return tema2 - tema1; 
}
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_quartiles_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, T ex){
	//compute the minmax 
	std::vector<std::pair<T, T>> minmax(1); 
	roll_minmax_update(first, minmax.begin(), dq_min, dq_max, i, k);
	if(i >= k - 1){
		T q1 = minmax[0].first + (25 * (minmax[0].second - minmax[0].first) / 100.0); 
		T q2 = minmax[0].first + (50 * (minmax[0].second - minmax[0].first) / 100.0); 
		T q3 = minmax[0].first + (75 * (minmax[0].second - minmax[0].first) / 100.0);
		*out = std::tuple<T, T, T>{q1, q2, q3}; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>
void utility::roll_quartiles_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex){
	std::vector<std::pair<T, T>> minmax(1); 
	roll_minmax_update(first, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op);
	if(i >= k - 1){
		T q1 = minmax[0].first + (25 * (minmax[0].second - minmax[0].first) / 100.0); 
		T q2 = minmax[0].first + (50 * (minmax[0].second - minmax[0].first) / 100.0); 
		T q3 = minmax[0].first + (75 * (minmax[0].second - minmax[0].first) / 100.0);
		*out = std::tuple<T, T, T>{q1, q2, q3}; 
	}
}

template <typename InputIt, typename OutputIt, typename T>
void utility::roll_percentile_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, T ex){
	//compute the minmax 
	std::vector<std::pair<T, T>> minmax(1); 
	roll_minmax_update(first, minmax.begin(), dq_min, dq_max, i, k);
	if(i >= k - 1){
		//grab the current value 
		auto it = std::next(first, i); 
		*out = percentile(minmax[0].first, minmax[0].second, *it);  
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>
void utility::roll_percentile_update(InputIt first, OutputIt out, std::deque<int>& dq_min, std::deque<int>& dq_max, int i, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex){
	//compute the minmax 
	std::vector<std::pair<T, T>> minmax(1); 
	roll_minmax_update(first, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op);
	if(i >= k - 1){
		//grab the current value 
		auto it = std::next(first, i); 
		*out = percentile(minmax[0].first, minmax[0].second, un_op(*it));  
	}
}

//functions for computing the rolling mean
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_mean(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	//iterator to keep track of the discarded value
	auto itd = first1; 
	//compute the mean of the first k values
	T m = mean(first1, std::next(first1, k), init);
	*first2 = m;
	auto comp_next_mean = [&itd, &m, &k](const auto& x){
		roll_mean_update(m, *itd, x, k);
		++itd; 
		return m; 
	}; 
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_mean); 
}

template <typename InputIt, typename OutputIt, typename UnaryOp, typename T>	
void utility::roll_mean(InputIt first1, InputIt last1, OutputIt first2, int k, UnaryOp un_op, T init){
	//iterator to keep track of the discarded value
	auto itd = first1; 
	//compute the mean of the first k values
	T m = mean(first1, std::next(first1, k), init, un_op);
	*first2 = m;
	auto comp_next_mean = [&itd, &m, &k, &un_op](const auto& x){
		roll_mean_update(m, un_op(*itd), un_op(x), k);
		++itd; 
		return m; 
	}; 
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_mean); 
}

//computing the rolling weighted mean (where the weights vector remains constant)
template <typename InputIt, typename OutputIt, typename W, typename T> 
void utility::roll_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init){
	int k = weights.size();
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = wtd_mean(std::prev(it, k - 1), std::next(it), weights, init);
		first2++; 
	}
}
//computing the rolling (w/ constant weights vector & we apply a unary to the elements of [first1, last))
template <typename InputIt, typename OutputIt, typename W, typename T, typename UnaryOp>	
void utility::roll_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, const std::vector<W>& weights, T init, UnaryOp un_op){
	int k = weights.size(); 
	for(auto it = std::next(first1, k - 1); it != last1; it++){
		*first2 = wtd_mean(std::prev(it, k - 1), std::next(it), weights, un_op, init);
		first2++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T>
void utility::roll_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T init){
	//check if the iterator ranges are the same size 
	int n1 = std::distance(first1, last1);
	int n2 = std::distance(first2, last2);
	if(n1 != n2){
		throw std::invalid_argument("roll_wtd_mean: Ranges [first1, last1) and [first2, last2) must be the same size."); 
	}
	//compute the sum of the first k weights 
	T w_sum = std::accumulate(first2, std::next(first2, k), init);
	//compute the first wtd mean value 
	T ip = std::inner_product(first1, std::next(first1, k), first2, init, std::plus<>(), std::multiplies<>()); 
	//set the first weighted mean value
	*first3 = ip / w_sum; 
	first3++;
	//iterators to the current element of the first and second ranges
	auto it1 = std::next(first1, k); 
	auto it2 = std::next(first2, k); 
	//iterator to the discarded elements of the first and second range
	auto itd1 = first1; 
	auto itd2 = first2;
	
	for(auto it = it1; it != last1; it++){
		//update the weighted sum
		w_sum = w_sum - *itd2 + *it2;
		//update the inner product 
		ip = ip - ((*itd1) * (*itd2)) + ((*it1) * (*it2)); 
		//increment the iterators 
		it1++; 
		it2++; 
		itd1++;
		itd2++; 
		//compute the weighted mean
		*first3 = (1 / w_sum) * ip; 
		first3++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2>
void utility::roll_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k, T init, UnaryOp1 un_op1, UnaryOp2 un_op2){
	//check if the iterator ranges are the same size 
	int n1 = std::distance(first1, last1);
	int n2 = std::distance(first2, last2);
	if(n1 != n2){
		throw std::invalid_argument("roll_wtd_mean: Ranges [first1, last1) and [first2, last2) must be the same size."); 
	}
	auto comp_ws = [&un_op2](T val, const auto& x){
		return val + un_op2(x); 	
	};
	//compute the sum of the first k weights 
	T w_sum = std::accumulate(first2, std::next(first2, k), init, comp_ws);
	//compute the first wtd mean value 
	auto mult_op = [&un_op1, &un_op2](const auto& x, const auto& y){
		return un_op1(x) * un_op2(y); 
	};
	T ip = std::inner_product(first1, std::next(first1, k), first2, init, std::plus<>(), mult_op); 
	//set the first weighted mean value
	*first3 = ip / w_sum; 
	first3++;
	//iterators to the current element of the first and second ranges
	auto it1 = std::next(first1, k); 
	auto it2 = std::next(first2, k); 
	//iterator to the discarded elements of the first and second range
	auto itd1 = first1; 
	auto itd2 = first2;
	
	for(auto it = it1; it != last1; it++){
		//update the weighted sum
		w_sum = w_sum - un_op2(*itd2) + un_op2(*it2);
		//update the inner product 
		ip = ip - mult_op(*itd1, *itd2) + mult_op(*it1, *it2); 
		//increment the iterators 
		it1++; 
		it2++; 
		itd1++;
		itd2++; 
		//compute the weighted mean
		*first3 = (1 / w_sum) * ip; 
		first3++; 
	}
}
template <typename InputIt, typename T> 
void utility::ema_setup(T& ema, InputIt first, int k, T init){
	ema = mean(first, std::next(first, k), init);      	
}
template <typename InputIt, typename T, typename UnaryOp> 
void utility::ema_setup(T& ema, InputIt first, int k, T init, UnaryOp un_op){
	ema = mean(first, std::next(first, k), init, un_op);
}
template <typename InputIt, typename T> 
void utility::dema_setup(T& ema_ema, T& ema, InputIt first, int k, T alpha1, T init){
	//compute the first k ema values	
	std::vector<T> ema_vals(k); 
	utility::ema(first, std::next(first, 2*k), ema_vals.begin(), k, alpha1, init);
	//get the kth ema value
	ema = *std::prev(ema_vals.end());
	//compute the mean of the first k ema values 
	ema_ema = mean(ema_vals.begin(), ema_vals.end(), init);
}
template <typename InputIt, typename T, typename UnaryOp> 
void utility::dema_setup(T& ema_ema, T& ema, InputIt first, int k, T alpha1, T init, UnaryOp un_op){
	//compute the first k ema values 
	std::vector<T> ema_vals(k); 
	utility::ema(first, std::next(first, 2*k), ema_vals.begin(), k, alpha1, init, un_op);
	//get the kth ema value
	ema = *std::prev(ema_vals.end());
	//compute the mean of the first k ema values 
	ema_ema = mean(ema_vals.begin(), ema_vals.end(), init);
}
template <typename InputIt, typename T> 
void utility::tema_setup(T& ema_ema_ema, T& ema_ema, T& ema, InputIt first, int k, T alpha1, T alpha2, T init){
	//compute the first 2k ema values 
	std::vector<T> ema_vals(2*k); 
	utility::ema(first, std::next(first, 3*k), ema_vals.begin(), k, alpha1, init);
	//get the 2kth ema value
	ema = *std::prev(ema_vals.end()); 
	//compute the first k ema(ema) values; 
	std::vector<T> ema_ema_vals(k);
	utility::ema(ema_vals.cbegin(), std::next(ema_vals.cbegin(), 2*k), ema_ema_vals.begin(), k, alpha2, init);
	//get the kth ema(ema) value
	ema_ema = *std::prev(ema_ema_vals.end()); 
	//compute the mean of the first k ema(ema) values
	ema_ema_ema = mean(ema_ema_vals.cbegin(), ema_ema_vals.cend(), init);
}
template <typename InputIt, typename T, typename UnaryOp> 
void utility::tema_setup(T& ema_ema_ema, T& ema_ema, T& ema, InputIt first, int k, T alpha1, T alpha2, T init, UnaryOp un_op){
	//compute the first 2k ema values 
	std::vector<T> ema_vals(2*k); 
	utility::ema(first, std::next(first, 3*k), ema_vals.begin(), k, alpha1, init, un_op);
	//get the 2kth ema value
	ema = *std::prev(ema_vals.end()); 
	//compute the first k ema(ema) values; 
	std::vector<T> ema_ema_vals(k);
	utility::ema(ema_vals.cbegin(), std::next(ema_vals.cbegin(), 2*k), ema_ema_vals.begin(), k, alpha2, init);
	//get the kth ema(ema) value
	ema_ema = *std::prev(ema_ema_vals.end()); 
	//compute the mean of the first k ema(ema) values
	ema_ema_ema = mean(ema_ema_vals.cbegin(), ema_ema_vals.cend(), init);
}

//exponential moving average
template <typename InputIt, typename OutputIt, typename T>
void utility::ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init){
	//call ema_setup
	T ema; 
	ema_setup(ema, first1, k, init); 
	//compute the ema values 
	for(auto it = std::next(first1, k); it != last1; it++){
		//calculate the ema value for the current period
		ema_update(ema, *it, alpha);
		//set the ema value and increment the write iterator
		*first2 = ema;
		first2++; 
	}; 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::ema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha, T init, UnaryOp un_op){
	//call ema_setup
	T ema; 
	ema_setup(ema, first1, k, init, un_op); 
	//compute the ema values 
	for(auto it = std::next(first1, k); it != last1; it++){
		//calculate the ema value for the current period
		ema_update(ema, un_op(*it), alpha);
		//set the ema value and increment the write iterator
		*first2 = ema;
		first2++; 
	}; 
}

//double exponential moving average (single pass)
template <typename InputIt, typename OutputIt, typename T> 
void utility::dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init){
	//call dema_setup
	T ema_ema, ema; 
	dema_setup(ema_ema, ema, first1, k, alpha1, init); 
	//compute the dema values 
	for(auto it = std::next(first1, 2*k); it != last1; it++){
		*first2 = dema_update(ema_ema, ema, *it, alpha1, alpha2);  
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::dema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T init, UnaryOp un_op){
	//call dema_setup
	T ema_ema, ema; 
	dema_setup(ema_ema, ema, first1, k, alpha1, init, un_op); 
	//compute the dema values 
	for(auto it = std::next(first1, 2*k); it != last1; it++){
		*first2 = dema_update(ema_ema, ema, un_op(*it), alpha1, alpha2);  
		first2++; 
	}
}

//triple exponential moving average 
template <typename InputIt, typename OutputIt, typename T> 
void utility::tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T init){
	//call tema_setup 
	T ema_ema_ema, ema_ema, ema;
	tema_setup(ema_ema_ema, ema_ema, ema, first1, k, alpha1, alpha2, init); 
	//compute the tema values 
	for(auto it = std::next(first1, 3*k); it != last1; it++){
		*first2 = tema_update(ema_ema_ema, ema_ema, ema, *it, alpha1, alpha2, alpha3);  
		first2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::tema(InputIt first1, InputIt last1, OutputIt first2, int k, T alpha1, T alpha2, T alpha3, T init, UnaryOp un_op){
	//call tema_setup 
	T ema_ema_ema, ema_ema, ema;
	tema_setup(ema_ema_ema, ema_ema, ema, first1, k, alpha1, alpha2, init, un_op); 
	//compute the tema values 
	for(auto it = std::next(first1, 3*k); it != last1; it++){
		*first2 = tema_update(ema_ema_ema, ema_ema, ema, un_op(*it), alpha1, alpha2, alpha3);  
		first2++; 
	}
}
//rolling mean of rolling means
//k1 is the period of the mean, k2 is the period of the mean of the means
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_meanmean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	auto itf = std::next(first1, k1 + k2 - 2); 
	auto itd = std::prev(itf, k1 - 1); 
	//compute the first k2 k1 period means 
	std::deque<T> mk1_vals(k2);
	T mk1, mk2;
	for(auto it = itf; it != last1; it++){
		if(it == itf){
			utility::roll_mean(first1, std::next(itf), mk1_vals.begin(), k1, init);
			mk1 = mk1_vals.back();
			mk2 = utility::mean(mk1_vals.begin(), mk1_vals.end(), init);
		}else{
			utility::roll_mean_update(mk1, *itd, *it,  k1);
			mk1_vals.push_back(mk1); 
			utility::roll_mean_update(mk2, mk1_vals.front(), mk1, k2);
			mk1_vals.pop_front();
			itd++; 
		}
		*first2 = mk2; 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void utility::roll_meanmean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, UnaryOp un_op){
	auto itf = std::next(first1, k1 + k2 - 2); 
	auto itd = std::prev(itf, k1 - 1); 
	//compute the first k2 k1 period means 
	std::deque<T> mk1_vals(k2);
	T mk1, mk2;
	for(auto it = itf; it != last1; it++){
		if(it == itf){
			utility::roll_mean(first1, std::next(itf), mk1_vals.begin(), k1, un_op, init);
			mk1 = mk1_vals.back();
			mk2 = utility::mean(mk1_vals.begin(), mk1_vals.end(), init);
		}else{
			utility::roll_mean_update(mk1, un_op(*itd), un_op(*it), k1);
			mk1_vals.push_back(mk1);
			utility::roll_mean_update(mk2, mk1_vals.front(), mk1, k2);
			mk1_vals.pop_front();
			itd++; 
		}
		*first2 = mk2; 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void utility::eema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init){
	int k = std::max(k1, k2); 
	T ema_ema, ema;
	std::vector<T> ema_vals(k2); 
	//call ema setup 
	utility::ema_setup(ema, std::next(first1, k - k1), k1, init);
	auto it = std::next(first1, k - k1); 
	for(int i = 0; i < k2; i++){
		utility::ema_update(ema, *it, alpha1); 
		ema_vals[i] = ema; 
		it++; 
	}
	//call ema_setup on the ema values 
	utility::ema_setup(ema_ema, ema_vals.begin(), k2, init); 
	//note: dema setup uses k1 for setting up the mean of the ema's
	for(it; it != last1; it++){
		utility::ema_update(ema, *it, alpha1);
		utility::ema_update(ema_ema, ema, alpha2);
		*first2 = ema_ema; 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void utility::eema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha1, int k2, T alpha2, T init, UnaryOp un_op){
	int k = std::max(k1, k2); 
	T ema_ema, ema;
	std::vector<T> ema_vals(k2); 
	//call ema setup 
	utility::ema_setup(ema, first1, k1, init, un_op);
	auto it = std::next(first1, k1); 
	for(int i = 0; i < k2; i++){
		utility::ema_update(ema, un_op(*it), alpha1); 
		ema_vals[i] = ema; 
		it++; 
	}
	//call ema_setup on the ema values 
	utility::ema_setup(ema_ema, ema_vals.begin(), k2, init); 
	//note: dema setup uses k1 for setting up the mean of the ema's
	for(it; it != last1; it++){
		utility::ema_update(ema, un_op(*it), alpha1);
		utility::ema_update(ema_ema, ema, alpha2);
		*first2 = ema_ema; 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void utility::ddema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init){
	T eema, ema;
	//call dema setup
	utility::dema_setup(eema, ema, first1, k1, alpha11, init);
	//compute the first 2k dema values 
	std::vector<T> dema_vals(2*k2);
	auto it = std::next(first1, 2*k1); 
	for(int i = 0; i < 2*k2; i++){
		dema_vals[i] = utility::dema_update(eema, ema, *it, alpha11, alpha12);
		it++; 
	}
	T eema_dema, ema_dema, dema_dema;
	//call dema setup on the dema values
	utility::dema_setup(eema_dema, ema_dema, dema_vals.cbegin(), k2, alpha21, init);
	for(it; it != last1; it++){
		//compute the new dema value 
		dema_dema = utility::dema_update(eema, ema, *it, alpha11, alpha12); 	
		*first2 = utility::dema_update(eema_dema, ema_dema, dema_dema, alpha21, alpha22);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::ddema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, int k2, T alpha21, T alpha22, T init, UnaryOp un_op){
	T eema, ema;
	//call dema setup
	utility::dema_setup(eema, ema, first1, k1, alpha11, init, un_op);
	//compute the first 2k dema values 
	std::vector<T> dema_vals(2*k2);
	auto it = std::next(first1, 2*k1); 
	for(int i = 0; i < 2*k2; i++){
		dema_vals[i] = utility::dema_update(eema, ema, un_op(*it), alpha11, alpha12);
		it++; 
	}
	T eema_dema, ema_dema, dema_dema;
	//call dema setup on the dema values
	utility::dema_setup(eema_dema, ema_dema, dema_vals.cbegin(), k2, alpha21, init);
	for(it; it != last1; it++){
		//compute the new dema value 
		dema_dema = utility::dema_update(eema, ema, un_op(*it), alpha11, alpha12); 	
		*first2 = utility::dema_update(eema_dema, ema_dema, dema_dema, alpha21, alpha22);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T> 
void utility::ttema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init){
	T eeema, eema, ema;
	//call tema_setup
	utility::tema_setup(eeema, eema, ema, first1, k1, alpha11, alpha12, init);
	//compute the first 2k tema values 
	std::vector<T> tema_vals(3*k2);
	auto it = std::next(first1, 3*k1);
	for(int i = 0; i < 3*k2; i++){
		tema_vals[i] = utility::tema_update(eeema,eema, ema, *it, alpha11, alpha12, alpha13);
		it++; 
	}
	//call tema setup on the tema values 
	T eeema_tema, eema_tema, ema_tema, tema_tema;
	utility::tema_setup(eeema_tema, eema_tema, ema_tema, tema_vals.cbegin(), k2, alpha21, alpha22, init);
	for(it; it != last1; it++){
		//compute the new tema value 
		tema_tema = utility::tema_update(eeema, eema, ema, *it, alpha11, alpha12, alpha13); 	
		*first2 = utility::tema_update(eeema_tema, eema_tema, ema_tema, tema_tema, alpha21, alpha22, alpha23);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::ttema(InputIt first1, InputIt last1, OutputIt first2, int k1, T alpha11, T alpha12, T alpha13, int k2, T alpha21, T alpha22, T alpha23, T init, UnaryOp un_op){
	T eeema, eema, ema;
	//call tema_setup
	utility::tema_setup(eeema, eema, ema, first1, k1, alpha11, alpha12, init, un_op);
	//compute the first 2k tema values 
	std::vector<T> tema_vals(3*k2);
	auto it = std::next(first1, 3*k1);
	for(int i = 0; i < 3*k2; i++){
		tema_vals[i] = utility::tema_update(eeema,eema, ema, un_op(*it), alpha11, alpha12, alpha13);
		it++; 
	}
	//call tema setup on the tema values 
	T eeema_tema, eema_tema, ema_tema, tema_tema;
	utility::tema_setup(eeema_tema, eema_tema, ema_tema, tema_vals.cbegin(), k2, alpha21, alpha22, init);
	for(it; it != last1; it++){
		//compute the new tema value 
		tema_tema = utility::tema_update(eeema, eema, ema, un_op(*it), alpha11, alpha12, alpha13); 	
		*first2 = utility::tema_update(eeema_tema, eema_tema, ema_tema, tema_tema, alpha21, alpha22, alpha23);
		first2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_diff_percent_change(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T ex){
	if(k1 > k2){
		std::swap(k1, k2); 
	}
	for(auto it = std::next(first1, k2 - 1); it != last1; it++){
		*first2 = percent_change_diff(it, k1, k2, ex, false);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T> 
void utility::roll_diff_percent_change(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, UnaryOp un_op, T ex){
	if(k1 > k2){
		std::swap(k1, k2); 
	}
	for(auto it = std::next(first1, k2 - 1); it != last1; it++){
		*first2 = percent_change_diff(it, k1, k2, un_op, ex, false);
		first2++; 
	}
}

//rolling difference between two rolling means 
//The difference computed is the mean with larger k - mean with smaller k
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_diff_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init){
	if(k1 > k2){
		//if k1 is larger swap them so k1 <= k2
		std::swap(k1, k2); 
	}
	//iterators to the discarded values 
	auto itd_k1 = std::next(first1, k2 - k1);
	auto itd_k2 = first1; 
	//compute the initial means
	T m_k1 = mean(std::next(first1, k2 - k1), std::next(std::next(first1, k2 - k1), k1), init);
	T m_k2 = mean(first1, std::next(first1, k2), init);      
	*first2 = m_k2 - m_k1;
	auto comp_next_mean_diff = [&itd_k1, &itd_k2, &m_k1, &m_k2, &k1, &k2](const auto& x){
		T md = roll_diff_mean_update(m_k1, m_k2, *itd_k1, *itd_k2, x, k1, k2); 
		itd_k1++;
		itd_k2++; 
		return md; 
	}; 
	std::transform(std::next(first1, k2), last1, std::next(first2), comp_next_mean_diff); 
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename T>	
void utility::roll_diff_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T init, UnaryOp un_op){
	if(k1 > k2){
		//if k1 is larger swap them so k1 <= k2
		std::swap(k1, k2); 
	}
	//iterators to the discarded values 
	auto itd_k1 = std::next(first1, k2 - k1);
	auto itd_k2 = first1;
	//compute the initial means
	T m_k1 = mean(std::next(first1, k2 - k1), std::next(std::next(first1, k2 - k1), k1), init, un_op);
	T m_k2 = mean(first1, std::next(first1, k2), init, un_op);      
	*first2 = m_k2 - m_k1;
	auto comp_next_mean_diff = [&itd_k1, &itd_k2, &m_k1, &m_k2, &k1, &k2, &un_op](const auto& x){
		T md = roll_diff_mean_update(m_k1, m_k2, un_op(*itd_k1), un_op(*itd_k2), un_op(x), k1, k2); 
		itd_k1++;
		itd_k2++; 
		return md; 
	}; 
	std::transform(std::next(first1, k2), last1, std::next(first2), comp_next_mean_diff); 
}

template <typename InputIt, typename OutputIt, typename W, typename T> 
void utility::roll_diff_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& w1, const std::vector<W>& w2,  T init){
	if(k1 > k2){
		//if k1 is larger swap them so k1 <= k2
		std::swap(k1, k2); 
	}
	//compute the weight sums 
	auto comp_sum = [](const T& val, const W& w){
		return val + w; 
	};
	T w1_sum = std::accumulate(w1.cbegin(), w1.cend(), init, comp_sum);
	T w2_sum = std::accumulate(w2.cbegin(), w2.cend(), init, comp_sum);

	//for(auto it = std::next(first1, k2); it != last1; it++){
	//	T wm_k1 = wtd_mean(std::prev(it, k1), it, w1, w1_sum, init);
	//	T wm_k2 = wtd_mean(std::prev(it, k2), it, w2, w2_sum, init);
	//	*first2 = wm_k2 - wm_k1;
	//	first2++; 
	//}
	for(auto it = std::next(first1, k2 - 1); it != last1; it++){
		T wm_k1 = wtd_mean(std::prev(it, k1 - 1), std::next(it), w1, w1_sum, init);
		T wm_k2 = wtd_mean(std::prev(it, k2 - 1), std::next(it), w2, w2_sum, init);
		*first2 = wm_k2 - wm_k1;
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename UnaryOp, typename W, typename T>	
void utility::roll_diff_wtd_mean(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, const std::vector<W>& w1, const std::vector<W>& w2, T init, UnaryOp un_op){
	if(k1 > k2){
		//if k1 is larger swap them so k1 <= k2
		std::swap(k1, k2); 
	}
	//compute the weight sums 
	auto comp_sum = [](const T& val, const W& w){
		return val + w; 
	};
	T w1_sum = std::accumulate(w1.cbegin(), w1.cend(), init, comp_sum);
	T w2_sum = std::accumulate(w2.cbegin(), w2.cend(), init, comp_sum);
//	for(auto it = std::next(first1, k2); it != last1; it++){
//		T wm_k1 = wtd_mean(std::prev(it, k1), it, w1, un_op, w1_sum, init);
//		T wm_k2 = wtd_mean(std::prev(it, k2), it, w2, un_op, w2_sum, init);
//		*first2 = wm_k2 - wm_k1;
//		first2++; 
//	}
	for(auto it = std::next(first1, k2 - 1); it != last1; it++){
		T wm_k1 = wtd_mean(std::prev(it, k1 - 1), std::next(it), w1, un_op, w1_sum, init);
		T wm_k2 = wtd_mean(std::prev(it, k2 - 1), std::next(it), w2, un_op, w2_sum, init);
		*first2 = wm_k2 - wm_k1;
		first2++; 
	}
}
//difference between two weighted means where the weights come from a second iterator range 
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T> 
void utility::roll_diff_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init){
	//check if the iterator ranges are the same size 
	int n1 = std::distance(first1, last1);
	int n2 = std::distance(first2, last2);
	if(n1 != n2){
		throw std::invalid_argument("roll_wtd_mean: Ranges [first1, last1) and [first2, last2) must be the same size."); 
	}
	if(k1 > k2){
		//if k1 is larger swap k1 and k2
		std::swap(k1, k2); 
	}
	//iterators to the current element of the first and second ranges
	auto it1 = std::next(first1, k2); 
	auto it2 = std::next(first2, k2); 
	//iterator to the discarded elements of the first and second range
	auto itk1_d1 = std::next(first1, k2 - k1); 
	auto itk1_d2 = std::next(first2, k2 - k1);
	auto itk2_d1 = first1; 
	auto itk2_d2 = first2;
	//compute the sum of the k1 weights prior to period k2 
	T w_sum_k1 = std::accumulate(itk1_d2, it2, init);
	//compute the first k1 period wtd mean value 
	T ip_k1 = std::inner_product(itk1_d1, it1, itk1_d2, init, std::plus<>(), std::multiplies<>());
	//compute the sum of the first k2 weights
	T w_sum_k2 = std::accumulate(itk2_d2, it2, init);
	//compute the first k2 period wtd mean value 
	T ip_k2 = std::inner_product(itk2_d2, it2, itk2_d1, init, std::plus<>(), std::multiplies<>());


	//set the first difference between the weighted means
	*first3 = (ip_k2 / w_sum_k2) - (ip_k1 / w_sum_k1); 
	first3++;
	
	for(auto it = it1; it != last1; it++){
		//update the weighted sums
		w_sum_k1 = w_sum_k1 - *itk1_d2 + *it2;
		w_sum_k2 = w_sum_k2 - *itk2_d2 + *it2;
		//update the inner products 
		ip_k1 = ip_k1 - ((*itk1_d1) * (*itk1_d2)) + ((*it1) * (*it2)); 
		ip_k2 = ip_k2 - ((*itk2_d1) * (*itk2_d2)) + ((*it1) * (*it2)); 
		//increment the iterators 
		it1++; 
		it2++; 
		itk1_d1++;
		itk1_d2++; 
		itk2_d1++;
		itk2_d2++; 
		//compute the difference between the weighted means
		*first3 = (ip_k2 / w_sum_k2) - (ip_k1 / w_sum_k1); 
		first3++; 
	}
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename T, typename UnaryOp1, typename UnaryOp2>	
void utility::roll_diff_wtd_mean(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, int k1, int k2, T init, UnaryOp1 un_op1, UnaryOp2 un_op2){
	//check if the iterator ranges are the same size 
	int n1 = std::distance(first1, last1);
	int n2 = std::distance(first2, last2);
	if(n1 != n2){
		throw std::invalid_argument("roll_wtd_mean: Ranges [first1, last1) and [first2, last2) must be the same size."); 
	}
	if(k1 > k2){
		//if k1 is larger swap k1 and k2
		std::swap(k1, k2); 
	}
	//define the lambdas for computing the weight sums & the multiplication used in the inner products
	auto comp_ws = [&un_op2](T val, const auto& x){
		return val + un_op2(x); 	
	};
	//compute the first wtd mean value 
	auto mult_op = [&un_op1, &un_op2](const auto& x, const auto& y){
		return un_op1(x) * un_op2(y); 
	};
	//iterators to the current element of the first and second ranges
	auto it1 = std::next(first1, k2); 
	auto it2 = std::next(first2, k2); 
	//iterator to the discarded elements of the first and second range
	auto itk1_d1 = std::next(first1, k2 - k1); 
	auto itk1_d2 = std::next(first2, k2 - k1);
	auto itk2_d1 = first1; 
	auto itk2_d2 = first2;

	//compute the sum of the k1 weights prior to period k2 
	T w_sum_k1 = std::accumulate(itk1_d2, it2, init, comp_ws);
	//compute the first k1 period wtd mean value 
	T ip_k1 = std::inner_product(itk1_d1, it1, itk1_d2, init, std::plus<>(), mult_op);
	//compute the sum of the first k2 weights
	T w_sum_k2 = std::accumulate(itk2_d2, it2, init, comp_ws);
	//compute the first k2 period wtd mean value 
	T ip_k2 = std::inner_product(itk2_d2, it2, itk2_d1, init, std::plus<>(), mult_op);

	//set the first difference between the weighted means
	*first3 = (ip_k2 / w_sum_k2) - (ip_k1 / w_sum_k1); 
	first3++;
	
	for(auto it = it1; it != last1; it++){
		//update the weighted sums
		w_sum_k1 = w_sum_k1 - un_op2(*itk1_d2) + un_op2(*it2);
		w_sum_k2 = w_sum_k2 - un_op2(*itk2_d2) + un_op2(*it2);
		//update the inner products 
		ip_k1 = ip_k1 - (un_op1(*itk1_d1) * un_op2(*itk1_d2)) + (un_op1(*it1) * un_op2(*it2)); 
		ip_k2 = ip_k2 - (un_op1(*itk2_d1) * un_op2(*itk2_d2)) + (un_op1(*it1) * un_op2(*it2)); 
		//increment the iterators 
		it1++; 
		it2++; 
		itk1_d1++;
		itk1_d2++; 
		itk2_d1++;
		itk2_d2++; 
		//compute the difference between the weighted means
		*first3 = (ip_k2 / w_sum_k2) - (ip_k1 / w_sum_k1); 
		first3++; 
	}

}

//difference bewteen two exponential moving averages (single pass algorithm)
template <typename InputIt, typename OutputIt, typename T>
void utility::ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T init){
	if(k1 > k2){
		//if k1 is larger sway the k's and the alphas
		std::swap(k1, k2);
		std::swap(alpha1, alpha2); 
	}
	//compute the first k2 - k1 ema values of the k1 period ema 
	std::vector<T> ema_k1_vals(k2 - k1);
	ema(first1, std::next(first1, k2), ema_k1_vals.begin(), k1, alpha1, init);
	//grab the second to last ema_k1 value
	T ema_k1 = ema_k1_vals.back(); 
	//compute the mean of the first k2 values 
	T ema_k2 = mean(first1, std::next(first1, k2), init);
	//compute the differences between the emas 
	for(auto it = std::next(first1, k2); it != last1; it++){
		//compute & assign the ema difference value
		*first2 = ema_diff_update(ema_k1, ema_k2, *it, alpha1, alpha2);
		//increment the write iterator
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void utility::ema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha1, T alpha2, T init, UnaryOp un_op){
	if(k1 > k2){
		//if k1 is larger swap the k's and the alphas
		std::swap(k1, k2);
		std::swap(alpha1, alpha2); 
	}
	//compute the first k2 - k1 ema values of the k1 period ema 
	std::vector<T> ema_k1_vals(k2 - k1);
	ema(first1, std::next(first1, k2), ema_k1_vals.begin(), k1, alpha1, init, un_op);
	//grab the last ema_k1 value
	T ema_k1 = ema_k1_vals.back();  
	//compute the mean of the first k2 values 
	T ema_k2 = mean(first1, std::next(first1, k2), init, un_op);
	//compute the differences between the emas 
	for(auto it = std::next(first1, k2); it != last1; it++){
		//compute & assign the ema difference value
		*first2 = ema_diff_update(ema_k1, ema_k2, un_op(*it), alpha1, alpha2);
		//increment the write iterator
		first2++; 
	}
}

//difference bewtween two double exponential moving averages (single pass algorithm)
template <typename InputIt, typename OutputIt, typename T>
void utility::dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha11, T alpha12, T alpha21, T alpha22, T init){
	if(k1 > k2){
		//if k1 is larger swap the ks and the alphas
		std::swap(k1, k2); 
		std::swap(alpha11, alpha21); 
		std::swap(alpha12, alpha22); 
	}
	std::vector<T> ema_k1_v(2*k2 - k1);
	ema(first1, std::next(first1, 2*k2), ema_k1_v.begin(), k1, alpha11, init); 
	std::vector<T> ema_ema_k1_v(2*(k2 - k1));
	ema(ema_k1_v.cbegin(), ema_k1_v.cend(), ema_ema_k1_v.begin(), k1, alpha12, init);
	T ema_k1 = ema_k1_v[2*k2 - k1 - 1]; 
	T ema_ema_k1 = ema_ema_k1_v[2*(k2 - k1) - 1]; 
	std::vector<T> ema_k2_v(k2);
	ema(first1, std::next(first1, 2*k2), ema_k2_v.begin(), k2, alpha21, init); 
	T ema_k2 = ema_k2_v.back();
	T ema_ema_k2 = mean(ema_k2_v.cbegin(), ema_k2_v.cend(), init);
	for(auto it = std::next(first1, 2*k2); it != last1; it++){
		*first2 = dema_diff_update(ema_ema_k1, ema_ema_k2, ema_k1, ema_k2, *it, alpha11, alpha12, alpha21, alpha22);
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void utility::dema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, T alpha11, T alpha12, T alpha21, T alpha22, T init, UnaryOp un_op){
	if(k1 > k2){
		//if k1 is larger swap the ks and the alphas
		std::swap(k1, k2); 
		std::swap(alpha11, alpha21); 
		std::swap(alpha12, alpha22); 
	}
	std::vector<T> ema_k1_v(2*k2 - k1);
	ema(first1, std::next(first1, 2*k2), ema_k1_v.begin(), k1, alpha11, init, un_op); 
	std::vector<T> ema_ema_k1_v(2*(k2 - k1));
	ema(ema_k1_v.cbegin(), ema_k1_v.cend(), ema_ema_k1_v.begin(), k1, alpha12, init);
	T ema_k1 = ema_k1_v[2*k2 - k1 - 1]; 
	T ema_ema_k1 = ema_ema_k1_v[2*(k2 - k1) - 1]; 
	std::vector<T> ema_k2_v(k2);
	ema(first1, std::next(first1, 2*k2), ema_k2_v.begin(), k2, alpha21, init, un_op); 
	T ema_k2 = ema_k2_v.back();
	T ema_ema_k2 = mean(ema_k2_v.cbegin(), ema_k2_v.cend(), init);
	for(auto it = std::next(first1, 2*k2); it != last1; it++){
		*first2 = dema_diff_update(ema_ema_k1, ema_ema_k2, ema_k1, ema_k2, un_op(*it), alpha11, alpha12, alpha21, alpha22);
		first2++; 
	}
}
//difference between two triple exponential moving averages (single pass algorithm)

template <typename InputIt, typename OutputIt, typename T>
void utility::tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, 
		T alpha11, T alpha12, T alpha13, T alpha21, T alpha22, T alpha23, T init){
	if(k1 > k2){
		//if k1 is larger swap the ks and alphas 
		std::swap(k1, k2); 
		std::swap(alpha11, alpha21); 
		std::swap(alpha12, alpha22); 
		std::swap(alpha13, alpha23); 
	}

	std::vector<T> ema_k1_v(3*k2 - k1);
	ema(first1, std::next(first1, 3*k2), ema_k1_v.begin(), k1, alpha11, init); 
	std::vector<T> ema_ema_k1_v(3*k2 - 2*k1);
	ema(ema_k1_v.cbegin(), ema_k1_v.cend(), ema_ema_k1_v.begin(), k1, alpha12, init);
	std::vector<T> ema_ema_ema_k1_v(3*(k2 - k1));
	ema(ema_ema_k1_v.cbegin(), ema_ema_k1_v.cend(), ema_ema_ema_k1_v.begin(), k1, alpha13, init); 

	T ema_k1 = ema_k1_v[3*k2 - k1 - 1]; 
	T ema_ema_k1 = ema_ema_k1_v[3*k2 - 2*k1 - 1];
	T ema_ema_ema_k1 = ema_ema_ema_k1_v[3*(k2 - k1) - 1];

	std::vector<T> ema_k2_v(2*k2);
	ema(first1, std::next(first1, 3*k2), ema_k2_v.begin(), k2, alpha21, init);
	std::vector<T> ema_ema_k2_v(k2);
	ema(ema_k2_v.cbegin(), ema_k2_v.cend(), ema_ema_k2_v.begin(), k2, alpha22, init);
	
	T ema_k2 = ema_k2_v.back();
	T ema_ema_k2 = ema_ema_k2_v.back();  
	T ema_ema_ema_k2 = mean(ema_ema_k2_v.cbegin(), ema_ema_k2_v.cend(), init);

	for(auto it = std::next(first1, 3*k2); it != last1; it++){
		*first2 = tema_diff_update(ema_ema_ema_k1, ema_ema_k1, ema_k1, ema_ema_ema_k2, ema_ema_k2, ema_k2, *it, 
				alpha11, alpha12, alpha13, alpha21, alpha22, alpha23); 
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>
void utility::tema_diff(InputIt first1, InputIt last1, OutputIt first2, int k1, int k2, 
		T alpha11, T alpha12, T alpha13, T alpha21, T alpha22, T alpha23, T init, UnaryOp un_op){
	if(k1 > k2){
		//if k1 is larger swap the ks and alphas 
		std::swap(k1, k2); 
		std::swap(alpha11, alpha21); 
		std::swap(alpha12, alpha22); 
		std::swap(alpha13, alpha23); 
	}

	std::vector<T> ema_k1_v(3*k2 - k1);
	ema(first1, std::next(first1, 3*k2), ema_k1_v.begin(), k1, alpha11, init, un_op); 
	std::vector<T> ema_ema_k1_v(3*k2 - 2*k1);
	ema(ema_k1_v.cbegin(), ema_k1_v.cend(), ema_ema_k1_v.begin(), k1, alpha12, init);
	std::vector<T> ema_ema_ema_k1_v(3*(k2 - k1));
	ema(ema_ema_k1_v.cbegin(), ema_ema_k1_v.cend(), ema_ema_ema_k1_v.begin(), k1, alpha13, init); 

	T ema_k1 = ema_k1_v[3*k2 - k1 - 1]; 
	T ema_ema_k1 = ema_ema_k1_v[3*k2 - 2*k1 - 1];
	T ema_ema_ema_k1 = ema_ema_ema_k1_v[3*(k2 - k1) - 1];

	std::vector<T> ema_k2_v(2*k2);
	ema(first1, std::next(first1, 3*k2), ema_k2_v.begin(), k2, alpha21, init, un_op);
	std::vector<T> ema_ema_k2_v(k2);
	ema(ema_k2_v.cbegin(), ema_k2_v.cend(), ema_ema_k2_v.begin(), k2, alpha22, init);
	
	T ema_k2 = ema_k2_v.back();
	T ema_ema_k2 = ema_ema_k2_v.back();  
	T ema_ema_ema_k2 = mean(ema_ema_k2_v.cbegin(), ema_ema_k2_v.cend(), init);

	for(auto it = std::next(first1, 3*k2); it != last1; it++){
		*first2 = tema_diff_update(ema_ema_ema_k1, ema_ema_k1, ema_k1, ema_ema_ema_k2, ema_ema_k2, ema_k2, un_op(*it), 
				alpha11, alpha12, alpha13, alpha21, alpha22, alpha23); 
		first2++; 
	}
}

//rolling variance 
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_var(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	//iterator to the discarded value
	auto itd = first1; 	
	//compute the mean and variance and assign the first variance 
	std::pair<T, T> mv = mean_var(first1, std::next(first1, k), init);
	*first2 = mv.second;
	//compute the other variances 
	auto comp_next_var = [&itd, &k, &mv](const auto& x){
		//update the mean & variance 
		roll_var_update(mv.second, mv.first, *itd, x, k);
		itd++; 
		return mv.second; 
	};
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_var); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
void utility::roll_var(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op){
	//iterator to the discarded value
	auto itd = first1; 	
	//compute the mean and variance and assign the first variance using the overload which takes a unary operator 
	std::pair<T, T> mv = mean_var(first1, std::next(first1, k), init, un_op);
	*first2 = mv.second;
	//compute the other variances 
	auto comp_next_var = [&itd, &k, &mv, &un_op](const auto& x){
		//update the mean & variance  
		roll_var_update(mv.second, mv.first, un_op(*itd), un_op(x), k);
		itd++; 
		return mv.second; 
	};
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_var); 

}

//computing the rolling standard deviation
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_std(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	//iterator to the discarded value 
	auto itd = first1; 
	//compute the mean and variance & assign the first standard deviation
	std::pair<T, T> mstd = mean_var(first1, std::next(first1, k), init);
	mstd.second = std::sqrt(mstd.second); 
	*first2 = mstd.second;  
	//compute the other standard deviations by calling roll_std_update 
	auto comp_next_std = [&k, &itd, &mstd](const auto &x){
		roll_std_update(mstd.second, mstd.first, *itd, x, k);
		itd++; 
		return mstd.second; 
	};
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_std); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
void utility::roll_std(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op){
	//iterator to the discarded value 
	auto itd = first1; 
	//compute the mean and variance & assign the first standard deviation using the overload which takes a unary operator
	std::pair<T, T> mstd = mean_var(first1, std::next(first1, k), init, un_op);
	mstd.second = std::sqrt(mstd.second); 
	*first2 = mstd.second;  
	//compute the other standard deviations by calling roll_std_update 
	auto comp_next_std = [&k, &itd, &mstd, &un_op](const auto &x){
		roll_std_update(mstd.second, mstd.first, un_op(*itd), un_op(x), k);
		itd++; 
		return mstd.second; 
	}; 
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_std); 
}

//computing the rolling z-score 
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_zscore(InputIt first1, InputIt last1, OutputIt first2, int k, T init){
	//iterator to the discarded value 
	auto itd = first1; 
	//compute the mean and variance 
	std::pair<T, T> mstd = mean_var(first1, std::next(first1, k), init);
	//take the square root of the variance to get the standard deviation
	mstd.second = std::sqrt(mstd.second);
	//compute the first zscore 
	*first2 = zscore(std::next(first1, k - 1), mstd.first, mstd.second);
	//compute the other zscores by calling roll_zscore_update 
	auto comp_next_zscore = [&k, &itd, &mstd](const auto& x){
		return roll_zscore_update(mstd.second, mstd.first, *itd, x, k); 
	};
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_zscore); 
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp>	
void utility::roll_zscore(InputIt first1, InputIt last1, OutputIt first2, int k, T init, UnaryOp un_op){
	//iterator to the discarded value 
	auto itd = first1; 
	//compute the mean and variance using the overload which takes a unary 
	std::pair<T, T> mstd = mean_var(first1, std::next(first1, k), init, un_op);
	//compute the standard deviation 
	mstd.second = std::sqrt(mstd.second);
	//compute the first zscore 
	*first2 = zscore(un_op(std::next(first1, k - 1)), mstd.first, mstd.second);
	//compute the other zscores 
	auto comp_next_zscore = [&k, &itd, &mstd, &un_op](const auto& x){
		return roll_zscore_update(mstd.second, mstd.first, un_op(*itd), un_op(x), k); 
	};
	std::transform(std::next(first1, k), last1, std::next(first2), comp_next_zscore); 
}
//rolling minimum 
template <typename InputIt, typename OutputIt> 
void utility::roll_min(InputIt first1, InputIt last1, OutputIt first2, int k){
	//deque of the indicies 
	std::deque<int> dq_min;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_min_update(first1, first2, dq_min, i, k);
		if(i >= k - 1){
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt> 
void utility::roll_max(InputIt first1, InputIt last1, OutputIt first2, int k){
	//deque of the indicies 
	std::deque<int> dq_max;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_max_update(first1, first2, dq_max, i, k);
		if(i >= k - 1){
			first2++; 
		}
	}
}
//Comp is a custom comparator (args 1, 2) return true if 1 <= 2 for min 
//return true if 1 >= 2 for max
template <typename InputIt, typename OutputIt, typename Comp, typename UnaryOp>	
void utility::roll_min_or_max(InputIt first1, InputIt last1, OutputIt first2, int k, Comp comp, UnaryOp un_op){
	//deque of the indicies 
	std::deque<int> dq;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_min_or_max_update(first1, first2, dq, i, k, comp, un_op);
		if(i >= k - 1){
			first2++; 
		}
	}
}
//rolling minmax 
template <typename InputIt, typename OutputIt> 
void utility::roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k){
	//deque of indices for computing the min and max 
	std::deque<int> dq_max; 
	std::deque<int> dq_min;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_minmax_update(first1, first2, dq_min, dq_max, i, k);
		if(i >= k - 1){
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp> 
void utility::roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op){
	//deque of indices for computing the min and max 
	std::deque<int> dq_max; 
	std::deque<int> dq_min;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_minmax_update(first1, first2, dq_min, dq_max, i, k, comp1, comp2, un_op);
		if(i >= k - 1){
			first2++; 
		}
	}
}
template <typename InputIt, typename OutputIt, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2> 
void utility::roll_minmax(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2){
	//deque of indices for computing the min and max 
	std::deque<int> dq_max; 
	std::deque<int> dq_min;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_minmax_update(first1, first2, dq_min, dq_max, i, k, comp1, comp2, un_op1, un_op2);
		if(i >= k - 1){
			first2++; 
		}
	}
}

//rolling percentile functions (return percentile of current element with regard to a lookback window of size k)
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_percentile(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_percentile_update(first1, first2, dq_min, dq_max, i, k, ex);
		if(i >= k - 1){
			first2++;
		}
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>	
void utility::roll_percentile(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex){
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	for(int i = 0; i < std::distance(first1, last1); i++){
		roll_percentile_update(first1, first2, dq_min, dq_max, i, k, comp1, comp2, un_op, ex);
		if(i >= k - 1){
			first2++;
		}
	}
}
//
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_iqr(InputIt first1, InputIt last1, OutputIt first2, int k, T ex){
	std::deque<int> dq_min;
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	//call minmax_update k - 1 times to fill the deques so that the next call produces a tuple of the min and max 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k); 
	}
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//call roll_minmax_update
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k); 
		//compute & write the inter quartile range
		*first2 = .5*(minmax[0].second - minmax[0].first); 
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp>	
void utility::roll_iqr(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T ex){
	std::deque<int> dq_min;
	std::deque<int> dq_max;
	std::vector<std::pair<T, T>> minmax(1); 
	//call minmax_update k - 1 times to fill the deques so that the next call produces a tuple of the min and max 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op); 
	}
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//call roll_minmax_update
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op); 
		//compute & write the inter quartile range
		*first2 = .5*(minmax[0].second - minmax[0].first); 
		first2++;
	}
}

//rolling scaling functions 

template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get first k - 1 values in the deques 
	std::deque<int> dq_min; 
	std::deque<int> dq_max; 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k); 
	}
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k); 
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = (*std::next(first1, i) - minmax[0].first) / (minmax[0].second - minmax[0].first);  
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = undef;  
		}
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
void utility::roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, Comp1 comp1, Comp2 comp2, UnaryOp un_op){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get the first k - 1 values in the deques 
	std::deque<int> dq_min; 
	std::deque<int> dq_max; 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op); 
	}
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op); 
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = (un_op(*std::next(first1, i)) - minmax[0].first) / (minmax[0].second - minmax[0].first);  
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = undef;  
		}
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp1, typename UnaryOp2, typename UnaryOp3> 
void utility::roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, Comp1 comp1, Comp2 comp2, UnaryOp1 un_op1, UnaryOp2 un_op2, UnaryOp3 un_op3, const std::string& ts_name){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get the first k - 1 values in the deques 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op1, un_op2); 
	}
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//iterator used for the dt 
		auto it = std::next(first1, i); 
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op1, un_op2); 
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = Timestamp<T>{it->dt(), (un_op3(*it) - minmax[0].first) / (minmax[0].second - minmax[0].first), ts_name};  
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = Timestamp<T>{it->dt(), std::move(undef), ts_name};  
		}
		first2++; 
	}
}

template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_minmax_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T undef, const std::string& ts_name){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get the first k - 1 values in the deques 
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	auto min_cmp = [](const auto& t1, const auto& t2){
		return t1.val() >= t2.val(); 
	};
	auto max_cmp = [](const auto& t1, const auto& t2){
		return t1.val() <= t2.val(); 
	};
	auto un_op = [](const auto& t1){
		return t1.val(); 
	}; 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, min_cmp, max_cmp, un_op); 
	}
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//iterator used for the dt 
		auto it = std::next(first1, i); 
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, min_cmp, max_cmp, un_op); 
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = Timestamp<T>{it->dt(), (un_op(*it) - minmax[0].first) / (minmax[0].second - minmax[0].first), ts_name};  
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = Timestamp<T>{it->dt(), std::move(undef), ts_name};  
		}
		first2++; 
	}
}

//rolling mean normalization 
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_mean_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T init, T undef){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get the min & max for the first window 
	std::deque<int> dq_min; 
	std::deque<int> dq_max; 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k); 
	}
	//compute the mean of the first window 
	T m = mean(first1, std::next(first1, k), init);
	//iterator to the discarded value & the position we write to 
	auto itd = first1; 
	auto out = first2; 
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//grab the value
		auto val = *std::next(first1, i); 
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k);
		//compute the mean for the current window
		if(i != k - 1){
			roll_mean_update(m, *itd, val, k);
			//increment the discard iterator 
			itd++; 
		}
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = (val - m) / (minmax[0].second - minmax[0].first); 
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = undef;  
		}
		first2++; 
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
void utility::roll_mean_norm(InputIt first1, InputIt last1, OutputIt first2, int k, T init, T undef, Comp1 comp1, Comp2 comp2, UnaryOp un_op){
	std::vector<std::pair<T, T>> minmax(1);
	//call update k times ==> get the min & max for the first window 
	std::deque<int> dq_min; 
	std::deque<int> dq_max; 
	for(int i = 0; i < k - 1; i++){
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op); 
	}
	//compute the mean of the first window 
	T m = mean(first1, std::next(first1, k), init, un_op);
	//iterator to the discarded value & the position we write to 
	auto itd = first1; 
	//compute the normalized values
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//grab the value
		T val = un_op(*std::next(first1, i)); 
		//compute min & max for the current window
		roll_minmax_update(first1, minmax.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op);
		//compute the mean for the current window
		if(i != k - 1){
			roll_mean_update(m, un_op(*itd), val, k); 
			//increment the discard iterator 
			itd++; 
		}
		//normalize & write the value
		if(minmax[0].first != minmax[0].second){
			//if min and max are not the same value the denominator is not zero 
			*first2 = (val - m) / (minmax[0].second - minmax[0].first); 
		}else{
			//if the denominator is zero we set the value to be the undefined value (should be large value for type T) 
			*first2 = undef; 
		}
		first2++; 
	}
}


//rolling robust scaling (single pass) 
template <typename InputIt, typename OutputIt, typename T> 
void utility::roll_robust_scale(InputIt first1, InputIt last1, OutputIt first2, int k, T undef){
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::tuple<T, T, T>> qrt(1);
	//call roll_quartiles_update k - 1 times to fill the deques so that the next call produces a tuple of quartiles
	for(int i = 0; i < k - 1; i++){
		//use the undefined values as the example for the T type in the roll_quartiles_update call
		roll_quartiles_update(first1, qrt.begin(), dq_min, dq_max, i, k, undef); 
	}
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//get the iterator to the current element of [first1, last)
		auto it = std::next(first1, i); 
		//call roll_quartiles_update
		roll_quartiles_update(first1, qrt.begin(), dq_min, dq_max, i, k, undef);
		//compute & write the normalized values
		if(std::get<0>(qrt[0]) != std::get<2>(qrt[0])){
			*first2 = (*it - std::get<1>(qrt[0])) / (std::get<2>(qrt[0]) - std::get<0>(qrt[0])); 
		}else{
			*first2 = undef;
		}
		first2++;
	}
}
template <typename InputIt, typename OutputIt, typename T, typename Comp1, typename Comp2, typename UnaryOp> 
void utility::roll_robust_scale(InputIt first1, InputIt last1, OutputIt first2, int k, Comp1 comp1, Comp2 comp2, UnaryOp un_op, T undef){
	std::deque<int> dq_min; 
	std::deque<int> dq_max;
	std::vector<std::tuple<T, T, T>> qrt(1);
	//call roll_quartiles_update k - 1 times to fill the deques so that the next call produces a tuple of quartiles
	for(int i = 0; i < k - 1; i++){
		//use the undefined values as the example for the T type in the roll_quartiles_update call
		roll_quartiles_update(first1, qrt.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op, undef); 
	}
	for(int i = k - 1; i < std::distance(first1, last1); i++){
		//get the iterator to the current element of [first1, last)
		auto it = std::next(first1, i); 
		//call roll_quartiles_update
		roll_quartiles_update(first1, qrt.begin(), dq_min, dq_max, i, k, comp1, comp2, un_op, undef);
		if(i == k - 1){
			std::cout << "first set of quartiles: " << std::endl; 
			std::cout << "q1: " << std::get<0>(qrt[0]) << " q2: " << std::get<1>(qrt[0]) << " q3: " << std::get<2>(qrt[0])  << std::endl;  
		}
		//compute & write the normalized values
		if(std::get<0>(qrt[0]) != std::get<2>(qrt[0])){
			*first2 = (un_op(*it) - std::get<1>(qrt[0])) / (std::get<2>(qrt[0]) - std::get<0>(qrt[0])); 
		}else{
			*first2 = undef;
		}
		first2++;
	}
}

//fill a range of timestamps from the iterator range [first1, last1) using datetimes from objects with datetimes that start at dt_first
//T is the type in the iterator range 
template <typename DtStartIt, typename InputIt, typename OutputIt, typename T> 
void utility::timestamp_zip(InputIt first1, InputIt last1, DtStartIt dt_first, OutputIt first2, const std::string& var, T ex){
	auto create_ts = [&dt_first, &first1, &var](const auto& x){
		return Timestamp<T>{std::next(dt_first, std::distance(&(*first1), &x))->dt(), std::move(x), var};
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, create_ts);
}
template <typename DtStartIt, typename InputIt1, typename InputIt2, typename OutputIt, typename BinOp, typename T> 
void utility::timestamp_zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, DtStartIt dt_first, OutputIt first3, BinOp bin_op, const std::string& name, T ex){
	auto create_ts = [&dt_first, &first1, &name, &bin_op](const auto& z1, const auto& z2){
		return std::move(Timestamp<T>{std::next(dt_first, std::distance(&(*first1), &z1))->dt(), bin_op(z1, z2), name}); 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, first3, create_ts); 
}
template <typename InputIt1, typename InputIt2, typename OutputIt, typename BinOp, typename T> 
void utility::timestamp_zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt first3, BinOp bin_op, const std::string& name, T ex){
	auto create_ts = [&first1, &name, &bin_op](const auto& z1, const auto& z2){
		return std::move(Timestamp<T>{z1.dt(), bin_op(z1, z2), name}); 
	};
	std::transform(std::execution::par_unseq, first1, last1, first2, first3, create_ts); 
}

template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::bb_timestamp(InputIt itc, OutputIt write, T m, T std, T nstd, const std::string& ts_name, UnaryOp un_op){
	//compute the lower and upper bands
	T lb = m - (nstd * std);
	T ub = m + (nstd * std);
	//compute the band value (default is .5 since this corresponds to the middle of the bands)
	T bb = .5;
	if(lb != ub){
		bb = ((un_op(*itc) - lb) / (ub - lb));  
	}
	//write the bollinger band timestamp 
	*write = Timestamp<T>{itc->dt(), std::move(bb), ts_name};
}
template <typename InputIt, typename OutputIt, typename T> 
void utility::rsi_timestamp(InputIt it, OutputIt write, T mu, T md, const std::string& ts_name){
	if((mu + md) == 0.0){
		//avoid dividing by zero and use the default value of 50
		*write = Timestamp<T>{it->dt(), 50.0, ts_name};
	}else{
		*write = Timestamp<T>{it->dt(), 100 - ((100*md) / (mu + md)), ts_name};
	}
}
template <typename InputIt, typename OutputIt, typename T, typename UnaryOp> 
void utility::cci_timestamp(InputIt it, OutputIt write, T m, int k, T w, UnaryOp un_op, T init, const std::string& ts_name){
	T denom = w * utility::mean_abs_diff(it, m, k, un_op, init);
	if(denom != 0.0){
		*write = Timestamp<T>{it->dt(), (un_op(*it) - m) / denom, ts_name};
	}else{
		//if the denominator is undefined use a value of 0.0 
		*write = Timestamp<T>{it->dt(), 0.0, ts_name}; 
	}
}
template <typename F> 
void utility::find_positions(const std::string& s, F f, std::vector<int>& positions){
	if(s.find(f) != std::string::npos){
		positions.push_back(s.find(f)); 
	}
	int i = 0; 
	while(s.find(f, positions[i] + 1) != std::string::npos){
		positions.push_back(s.find(f, positions[i] + 1));
		i++; 
	}
}
//ensure_exists creates the parent directories of the file in the path
template <typename S> 
void utility::ensure_exists(S path){
	std::filesystem::path p(path);
	if(!std::filesystem::exists(p)){
		std::filesystem::path directory = p;
		if(p.has_filename()){
			directory = p.remove_filename(); 
		}
		//call create directories (using the throwing overload)
		std::filesystem::create_directories(directory); 
	}
}
template <typename FileStream, typename S> 
void utility::fio_open(FileStream& fs, S path){
	fs.open(path); 
	if(!fs.is_open()){
		throw std::runtime_error("fio_open: Could not open file"); 
	}
}
template <typename S> 
void utility::fout_open(std::ofstream& ofs, S path){
	//call ensure_exists with the path
	utility::ensure_exists(path);
	ofs.open(path);
	if(!ofs.is_open()){
		throw std::runtime_error("fout_open: Could not open the file."); 
	}
}
template <typename InputIt> 
InputIt utility::find_iterator_to_dt(InputIt it, InputIt last, const Datetime& dt, bool forwards){
	if(forwards){
		for(it; it != last; it++){
			if(it->dt() == dt){
				return it; 
			}
		}
	}else{
		for(it; it != last; it--){
			if(it->dt() == dt){
				return it; 
			}
		}
	}
	throw std::runtime_error("find_iterator_to_dt: Datetime dt did not match any of the .dt() call outputs for objects in the iterator range."); 
}
template <typename... ItPairs> 
void utility::sync_iterators(std::tuple<ItPairs...>& iterator_pairs){
	auto indices = std::make_index_sequence<sizeof ...(ItPairs)>{};
	utility::sync_iterators_impl(iterator_pairs, indices); 
}
//assumes that the second iterators in the pairs are .end() iterators
template <size_t... Is, typename... ItPairs> 
void utility::sync_iterators_impl(std::tuple<ItPairs...>& iterator_pairs, std::index_sequence<Is...> indices){
	Datetime lf_dt = std::get<0>(iterator_pairs).first->dt();  
	Datetime fl_dt = std::prev(std::get<0>(iterator_pairs).second)->dt(); 
	auto dt_cmp = [&lf_dt, &fl_dt](const auto& p){
		if(p.first->dt() > lf_dt){
			lf_dt = p.first->dt(); 
		}
		if(std::prev(p.second)->dt() < fl_dt){
			fl_dt = std::prev(p.second)->dt(); 
		}
	}; 
	((dt_cmp(std::get<Is>(iterator_pairs))), ...);
	auto sync = [&lf_dt, &fl_dt](auto& p){
		p.first = utility::find_iterator_to_dt(p.first, p.second, lf_dt); 	
		p.second = utility::find_iterator_to_dt(std::prev(p.second), p.first, fl_dt, false);
	};
	((sync(std::get<Is>(iterator_pairs))), ...);
}
template <typename T, typename... ItPairs, typename... Fcns> 
void utility::generate_matrix(const std::tuple<ItPairs...>& iterator_pairs, std::tuple<Fcns...>& fcns, arma::Mat<T>& matrix){
	auto indices = std::make_index_sequence<sizeof ...(ItPairs)>{}; 
	utility::generate_matrix_impl(iterator_pairs, fcns, indices, matrix); 	

}
template <size_t... Is, typename T, typename... ItPairs, typename... Fcns> 
void utility::generate_matrix_impl(const std::tuple<ItPairs...>& iterator_pairs, std::tuple<Fcns...>& fcns, std::index_sequence<Is...> indices, arma::Mat<T>& matrix){
	auto fill_row = [&matrix](const auto& p, const auto& f, int r){
		auto it = p.first; 
		for(int i = 0; i < std::distance(p.first, p.second); i++){
			matrix.at(r, i) = f(*it); 
			it++; 
		}
	};
	//fold expression
	((fill_row(std::get<Is>(iterator_pairs), std::get<Is>(fcns), Is)), ...); 
}
template <typename InputIt, typename L> 
void utility::generate_labels(const std::pair<InputIt, InputIt>& it_pair,  arma::Row<L>& labels){
	int i = 0; 
	for(auto it = it_pair.first; it != std::next(it_pair.second); it++){
		labels.at(0, i) = *it;
		i++; 
	}
}
template <typename InputIt, typename L, typename UnaryOp> 
void utility::generate_labels(const std::pair<InputIt, InputIt>& it_pair, arma::Row<L>& labels, UnaryOp un_op){
	int i = 0; 
	for(auto it = it_pair.first; it != it_pair.second; it++){
		labels.at(0, i) = un_op(*it);
		i++; 
	}
}
template <typename LabelItPair, typename... ItPairs> 
void utility::sync_matrix_labels_iterators(std::tuple<ItPairs...>& mat_it_pairs,  LabelItPair& lab_it_pair){
	auto lab_it_pair_tup = std::make_tuple(lab_it_pair);
	//concatenating the tuples 
	auto it_pairs = std::tuple_cat(mat_it_pairs, lab_it_pair_tup);
	//calling sync_iterators 
	utility::sync_iterators(it_pairs);
	//split the tuple 
	utility::tuple_split(it_pairs, mat_it_pairs, lab_it_pair_tup);
	lab_it_pair = std::get<0>(lab_it_pair_tup); 
}
template <typename... ItPairs> 
void utility::sync_matrix_cs_iterators(std::tuple<ItPairs...>& mat_it_pairs, std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>& csitp){
	utility::sync_iterators(mat_it_pairs);
	Datetime lf_dt = std::get<0>(mat_it_pairs).first->dt(); 
	Datetime fl_dt = std::get<0>(mat_it_pairs).second->dt();
	auto out = csitp; 
	for(auto it = csitp.first; it != csitp.second; it++){
		if(*(it->dt()) == lf_dt){
			out.first = it;  
		}
	}
	for(auto it = csitp.second; it != csitp.first; it--){
		if(*(it->dt()) == fl_dt){
			out.second = it;  
		}
	}
}
template <typename... ItPairs> 
void utility::sync_matrix_cs_iterators(std::tuple<ItPairs...>& mat_it_pairs, std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>& csitp){
	utility::sync_matrix_labels_iterators(mat_it_pairs, csitp); 
}

template <typename T, typename It>
void utility::split_matrix(const arma::Mat<T>& mat, std::vector<arma::Mat<T>>& mat_splits, const std::pair<It, It> flc_its, std::vector<std::pair<It, It>>& flc_its_splits, int k){
	int n_in_split = mat.n_cols / k;
	for(int i = 0; i < k; i++){
		if(i != k - 1){
			mat_splits.push_back(mat.cols(i*n_in_split, (i + 1)*n_in_split - 1));
			flc_its_splits.push_back(std::make_pair(std::next(flc_its.first, (i*n_in_split)), std::next(flc_its.first, (i + 1)*n_in_split - 1))); 
		}else{
			mat_splits.push_back(mat.cols(i*n_in_split, mat.n_cols - 1)); 
			flc_its_splits.push_back(std::make_pair(std::next(flc_its.first, (i*n_in_split)), flc_its.second)); 
		}
	}
}
template <typename T, typename It> 
void utility::split_matrix(const arma::Mat<T>& mat, std::vector<arma::Mat<T>>& mat_splits, const std::pair<It, It> unsynced_flc_its, const std::pair<It, It>& synced_flc_its, 
		std::vector<std::pair<It, It>>& flc_its_splits, int k){
	int n_cols = std::distance(synced_flc_its.first, synced_flc_its.second);  
	int n_in_split = n_cols / k;
	int n_offset = std::distance(unsynced_flc_its.first, synced_flc_its.first); 
	for(int i = 0; i < k; i++){
		if(i != k - 1){
			mat_splits.push_back(mat.cols(n_offset + (i*n_in_split), n_offset + ((i + 1)*n_in_split) - 1));
			flc_its_splits.push_back(std::make_pair(std::next(synced_flc_its.first, i*n_in_split), std::next(synced_flc_its.first, (i + 1)*n_in_split) - 1)); 
		}else{
			mat_splits.push_back(mat.cols(n_offset + (i*n_in_split), n_offset + n_cols)); 
			flc_its_splits.push_back(std::make_pair(std::next(synced_flc_its.first, (i*n_in_split)), synced_flc_its.second)); 
		}
	}
}
template <std::size_t ...Is1, std::size_t ...Is2, typename T, typename ...Its> 
void utility::split_matrices(const std::vector<arma::Mat<T>>& mats, std::vector<std::vector<arma::Mat<T>>>& mats_splits, const std::tuple<std::pair<Its, Its>...>& flc_its, 
		std::vector<std::tuple<std::pair<Its, Its>...>>& flc_its_splits, std::index_sequence<Is1...> idx1, std::index_sequence<Is2...> idx2){
	//sync the iterators
	auto synced_flc_its = flc_its; 
	utility::sync_iterators(synced_flc_its); 
	//each vector of mat_splits_temp is the splits of the corresponding element of mats
	std::vector<std::vector<arma::Mat<T>>> mat_splits_temp(idx2.size()); 
	auto tup_of_vecs = utility::vectors_from_tuples(flc_its); 
	((utility::split_matrix(mats[Is2], mat_splits_temp[Is2], std::get<Is2>(flc_its), std::get<Is2>(synced_flc_its), std::get<Is2>(tup_of_vecs), idx1.size()), ...));
	//Note: each vector in mats splits temp corresponds to an asset
	//ith element of mats_splits ought to be a vector of the ith splits of each matrix in mats 
	//need to go from a tuple of vectors to a vector of tuples where the ith element is the tuple of the ith element from each vector
	mats_splits.resize(idx1.size()); 
	for(int i = 0; i < idx1.size(); i++){
		mats_splits[i].resize(idx2.size()); 
		for(int j = 0; j < idx2.size(); j++){
			mats_splits[i][j] = mat_splits_temp[j][i]; 
		}
		flc_its_splits.push_back(std::make_tuple(std::get<Is2>(tup_of_vecs)[i]...)); 
	}
}
template <typename T> 
void utility::range_based_to_string(const T& input, std::string& out, char left, char sep, char right){ 
	int i = 0;
	out += left;
	int n = std::distance(input.begin(), input.end()); 
	for(auto it = input.begin(); it != input.end(); it++){
		if(i < n - 1){
			out += (std::to_string(*it) + sep);
		}else{
			out += (std::to_string(*it));
		}
		i++; 
	}
	out += right; 
}
template <typename F> 
std::string utility::float_format(F f, int k){
	return std::format("{:.{}f}", f, k); 
}
template <typename I> 
std::string utility::integral_format(I i){
	return std::format("{:d}", i); 
}
template <typename... Types1, typename... Types2>
void utility::row_print(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, const std::vector<int>& col_widths, char col_sep){
	auto indices = std::make_index_sequence<sizeof ...(Types1)>{};
	utility::row_print_impl(row_tup, fcn_tup, indices, col_widths, col_sep); 
}
template <size_t... Is, typename... Types1, typename... Types2> 
void utility::row_print_impl(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, std::index_sequence<Is...>, const std::vector<int>& col_widths, char col_sep){
	std::cout << col_sep; 
	((std::cout << std::left << std::setw(col_widths[Is]) << std::get<Is>(fcn_tup)(std::get<Is>(row_tup)) << col_sep), ...);  
}
template <typename... Types1, typename... Types2, typename... Types3, typename... Types4>
void utility::table_print(const std::tuple<Types1...> headers, const std::tuple<Types2...> header_fcns, const std::vector<std::tuple<Types3...>>& rows, const std::tuple<Types4...> row_fcns,  
		const std::vector<int>& col_widths, char head_sep, char row_sep, char corner, char col_sep){
	//top line 
	utility::row_separator_print(col_widths, row_sep, row_sep);
	std::cout << std::endl; 
	//print the headers 
	utility::row_print(headers, header_fcns, col_widths, col_sep);
	std::cout << std::endl; 
	//print the header separating line 
	utility::row_separator_print(col_widths, head_sep, head_sep);
	std::cout << std::endl; 
	//print the rest of the table
	for(int i = 0; i < rows.size() - 1; i++){
		utility::row_print(rows[i], row_fcns, col_widths, col_sep);
		std::cout << std::endl; 
		utility::row_separator_print(col_widths, row_sep, corner);
		std::cout << std::endl; 
	}
	utility::row_print(rows.back(), row_fcns, col_widths, col_sep);
	std::cout << std::endl; 
	utility::row_separator_print(col_widths, row_sep, row_sep);
	std::cout << std::endl; 
}

//use these in lambdas and pass to table_latex's fcn_tup argument 
template <typename F> 
std::string utility::float_cell(F f, int k){
	return "$" + std::format("{:.{}f}", f, k) + "$"; 
}
template <typename I> 
std::string utility::integral_cell(I i){
	return "$" + std::format("{:d}", i) + "$"; 
}

template <typename... Types1, typename... Types2>
std::string utility::row_latex(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup){
	auto indices = std::make_index_sequence<sizeof ...(Types1) - 1>{};
	std::string latex = std::get<0>(fcn_tup)(std::get<0>(row_tup)) + " "; 	
	latex += utility::row_latex_impl(row_tup, fcn_tup, indices);
	latex += "\\\\ \\hline ";
	return latex; 
}

template <size_t... Is, typename... Types1, typename... Types2>
std::string utility::row_latex_impl(std::tuple<Types1...> row_tup, std::tuple<Types2...> fcn_tup, std::index_sequence<Is...> indices){
	std::string latex;
	((latex += ("& " + std::get<Is + 1>(fcn_tup)(std::get<Is + 1>(row_tup))) + "  "), ...); 
	return latex; 
}

//table alignment is passed to /begin{}
//col alignment is used as  /begin{tabu}{col_alignment}
template <typename... Types1, typename... Types2, typename... Types3, typename... Types4> 
std::string utility::table_latex(const std::tuple<Types1...> headers, const std::tuple<Types2...> header_fcns, const std::vector<std::tuple<Types3...>>& rows, const std::tuple<Types4...> row_fcns,
		const std::string& table_alignment, const std::string& col_alignment){
	std::string latex;  
	latex += ("\\begin{" + table_alignment + "} \n"); 
	latex += ("\\begin{tabu}{" + col_alignment + "} \\hline \n");
	latex += (utility::row_latex(headers, header_fcns) + "\\hline  \n");
	for(auto r : rows){
		latex += (utility::row_latex(r, row_fcns) + " \n");
	}
	latex += "\\end{tabu} \n"; 
	latex += ("\\end{" + table_alignment + "} \n"); 
	return latex; 	
}

template <typename TupleT> 
void utility::tuple_print(TupleT tup, char left, char sep, char right){
	auto apply_fcn = [left, sep, right](const auto& first, const auto&... other_args){
		std::cout << left << first; 
		((std::cout << sep << " " << other_args), ...); 
		std::cout << right; 
	}; 
	std::apply(apply_fcn, tup); 
}
template <typename TupleT> 
std::string utility::tuple_string(TupleT tup, char left, char sep, char right){
	std::string out = ""; 
	auto apply_fcn = [&out, left, sep, right](const auto& first, const auto&... other_args){
		out += (left + std::to_string(first)); 
		((out += (sep + (" " + std::to_string(other_args)))), ...); 
		out += right; 
	}; 
	std::apply(apply_fcn, tup); 
	return out; 
}
template <std::size_t N, std::size_t... Is> 
std::index_sequence<(N + Is)...> utility::offset(std::index_sequence<Is...>){
	return {}; 
}; 
template <std::size_t Offset, std::size_t N> 
auto utility::make_index_sequence_with_offset(){
	return offset<Offset>(std::make_index_sequence<N>{}); 
}
template <typename Tuple, std::size_t... Is> 
auto utility::tuple_select(Tuple tup, std::index_sequence<Is...>){
	return std::make_tuple(std::get<Is>(std::forward<Tuple>(tup))...); 
}
template <typename... Types1, typename... Types2, typename... Types3> 
void utility::tuple_split(const std::tuple<Types1...>& tup_in, std::tuple<Types2...>& tup_out1, std::tuple<Types3...>& tup_out2){
	auto idx1 = std::make_index_sequence<sizeof ...(Types2)>{};
	auto idx2 = utility::make_index_sequence_with_offset<sizeof ...(Types2), sizeof ...(Types3)>();
	tup_out1 = utility::tuple_select(tup_in, idx1);
	tup_out2 = utility::tuple_select(tup_in, idx2);
}
template <typename ...Types> 
auto utility::sub_tuples_1_to_n(const std::tuple<Types...>& tup){
	auto indices = std::make_index_sequence<sizeof ...(Types)>{}; 
	return utility::sub_tuples_1_to_n_impl(tup, indices); 
}
template <std::size_t... Is, typename Tuple> 
auto utility::sub_tuples_1_to_n_impl(Tuple tup, std::index_sequence<Is...> indices){
	return std::make_tuple(utility::tuple_select(tup, std::make_index_sequence<1 + Is>{})...); 
}
template <typename ...Types, typename V> 
std::vector<std::tuple<Types...>> utility::validate_tuples(const std::vector<std::tuple<Types...>>& vec_of_tuples, V validator, int n){
	std::vector<std::tuple<Types...>> output;
	output.reserve(n);
	for(auto tp : vec_of_tuples){
		if(output.size() == n){
			return output; 
		}else{
			if(validator(tp)){
				output.push_back(tp); 
			}
		}
	}
	return output; 
}
template <typename ...Types> 
std::vector<std::tuple<Types...>> utility::validate_tuples(const std::vector<std::tuple<Types...>>& vec_of_tuples, std::function<bool (const std::tuple<Types...>&)> validator){
	std::vector<std::tuple<Types...>> output;
	output.reserve(vec_of_tuples.size()); 
	for(auto tp : vec_of_tuples){
		if(validator(tp)){
			output.push_back(tp); 
		}
	}
	return output; 
}
template <typename ...Types> 
void utility::normalize_tuple(std::tuple<Types...>& tup){
	auto indices = std::make_index_sequence<sizeof ...(Types)>{};
	//call normalize sub tuple with an index_sequence of all the indices
	utility::normalize_sub_tuple(tup, indices); 
}
template <std::size_t ...Is, typename ...Types> 
void utility::normalize_sub_tuple(std::tuple<Types...>& tup, std::index_sequence<Is...>& indices){
	//compute the sum of the tuple elements to be normalized
	double sum = 0.0;
	((sum += (double)std::get<Is>(tup)), ...);
	//if the sum equals zero then we dont do anything
	if(sum != 0){
		((std::get<Is>(tup) /= sum), ...); 
	}
}
template <typename T, typename ...Types> 
void utility::tuple_to_vector(const std::tuple<Types...>& tuple, std::vector<T>& out){
	auto indices = std::make_index_sequence<sizeof ...(Types)>{};
	utility::sub_tuple_to_vector(tuple, out, indices); 
}
template <typename T, size_t ...Is, typename ...Types> 
void utility::sub_tuple_to_vector(const std::tuple<Types...>& tuple, std::vector<T>& out, const std::index_sequence<Is...>& indices){
	out.resize(sizeof ...(Is)); 
	((out[Is] = (T)(std::get<Is>(tuple))), ...); 
}
template <typename ...Types> 
auto utility::triple_first_elements(const std::tuple<std::tuple<Types, Types, Types>...>& tup){
	auto indices = std::make_index_sequence<sizeof ...(Types)>{};
	return triple_first_elements_impl(tup, indices); 
}
template <std::size_t... Is, typename Tuple> 
auto utility::triple_first_elements_impl(Tuple tup, std::index_sequence<Is...> indices){
	return std::make_tuple(std::get<0>(std::get<Is>(tup))...); 
}

template <typename ...Types> 
std::tuple<std::vector<Types>...> utility::vectors_from_tuples(const std::tuple<Types...>& tup){
	return std::make_tuple(std::vector<Types>()...); 
}
template <typename ...Types> 
std::tuple<std::vector<Types>...> utility::vectors_from_tuple_of_triples(const std::tuple<std::tuple<Types, Types, Types>...>& tup){
	return std::make_tuple(std::vector<Types>()...); 
}

//Methods for creating grids to be used in GridSearch 

template <size_t... Is, typename ...PointTypes> 
void utility::discretization_helper(auto& tuple_of_vecs, const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...> indices){
	auto f = [](auto& cv, auto& pv, auto& bdt){
		cv = utility::discretization(pv, bdt);
		pv.clear(); 
	}; 
	((f(std::get<Is + 2>(tuple_of_vecs), std::get<Is + 1>(tuple_of_vecs), std::get<Is + 2>(bound_disc_tuples))), ...);
}
//Note: will return an empty vector if the lower bound given is larger than the upper bound given
template <typename T>
std::vector<T> utility::discretization(const std::tuple<T, T, T>& bnd_disc_tup){
	//first point is the lower bound
	T point = std::get<0>(bnd_disc_tup); 
	std::vector<T> points;
	points.reserve((int)((std::get<1>(bnd_disc_tup) - std::get<0>(bnd_disc_tup)) / std::get<2>(bnd_disc_tup)) + 1);
	//while the point is below the upper bound add it to points and add the discretization step 
	while(point < std::get<1>(bnd_disc_tup)){
		points.push_back(point);
		//add the discretization step
		point += std::get<2>(bnd_disc_tup); 
	}
	//push the upper bound onto the back of points
	points.push_back(std::get<1>(bnd_disc_tup));
	return points; 
}
template <typename T1, typename T2> 
std::vector<std::tuple<T1, T2>> utility::discretization(const std::tuple<T1, T1, T1>& bd_tup1, const std::tuple<T2, T2, T2>& bd_tup2){
	auto v1 = utility::discretization(bd_tup1);
	auto v2 = utility::discretization(bd_tup2);
	return utility::cartesian_product(v1, v2); 
}
template <typename ...PointTypes, typename T>
std::vector<std::tuple<PointTypes..., T>> utility::discretization(std::vector<std::tuple<PointTypes...>>& points, const std::tuple<T, T, T>& bound_disc_tuple){
	auto v = utility::discretization(bound_disc_tuple);
	return utility::cartesian_product(points, v); 
}
template <typename ...PointTypes> 
std::vector<std::tuple<PointTypes...>> utility::discretization_sample(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, int n){
	auto indices = std::make_index_sequence<sizeof ...(PointTypes)>{};
	return utility::discretization_sample_impl(bound_disc_tuples, indices, n);  
}
template <std::size_t ...Is, typename ...PointTypes> 
std::vector<std::tuple<PointTypes...>> utility::discretization_sample_impl(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...>& indices, int n){
	auto fcn = [](const auto& bdt){
		//the number of discretization steps between the upper and lower bound
		int n = (std::get<1>(bdt) - std::get<0>(bdt)) / std::get<2>(bdt); 	
		int n_steps = arma::randi<int>(arma::distr_param(0, n));
		return (std::get<0>(bdt) + (n_steps * std::get<2>(bdt))); 
	};
	std::vector<std::tuple<PointTypes...>> output(n);
	for(auto& tp : output){
		tp = std::make_tuple(fcn(std::get<Is>(bound_disc_tuples))...); 
	}
	return output; 
}

template <typename ...PointTypes> 
std::vector<std::tuple<PointTypes...>> utility::discretization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples){
	//get a tuple of the first elements of all tuples in bound_disc_tuples 
	auto first_ele_tup = utility::triple_first_elements(bound_disc_tuples);
	//get subtuples from first_ele_tup of with element sets {1}, {1, 2}, ..., {1, 2, 3, ..., N}
	auto sub_tuples = utility::sub_tuples_1_to_n(first_ele_tup);
	//tuple of vectors filled with the each sub_tuple type
	auto tuple_of_vecs = utility::vectors_from_tuples(sub_tuples);
	//make & assign the first cartesian product
	std::get<1>(tuple_of_vecs) = utility::discretization(std::get<0>(bound_disc_tuples), std::get<1>(bound_disc_tuples));
	//auto indices = utility::make_index_sequence_with_offset<2, sizeof ...(PointTypes)>;
	auto indices = std::make_index_sequence<sizeof ...(PointTypes) - 2>{}; 
	utility::discretization_helper(tuple_of_vecs, bound_disc_tuples, indices);
	return std::get<sizeof ...(PointTypes) - 1>(tuple_of_vecs); 
}

template <typename T1, typename T2> 
std::vector<std::tuple<T1, T2>> utility::cartesian_product(const std::vector<T1>& points1, const std::vector<T2>& points2){
	std::vector<std::tuple<T1, T2>> cart_prod; 
	cart_prod.reserve(points1.size() * points2.size()); 
	for(auto p1 : points1){
		for(auto p2 : points2){
			cart_prod.push_back(std::make_tuple(p1, p2)); 
		}
	}
	return cart_prod; 
}
template <typename ...Types1, typename T> 
std::vector<std::tuple<Types1..., T>> utility::cartesian_product(const std::vector<std::tuple<Types1...>>& points1, const std::vector<T>& points2){
	std::vector<std::tuple<Types1..., T>> cart_prod;
	cart_prod.reserve(points1.size() * points2.size());
	for(auto p1 : points1){
		for(auto p2 : points2){
			cart_prod.push_back(std::tuple_cat(p1, std::make_tuple(p2))); 
		}
	}
	return cart_prod; 
}
//cartesian product (concatenates every tuple in points1 with every tuple in points2 and writes the output to the tuples in ouput)
template <typename ...Types1, typename ...Types2> 
std::vector<std::tuple<Types1..., Types2...>> utility::cartesian_product(const std::vector<std::tuple<Types1...>>& points1, const std::vector<std::tuple<Types2...>>& points2){
	std::vector<std::tuple<Types1..., Types2...>> cart_prod; 
	cart_prod.reserve(points1.size() * points2.size()); 
	for(auto p1 : points1){
		for(auto p2 : points2){
			cart_prod.push_back(std::tuple_cat(p1, p2)); 
		}
	}
	return cart_prod; 
}

template <typename ...PointTypes> 
std::vector<std::tuple<PointTypes...>> utility::initialization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, int n){
	//set the seed randomly so that individuals selected vary from call to call
	arma::arma_rng::set_seed_random();
	//create the initial population
	return utility::discretization_sample(bound_disc_tuples, n); 
}
template <std::size_t ...Is, typename ...PointTypes> 
std::vector<std::tuple<PointTypes...>> utility::initialization(const std::tuple<std::tuple<PointTypes, PointTypes, PointTypes>...>& bound_disc_tuples, std::index_sequence<Is...>& indices, int n){
	//set the seed randomly so that individuals selected vary from call to call
	arma::arma_rng::set_seed_random();
	auto init_pop = utility::discretization_sample(bound_disc_tuples, n);
	//normalize the tuple elements with indices in indices
	for(auto& ind : init_pop){
		utility::normalize_sub_tuple(ind, indices);
	}
	return init_pop;  
}
template <typename ...PointTypes> 
std::tuple<PointTypes...> utility::discrete_recombination(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double p){
	arma::arma_rng::set_seed_random(); 
	auto indices = std::make_index_sequence<sizeof ...(PointTypes)>{};
	return utility::discrete_recombination_impl(parent1, parent2, p, indices); 
}
template <size_t ...Is, typename ...PointTypes> 
std::tuple<PointTypes...> utility::discrete_recombination_impl(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double p, std::index_sequence<Is...> indices){
	auto allele_select = [p](const auto& a1, const auto& a2){
		double x = arma::randu<double>(arma::distr_param(0.0, 1.0));
		if(x < p){
			//return the allele from parent 1
			return a1; 
		}else{
			//return the allele from parent 2
			return a2; 
		}
	};
	//pass (seems to unpack the elements backwards) 
	return std::make_tuple(allele_select(std::get<Is>(parent1), std::get<Is>(parent2))...); 
}

template <typename ...PointTypes> 
std::tuple<PointTypes...> utility::intermediate_recombination(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double beta){
	auto indices = std::make_index_sequence<sizeof ...(PointTypes)>{};
	return utility::intermediate_recombination_impl(parent1, parent2, beta, indices); 
}
template <size_t ...Is, typename ...PointTypes> 
std::tuple<PointTypes...> utility::intermediate_recombination_impl(const std::tuple<PointTypes...>& parent1, const std::tuple<PointTypes...>& parent2, double beta, std::index_sequence<Is...> indices){
	auto allele_form = [beta](const auto& a1, const auto& a2){
		return ((beta * a1) + ((1 - beta) * a2));
	}; 
	return std::make_tuple(allele_form(std::get<Is>(parent1), std::get<Is>(parent2))...); 
}

template <typename FitComp, typename ...PointTypes> 
std::vector<std::tuple<std::tuple<PointTypes...>, std::tuple<PointTypes...>, double>> utility::selection(
			std::vector<std::pair<std::tuple<PointTypes...>, double>>& generation, FitComp fit_comp, double scale){
	//sort the vector of the individual & score pairs (takes in individual score pairs) 
	auto cmp = [fit_comp](const auto& si_1, const auto& si_2){
		return (fit_comp(si_1.second, si_2.second)); 	
	};
	std::sort(generation.begin(), generation.end(), cmp);
	//compute the number of k highest scoring individuals to
	int n = generation.size();
	int k = std::ceil((1 + std::sqrt(1.0 + (4*n))) / 2);
	//declare & reserve memory for the output vector of pairs of selected individuals 
	std::vector<std::tuple<std::tuple<PointTypes...>, std::tuple<PointTypes...>, double>> selected_ind_pairs;
	selected_ind_pairs.reserve(n);
	for(int i = 0; i < k - 1; i++){
		//beta/p used for the first parent 
		double score = ((scale * (n - i)) / n); 
		for(int j = 0; j < k; j++){
			if(i != j){
				selected_ind_pairs.push_back(std::make_tuple(std::get<0>(generation[i]), std::get<0>(generation[j]), score)); 
			}
		}
	}
	//score for the kth strongest individual
	double score = ((scale * (n - (k - 1))) / n);
	int sip_size = selected_ind_pairs.size(); 
	for(int i = 0; i < n - sip_size; i++){
		selected_ind_pairs.push_back(std::make_tuple(std::get<0>(generation[k - 1]), std::get<0>(generation[i]), score)); 
	}
	return selected_ind_pairs; 
}
template <typename FitFcn, typename ...PointTypes> 
std::vector<std::pair<std::tuple<PointTypes...>, double>> utility::generation_fitness(const std::vector<std::tuple<PointTypes...>>& generation, FitFcn fit_fcn, int n_threads){
	int n = generation.size();
	std::vector<std::pair<std::tuple<PointTypes...>, double>> scored_individuals(n);
	auto si_it_beg = scored_individuals.begin(); 
	int i = 0; 
	while(i < n){
		int n_tasks = std::min(n - i, n_threads);
		std::vector<std::thread> vec_of_threads;
		vec_of_threads.reserve(n_tasks);
		for(int k = 0; k < n_tasks; k++){
			vec_of_threads.push_back(std::thread([si_it_beg, fit_fcn, &generation, i, k](){*std::next(si_it_beg, i + k) = std::make_pair(generation[i + k], fit_fcn(generation[i + k]));}));}
		for(auto& th : vec_of_threads){
			th.join(); 
		}
		vec_of_threads.clear(); 
		i += n_tasks; 
	}
	return scored_individuals; 
}
template <typename T> 
std::size_t utility::vec_hash<T>::operator()(const std::vector<T>& v) const{
	std::size_t seed;
	for(T t : v){
		seed ^= std::hash<T>{}(t) + 0x9e3779b9 + (seed << 6); 
	}
	return seed; 
}
template <typename T> 
void utility::fix_duplicates(arma::Mat<T>& matrix, T pert){
	//Note: Not ideal since we copy the each arma::Col to a std::vector and then use vec hash

	std::unordered_map<std::vector<T>, std::vector<int>, vec_hash<T>> um;
	for(int i = 0; i < matrix.n_cols; i++){
		std::vector<T> v = arma::conv_to<std::vector<T>>::from(matrix.col(i));
		um[v].push_back(i); 
	}
	arma::arma_rng::set_seed_random();
	for(auto it = um.begin(); it != um.end(); it++){
		if(it->second.size() > 1){
			for(auto i : it->second){
				arma::Col<T> pert_v = arma::randu<arma::Col<T>>(matrix.n_rows, arma::distr_param(-1*pert, pert));
				matrix.col(i) += pert_v; 
			}
		}
	}
}

