#include <numeric>
#include <random>
#include <algorithm>
#include <stack> 
//Note: you need to have downloaded the gnuplot-iostream header file and placed it in the folder gnuplot_iostream in 
//the same directory as qfinapack
#include "../../gnuplot_iostream/gnuplot-iostream.h"
#include "../Utility/utility.h"
#include "../Portfolio/OptimalPortfolio.h"
#include "../CandlestickPatterns/cand_pat.h"
#include "../CandleSeries/CandleSeries.h" 
#include "../Asset/Forex.h"
#include "../Asset/Equity.h"
#include "../Asset/Bond.h"

namespace backtest{
	/*
		Order Type Classes
		InputIt is an iterator type pointing to the place in the CandleSeries
		PSType is the type of the position size (which should be in the amount of the account currency)
		PriceType is the type used for desired prices on limit orders
	*/
	template <typename InputIt, typename PosSize> 
	class Order{
		public: 
			Order() = default; 
			Order(InputIt it_placed, PosSize position_size);
			void cancel(); 
			void execute(InputIt it_execute);
			InputIt it_placed;
			InputIt it_executed;
			PosSize position_size; 
			bool executed;
			bool cancelled; 
	}; 

	template <typename InputIt, typename PosSize> 
	class MarketBuyOrder : public Order<InputIt, PosSize>{
		public: 
			MarketBuyOrder() = default; 
			MarketBuyOrder(InputIt it_placed_, PosSize position_size);
	};
	template <typename InputIt, typename PosSize, typename PriceType> 
	class LimitBuyOrder : public Order<InputIt, PosSize>{
		public:
			LimitBuyOrder() = default; 
			LimitBuyOrder(InputIt it_placed, PriceType desired_price, PosSize position_size);
			PriceType desired_price; 
	};
	template <typename InputIt, typename PosSize> 
	class MarketSellOrder : public Order<InputIt, PosSize>{
		public:
			MarketSellOrder() = default; 
			MarketSellOrder(InputIt it_placed, PosSize position_size);
	};
	template <typename InputIt, typename PSType, typename PriceType> 
	class LimitSellOrder : public Order<InputIt, PSType>{
		public:
			LimitSellOrder() = default; 
			LimitSellOrder(InputIt it_placed, PriceType desired_price, PSType position_size);
			PriceType desired_price; 
	};
	//Buy stop losses are sell orders
	template <typename InputIt, typename PosSize, typename PriceType> 
	class BuyStopLoss : public LimitSellOrder<InputIt, PosSize, PriceType>{
		public:
			BuyStopLoss() = default; 
			BuyStopLoss(const MarketBuyOrder<InputIt, PosSize>& open, PriceType stop_loss); 
	}; 
	template <typename InputIt, typename PosSize, typename PriceType> 
	class BuyTakeProfit : public LimitSellOrder<InputIt, PosSize, PriceType>{
		public:
			BuyTakeProfit() = default; 
			BuyTakeProfit(const MarketBuyOrder<InputIt, PosSize>& open, PriceType take_profit); 
	}; 
	//Sell Stop losses are buy orders
	template <typename InputIt, typename PosSize, typename PriceType> 
	class SellStopLoss : public LimitBuyOrder<InputIt, PosSize, PriceType>{
		public:
			SellStopLoss() = default; 
			SellStopLoss(const MarketSellOrder<InputIt, PosSize>& open, PriceType stop_loss); 
	}; 
	template <typename InputIt, typename PosSize, typename PriceType> 
	class SellTakeProfit : public LimitBuyOrder<InputIt, PosSize, PriceType>{
		public:
			SellTakeProfit() = default; 
			SellTakeProfit(const MarketSellOrder<InputIt, PosSize>& open, PriceType take_profit); 
	}; 

	/*
		Trade Class 
	*/
	template <typename A, typename TCM> 
	class Trade{
		public:
			template <typename InputIt, typename PosSize, typename T> 
			Trade(A& asset, TCM& tcm, const MarketBuyOrder<InputIt, PosSize>& open, InputIt close_it, T balance); 
			template <typename InputIt, typename PosSize, typename T> 
			Trade(A& asset, TCM& tcm, const MarketSellOrder<InputIt, PosSize>& open, InputIt close_it, T balance); 
			template <typename InputIt, typename PosSize, typename PriceType, typename T> 
			Trade(A& asset, TCM& tcm, const MarketBuyOrder<InputIt, PosSize>& open_order, PriceType stop_loss_price, PriceType take_profit_price, T balance); 
			template <typename InputIt, typename PosSize, typename PriceType, typename T> 
			Trade(A& asset, TCM& tcm, const MarketSellOrder<InputIt, PosSize>& open, PriceType stop_loss_price, PriceType take_profit_price, T balance);
			//Trade constructor which takes & buy and sell order and alters them (used for partial closes)
			template <typename InputIt, typename PosSize, typename T> 
			Trade(A& asset, TCM& tcm, MarketBuyOrder<InputIt, PosSize>& buy, MarketSellOrder<InputIt, PosSize>& sell, T balance); 
			//use the default copy constructor 
			template <typename InputIt, typename PosSize> 
			Trade(const Trade& t); 

			//print the trade results to the cosole
			void display() const;
			//public data members
			Datetime open_dt;
			Datetime close_dt;
			bool win; 
			double profit;
			double percentage_return;
			double log_return;
			double slippage;
	}; 


	/*
		Linear Transaction Cost Model 
	*/
	class LinearTCM{
		public: 
			LinearTCM() = default;
			//factor is used in computing fill price (we add fac * (position size in units / previous volume) * bid ask spread) to the desired price
			//higher factors simulate higher transaction costs
			LinearTCM(double fac);
			//computing the fill prices for each type of order
			template <typename InputIt, typename PosSize> 
			std::tuple<double, double, double> compute_fill_slippage(double ps_units, const MarketBuyOrder<InputIt, PosSize>& buy_to_open, const MarketSellOrder<InputIt, PosSize>& sell_to_close);
			template <typename InputIt, typename PosSize> 
			std::tuple<double, double, double> compute_fill_slippage(double ps_units, const MarketSellOrder<InputIt, PosSize>& sell_to_open, const MarketBuyOrder<InputIt, PosSize>& buy_to_close);
			template <typename InputIt, typename PosSize, typename PriceType> 
			std::tuple<double, double, double> compute_fill_slippage(double ps_units, const MarketBuyOrder<InputIt, PosSize>& buy_to_open, const LimitSellOrder<InputIt, PosSize, PriceType>& sell_to_close);
			template <typename InputIt, typename PosSize, typename PriceType> 
			std::tuple<double, double, double> compute_fill_slippage(double ps_units, const MarketSellOrder<InputIt, PosSize>& sell_to_open, const LimitBuyOrder<InputIt, PosSize, PriceType>& buy_to_close); 
		private:
			double fac_ = 2.0;
			void validate_() const; 
	}; 
	

	/*
		Position Sizing based on the proportion of the account balance
		Note: Position Sizing classes should return an amount of the account currency
	*/
	class BalPropPosSize{
		public: 
			BalPropPosSize() = default;
			BalPropPosSize(double p, double l);
			double compute_position_size(double bal) const;
		private:
			double proportion_ = 0.1;
			double lev_fac_ = 1.0; 
			void validate_() const; 
	};	 
	
	
	//Function for reconcilling open orders (updates the open position size of the MarketBuyOrders and MarketSellOrders) 
	template <typename A, typename TCM, typename InputIt, typename PosSize> 
	void reconcile_orders(const A& asset, const TCM& tcm, std::stack<MarketBuyOrder<InputIt, PosSize>>& buy_orders, std::stack<MarketSellOrder<InputIt, PosSize>>& sell_orders, 
			PosSize& buy_ps_still_open, PosSize& sell_ps_still_open, std::vector<Trade<A, TCM>>& trade_log); 

	//Return a pair of year_month_day objects created from the earliest open_dt object and the oldest close_dt object in the trade_log 
	template <typename A, typename TCM> 
	std::pair<std::chrono::year_month_day, std::chrono::year_month_day> earliest_open_oldest_close(const std::vector<Trade<A, TCM>>& trade_log);
	//Return a pair of year_month_day objects created from the earliest open_dt object and the oldest close_dt object in all of the trade logs 
	template <typename ...A, typename ...TCM> 
	std::pair<std::chrono::year_month_day, std::chrono::year_month_day> earliest_open_oldest_close(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs); 
	template <std::size_t ...Is, typename ...A, typename ...TCM> 
	std::pair<std::chrono::year_month_day, std::chrono::year_month_day> earliest_open_oldest_close_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, std::index_sequence<Is...> indices); 
	//update the map of daily log returns given a trade 
	template <typename A, typename TCM, typename T> 
	void update_daily_log_rets_map(const Trade<A, TCM>& trade, std::map<std::chrono::year_month_day, T>& dlr_map); 
	//Generate an arma::Row<T> containing daily log returns given a trade_log 
	template <typename A, typename T, typename TCM> 
	void daily_log_rets_from_trade_log(const std::vector<Trade<A, TCM>>& trade_log, arma::Row<T>& daily_log_rets); 
	//Version which takes first_open_ymd and last_close_ymd objects 
	template <typename A, typename T, typename TCM> 
	void daily_log_rets_from_trade_log(const std::vector<Trade<A, TCM>>& trade_log, std::chrono::year_month_day first_open_ymd, std::chrono::year_month_day last_close_ymd, arma::Row<T>& daily_log_rets); 
	//Generate an arma::Mat<T> containing daily_log_returns for each asset given a tuple of trade_logs 
	template <typename T, typename ...A, typename ...TCM> 
	void daily_log_rets_from_trade_log(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& daily_log_rets);
	template <typename T, std::size_t ...Is, typename ...A, typename ...TCM>
	void daily_log_rets_from_trade_log_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& daily_log_rets, std::index_sequence<Is...> indices);

	//compute optimal portfolio weights given a tuple of trade logs 
	template <typename T, typename ...A, typename ...TCM> 
	void optimal_weights_from_trade_logs(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Col<T>& optimal_weights, OptimalPortfolio& opt_port);
	//given a total balance and optimal weights compute the balance allocated to each asset
	//assumes that asset_balances is pre-sized
	template <typename T, typename PosSize> 
	void optimal_balances(PosSize total_balance, std::vector<PosSize>& asset_balances, const arma::Col<T>& optimal_weights); 
	
	//given a tuple of trade logs and starting balances compute the new total balance 
	template <typename ...A, typename ...TCM>
	std::vector<double> compute_new_balances(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::vector<double>& starting_balances); 
	template <std::size_t ...Is, typename ...A, typename ...TCM> 
	void compute_new_balances_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, std::vector<double>& balances, std::index_sequence<Is...> indices); 

	/*
		Classes For Computing Stop Losses & Take Profit Levels
	*/
	//Note: The stop_loss_take_profit_backtest function requires SLTP classes to have buy_stop_loss, sell_stop_loss, buy_take_profit & sell_take_profit methods which take an iterator and the 
	//classification probability for the min class regardless of whether or not the use them
	
	//computes stop losses and take profits bases on multiples of the atr (does not use min_class_prob) 
	class ATRSLTP{
		public: 
			ATRSLTP() = default;
			ATRSLTP(int k, double stop_mult, double take_mult);
			template <typename InputIt> 
			double buy_stop_loss(InputIt it, double min_class_prob) const;
			template <typename InputIt> 
			double sell_stop_loss(InputIt it, double min_class_prob) const;
			template <typename InputIt> 
			double buy_take_profit(InputIt it, double min_class_prob) const;
			template <typename InputIt> 
			double sell_take_profit(InputIt it, double min_class_prob) const;  
		private:
			//the number of periods to include in the lookback period when computing the atr 
			int k_ = 15;
			//the number of multiples of the atr above/below current price at which to set the stop loss / take profit
			double stop_mult_ = 1.5;
			double take_mult_ = 3;
			//compute the average of the past k_ true ranges
			template <typename InputIt> 
			double comp_atr_(InputIt it) const; 
	}; 

	/* 
		Functions for performing backtests 
	*/
	//market order backtest places simple market orders when buy_sig_model or sell_sig_model generate positive signals
	//we fill the trade_log
	//T is the type stored in the arma::Mat matrix 
	//BSM is the type of the classification model which generates buy signals 
	//SSM is the type of the classification model which generates sell signals
	//buy_sig_model takes in a column of bsm_mat in a .Classify method
	//sell_sig_model takes in a column of ssm_mat in its .Classify method
	//bs_thresh and ss_thresh are probability thresholds for classifying a column of matrix as the minority class
	//PSA is a type used for computing position sizes
	//TCM is a transaction cost model type
	//start_bal is the starting balance for the backtest
	//It are std::vector<Candle> iterators which point to the positions in the asset's candleseries member corresponding to the first and last column of matrix
	//bsm_mat_it_pair is a pair of iterators to for a vector of Candle or CandlePtr objects which correspond to the first and last columns of bsm_mat. Likewise for ssm_mat. 
	//reverse_on_close specifies if a trade of the opposite type should be opened when a trade is closed
	//L is the label type and min_class is the minority class of the labels
	template <typename A, typename T, typename BSM, typename SSM, typename PSA, typename TCM, typename It1, typename It2, typename L = std::size_t> 
	void market_order_backtest(A& asset, std::vector<Trade<A, TCM>>& trade_log, BSM& buy_sig_model, SSM& sell_sig_model, const arma::Mat<T>& bsm_mat, const arma::Mat<T>& ssm_mat, PSA psa, TCM tcm, 
			const std::pair<It1, It1>& bsm_mat_it_pair, const std::pair<It2, It2>& ssm_mat_it_pair, double start_bal = 20000, bool reverse_on_close = true, 
			double bs_entry_thresh = .65, double bs_exit_thresh = .65, double ss_entry_thresh = .65, double ss_exit_thresh = .65, L bs_mc = 1, L ss_mc = 1); 
	
	//indices should be an index sequence with the size of all the tuples (which themselves should be the same size)
	//Siloed multi-asset market_order backtester (splits start_balance & performs market_order backtest on each asset, buy_sig_model, sell_sig_model, tcm collection)
	template <typename T, typename PSA, size_t ...Is, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename L = std::size_t>
	void siloed_market_order_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
			const std::vector<arma::Mat<T>>& bsm_mats, const std::vector<arma::Mat<T>>& ssm_mats,  PSA psa, const std::tuple<TCM...>& tcms, const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, 
			const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, const std::vector<double>& bs_entry_threshs, const std::vector<double>& bs_exit_threshs, 
			const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs, const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, 
			std::vector<double> start_balances, std::index_sequence<Is...> indices, bool reverse_on_close = true); 
	
	//SLTP is a class which has (buy/sell)stop_loss and (buy/sell)take_profit methods which compute stops and take profit levels given at iterator to the position in the CandleSeries 
	template <typename A, typename T, typename BSM, typename SSM, typename PSA, typename TCM, typename It1, typename It2, typename SLTP, typename L = std::size_t> 
	void stop_loss_take_profit_backtest(A& asset, std::vector<Trade<A, TCM>>& trade_log, BSM& buy_sig_model, SSM& sell_sig_model, const arma::Mat<T>& bsm_mat, const arma::Mat<T>& ssm_mat, 
			PSA psa, TCM tcm, SLTP sltp, const std::pair<It1, It1>& bsm_mat_it_pair, const std::pair<It2, It2>& ssm_mat_it_pair, double start_bal = 20000, double bs_entry_thresh = .65, 
			double bs_exit_thresh = .65, double ss_entry_thresh = .65, double ss_exit_thresh = .65, L bs_mc = 1, L ss_mc = 1); 
	
	//Siloed multi-asset stop_loss_take_profit backtester
	template <typename T, typename PSA, size_t ...Is, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename ...SLTP, typename L = std::size_t>
	void siloed_stop_loss_take_profit_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
			const std::vector<arma::Mat<T>>& bsm_mats, const std::vector<arma::Mat<T>>& ssm_mats, PSA psa, const std::tuple<TCM...>& tcms, const std::tuple<SLTP...>& sltps, 
			const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, const std::vector<double>& bs_entry_threshs, 
			const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs, const std::vector<L>& bs_mcs, 
			const std::vector<L>& ss_mcs, std::vector<double> start_balances, std::index_sequence<Is...> indices); 
	
	//multi-asset backtester which uses OptimalPortfolio and some class to generate expected returns
	//OptimalPortfolio object is creted in the function. We use start_weights to begin
	//we assume bsm_matrices, ssm_matrices, exp_rets_matrices are all of the same size and the corresponding mat_cs_it_pair corresponds to the first and last column of the matrices
	//n_rebal_idx_seq is an index sequence with the size of the number of times you desire to rebalance
	template <std::size_t ...Is, typename T, typename PSA, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename L = std::size_t>
	void optimal_portfolio_market_order_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& rebalanced_weights, std::vector<std::vector<double>>& rebalanced_balances,
			std::vector<Datetime>& rebalance_datetimes, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, const std::vector<arma::Mat<T>>& bsm_matrices, 
			const std::vector<arma::Mat<T>>& ssm_matrices, PSA psa, const std::tuple<TCM...> tcms, const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, 
			const std::vector<double>& bs_entry_threshs, const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs,  
			const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, std::index_sequence<Is...> n_rebal_idx_seq,  
			double balance = 20000, bool reverse_on_close = true, double target_return = .0005, double neg_exp_weight = .15);  
	
	
	template <std::size_t ...Is, typename T, typename PSA, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename ...SLTP, typename L = std::size_t>
	void optimal_portfolio_stop_loss_take_profit_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& rebalanced_weights, std::vector<std::vector<double>>& rebalanced_balances,
			std::vector<Datetime>& rebalance_datetimes, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, const std::vector<arma::Mat<T>>& bsm_matrices, 
			const std::vector<arma::Mat<T>>& ssm_matrices, PSA psa, const std::tuple<TCM...> tcms, const std::tuple<SLTP...> sltps,  
			const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, 
			const std::vector<double>& bs_entry_threshs, const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs,  
			const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, std::index_sequence<Is...> n_rebal_idx_seq,  
			double balance = 20000, double target_return = .0005, double neg_exp_weight = .15);  

	/*
		Helper functions for creating a backtest report	
	*/
	
	//returns a tuple containing the starting balance, final balance, total log return, total percentage return, win percentage, average win, average loss, average win log return, and average loss log return 
	template <typename A, typename TCM> 
	std::tuple<int, double, double, double, double, double, double, double, double, double> basic_performance_metrics(const std::vector<Trade<A, TCM>>& trade_log, double starting_balance);  
	//Compute the sharpe and sortino ratio
	template <typename A, typename TCM> 
	std::pair<double, double> sharpe_and_sortino(const A& asset, const std::vector<Trade<A, TCM>>& trade_log, double target_daily_log_ret = 0.0025, double annual_rfr = 2.5); 
	//Compute the max drawdown in log return given a trade log 
	template <typename A, typename TCM> 
	double maximum_drawdown(const std::vector<Trade<A, TCM>>& trade_log);
	//compute & return an arma::Col<double> object containing the 10th and 90th percentiles of the maximum drawdowns on n_shuffles shuffles of the trade_log 
	template <typename A, typename TCM> 
	arma::Col<double> reshuffled_maximum_drawdown(const std::vector<Trade<A, TCM>>& trade_log, int n_shuffles);
	
	//Generates a latex table with basic backtesting information & prints the console version of the table (concatenates the table latex to the string passed in) 
	template <typename A, typename It> 
	void basic_info_table(std::string& latex, const A& asset, const std::pair<It, It>& start_end_it, int n_trades); 	
	//Generates a latex table with the backtesting performance metrics & prints the console version of the table 
	void performance_metrics_table(std::string& latex, const std::tuple<int, double, double, double, double, double, double, double, double, double>& basic_metrics, 
			const std::pair<double, double>& sharpe_and_sortino, double max_drawdown, const arma::Col<double>& reshuffled_mdd_90th_10th); 
	//Generates an image of the equity curve & saves the image to the path (adds the latex to include the equity curve image)
	template <typename A, typename TCM, typename It> 
	void equity_curve(const std::string& path, std::string& latex, const std::vector<Trade<A, TCM>>& trade_log, const std::pair<It, It>& start_end_it, 
			double starting_balance, double final_balance, int xtics_seconds); 
	/*
		Function for evaluating the backtest performance & generating a latex document for a single asset backtest 
	*/
	//start_end_it is a pair of iterators pointing to objects which have .dt() members representing the beginning and end of the backtest
	//n_shuffles is the number of times trade_log is reshuffled when computing the max drawdown on reshuffled trades 
	//target_daily_log_ret is aptly named & is used in computing the Sortino Ratio (defaulted to a log return corresponding to about .25% per day)
	//double annual_rfr is the annual risk free rate as a percentage (converted to a daily risk free rate from asset type A's n_trading_days & used in computing the Sharpe Ratio)
	//xtics_seconds is the number of seconds between xtics in the equity curve image
	template <typename It, typename A, typename TCM> 
	void single_report(const A& asset, const std::string& tex_path, const std::string& img_path, const std::pair<It, It>& start_end_it, double starting_balance, const std::vector<Trade<A, TCM>>& trade_log, 
			double target_daily_log_ret = 0.0025, double annual_rfr = 2.5, int n_shuffles = 5000, int xtics_seconds = 604800); 
	
	/*
		Function for evaluating the backtest performance of a siloed multi-asset backtest	
	*/
	//Takes in a folder for the img path since we will have to generate a separate equity curve for each silo
	template <typename ...Its, typename ...A, typename ...TCM> 
	void siloed_report(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs,  
			const std::tuple<std::pair<Its, Its>...>& start_end_its, const std::vector<double>& starting_balances, double target_daily_log_ret = 0.0025, 
			double annual_rfr = 2.5, int n_shuffles = 5000, int xtics_seconds = 604800);
	template <std::size_t ...Is, typename ...Its, typename ...A, typename ...TCM>
	void siloed_report_impl(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs,  
			const std::tuple<std::pair<Its, Its>...>& start_end_its, const std::vector<double>& starting_balances, double target_daily_log_ret, 
			double annual_rfr, int n_shuffles, int xtics_seconds, std::index_sequence<Is...> indices);
	
	/*
		Function for evaluating the perfomance of a multi-asset backtest which uses OptimalPortfolio
		Each column of portfolio weights is the optimal portfolio weights found from rebalancing and corresponds to the vector of starting balances in portfolio_starting_balances	
	*/
	template <typename ...Its, typename ...A, typename ...TCM> 
	void optimal_portfolio_backtest_report(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, 
			const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<std::pair<Its, Its>...>& start_end_its, const arma::Mat<double>& portfolio_weights, 
			const std::vector<std::vector<double>>& portfolio_starting_balances, const std::vector<Datetime>& rebalancing_datetimes, double target_daily_log_ret = 0.0025, 
			double annual_rfr = 2.5, int n_shuffles = 5000, int xtics_seconds = 604800);
	
	template <std::size_t ...Is, typename ...Its, typename ...A, typename ...TCM> 
	void optimal_portfolio_backtest_report_impl(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, 
			const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<std::pair<Its, Its>...>& start_end_its, const arma::Mat<double>& portfolio_weights, 
			const std::vector<std::vector<double>>& portfolio_starting_balances, const std::vector<Datetime>& rebalancing_datetimes, double target_daily_log_ret, 
			double annual_rfr, int n_shuffles, int xtics_seconds, std::index_sequence<Is...> indices);
}

template <typename InputIt, typename PosSize> 
backtest::Order<InputIt, PosSize>::Order(InputIt it_placed, PosSize position_size) : it_placed{it_placed}, position_size{position_size} {}; 

template <typename InputIt, typename PosSize> 
void backtest::Order<InputIt, PosSize>::cancel(){
	cancelled = true;
	//if we cancel the order it is also not executed
	executed = false; 
};

template <typename InputIt, typename PosSize> 
void backtest::Order<InputIt, PosSize>::execute(InputIt it_execute){
	it_executed = it_execute; 
	//if we execute the order it was not cancelled 
	cancelled = false;
	executed = true; 
}; 

template <typename InputIt, typename PosSize> 
backtest::MarketBuyOrder<InputIt, PosSize>::MarketBuyOrder(InputIt it_placed, PosSize position_size) : Order<InputIt, PosSize>{it_placed, position_size} 
	{
		//market orders are executed at the same time they are placed (likely)
		this->execute(it_placed); 
	}; 

template <typename InputIt, typename PosSize, typename PriceType> 
backtest::LimitBuyOrder<InputIt, PosSize, PriceType>::LimitBuyOrder(InputIt it_placed, PriceType desired_price, PosSize position_size) : 
	Order<InputIt, PosSize>{it_placed, position_size}, desired_price{desired_price} {};  


template <typename InputIt, typename PosSize> 
backtest::MarketSellOrder<InputIt, PosSize>::MarketSellOrder(InputIt it_placed, PosSize position_size) : Order<InputIt, PosSize>{it_placed, position_size} 
	{
		//market orders are executed at the same time they are placed (likely)
		this->execute(it_placed); 
	}; 

template <typename InputIt, typename PosSize, typename PriceType> 
backtest::LimitSellOrder<InputIt, PosSize, PriceType>::LimitSellOrder(InputIt it_placed, PriceType desired_price, PosSize position_size) : 
	Order<InputIt, PosSize>{it_placed, position_size}, desired_price{desired_price} {};  

//Buy Stop Loss order (throws if the stop loss is >= the bid at it_placed) 
template <typename InputIt, typename PosSize, typename PriceType> 
backtest::BuyStopLoss<InputIt, PosSize, PriceType>::BuyStopLoss(const MarketBuyOrder<InputIt, PosSize>& open, PriceType stop_loss) : 
	LimitSellOrder<InputIt, PosSize, PriceType>{open.it_placed_, stop_loss, open.position_size_} 
	{
		if(stop_loss >= open.it_placed_->b()){
			throw std::invalid_argument("BuyStopLoss Constructor: The stop loss must be less than the bid when the MarketBuyOrder is placed."); 
		}
	}; 
//Buy Take Profit Order (throws if the take profit is <= bid at it_placed)
template <typename InputIt, typename PosSize, typename PriceType> 
backtest::BuyTakeProfit<InputIt, PosSize, PriceType>::BuyTakeProfit(const MarketBuyOrder<InputIt, PosSize>& open, PriceType take_profit) : 
	LimitSellOrder<InputIt, PosSize, PriceType>{open.it_placed_, take_profit, open.position_size_} 
	{
		if(take_profit <= open.it_placed_->b()){
			throw std::invalid_argument("BuyTakeProfit Constructor: The take profit level must be greater than the bid when the MarketBuyOrder is placed."); 
		}
	}; 
//Sell Stop Loss (throws if the stop loss is <= the ask at it_placed)
template <typename InputIt, typename PosSize, typename PriceType> 
backtest::SellStopLoss<InputIt, PosSize, PriceType>::SellStopLoss(const MarketSellOrder<InputIt, PosSize>& open, PriceType stop_loss) : 
	LimitBuyOrder<InputIt, PosSize, PriceType>{open.it_placed_, stop_loss, open.position_size_} 
	{
		if(stop_loss <= open.it_placed_->a()){
			throw std::invalid_argument("SellStopLoss Constructor: The stop loss must be less than the ask when the MarketSellOrder is placed."); 
		}
	}; 
//Sell Take Profit (throws if the take profit is >= the ask at it_placed)
template <typename InputIt, typename PosSize, typename PriceType> 
backtest::SellTakeProfit<InputIt, PosSize, PriceType>::SellTakeProfit(const MarketSellOrder<InputIt, PosSize>& open, PriceType take_profit) : 
	LimitBuyOrder<InputIt, PosSize, PriceType>{open.it_placed_, take_profit, open.position_size_} 
	{
		if(take_profit >= open.it_placed_->a()){
			throw std::invalid_argument("SellTakeProfit Constructor: The take profit must be greater than the ask when the MarketSellOrder is placed."); 
		}
	}; 

//Trade Class Constructor Implementations 
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize, typename T> 
backtest::Trade<A, TCM>::Trade(A& asset, TCM& tcm, const MarketBuyOrder<InputIt, PosSize>& open, InputIt close_it, T balance){
	MarketSellOrder<InputIt, PosSize> close{close_it, open.position_size};
	open_dt = open.it_executed->dt();
	close_dt = close.it_executed->dt();
	//get the position size in units of the underlying given the amount of the usd to buy at the point in time given by it_executed
	double ps_units = asset.position_size_units(open.position_size, open.it_executed);
	//get the opening fill, closing fill, and slippage (given as a log return)
	//takse in ps in units and the open & close order
	auto fill_slip_tuple = tcm.compute_fill_slippage(ps_units, open, close);
	//compute the profit in the quote currency 
	//buy profit uses (close fill - open fill) 
	profit = (ps_units * asset.get_unit_multiplier()) * (std::get<1>(fill_slip_tuple) - std::get<0>(fill_slip_tuple)) * asset.quote_usd(close.it_executed);
	if(profit > 0){
		win = true; 
	}else{
		win = false; 
	}
	//compute the log return based on the balance
	log_return = utility::log_return(balance + profit, balance); 
	//use scaled percentage return
	percentage_return = utility::log_to_simple_return(log_return, true);  
	slippage = std::get<2>(fill_slip_tuple); 
}
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize, typename T>
backtest::Trade<A, TCM>::Trade(A& asset, TCM& tcm, const MarketSellOrder<InputIt, PosSize>& open, InputIt close_it, T balance){
	MarketBuyOrder<InputIt, PosSize> close{close_it, open.position_size};
	open_dt = open.it_executed->dt();
	close_dt = close.it_executed->dt();
	double ps_units = asset.position_size_units(open.position_size, open.it_executed);
	auto fill_slip_tuple = tcm.compute_fill_slippage(ps_units, open, close);
	//Sell profit uses (open fill - close fill) 
	profit = (ps_units * asset.get_unit_multiplier()) * (std::get<0>(fill_slip_tuple) - std::get<1>(fill_slip_tuple)) * asset.quote_usd(close.it_executed);
	if(profit > 0){
		win = true; 
	}else{
		win = false; 
	}
	//compute the log return based on the balance
	log_return = utility::log_return(balance + profit, balance); 
	//use scaled percentage return
	percentage_return = utility::log_to_simple_return(log_return, true);  
	slippage = std::get<2>(fill_slip_tuple); 
}

//Trade constructor for a MarketBuyOrder which has a stop loss and take_profit 
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize, typename PriceType, typename T> 
backtest::Trade<A, TCM>::Trade(A& asset, TCM& tcm, const MarketBuyOrder<InputIt, PosSize>& open_order, PriceType stop_loss_price, PriceType take_profit_price, T balance){
	//Make the LimitSellOrders for the take_profit & stop_loss 
	LimitSellOrder<InputIt, PosSize, PriceType> stop_loss_order{open_order.it_executed, stop_loss_price, open_order.position_size};
	LimitSellOrder<InputIt, PosSize, PriceType> take_profit_order(open_order.it_executed, take_profit_price, open_order.position_size);
	LimitSellOrder<InputIt, PosSize, PriceType> close_order; 
	//iterate forward from open_order.executed to find if the stop loss or take profit is hit first
	//start at the iterator after the it_executed (assuming we place orders at the end of a period)
	auto it = std::next(open_order.it_executed); 
	while(!stop_loss_order.executed && !take_profit_order.executed){
		//trigger the stop loss of a buy order if the low or bottom of a candle dips below the the stop loss price
		//trigger the take_profit of a buy order if the high or top of a candle rises above the take_profit price
		if(((cand_pat::body_bottom(*it, 0.0) <= stop_loss_order.desired_price) || (it->l() <= stop_loss_order.desired_price))){
			stop_loss_order.execute(it);
			take_profit_order.cancel();
			close_order = stop_loss_order; 
		}else if((cand_pat::body_top(*it, 0.0) >= take_profit_order.desired_price) || (it->h() >= take_profit_order.desired_price)){
			//if price rises above the take profit price then we execute the take profit order
			take_profit_order.execute(it);
			stop_loss_order.cancel(); 
			close_order = take_profit_order;  
		}
		it++; 
	}
	open_dt = open_order.it_executed->dt();
	close_dt = close_order.it_executed->dt();
	double ps_units = asset.position_size_units(open_order.position_size, open_order.it_executed);
	auto fill_slip_tuple = tcm.compute_fill_slippage(ps_units, open_order, close_order);
	//Buy profit uses (close fill - open fill) 
	profit = (ps_units * asset.get_unit_multiplier()) * (std::get<1>(fill_slip_tuple) - std::get<0>(fill_slip_tuple)) * asset.quote_usd(close_order.it_executed);
	if(profit > 0){
		win = true; 
	}else{
		win = false; 
	}
	//compute the log return based on the balance
	log_return = utility::log_return(balance + profit, balance); 
	//use scaled percentage return
	percentage_return = utility::log_to_simple_return(log_return, true);  
	slippage = std::get<2>(fill_slip_tuple); 
}

//Trade Constructor for a Market Sell order with a stop loss and take profit 
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize, typename PriceType, typename T> 
backtest::Trade<A, TCM>::Trade(A& asset, TCM& tcm, const MarketSellOrder<InputIt, PosSize>& open_order, PriceType stop_loss_price, PriceType take_profit_price, T balance){
	//Make the LimitBuyOrders for the take_profit & stop_loss 
	LimitBuyOrder<InputIt, PosSize, PriceType> stop_loss_order{open_order.it_executed, stop_loss_price, open_order.position_size};
	LimitBuyOrder<InputIt, PosSize, PriceType> take_profit_order(open_order.it_executed, take_profit_price, open_order.position_size);
	LimitBuyOrder<InputIt, PosSize, PriceType> close_order; 
	//iterate forward from open_order.executed to find if the stop loss or take profit is hit first
	//start at the next iterator after open_order.it_executed
	auto it = std::next(open_order.it_executed); 
	while(!stop_loss_order.executed && !take_profit_order.executed){
		//trigger the stop loss of a sell order if the top of the candle or the high rises above the stop loss price
		//trigger the take profit of a sell order if the bottom of the candle or the low dips below the take profit price
		if((cand_pat::body_top(*it, 0.0) >= stop_loss_order.desired_price) || (it->h() >= stop_loss_order.desired_price)){
			//if the bid price rises above the stop_loss price then we execute the stop loss order 
			stop_loss_order.execute(it);
			take_profit_order.cancel();
			close_order = stop_loss_order; 
		}else if((cand_pat::body_bottom(*it, 0.0) <= take_profit_order.desired_price) || (it->l() <= take_profit_order.desired_price)){
			//if price falls below the take profit price then we execute the take profit order
			take_profit_order.execute(it);
			stop_loss_order.cancel(); 
			close_order = take_profit_order;  
		}
		it++; 
	}
	open_dt = open_order.it_executed->dt();
	close_dt = close_order.it_executed->dt();
	double ps_units = asset.position_size_units(open_order.position_size, open_order.it_executed);
	auto fill_slip_tuple = tcm.compute_fill_slippage(ps_units, open_order, close_order);
	//Sell profit uses (open fill - close fill) 
	profit = (ps_units * asset.get_unit_multiplier()) * (std::get<0>(fill_slip_tuple) - std::get<1>(fill_slip_tuple)) * asset.quote_usd(close_order.it_executed);
	if(profit > 0){
		win = true; 
	}else{
		win = false; 
	}
	//compute the log return based on the balance
	log_return = utility::log_return(balance + profit, balance); 
	//use scaled percentage return
	percentage_return = utility::log_to_simple_return(log_return, true);  
	slippage = std::get<2>(fill_slip_tuple); 
}
//Trade constructor which takes a market buy order & market sell order and adjusts the position sizes to reflect how much of the trade was closed out
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize, typename T> 
backtest::Trade<A, TCM>::Trade(A& asset, TCM& tcm, MarketBuyOrder<InputIt, PosSize>& buy, MarketSellOrder<InputIt, PosSize>& sell, T balance){
	PosSize min_pos_size = std::min(buy.position_size, sell.position_size); 
	MarketBuyOrder<InputIt, PosSize> buy_adjusted(buy.it_placed, min_pos_size); 
	MarketSellOrder<InputIt, PosSize> sell_adjusted(sell.it_placed, min_pos_size);
	//if the buy trade comes first
	if(buy.it_executed->dt() < sell.it_executed->dt()){
		//if the buy trade comes first
		Trade<A, TCM> partial_close(asset, tcm, buy_adjusted, sell.it_executed); 	
		*this = Trade<A, TCM>(partial_close); 		
	}else{
		//if the sell trade comes first
		Trade<A, TCM> partial_close(asset, tcm, sell_adjusted, buy.it_executed); 	
		*this = Trade<A, TCM>(partial_close); 	
	}
	//adjust the buy and sell orders 
	buy = Trade<A, TCM>(buy.it_placed, buy.position_size - min_pos_size, balance);
	sell = Trade<A, TCM>(sell.it_placed, sell.position_size - min_pos_size, balance);
}

//copy constructor
template <typename A, typename TCM> 
template <typename InputIt, typename PosSize> 
backtest::Trade<A, TCM>::Trade(const Trade& t) : open_dt{t.open_dt}, close_dt{t.close_dt}, win{t.win}, profit{t.profit}, percentage_return{t.percentage_return}, 
	log_return{t.log_return}, slippage{t.slippage} {};  
//Trade Display function
template <typename A, typename TCM> 
void backtest::Trade<A, TCM>::display() const{
	std::cout << "Open Datetime          : " << std::format("{:%Y.%m.%d %H:%M:%S}", open_dt.zt()) << std::endl; 
	std::cout << "Close Datetime         : " << std::format("{:%Y.%m.%d %H:%M:%S}", close_dt.zt()) << std::endl; 
	std::cout << "Win                    : " << win << std::endl;  
	std::cout << "Profit (USD)           : " << profit << std::endl; 
	std::cout << "Log Return             : " << log_return << std::endl; 
	std::cout << "Simple Return          : " << percentage_return << std::endl;  
	std::cout << "Slippage (Log Return)  : " << slippage << std::endl; 
}


//LinearTCM method implementations
//compute fill slippage overloads 
template <typename InputIt, typename PosSize> 
std::tuple<double, double, double> backtest::LinearTCM::compute_fill_slippage(double ps_units, const MarketBuyOrder<InputIt, PosSize>& buy_to_open, const MarketSellOrder<InputIt, PosSize>& sell_to_close){
	double open_fill, close_fill;
	//fill for a buy market buy order has a desired price of the ask (may be filled slightly higher) 
	open_fill = buy_to_open.it_executed->a() + (fac_ * (ps_units / std::prev(buy_to_open.it_executed)->v()) * (buy_to_open.it_executed->a() - buy_to_open.it_executed->b()));
	double slippage = utility::log_return(open_fill, buy_to_open.it_executed->a());
	//fill for a market sell order has a desired price of the bid & may be filled slightly lower
	close_fill = sell_to_close.it_executed->b() - (fac_ * (ps_units / std::prev(sell_to_close.it_executed)->v()) * (sell_to_close.it_executed->a() - sell_to_close.it_executed->b()));
	slippage += utility::log_return(sell_to_close.it_executed->b(), close_fill);
	return std::make_tuple(open_fill, close_fill, slippage); 
}
template <typename InputIt, typename PosSize> 
std::tuple<double, double, double> backtest::LinearTCM::compute_fill_slippage(double ps_units, const MarketSellOrder<InputIt, PosSize>& sell_to_open, const MarketBuyOrder<InputIt, PosSize>& buy_to_close){
	double open_fill, close_fill;  
	//fill for a market sell order has a desired price of the bid & may be filled slightly lower
	open_fill = sell_to_open.it_executed->b() - (fac_ * (ps_units / std::prev(sell_to_open.it_executed)->v()) * (sell_to_open.it_executed->a() - sell_to_open.it_executed->b())); 
	double slippage = utility::log_return(sell_to_open.it_executed->b(), open_fill);
	//fill for a market buy order has a desired price of the ask (may be filled slightly higher) 
	close_fill = buy_to_close.it_executed->a() + (fac_ * (ps_units / std::prev(buy_to_close.it_executed)->v()) * (buy_to_close.it_executed->a() - buy_to_close.it_executed->b()));
	slippage += utility::log_return(close_fill, buy_to_close.it_executed->a());
	return std::make_tuple(open_fill, close_fill, slippage); 

}
template <typename InputIt, typename PosSize, typename PriceType> 
std::tuple<double, double, double> backtest::LinearTCM::compute_fill_slippage(double ps_units, const MarketBuyOrder<InputIt, PosSize>& buy_to_open, const LimitSellOrder<InputIt, PosSize, PriceType>& sell_to_close){
	double open_fill, close_fill;
	//fill for a buy market buy order has a desired price of the ask (may be filled slightly higher) 
	open_fill = buy_to_open.it_executed->a() + (fac_ * (ps_units / std::prev(buy_to_open.it_executed)->v()) * (buy_to_open.it_executed->a() - buy_to_open.it_executed->b()));
	double slippage = utility::log_return(open_fill, buy_to_open.it_executed->a());
	//fill for a limit sell order we may be filled lower than the desired price 
	close_fill = sell_to_close.desired_price - (fac_ * (ps_units / std::prev(sell_to_close.it_executed)->v()) * (sell_to_close.it_executed->a() - sell_to_close.it_executed->b()));
	slippage += utility::log_return(sell_to_close.desired_price, close_fill);
	return std::make_tuple(open_fill, close_fill, slippage); 

}
template <typename InputIt, typename PosSize, typename PriceType> 
std::tuple<double, double, double> backtest::LinearTCM::compute_fill_slippage(double ps_units, const MarketSellOrder<InputIt, PosSize>& sell_to_open, const LimitBuyOrder<InputIt, PosSize, PriceType>& buy_to_close){
	double open_fill, close_fill;  
	//fill for a market sell order has a desired price of the bid & may be filled slightly lower
	open_fill = sell_to_open.it_executed->b() - (fac_ * (ps_units / std::prev(sell_to_open.it_executed)->v()) * (sell_to_open.it_executed->a() - sell_to_open.it_executed->b()));
	double slippage = utility::log_return(sell_to_open.it_executed->b(), open_fill);
	//for a limit buy order we may be filled higher than the desired price	
	close_fill = buy_to_close.desired_price + (fac_ * (ps_units / std::prev(buy_to_close.it_executed)->v()) * (buy_to_close.it_executed->a() - buy_to_close.it_executed->b())); 
	slippage += utility::log_return(close_fill, buy_to_close.desired_price);
	return std::make_tuple(open_fill, close_fill, slippage); 
}

//function for reconcilling open orders
template <typename A, typename TCM, typename InputIt, typename PosSize> 
void backtest::reconcile_orders(const A& asset, const TCM& tcm, std::stack<MarketBuyOrder<InputIt, PosSize>>& buy_orders, std::stack<MarketSellOrder<InputIt, PosSize>>& sell_orders, 
		PosSize& buy_ps_still_open, PosSize& sell_ps_still_open, std::vector<Trade<A, TCM>>& trade_log){ 
	while(!buy_orders.empty() && !sell_orders.empty()){
		//push the partial close trade onto the trade log
		PosSize ps_closed = std::min(buy_orders.top().position_size, sell_orders.top().position_size); 
		trade_log.push_back(Trade<A, TCM>(asset, tcm, buy_orders.top(), sell_orders.top()));
		//either the buy order or sell order was closed out ==> pop the order which was closed off the stack
		if(buy_orders.top().position_size == 0.0){
			buy_orders.pop(); 
		}else{
			sell_orders.pop(); 
		}
		//update the open position sizes 
		buy_ps_still_open -= ps_closed; 
		sell_ps_still_open -= ps_closed;
	}
}

template <typename A, typename TCM> 
std::pair<std::chrono::year_month_day, std::chrono::year_month_day> backtest::earliest_open_oldest_close(const std::vector<Trade<A, TCM>>& trade_log){
	auto earliest_open = std::min_element(trade_log.cbegin(), trade_log.cend(), [](const auto& t1, const auto& t2){return t1.open_dt < t2.open_dt;})->open_dt;
	auto latest_close = std::max_element(trade_log.cbegin(), trade_log.cend(), [](const auto& t1, const auto& t2){return t1.close_dt < t2.close_dt;})->close_dt;
	std::chrono::year_month_day earliest_open_ymd; 
	std::chrono::year_month_day latest_close_ymd;
	earliest_open.get_ymd(earliest_open_ymd);
	latest_close.get_ymd(latest_close_ymd); 
	return std::make_pair(earliest_open_ymd, latest_close_ymd); 
}
template <typename ...A, typename ...TCM> 
std::pair<std::chrono::year_month_day, std::chrono::year_month_day> backtest::earliest_open_oldest_close(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs){
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	return backtest::earliest_open_oldest_close_impl(trade_logs, indices); 
}
template <std::size_t ...Is, typename ...A, typename ...TCM> 
std::pair<std::chrono::year_month_day, std::chrono::year_month_day> backtest::earliest_open_oldest_close_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, std::index_sequence<Is...> indices){
	std::chrono::year_month_day earliest_open_ymd;
	std::chrono::year_month_day latest_close_ymd;
	std::get<0>(trade_logs)[0].open_dt.get_ymd(earliest_open_ymd);
	std::get<0>(trade_logs)[0].close_dt.get_ymd(latest_close_ymd); 
	auto f = [&earliest_open_ymd, &latest_close_ymd](const auto& trade_log){
		auto ymd_pair = backtest::earliest_open_oldest_close(trade_log);
		if(ymd_pair.first < earliest_open_ymd){
			earliest_open_ymd = ymd_pair.first; 
		}
		if(ymd_pair.second > latest_close_ymd){
			latest_close_ymd = ymd_pair.second; 
		}
	};
	//call f in a fold expression 
	((f(std::get<Is>(trade_logs))), ...);
	return std::make_pair(earliest_open_ymd, latest_close_ymd); 
}

template <typename A, typename TCM, typename T> 
void backtest::update_daily_log_rets_map(const Trade<A, TCM>& trade, std::map<std::chrono::year_month_day, T>& dlr_map){
	//compute the number of days between the open and close datetimes 
	int n_days = utility::n_days_between(trade.open_dt, trade.close_dt);
	T dlr = trade.log_return / n_days;
	//get the year month days objects corresponding to the open and close datetimes
	std::chrono::year_month_day current_ymd;
	std::chrono::year_month_day close_ymd;
	trade.open_dt.get_ymd(current_ymd);
	trade.close_dt.get_ymd(close_ymd);
	while(current_ymd <= close_ymd){
		//update the daily log return for the current_day 
		dlr_map[current_ymd] += dlr;
		//add a day to current_ymd 
		current_ymd = (std::chrono::sys_days(current_ymd) + std::chrono::days(1)); 
	}
}

//Note: trades do not have to be in chronological order (we pass through the trade_log to obtain the datetime of the earliest & latest trades)
template <typename A, typename T, typename TCM> 
void backtest::daily_log_rets_from_trade_log(const std::vector<Trade<A, TCM>>& trade_log, arma::Row<T>& daily_log_returns){
	//get the year_month_day objects corresponding to the earliest open and oldest close
	auto ymd_pair = backtest::earliest_open_oldest_close(trade_log); 
	backtest::daily_log_rets_from_trade_log(trade_log, ymd_pair.first, ymd_pair.second, daily_log_returns); 
}
template <typename A, typename T, typename TCM> 
void backtest::daily_log_rets_from_trade_log(const std::vector<Trade<A, TCM>>& trade_log, std::chrono::year_month_day first_open_ymd, std::chrono::year_month_day last_close_ymd, arma::Row<T>& daily_log_returns){
	//initialize the map object with the correct datetimes 
	std::map<std::chrono::year_month_day, T> dlr_map;
	while(first_open_ymd <= last_close_ymd){
		dlr_map[first_open_ymd] = 0.0;
		//add a day to first_open_ymd
		first_open_ymd = (std::chrono::sys_days(first_open_ymd) + std::chrono::days(1)); 
	}
	//Now that the map has all the correct year_month_day keys we call update_daily_log_rets_map on each trade 
	for(auto t : trade_log){
		backtest::update_daily_log_rets_map(t, dlr_map); 
	}
	//iterate through the map and copy the daily log returns to the arma::Row
	daily_log_returns.resize(dlr_map.size());
	int i = 0; 
	for(auto it = dlr_map.cbegin(); it != dlr_map.cend(); it++){
		if(std::isnan(it->second)){
			std::cout << "NaN daily log return" << std::endl; 
		}
		daily_log_returns.at(i) = it->second;
		i++; 
	}
}
//Generate an arma::Mat<T> containing daily_log_returns for each asset given a tuple of trade_logs 
template <typename T, typename ...A, typename ...TCM> 
void backtest::daily_log_rets_from_trade_log(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& daily_log_rets){
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	backtest::daily_log_rets_from_trade_log_impl(trade_logs, daily_log_rets, indices); 
}
template <typename T, std::size_t ...Is, typename ...A, typename ...TCM>
void backtest::daily_log_rets_from_trade_log_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& daily_log_rets, std::index_sequence<Is...> indices){
	//get the ymd pair of the earliest open and latest close in all of the trade logs 
	auto ymd_pair = backtest::earliest_open_oldest_close(trade_logs);
	//call daily_log_rets_from_trade_log on the first trade log so that we can accurately size daily_log_rets 
	arma::Row<T> dlr_row;
	backtest::daily_log_rets_from_trade_log(std::get<0>(trade_logs), ymd_pair.first, ymd_pair.second, dlr_row);
	daily_log_rets.resize(indices.size(), dlr_row.n_elem);
	//lambda for filling a row of the matrix of daily log returns
	auto fill_row = [&ymd_pair, &daily_log_rets](const auto& trade_log, int i){
		//call daily_log_rets (no need to size since the row is resized in daily_log_rets_from_trade_log) 
		arma::Row<T> dlr_row;
		backtest::daily_log_rets_from_trade_log(trade_log, ymd_pair.first, ymd_pair.second, dlr_row);
		//write the row to the daily_log_rets matrix 
		daily_log_rets.row(i) = dlr_row; 
	}; 
	//call fill row in a fold_expression
	((fill_row(std::get<Is>(trade_logs), Is)), ...); 	
}
//compute optimal portfolio weights given a tuple of trade logs 
template <typename T, typename ...A, typename ...TCM> 
void backtest::optimal_weights_from_trade_logs(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Col<T>& optimal_weights, OptimalPortfolio& op){
	arma::Mat<T> daily_log_rets; 
	//call daily_log_rets_from_trade_log 
	backtest::daily_log_rets_from_trade_log(trade_logs, daily_log_rets);
	op.update_Q(daily_log_rets);
	//compute the row-wise median of the daily_log_rest matrix (==> ensure that the expected returns are robust to outlier days) 
	arma::vec expected_returns = arma::median(daily_log_rets, 1);
	//call compute_optimal_weights (we use evenly distrubuted weights as an initial guess)
	arma::Col<T> initial_guess(optimal_weights.size());
	initial_guess.fill(1.0 / optimal_weights.size()); 
	op.compute_optimal_weights(initial_guess, expected_returns);
	optimal_weights = initial_guess; 
}

//given a total balance and optimal weights compute the balance allocated to each asset
//assumes that asset_balances is pre-sized
template <typename T, typename PosSize> 
void backtest::optimal_balances(PosSize total_balance, std::vector<PosSize>& asset_balances, const arma::Col<T>& optimal_weights){
	for(int i = 0; i < optimal_weights.n_rows; i++){
		asset_balances[i] = (optimal_weights.at(i) * total_balance); 	
	}
}
template <typename ...A, typename ...TCM>
std::vector<double> backtest::compute_new_balances(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::vector<double>& starting_balances){
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	std::vector<double> asset_balances = starting_balances; 
	backtest::compute_new_balances_impl(trade_logs, asset_balances, indices);
	return asset_balances; 
}
template <std::size_t ...Is, typename ...A, typename ...TCM> 
void backtest::compute_new_balances_impl(const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, std::vector<double>& balances, std::index_sequence<Is...> indices){
	auto comp_bal = [&balances](const auto& trade_log, int i){
		for(auto t : trade_log){
			balances[i] += t.profit;  	
		}
	};
	//fold expression to compute the new balance for each asset 
	((comp_bal(std::get<Is>(trade_logs), Is)), ...);
	for(auto& b : balances){
		if(b < 0){
			//check if any of the balances are negative and if they are set them to zero
			b = 0.0; 
		}
	}
}
//returns a tuple containing the starting balance, final balance, total log return, total percentage return, win percentage, average win, average loss, average win log return, and average loss log return and n_trades 
template <typename A, typename TCM> 
std::tuple<int, double, double, double, double, double, double, double, double, double> backtest::basic_performance_metrics(const std::vector<Trade<A, TCM>>& trade_log, double starting_balance){ 
	int n_trades = trade_log.size();
	int n_wins = 0.0;
	double profit = 0.0; 
	double loss = 0.0;
	double avg_win_log_ret = 0.0;
	double avg_loss_log_ret = 0.0; 
	double final_balance = starting_balance;  
	for(auto t : trade_log){
		if(t.win){
			n_wins++;
			profit += t.profit;
			avg_win_log_ret += t.log_return; 
		}else{
			loss += t.profit;
			avg_loss_log_ret += t.log_return; 
		}
		//add the profit to the balance
		final_balance += t.profit; 
	}
	//compute the average win, average loss, win percentage, total log & simple returns, and average log and simple returns
	double avg_win = profit / n_wins;
	double avg_loss = loss / (n_trades - n_wins);
	double win_percentage = ((double)n_wins / n_trades) * 100;
	//double total_log_return = avg_win_log_ret + avg_loss_log_ret;
	double total_log_return = utility::log_return(final_balance, starting_balance); 
	double total_percentage_return = utility::log_to_simple_return(total_log_return, false);
	avg_win_log_ret /= n_wins;
	avg_loss_log_ret /= (n_trades - n_wins);
	double avg_log_return = total_log_return / n_trades;
	double avg_percentage_return = utility::log_to_simple_return(avg_log_return, false);
	return std::make_tuple(n_trades, starting_balance, final_balance, total_log_return, total_percentage_return, win_percentage, avg_win, avg_loss, avg_win_log_ret, avg_loss_log_ret); 
}

template <typename A, typename TCM> 
std::pair<double, double> backtest::sharpe_and_sortino(const A& asset, const std::vector<Trade<A, TCM>>& trade_log, double target_daily_log_ret, double annual_rfr){ 
	//convert the annual_rfr to a daily risk free log return
	double daily_rf_log_ret = utility::simple_to_log_return(annual_rfr, true) / asset.get_n_trading_days();

	//compute the daily log returns from the trade log 
	arma::Row<double> dlr_row;
	backtest::daily_log_rets_from_trade_log(trade_log, dlr_row);
	//compute the excess returns 
	arma::Row<double> excess_to_target = dlr_row - target_daily_log_ret;
	arma::Row<double> sub_target_dlr = excess_to_target.cols(arma::find(excess_to_target < 0));	
	arma::Row<double> excess_to_drflr = dlr_row - daily_rf_log_ret;
	//compute the mean and variance of the excess to daily risk free rate log returns 
	double m = arma::mean(excess_to_drflr);
	double std = arma::stddev(excess_to_drflr);
	double sharpe_ratio = std::numeric_limits<double>::max();  
	if(std != 0.0){
		sharpe_ratio = m / std; 
	}
	//compute the sortino ratio
	m = arma::mean(excess_to_target);
	std = arma::stddev(sub_target_dlr);
	double sortino_ratio = std::numeric_limits<double>::max();   
	if(std != 0.0){
		sortino_ratio = m / std; 
	}
	return std::make_pair(sharpe_ratio, sortino_ratio); 
}
template <typename A, typename TCM> 
double backtest::maximum_drawdown(const std::vector<Trade<A, TCM>>& trade_log){
	double max_dd_log_ret = 0.0;
	for(auto it = trade_log.begin(); it != trade_log.end(); it++){
		if(!it->win){
			double dd = 0.0;
			//left operand is evaluated first ==> if it == trades.end we dont end up dereferencing it
			while((it != trade_log.end()) && !it->win){
				//sum the log returns in the drawdown
				dd += it->log_return;
				it++; 
			}
			//it points to the trade after the draw down ends ==> next iterator points to two after drawdown ends ==> decrement it 
			it--;
			if(dd < max_dd_log_ret){
				max_dd_log_ret = dd; 
			}
		}
	}
	return max_dd_log_ret; 
}
template <typename A, typename It> 
void backtest::basic_info_table(std::string& latex, const A& asset, const std::pair<It, It>& start_end_it, int n_trades){
	auto headers = std::make_tuple("Symbol", "Timeframe", "Start Datetime", "End Datetime", "Number of Trades");
	std::vector<std::tuple<std::string, int, Datetime, Datetime, int>> rows{
		std::make_tuple(asset.get_symbol(), asset.cs.tf(), start_end_it.first->dt(), start_end_it.second->dt(), n_trades)};
	auto h_fcn_console = [](const std::string& s){
		return s; 
	};
	auto int_console = [](int i){return std::format("{:d}", i);};
	auto dt_to_string = [](const Datetime& dt){return std::format("{:%Y.%m.%d %H:%M:%S}", dt.zt());}; 
	auto h_fcns_console = std::make_tuple(h_fcn_console, h_fcn_console, h_fcn_console, h_fcn_console, h_fcn_console);
	auto row_fcns_console = std::make_tuple(h_fcn_console, int_console, dt_to_string, dt_to_string, int_console);
	std::vector<int> t1_col_widths{10, 13, 25, 25, 19};
	utility::table_print(headers, h_fcns_console, rows, row_fcns_console, t1_col_widths); 			
	
	auto int_latex = [](int i){return utility::integral_cell(i);}; 
	auto h_fcn_latex = [](const std::string& s){
		return "\\textbf{" + s + "}"; 
	}; 
	auto h_fcns_latex = std::make_tuple(h_fcn_latex, h_fcn_latex, h_fcn_latex, h_fcn_latex, h_fcn_latex, h_fcn_latex);
	auto row_fcns_latex = std::make_tuple(h_fcn_console, int_latex, dt_to_string, dt_to_string, int_latex); 
	latex += utility::table_latex(headers, h_fcns_latex, rows, row_fcns_latex, "center", "|| c | c | c | c | c ||"); 
}

template <typename A, typename TCM> 
arma::Col<double> backtest::reshuffled_maximum_drawdown(const std::vector<Trade<A, TCM>>& trade_log, int n_shuffles){
	std::random_device rd; 
	//lambda for reshuffling trades & computing max drawdown 
	auto comp_dd_reshuffled = [&rd](const std::vector<Trade<A, TCM>>& trades){
		//shuffle trades vector
		std::vector<Trade<A, TCM>> reshuffled_trades = trades;
		std::mt19937 g(rd()); 
		std::shuffle(reshuffled_trades.begin(), reshuffled_trades.end(), g);
		return backtest::maximum_drawdown(reshuffled_trades); 
	};
	arma::Col<double> reshuffled_drawdowns(n_shuffles);
	std::transform(std::execution::par_unseq, reshuffled_drawdowns.begin(), reshuffled_drawdowns.end(), reshuffled_drawdowns.begin(), 
			[comp_dd_reshuffled, &trade_log](double d){return comp_dd_reshuffled(trade_log);}); 
	//compute the 10th and 90th percentile of the max
	arma::Col<double> qtiles{.10, .90}; 
	return arma::quantile(reshuffled_drawdowns, qtiles);
}
template <typename A, typename TCM, typename It> 
void backtest::equity_curve(const std::string& path, std::string& latex, const std::vector<Trade<A, TCM>>& trade_log, const std::pair<It, It>& start_end_it, 
		double starting_balance, double final_balance, int xtics_seconds){ 
	double equity = starting_balance;
	//iterator to the current trade
	auto ctit = trade_log.begin(); 
	//vector of x, y points
	std::vector<std::tuple<std::string, double>> pts; 
	for(auto it = start_end_it.first; it != std::next(start_end_it.second); it++){
		//iterate through the buy signals 
		if(it->dt() == ctit->close_dt){
			//alter the by the trade profit at the trade close
			equity += ctit->profit;
			//make a point and push it back onto the pts vector
			std::string ctdts = std::format("{:%Y-%m-%d %H:%M}", ctit->close_dt.zt());
			pts.push_back(std::make_tuple(ctdts, equity)); 
			ctit++; 
		}
	}
	Gnuplot gp;
	//set the terminal size 
	gp << "set terminal png size 900,500 \n";
	//set the file destination 
	gp << "set output '" + path + "' \n"; 
	//set the y range
	double y_min = .9*std::min(starting_balance, final_balance);
	double y_max = 1.1 * std::max(starting_balance, final_balance);
	gp << " set yrange [" + std::to_string(y_min) + ":" + std::to_string(y_max) + "]\n";
	//setting the x range and the tick labels 
	gp << "set xdata time \n"; 
	gp << "set timefmt '%Y-%m-%d %H:%M' \n";
	gp << "set format x '%Y-%m-%d' \n";
	Datetime start_dt = start_end_it.first->dt();
	Datetime end_dt = start_end_it.second->dt(); 
	std::string stdt = std::format("{:%Y-%m-%d %H:%M}", start_dt.zt()); 
	std::string enddt = std::format("{:%Y-%m-%d %H:%M}", end_dt.zt());

	gp << "set xrange ['" + stdt + "':'" + enddt + "'] \n"; 
	//set the xtics
	gp << "set xtics '" + stdt + "' , " + std::to_string(xtics_seconds) + ", '" + enddt + "' \n";
	//first xtic doesn't seem to be appearing ==> add it manually 
	gp << "set xtics add('" + stdt + "') \n";
	//rotate the xtics 
	gp << "set xtics rotate by 45 right \n";
	//set the labels and title
	gp << "set xlabel 'Time' \n"; 	
	gp << "set ylabel 'Equity' \n"; 	
	gp << "set title 'Backtest Equity Curve' \n";
	//plot the equity curve with line witdth 2
	//set the style for the curve fill (I can't seem to get the border to be black or to overlay another line curve on top of it)
	gp << "plot '-' using 1:3 with filledcurves x1 fc '#00b088' fs transparent solid 0.5 lc rgb '#000000' title ''\n"; 
	//save the plot
	gp.send1d(pts);
	//add the equity curve image to the latex file
	latex += "\\sect{Backtest Equity Curve} \n";
	latex += "\\includegraphics[width = \\textwidth]{" + path + "} \n"; 
}

//tex_path is the path at which to write the latex file
//img_path is the path at which to save the equity curve image
template <typename It, typename A, typename TCM> 
void backtest::single_report(const A& asset, const std::string& tex_path, const std::string& img_path, const std::pair<It, It>& start_end_it, double starting_balance, const std::vector<Trade<A, TCM>>& trade_log, 
		double target_daily_log_ret, double annual_rfr, int n_shuffles, int xtics_seconds){
	
	auto basic_metrics = backtest::basic_performance_metrics(trade_log, starting_balance); 
	auto ss_ratio_pair = backtest::sharpe_and_sortino(asset, trade_log, target_daily_log_ret, annual_rfr);
	double max_drawdown = backtest::maximum_drawdown(trade_log);
	arma::Col<double> rmdd_10th_90th = backtest::reshuffled_maximum_drawdown(trade_log, n_shuffles); 
	
	std::string latex = "\\sect{Basic Backtest Information}\n";
	//call the basic_info_table function (first element of basic metrics is n_trades) 
	backtest::basic_info_table(latex, asset, start_end_it, std::get<0>(basic_metrics)); 
	latex += "\\sect{Backtest Performance Metrics}\n";
	//call performance_metrics_table
	backtest::performance_metrics_table(latex, basic_metrics, ss_ratio_pair, max_drawdown, rmdd_10th_90th); 
	//call equity curve 
	backtest::equity_curve(img_path, latex, trade_log, start_end_it, starting_balance, std::get<2>(basic_metrics), xtics_seconds); 	
	//write the latex document 
	utility::write_latex(tex_path, latex);
}
template <typename ...Its, typename ...A, typename ...TCM> 
void backtest::siloed_report(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs,  
		const std::tuple<std::pair<Its, Its>...>& start_end_its, const std::vector<double>& starting_balances, double target_daily_log_ret, 
		double annual_rfr, int n_shuffles, int xtics_seconds){
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	backtest::siloed_report_impl(assets, tex_path, img_folder_path, trade_logs, start_end_its, starting_balances, target_daily_log_ret, annual_rfr, n_shuffles, xtics_seconds, indices); 
}
template <std::size_t ...Is, typename ...Its, typename ...A, typename ...TCM>
void backtest::siloed_report_impl(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs,  
		const std::tuple<std::pair<Its, Its>...>& start_end_its, const std::vector<double>& starting_balances, double target_daily_log_ret, 
		double annual_rfr, int n_shuffles, int xtics_seconds, std::index_sequence<Is...> indices){
	std::string latex = "\\sect{Siloed Backtest Report} \\newline \n"; 
	auto single_asset_report = [&](const auto& asset, const auto& trade_log, const auto& start_end_it, int i){
		latex += "\\sect{Silo #" + std::to_string(i) + asset.get_symbol() + "} \\newline \n";
		auto basic_metrics = backtest::basic_performance_metrics(trade_log, starting_balances[i]); 
		auto ss_ratio_pair = backtest::sharpe_and_sortino(asset, trade_log, target_daily_log_ret, annual_rfr);
		double max_drawdown = backtest::maximum_drawdown(trade_log);
		arma::Col<double> rmdd_10th_90th = backtest::reshuffled_maximum_drawdown(trade_log, n_shuffles); 
		latex += "\\sect{Basic Backtest Information} \\newline \n";
		//call the basic_info_table function (first element of basic metrics is n_trades) 
		backtest::basic_info_table(latex, asset, start_end_it, std::get<0>(basic_metrics)); 
		latex += "\\sect{Backtest Performance Metrics} \\newline \n";
		//call performance_metrics_table
		backtest::performance_metrics_table(latex, basic_metrics, ss_ratio_pair, max_drawdown, rmdd_10th_90th);
		//make the image path 
		std::string img_path = img_folder_path + "/" + asset.get_symbol() + "_equity_curve.png";  
		//call equity curve 
		backtest::equity_curve(img_path, latex, trade_log, start_end_it, starting_balances[i], std::get<1>(basic_metrics), xtics_seconds); 	
	};
	//call single_asset_report in a fold expression
	((single_asset_report(std::get<Is>(assets), std::get<Is>(trade_logs), std::get<Is>(start_end_its), Is)), ...);
	//write the latex to the tex_path 
	utility::write_latex(tex_path, latex); 	
}
template <typename ...Its, typename ...A, typename ...TCM> 
void backtest::optimal_portfolio_backtest_report(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, 
		const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<std::pair<Its, Its>...>& start_end_its, const arma::Mat<double>& portfolio_weights, 
		const std::vector<std::vector<double>>& portfolio_starting_balances, const std::vector<Datetime>& rebalancing_datetimes, double target_daily_log_ret, 
		double annual_rfr, int n_shuffles, int xtics_seconds){
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	backtest::optimal_portfolio_backtest_report_impl(assets, tex_path, img_folder_path, trade_logs, start_end_its, portfolio_weights, portfolio_starting_balances, 
			rebalancing_datetimes, target_daily_log_ret, annual_rfr, n_shuffles, xtics_seconds, indices); 
}

template <std::size_t ...Is, typename ...Its, typename ...A, typename ...TCM> 
void backtest::optimal_portfolio_backtest_report_impl(const std::tuple<A...>& assets, const std::string& tex_path, const std::string& img_folder_path, 
		const std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<std::pair<Its, Its>...>& start_end_its, const arma::Mat<double>& portfolio_weights, 
		const std::vector<std::vector<double>>& portfolio_starting_balances, const std::vector<Datetime>& rebalancing_datetimes, double target_daily_log_ret, 
		double annual_rfr, int n_shuffles, int xtics_seconds, std::index_sequence<Is...> indices){
	std::string latex = "\\sect{Optimal Portfolio Backtest} \\newline \n";
	latex += "\\sect{Optimal Portfolio Rebalances} \\newline \n"; 
	auto dt_to_string = [](const Datetime& dt){return std::format("{:%Y.%m.%d %H:%M:%S}", dt.zt());}; 
	for(int i = 0; i < rebalancing_datetimes.size(); i++){
		//string to hold the rebalance date, the rebalanced portfolio weights and the rebalanced portfolio balances 
		latex += "\\textbf{Rebalancing Date}: " + dt_to_string(rebalancing_datetimes[i]) + " \\newline \n";
		latex += "\\textbf{Portfolio Weights}: ";
		arma::Col<double> pw = portfolio_weights.col(i); 
		utility::range_based_to_string(pw, latex, '[', ',', ']');
		latex += "\\newline \n";
		latex += "\\textbf{Portfolio Balances}: "; 
		utility::range_based_to_string(portfolio_starting_balances[i], latex, '[', ',', ']');
		latex += "\\newline \n";
	}
	//display the rebalancing info to the console
	std::cout << latex;
	latex += "\\sect{Individual Asset Backtest Performance} \n"; 
	auto single_asset_report = [&](const auto& asset, const auto& trade_log, const auto& start_end_it, int i){
		latex += "\\sect{Asset: " + asset.get_symbol() + "} \\newline \n";
		//use the ith starting balance of the first portfolio starting balances
		auto basic_metrics = backtest::basic_performance_metrics(trade_log, portfolio_starting_balances[0][i]); 
		auto ss_ratio_pair = backtest::sharpe_and_sortino(asset, trade_log, target_daily_log_ret, annual_rfr);
		double max_drawdown = backtest::maximum_drawdown(trade_log);
		arma::Col<double> rmdd_10th_90th = backtest::reshuffled_maximum_drawdown(trade_log, n_shuffles); 
		latex += "\\sect{Basic Backtest Information} \\newline \n";
		//call the basic_info_table function (first element of basic metrics is n_trades) 
		backtest::basic_info_table(latex, asset, start_end_it, std::get<0>(basic_metrics)); 
		latex += "\\sect{Backtest Performance Metrics} \\newline \n";
		//call performance_metrics_table
		backtest::performance_metrics_table(latex, basic_metrics, ss_ratio_pair, max_drawdown, rmdd_10th_90th);
		//make the image path 
		std::string img_path = img_folder_path + "/" + asset.get_symbol() + "_equity_curve.png";  
		//call equity curve 
		backtest::equity_curve(img_path, latex, trade_log, start_end_it, portfolio_starting_balances[0][i], std::get<1>(basic_metrics), xtics_seconds); 	
	};
	//we need to make a tuple with the start end iterators for the entire backtest from the split start end iterators
	//
	//call single_asset_report in a fold expression
	((single_asset_report(std::get<Is>(assets), std::get<Is>(trade_logs), std::get<Is>(start_end_its), Is)), ...);
	//write the latex to the tex_path 
	utility::write_latex(tex_path, latex); 	
}

//ATRSLTP class 

template <typename InputIt> 
double backtest::ATRSLTP::comp_atr_(InputIt it) const{
	auto first = std::prev(it, k_ - 1); 
	auto tr_op = [first](const auto& c){
		auto itp = std::next(first, std::distance(&(*first), &c) - 1);  
		double hl = c.h() - c.l();
		double hc = std::abs(c.h() - itp->c()); 
		double lc = std::abs(c.l() - itp->c()); 
		return std::max(hl, std::max(hc, lc)); 
	};
	return utility::mean(first, std::next(it), 0.0, tr_op); 
}; 

template <typename InputIt> 
double backtest::ATRSLTP::buy_stop_loss(InputIt it, double min_class_prob) const{
	//use bid because it is lower than the ask
	return (it->b() - (stop_mult_ * comp_atr_(it))); 
}; 
template <typename InputIt> 
double backtest::ATRSLTP::sell_stop_loss(InputIt it, double min_class_prob) const{
	//use ask because it is higher than the bid
	return (it->a() + (stop_mult_ * comp_atr_(it))); 
}; 
template <typename InputIt> 
double backtest::ATRSLTP::buy_take_profit(InputIt it, double min_class_prob) const{
	//use ask because it is higher than the bid
	return (it->a() + (take_mult_ * comp_atr_(it))); 
}; 
template <typename InputIt> 
double backtest::ATRSLTP::sell_take_profit(InputIt it, double min_class_prob) const{
	//use bid because it is lower than the ask
	return (it->b() - (take_mult_ * comp_atr_(it))); 
}; 

template <typename A, typename T, typename BSM, typename SSM, typename PSA, typename TCM, typename It1, typename It2, typename L> 
void backtest::market_order_backtest(A& asset, std::vector<Trade<A, TCM>>& trade_log, BSM& buy_sig_model, SSM& sell_sig_model, const arma::Mat<T>& bsm_mat, const arma::Mat<T>& ssm_mat, PSA psa, TCM tcm, 
		const std::pair<It1, It1>& bsm_mat_it_pair, const std::pair<It2, It2>& ssm_mat_it_pair, double start_bal, bool reverse_on_close, 
			double bs_entry_thresh, double bs_exit_thresh, double ss_entry_thresh, double ss_exit_thresh, L bs_mc, L ss_mc){ 
	//variable to keep track of the balance
	double balance = start_bal;
	//variable to store the position size
	double pos_size; 
	//booleans to keep track of whether or not there is an open buy or sell trade
	bool open_buy_trade = false;
	bool open_sell_trade = false;
	//variables to hold the open orders 
	MarketBuyOrder<It1, double> buy_to_open;
	MarketSellOrder<It1, double> sell_to_open;
	//variables to store the buy_sig_model & sell_sig_model predictions and class probabilities
	size_t bs_pred, ss_pred;
	arma::vec bs_probs, ss_probs;
	//we first concatenate mat it pairs into a tuple 
	auto mat_it_pairs = std::make_tuple(bsm_mat_it_pair, ssm_mat_it_pair);
	//auto mat_it_pairs = std::make_pair(bsm_mat_it_pair, ssm_mat_it_pair); 
	//call sync iterators
	utility::sync_iterators(mat_it_pairs);
	//get the number of columns before the synced first iterators for each matrix
	int n1 = std::distance(bsm_mat_it_pair.first, std::get<0>(mat_it_pairs).first);
	int n2 = std::distance(ssm_mat_it_pair.first, std::get<1>(mat_it_pairs).first);
	
	//std::cout << "displaying the first of bsm_mat_it_pair with their offsets" << std::endl;  
	//std::next(bsm_mat_it_pair.first, n1)->display();
	//std::cout << std::endl; 
	//std::next(ssm_mat_it_pair.first, n2)->display(); 
	//std::cout << std::endl; 

	//iterate through the candle series
	int i = 0; 
	for(auto it = std::get<0>(mat_it_pairs).first; it != std::next(std::get<0>(mat_it_pairs).second); it++){
		//used for getting the column of bsm_mat which corresponds to it 
		int n = std::distance(std::get<0>(mat_it_pairs).first, it);
		if(!open_buy_trade && !open_sell_trade && balance > 0.0){
			buy_sig_model.Classify(bsm_mat.col(n + n1), bs_pred, bs_probs);
			sell_sig_model.Classify(ssm_mat.col(n + n2), ss_pred, ss_probs); 
			//if the probability of the minority class is > bs_thresh then we make a buy_to_open_order
			if(bs_probs.at(bs_mc) >= bs_entry_thresh){
				buy_to_open = MarketBuyOrder<It1, double>(it, psa.compute_position_size(balance));
				open_buy_trade = true; 
			}else if(ss_probs.at(ss_mc) >= ss_entry_thresh){
				sell_to_open = MarketSellOrder<It1, double>(it, psa.compute_position_size(balance));
				open_sell_trade = true; 
			}
		}else if(open_buy_trade && !open_sell_trade && balance > 0.0){
			//if there is a buy trade open
			sell_sig_model.Classify(ssm_mat.col(n + n2), ss_pred, ss_probs);
			if(ss_probs.at(ss_mc) >= ss_exit_thresh){
				//if we generate a sell signal to close out the open buy trade 
				trade_log.push_back(backtest::Trade(asset, tcm, buy_to_open, it, balance));
				//increment the balance 
				balance += trade_log.back().profit;
				//set open_buy_trade to false 
				open_buy_trade = false; 
				//if reverse_on_close is true open a sell trade 
				if(reverse_on_close){
					sell_to_open = MarketSellOrder<It1, double>(it, psa.compute_position_size(balance)); 		
					open_sell_trade = true; 
				}
			}
		}else if(!open_buy_trade && open_sell_trade && balance > 0.0){
			//if there is a sell trade open
			buy_sig_model.Classify(bsm_mat.col(n + n1), bs_pred, bs_probs);
			if(bs_probs.at(bs_mc) >= bs_exit_thresh){
				trade_log.push_back(backtest::Trade(asset, tcm, sell_to_open, it, balance));
				//increment the balance
				balance += trade_log.back().profit; 	
				//set open_sell_trade to false
				open_sell_trade = false;
				//if reverse_on_close is true open a buy trade
				if(reverse_on_close){
					buy_to_open = MarketBuyOrder<It1, double>(it, psa.compute_position_size(balance));
					open_buy_trade = true; 
				}
			}
		}
	}
}
//siloed market order backtester
template <typename T, typename PSA, size_t ...Is, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename L>
void backtest::siloed_market_order_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
		const std::vector<arma::Mat<T>>& bsm_mats, const std::vector<arma::Mat<T>>& ssm_mats, PSA psa, const std::tuple<TCM...>& tcms, 
		const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, const std::vector<double>& bs_entry_threshs, 
		const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs, 
		const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, std::vector<double> start_balances, std::index_sequence<Is...> indices, bool reverse_on_close){
	//just use a fold expression to call market_order_backtest on the corresponding tuple elements
	((backtest::market_order_backtest(std::get<Is>(assets), std::get<Is>(trade_logs), std::get<Is>(buy_sig_models), std::get<Is>(sell_sig_models),
									  bsm_mats[Is], ssm_mats[Is], psa,  std::get<Is>(tcms), std::get<Is>(bsm_mat_it_pairs), 
									  std::get<Is>(ssm_mat_it_pairs), start_balances[Is], reverse_on_close, bs_entry_threshs[Is], bs_exit_threshs[Is], 
									  ss_entry_threshs[Is], ss_exit_threshs[Is], bs_mcs[Is], ss_mcs[Is])), ...); 	
}
//Note if a closing signal is generated prior to a stop loss or take profit level being hit then we close the trade with a market order
template <typename A, typename T, typename BSM, typename SSM, typename PSA, typename TCM, typename It1, typename It2, typename SLTP, typename L> 
void backtest::stop_loss_take_profit_backtest(A& asset, std::vector<Trade<A, TCM>>& trade_log, BSM& buy_sig_model, SSM& sell_sig_model, const arma::Mat<T>& bsm_mat, const arma::Mat<T>& ssm_mat, 
		PSA psa, TCM tcm, SLTP sltp, const std::pair<It1, It1>& bsm_mat_it_pair, const std::pair<It2, It2>& ssm_mat_it_pair, double start_bal, double bs_entry_thresh, double bs_exit_thresh,  
		double ss_entry_thresh, double ss_exit_thresh, L bs_mc, L ss_mc){ 
	//variable to keep track of the balance 
	double balance = start_bal; 
	//variable to store the position size
	double pos_size; 
	//booleans to keep track of whether or not there is an open buy or sell trade
	bool open_buy_trade = false;
	bool open_sell_trade = false;
	//variables to hold the open orders and the Limit orders for the stops which are executed 
	MarketBuyOrder<It1, double> buy_to_open;
	MarketSellOrder<It1, double> sell_to_open;
	double stop_loss, take_profit; 
	//variables to store the buy_sig_model & sell_sig_model predictions and class probabilities
	size_t bs_pred, ss_pred;
	arma::vec bs_probs, ss_probs;
	//we first concatenate mat it pairs into a tuple 
	auto mat_it_pairs = std::make_tuple(bsm_mat_it_pair, ssm_mat_it_pair);
	//call sync iterators
	utility::sync_iterators(mat_it_pairs);
	//get the number of columns before the synced first iterators for each matrix
	int n1 = std::distance(bsm_mat_it_pair.first, std::get<0>(mat_it_pairs).first);
	int n2 = std::distance(ssm_mat_it_pair.first, std::get<1>(mat_it_pairs).first); 
	//iterate through the candle series
	for(auto it = std::get<0>(mat_it_pairs).first; it != std::next(std::get<0>(mat_it_pairs).second); it++){
		//used for getting the column of bsm_mat which corresponds to it 
		int n = std::distance(std::get<0>(mat_it_pairs).first, it);
		if(!open_buy_trade && !open_sell_trade && balance > 0){
			//if there are no trades open see if we have a buy or sell signal
			buy_sig_model.Classify(bsm_mat.col(n + n1), bs_pred, bs_probs);
			sell_sig_model.Classify(ssm_mat.col(n + n2), ss_pred, ss_probs); 
			//if the probability of the minority class is > bs_thresh then we make a buy_to_open_order
			if(bs_probs.at(bs_mc) >= bs_entry_thresh){
				buy_to_open = MarketBuyOrder<It1, double>(it, psa.compute_position_size(balance));
				//compute the stop loss and take profit levels 
				stop_loss = sltp.buy_stop_loss(it, bs_probs.at(bs_mc));
				take_profit = sltp.buy_take_profit(it, bs_probs.at(bs_mc)); 
				open_buy_trade = true;
			}else if(ss_probs.at(ss_mc) >= ss_entry_thresh){
				sell_to_open = MarketSellOrder<It1, double>(it, psa.compute_position_size(balance));
				//compute the stop loss and take profit levels
				stop_loss = sltp.sell_stop_loss(it, ss_probs.at(ss_mc));
				take_profit = sltp.sell_take_profit(it, ss_probs.at(ss_mc)); 
				open_sell_trade = true; 
			}
		}else if(open_buy_trade && !open_sell_trade && balance > 0){
			//if we have an open buy trade
			sell_sig_model.Classify(ssm_mat.col(n + n2), ss_pred, ss_probs); 
			if(((cand_pat::body_bottom(*it, 0.0) <= stop_loss) || (it->l() <= stop_loss)) || ((cand_pat::body_top(*it, 0.0) >= take_profit) || (it->h() >= take_profit))){
				//if the stop_loss or take_profit has been triggered close the trade
				trade_log.push_back(Trade<A, TCM>(asset, tcm, buy_to_open, stop_loss, take_profit, balance));
				//adjust the balance
				balance += trade_log.back().profit; 
				//set open_buy_trade to false 
				open_buy_trade = false;
			}else if(ss_probs.at(ss_mc) >= ss_exit_thresh){
				//if a sell signal has been generated
				trade_log.push_back(Trade<A, TCM>(asset, tcm, buy_to_open, it, balance));
				//adjust the balance
				balance += trade_log.back().profit; 
				//set open_buy_trade to false 
				open_buy_trade = false; 
			}
		}else if(!open_buy_trade && open_sell_trade && balance > 0){
			//if we have an open sell trade
			buy_sig_model.Classify(bsm_mat.col(n + n1), bs_pred, bs_probs); 
			if(((cand_pat::body_top(*it, 0.0) >= stop_loss) || (it->h() >= stop_loss)) || ((cand_pat::body_bottom(*it, 0.0) <= take_profit) || (it->l() <= take_profit))){
				//if the stop_loss or take_profit has been triggered close the trade
				trade_log.push_back(Trade<A, TCM>(asset, tcm, sell_to_open, stop_loss, take_profit, balance));
				//adjust the balance
				balance += trade_log.back().profit; 
				//set open_sell_trade to false 
				open_sell_trade = false; 
			}else if(bs_probs.at(bs_mc) >= bs_exit_thresh){
				//if a buy signal has been generated
				trade_log.push_back(Trade<A, TCM>(asset, tcm, sell_to_open, it, balance));
				//adjust the balance
				balance += trade_log.back().profit;
				//set open_sell_trade to false 
				open_sell_trade = false; 
			}
		}
	}
}

//Siloed multi-asset stop_loss take_profit backtester
template <typename T, typename PSA, size_t ...Is, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename ...SLTP, typename L>
void backtest::siloed_stop_loss_take_profit_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
		const std::vector<arma::Mat<T>>& bsm_mats, const std::vector<arma::Mat<T>>& ssm_mats, PSA psa, const std::tuple<TCM...>& tcms, const std::tuple<SLTP...>& sltps, 
		const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, const std::vector<double>& bs_entry_threshs, 
		const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs, const std::vector<L>& bs_mcs, 
		const std::vector<L>& ss_mcs, std::vector<double> start_balances, std::index_sequence<Is...> indices){ 
	
	//call the single asset stop_loss_take_profit function in a fold expression over indices to access corresponding elements of the tuples and arrays 
	((backtest::stop_loss_take_profit_backtest(std::get<Is>(assets), std::get<Is>(trade_logs), std::get<Is>(buy_sig_models), std::get<Is>(sell_sig_models),
											   bsm_mats[Is], ssm_mats[Is],  psa, std::get<Is>(tcms), std::get<Is>(sltps), std::get<Is>(bsm_mat_it_pairs),
											   std::get<Is>(ssm_mat_it_pairs), start_balances[Is], bs_entry_threshs[Is], bs_exit_threshs[Is], 
											   ss_entry_threshs[Is], ss_exit_threshs[Is], bs_mcs[Is], ss_mcs[Is])
	 ), ...); 	
}

//multi-asset market order backtest which uses OptimalPortfolio to perform portfolio rebalancing
template <std::size_t ...Is, typename T, typename PSA, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename L>
void backtest::optimal_portfolio_market_order_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& rebalanced_weights, 
		std::vector<std::vector<double>>& rebalanced_balances,std::vector<Datetime>& rebalance_datetimes, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
		const std::vector<arma::Mat<T>>& bsm_matrices, const std::vector<arma::Mat<T>>& ssm_matrices, PSA psa, const std::tuple<TCM...> tcms, const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, 
		const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, const std::vector<double>& bs_entry_threshs, const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, 
		const std::vector<double>& ss_exit_threshs, const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, std::index_sequence<Is...> n_rebal_idx_seq,  
		double balance, bool reverse_on_close, double target_return, double neg_exp_weight){ 
	OptimalPortfolio op = OptimalPortfolio(target_return, neg_exp_weight); 	
	//make the index sequence 
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	arma::Col<double> weights(indices.size());
	weights.fill(1.0 / indices.size()); 
	//get the initial starting balances 
	std::vector<double> asset_balances(sizeof ...(A));
	backtest::optimal_balances(balance, asset_balances, weights);
	std::vector<double> starting_balances = asset_balances; 
	//create the rebalancing splits 
	std::vector<std::tuple<std::pair<Its1, Its1>...>> bsm_mat_splits_it_pairs; 
	std::vector<std::tuple<std::pair<Its2, Its2>...>> ssm_mat_splits_it_pairs;
	std::vector<std::vector<arma::Mat<T>>> bsm_mat_splits; 
	std::vector<std::vector<arma::Mat<T>>> ssm_mat_splits;
	
	utility::split_matrices(bsm_matrices, bsm_mat_splits, bsm_mat_it_pairs, bsm_mat_splits_it_pairs, n_rebal_idx_seq, indices); 
	utility::split_matrices(ssm_matrices, ssm_mat_splits, ssm_mat_it_pairs, ssm_mat_splits_it_pairs, n_rebal_idx_seq, indices); 
	
	rebalanced_weights.insert_cols(rebalanced_weights.n_cols, weights);
	rebalanced_balances.push_back(starting_balances);
	rebalance_datetimes.push_back(std::get<0>(bsm_mat_splits_it_pairs[0]).first->dt());

	for(int i = 0; i < bsm_mat_splits.size(); i++){
		//call market_order_backtest (calls sync_iterators so it doesn't matter if the corresponding bsm_mat_splits_it_pair and ssm_mat_splits_it_pairs are a bit off from each other) 
		backtest::siloed_market_order_backtest(assets, trade_logs, buy_sig_models, sell_sig_models, bsm_mat_splits[i], ssm_mat_splits[i], psa, tcms,
				bsm_mat_splits_it_pairs[i], ssm_mat_splits_it_pairs[i], bs_entry_threshs, bs_exit_threshs, ss_entry_threshs, ss_exit_threshs, 
				bs_mcs, ss_mcs, asset_balances, indices, reverse_on_close);
		//use the trade_logs to compute the optimal portfolio weights 
		backtest::optimal_weights_from_trade_logs(trade_logs, weights, op);
		rebalanced_weights.insert_cols(rebalanced_weights.n_cols, weights); 
		//reset the target (it will have been lowered if the max of the expected returns was lower than the target)
		op.update_target(target_return); 
		//compute the new asset balances
		asset_balances = backtest::compute_new_balances(trade_logs, starting_balances);
		balance = std::accumulate(asset_balances.cbegin(), asset_balances.cend(), 0.0);
		//compute the rebalanced asset balances
		backtest::optimal_balances(balance, asset_balances, weights);
		rebalanced_balances.push_back(asset_balances); 
		//insert the datetime of the rebalance 
		rebalance_datetimes.push_back(std::get<0>(bsm_mat_splits_it_pairs[i]).second->dt());
	}
}

template <std::size_t ...Is, typename T, typename PSA, typename ...A, typename ...BSM, typename ...SSM, typename ...TCM, typename ...Its1, typename ...Its2, typename ...SLTP, typename L>
void backtest::optimal_portfolio_stop_loss_take_profit_backtest(std::tuple<A...>& assets, std::tuple<std::vector<Trade<A, TCM>>...>& trade_logs, arma::Mat<T>& rebalanced_weights, 
		std::vector<std::vector<double>>& rebalanced_balances, std::vector<Datetime>& rebalance_datetimes, const std::tuple<BSM...>& buy_sig_models, const std::tuple<SSM...>& sell_sig_models, 
		const std::vector<arma::Mat<T>>& bsm_matrices, const std::vector<arma::Mat<T>>& ssm_matrices, PSA psa, const std::tuple<TCM...> tcms, const std::tuple<SLTP...> sltps,  
		const std::tuple<std::pair<Its1, Its1>...>& bsm_mat_it_pairs, const std::tuple<std::pair<Its2, Its2>...>& ssm_mat_it_pairs, 
		const std::vector<double>& bs_entry_threshs, const std::vector<double>& bs_exit_threshs, const std::vector<double>& ss_entry_threshs, const std::vector<double>& ss_exit_threshs,  
		const std::vector<L>& bs_mcs, const std::vector<L>& ss_mcs, std::index_sequence<Is...> n_rebal_idx_seq,  
		double balance, double target_return, double neg_exp_weight){ 
	OptimalPortfolio op = OptimalPortfolio(target_return, neg_exp_weight); 	
	//make the index sequence 
	auto indices = std::make_index_sequence<sizeof ...(A)>{};
	arma::Col<double> weights(indices.size());
	weights.fill(1.0 / indices.size()); 
	//get the initial starting balances 
	std::vector<double> asset_balances(sizeof ...(A));
	backtest::optimal_balances(balance, asset_balances, weights);
	std::vector<double> starting_balances = asset_balances; 
	//create the rebalancing splits 
	std::vector<std::tuple<std::pair<Its1, Its1>...>> bsm_mat_splits_it_pairs; 
	std::vector<std::tuple<std::pair<Its2, Its2>...>> ssm_mat_splits_it_pairs;
	std::vector<std::vector<arma::Mat<T>>> bsm_mat_splits; 
	std::vector<std::vector<arma::Mat<T>>> ssm_mat_splits;
	
	utility::split_matrices(bsm_matrices, bsm_mat_splits, bsm_mat_it_pairs, bsm_mat_splits_it_pairs, n_rebal_idx_seq, indices); 
	utility::split_matrices(ssm_matrices, ssm_mat_splits, ssm_mat_it_pairs, ssm_mat_splits_it_pairs, n_rebal_idx_seq, indices); 
	
	rebalanced_weights.insert_cols(rebalanced_weights.n_cols, weights);
	rebalanced_balances.push_back(starting_balances);
	rebalance_datetimes.push_back(std::get<0>(bsm_mat_splits_it_pairs[0]).first->dt());

	for(int i = 0; i < bsm_mat_splits.size(); i++){
		//call market_order_backtest (calls sync_iterators so it doesn't matter if the corresponding bsm_mat_splits_it_pair and ssm_mat_splits_it_pairs are a bit off from each other) 
		backtest::siloed_stop_loss_take_profit_backtest(assets, trade_logs, buy_sig_models, sell_sig_models, bsm_mat_splits[i], ssm_mat_splits[i], psa, tcms, sltps, 
				bsm_mat_splits_it_pairs[i], ssm_mat_splits_it_pairs[i], bs_entry_threshs, bs_exit_threshs, ss_entry_threshs, ss_exit_threshs, 
				bs_mcs, ss_mcs, asset_balances, indices);
		//use the trade_logs to compute the optimal portfolio weights 
		backtest::optimal_weights_from_trade_logs(trade_logs, weights, op);
		rebalanced_weights.insert_cols(rebalanced_weights.n_cols, weights); 
		//reset the target (it will have been lowered if the max of the expected returns was lower than the target)
		op.update_target(target_return); 
		//compute the new asset balances
		asset_balances = backtest::compute_new_balances(trade_logs, starting_balances);
		balance = std::accumulate(asset_balances.cbegin(), asset_balances.cend(), 0.0);
		//compute the rebalanced asset balances
		backtest::optimal_balances(balance, asset_balances, weights);
		rebalanced_balances.push_back(asset_balances); 
		//insert the datetime of the rebalance 
		rebalance_datetimes.push_back(std::get<0>(bsm_mat_splits_it_pairs[i]).second->dt());
	}
}

