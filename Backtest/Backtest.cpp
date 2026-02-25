#include "Backtest.h" 

backtest::BalPropPosSize::BalPropPosSize(double p, double l) : proportion_{p}, lev_fac_{l} {validate_();};

double backtest::BalPropPosSize::compute_position_size(double bal) const{
	if(bal < 0.0){
		//std::cout << "ProportionBalancePS: Balance parameter bal is negative. We return a position size of 0.0" << std::endl; 
		throw std::invalid_argument("ProportionBalancePS: balance parameter bal is negative"); 
	}
	return bal * lev_fac_ * proportion_; 
}
void backtest::BalPropPosSize::validate_() const{
	if(proportion_ <= 0 || lev_fac_ <= 0){
		throw std::invalid_argument("ProportionBalancePS constructor: proportion_ and lev_fac_ must be positive"); 
	}
}

//ATRSLTP constructor
backtest::ATRSLTP::ATRSLTP(int k, double stop_mult, double take_mult) : k_{k}, stop_mult_{stop_mult}, take_mult_{take_mult} {}; 

//LinearTCM constructor 
backtest::LinearTCM::LinearTCM(double fac) : fac_{fac} {};
	
	

void backtest::performance_metrics_table(std::string& latex, const std::tuple<int, double, double, double, double, double, double, double, double, double>& basic_metrics, 
		const std::pair<double, double>& sharpe_and_sortino, double max_drawdown, const arma::Col<double>& reshuffled_mdd_90th_10th){ 
	
	auto headers = std::make_tuple("Performance Metric", "Value");
	std::vector<std::tuple<std::string, double>> rows{std::make_tuple("Starting Balance", std::get<1>(basic_metrics)),
								   std::make_tuple("Final Balance", std::get<2>(basic_metrics)), 
								   std::make_tuple("Total Log Return", std::get<3>(basic_metrics)),
								   std::make_tuple("Total Percentage Return", std::get<4>(basic_metrics)), 
								   std::make_tuple("Win Percentage", std::get<5>(basic_metrics)),
								   std::make_tuple("Average Win", std::get<6>(basic_metrics)),
								   std::make_tuple("Average Loss", std::get<7>(basic_metrics)),
								   std::make_tuple("Average Win Log Return", std::get<8>(basic_metrics)),
								   std::make_tuple("Average Loss Log Return", std::get<9>(basic_metrics)),
								   std::make_tuple("Sharpe Ratio", sharpe_and_sortino.first),
								   std::make_tuple("Sortino Ratio", sharpe_and_sortino.second),
								   std::make_tuple("Maximum Drawdown", max_drawdown),
								   std::make_tuple("Reshuffled: 10th Percentile of Max DD", reshuffled_mdd_90th_10th.at(0)),
								   std::make_tuple("Reshuffled: 90th Percentile of Max DD", reshuffled_mdd_90th_10th.at(1))};

	auto h_fcn_console = [](const std::string& s){
		return s; 
	};
	auto num_console = [](const double& d){
		return d; 
	}; 
	auto num_latex = [](double d){return utility::float_cell(d, 4);};
	auto h_fcn_latex = [](const std::string& s){
		return "\\textbf{" + s + "}"; 
	}; 
	auto h_fcns_console = std::make_tuple(h_fcn_console, h_fcn_console);
	auto row_fcns_console = std::make_tuple(h_fcn_console, num_console);
	auto h_fcns_latex = std::make_tuple(h_fcn_latex, h_fcn_latex);
	auto row_fcns_latex = std::make_tuple(h_fcn_latex, num_latex); 
	//console table
	std::vector<int> col_widths{42, 15};  
	utility::table_print(headers, h_fcns_console, rows, row_fcns_console, col_widths);
	//latex table
	latex += utility::table_latex(headers, h_fcns_latex, rows, row_fcns_latex, "center", "|| l | c ||"); 
}

