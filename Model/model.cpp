#include "model.h" 

std::string model::sampling_report(std::string sampling_type, std::tuple<int, int, int, double> cib, std::tuple<int, int, int, double> cia, int k){
	std::string r1_label = "Prior To " + sampling_type;
	std::string r2_label = "After " + sampling_type;
	std::vector<std::tuple<std::string, int, int, int, double>> rows; 
	rows.push_back(std::tuple_cat(std::make_tuple(r1_label), cib)); 
	rows.push_back(std::tuple_cat(std::make_tuple(r2_label), cia));
	auto headers = std::make_tuple("", "No. Instances", "No. Minority", "No. Majority", "Minority Proportion");
	auto s_fcn = [](std::string s){
		return s; 
	}; 
	auto lab_fcn = [](std::string s){
		return "\\textbf{" + s + "}"; 
	};
	auto i_fcn = [](int i){
		return utility::integral_format(i); 
	}; 
	auto i_cell = [](int i){
		return utility::integral_cell(i); 
	};
	auto d_fcn = [k](double d){
		return utility::float_format(d, k); 
	}; 
	auto d_cell = [k](double d){
		return utility::float_cell(d, k); 
	};
	auto header_fcns_console = std::make_tuple(s_fcn, s_fcn, s_fcn, s_fcn, s_fcn); 
	auto header_fcns = std::make_tuple(lab_fcn, lab_fcn, lab_fcn, lab_fcn, lab_fcn);
	auto r_fcns_console = std::make_tuple(s_fcn, i_fcn, i_fcn, i_fcn, d_fcn);
	auto r_fcns = std::make_tuple(lab_fcn, i_cell, i_cell, i_cell, d_cell);
	//print the table to the console
	std::vector<int> col_widths{(11 + (int)sampling_type.length()), 15, 14, 14, 21}; 
	utility::table_print(headers, header_fcns_console, rows, r_fcns_console, col_widths);
	//generate & return the latex for the table
	return "\\sect{" + sampling_type + "} \n" + utility::table_latex(headers, header_fcns, rows, r_fcns, "center", "|| l | c | c | c | c ||"); 
}

std::string model::tomeks_report(std::tuple<int, int, int, double> cib, std::tuple<int, int, int, double> cia){
	//the number of majority instances removed from tomek links (second element of tuples from class info is #majority)
	int nmajr = std::get<2>(cib) - std::get<2>(cia);  
	std::cout << "Tomeks Algorithm: Removed " << nmajr << " majority class instances from Tomek Links." << std::endl; 
	std::string latex = "\\sect{Tomek's Algorithm} \n";
	latex += "\\textbf{Result}: Removed $" + std::format("{:d}", nmajr) + "$ majority class instances from Tomek Links. \n";
	return latex; 
}
//Note: if we have a tie, the point is assigned the highest label
size_t model::MajorityVote::vote(arma::Row<size_t>& nbs_labs, arma::mat& dists, int n){
	//reciprocal of the number of neighbors
	double rep_k = 1 / double(nbs_labs.n_cols);
	//to keep track of the max probability encountered
	double max_p = 0.0; 
	size_t label = -1;
	for(int i = 0; i < n; i++){
		//get the indicies of the neighbors with class label i
		arma::uvec class_i_idx = arma::find(nbs_labs == i);
		//probability of class i
		double p_i = rep_k * class_i_idx.n_elem;
		if(p_i >= max_p){
			//if we have a new highest class probability, set the max and the label
			max_p = p_i;
			label = i; 
		}
	}
	return label; 
}
size_t model::MajorityVote::vote(arma::Row<size_t>& nbs_labs, arma::mat& dists, arma::vec& p_vec, int n){
	size_t label = -1;
	//reciprocal of the number of neighbors
	double rep_k = 1 / double(nbs_labs.n_cols);
	//to keep track of the max probability encountered
	double max_p = 0.0; 
	for(int i = 0; i < n; i++){
		//get the indicies of the neighbors with class label i
		arma::uvec class_i_idx = arma::find(nbs_labs == i);
		//probability of class i
		double p_i = rep_k * class_i_idx.n_elem;
		//write the probability of class i to p_vec 
		p_vec.at(i) = p_i;
		if(p_i >= max_p){
			//if we have a new highest class probability, set the max and the label
			max_p = p_i;
			label = i; 
		}
	}
	return label; 
}

size_t model::InverseDistanceVote::vote(arma::Row<size_t>& nbs_labs, arma::vec& dists, int n){
	size_t label = -1;
	//set the 0.0 distances to an epsilon value 
	arma::uvec zdists_idx = arma::find(dists == 0.0);
	for(int i : zdists_idx){
		dists.at(0, i) = 5 * (std::pow(10, -6)); 
	}
	//sum the inverse of the distances
	double dsum = arma::accu(1 / dists);
	//keep track of the maximum class probability 
	double max_p = 0.0;
	for(size_t i = 0; i < n; i++){
		arma::uvec class_i_idx = arma::find(nbs_labs == i);
		arma::vec class_i_dists = dists.rows(class_i_idx); 
		double p_i = 0.0; 
		if(class_i_dists.n_elem != 0){
			p_i = (1 / dsum) * arma::accu((1 / class_i_dists));
		}	
		if(p_i >= max_p){
			//if we have a new maximum class probability 
			max_p = p_i; 
			label = i; 
		}
	}
	return label; 
}
size_t model::InverseDistanceVote::vote(arma::Row<size_t>& nbs_labs, arma::vec& dists, arma::vec& p_vec, int n){
	size_t label = -1;
	//set the 0.0 distances to an epsilon value 
	arma::uvec zdists_idx = arma::find(dists == 0.0);
	for(int i : zdists_idx){
		dists.at(0, i) = 5 * (std::pow(10, -6)); 
	}
	//sum the inverse of the distances
	double dsum = arma::accu(1 / dists);
	//keep track of the maximum class probability 
	double max_p = 0.0;
	for(size_t i = 0; i < n; i++){
		arma::uvec class_i_idx = arma::find(nbs_labs == i);
		arma::vec class_i_dists = dists.rows(class_i_idx); 
		double p_i = 0.0; 
		if(class_i_dists.n_elem != 0){
			p_i = (1 / dsum) * arma::accu((1 / class_i_dists));
		}
		//write the class i probability
		p_vec.at(i) = p_i; 
		if(p_i >= max_p){
			//if we have a new maximum class probability 
			max_p = p_i; 
			label = i; 
		}
	}
	return label; 
}


