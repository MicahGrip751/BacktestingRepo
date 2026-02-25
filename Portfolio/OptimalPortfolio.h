#include <vector> 
#include <algorithm> 
#include "../Timestamp/Timestamp.h"
#include <armadillo> 
#include <execution>
#include <stdexcept>
#include <vector>
#include <libalglib/stdafx.h>
#include <libalglib/specialfunctions.h>
#include <libalglib/ap.h>
#include <libalglib/solvers.h>
#include <libalglib/linalg.h>
#include <libalglib/alglibmisc.h>
#include <libalglib/alglibinternal.h>
#include <libalglib/optimization.h>

class OptimalPortfolio{
	public:
		OptimalPortfolio(double target_return, double neg_exp_asset_weight); 
		template <typename T>
		OptimalPortfolio(const arma::Mat<T>& returns_mat, double target_return, double neg_exp_asset_weight);
		template <typename T> 
		void update_Q(const arma::Mat<T>& returns_mat);
		template <typename T> 
		void update_target(T new_target_return); 
		template <typename T> 
		T objective_function(const arma::Col<T>& w) const;
		//evaluate the gradient of the objective function at the point w
		template <typename T> 
		void evaluate_gradient(const arma::Col<T>& w, arma::Col<T>& grad_w) const;
		//compute the step size for the next iteration of gradient descent
		template <typename T> 
		T step_size(const arma::Col<T>& w_n, const arma::Col<T>& w_n_1, const arma::Col<T>& grad_w_n, const arma::Col<T>& grad_w_n_1) const;
		//project w onto the constraints
		template <typename T> 
		void constraint_projection(arma::Col<T>& w, const arma::Col<T>& exprected_returns) const; 
		//return if a weights vector satisfies the constraints
		template <typename T> 
		bool satisfies_constraints(const arma::Col<T>& w, const arma::Col<T>& expected_returns) const;
		//compute the optimal weights using gradient descent & projection onto constraints (optimal weights are stored in w when finished)
		//w is an initial guess, step is an initial step size
		template <typename T> 
		void compute_optimal_weights(arma::Col<T>& w, const arma::Col<T>& expected_returns, T step = 0.1, T eps = std::pow(10, -5), int max_iter = 200); 
	private:
		//Q matrix (size is returns_mat.n_rows x returns_mat.n_rows) contains the variance of each row in returns_mat on the diagonal & the covariances in the strictly
		//upper triangular portion
		arma::sp_mat Q_;
		double target_return_; 
		//the weight to assign to assets with negative expected value in the event that all the assets have negative expected return
		double neg_exp_asset_weight_; 
}; 

template <typename T> 
OptimalPortfolio::OptimalPortfolio(const arma::Mat<T>& returns_mat, double target_return, double neg_exp_asset_weight) : target_return_{target_return}, neg_exp_asset_weight_{neg_exp_asset_weight} 
{
	//call update_Q with the returns_mat
	update_Q(returns_mat); 
}

template <typename T>
void OptimalPortfolio::update_Q(const arma::Mat<T>& returns_mat){
	//copy the covariances from cov_mat & store them in the upper triangular portion of Q_
	Q_ = arma::trimatu(arma::cov(returns_mat.t()));
	//compute the variances & store them in the diagonal of Q_ 
	Q_.diag() = arma::var(returns_mat.t()); 
}

template <typename T> 
void OptimalPortfolio::update_target(T new_target_return){
	target_return_ = (double) new_target_return; 
}

template <typename T> 
T OptimalPortfolio::objective_function(const arma::Col<T>& w) const{
	//the objective function is w^TQw
	arma::vec obj = w.t() * Q_ * w;
	//obj is a 1x1 arma vector
	return obj.at(0); 
}
template <typename T> 
void OptimalPortfolio::evaluate_gradient(const arma::Col<T>& w, arma::Col<T>& grad_w) const{
	arma::uvec idx = arma::linspace<arma::uvec>(0, Q_.n_rows - 1, Q_.n_rows); 
	//compute the ith component of the gradient at w 
	auto ith_comp = [&grad_w, &w, this](int i){
		arma::sp_vec col_i = Q_.col(i);
		arma::sp_vec row_i = Q_.row(i).as_col();
		T ip1 = arma::dot(w, col_i);
		T ip2 = arma::dot(w, row_i); 
		grad_w.at(i) = ip1 + ip2;
	};
	std::for_each(idx.begin(), idx.end(), ith_comp);
}
template <typename T> 
T OptimalPortfolio::step_size(const arma::Col<T>& w_n, const arma::Col<T>& w_n_1, const arma::Col<T>& grad_w_n, const arma::Col<T>& grad_w_n_1) const{
	arma::Col<T> grad_diff = grad_w_n - grad_w_n_1;  
	arma::Col<T> num = (w_n - w_n_1).t() * (grad_diff);
	double denom = arma::norm2est(grad_diff);  
	return num.at(0) / denom; 
}

template <typename T> 
void OptimalPortfolio::constraint_projection(arma::Col<T>& w, const arma::Col<T>& expected_returns) const{
	int n = w.n_elem; 
	alglib::minlpstate state;
	
	//problem size (first parameter) is the number of variables in the LP
	alglib::minlpcreate(2*n, state);  

	//cost function
	std::vector<double> cv(n, 0);
	for(int i = 0; i < n; i++){
		cv.push_back(1.0);
	}

	alglib::real_1d_array cost;
	cost.setcontent(2*n, &cv[0]);
	alglib::minlpsetcost(state, cost);
	
	//linear constraint for the weights to sum to 1
	arma::mat m = arma::ones(1, n);
	m = arma::join_rows(m, arma::zeros(1, n));
	m = arma::join_rows(m, arma::ones(1, 1));
	//linear constraints for linearizing the absolute value 
	arma::mat I = arma::eye(n, n);
	arma::mat block = arma::join_rows(I, I);
	block = arma::join_rows(block, w);
	m = arma::join_cols(m, block);
	block = arma::join_rows(-1*I, I);
	block = arma::join_rows(block, -1*w);
	m = arma::join_cols(m, block);
	
	//linear constraint for the weighted expected return to be above the target 
	block = expected_returns.as_row();
	block = arma::join_rows(block, arma::zeros(1, n)); 
	block = arma::join_rows(block, arma::rowvec{target_return_}); 
	m = arma::join_cols(m, block); 
	//alglib takes row major matrices ==> we have to transpose the arma::mat
	m = m.t();

	//set the box constraints (for w_i' and t_i) 
	alglib::minlpsetbcall(state, 0, 1); 
	// linear constraints
	alglib::real_2d_array constr_mat; 
	int n_constr = 2*n + 2;
	constr_mat.setlength(n_constr, 2*n + 1); 
	constr_mat.setcontent(n_constr, 2*n +1, &(*m.begin()));
	
	//constraint types (integer 1d array with positive values as the constraints are >= )
	std::vector<alglib::ae_int_t> ct_vec{0};
	for(int i = 0; i < 2*n + 1; i++){
		ct_vec.push_back(1); 
	}

	alglib::integer_1d_array ct; 
	ct.setcontent( n_constr, &ct_vec[0]);
	//set the constraints
	alglib::minlpsetlc(state, constr_mat, ct, n_constr); 
	//create minlpreport object 
	alglib::minlpreport report;
	alglib::real_1d_array opt;
	//solve the LP
	alglib::minlpsetalgoipm(state); 
	alglib::minlpoptimize(state); 
	//get the report 
	alglib::minlpresults(state, opt, report);

	int term = report.terminationtype; 
	if(term == -4){
		std::cout << "LP is primal unbounded (dual infeasible)" << std::endl; 
	}else if(term == -3){
		std::cout << "LP is primal infeasible (dual unbounded)" << std::endl; 
	}else if(term >= 1 && term <= 4){
		//print nothing if the LP was solved successfully
	}else if(term == 5){
		std::cout << "max iterations were taken." << std::endl; 
	}else{
		std::cout << "LP was not solved successfully." << std::endl; 
	}
	//assign each component of opt to w 
	auto opt_ptr = opt.getcontent(); 
	for(int i = 0; i < n; i++){
		w.at(i) = *(opt_ptr);
		opt_ptr++; 
	}
}

template <typename T> 
bool OptimalPortfolio::satisfies_constraints(const arma::Col<T>& w, const arma::Col<T>& expected_returns) const{
	//compute the weighted exprected return 
	T weighted_expected_return = arma::dot(w, expected_returns);  	
	if((weighted_expected_return < target_return_) || ((arma::accu(w) - 1.0) >= std::pow(10, -6))){
		return false; 	
	}
	//check if the sum of the weights is within 10^-8 of 1
	if(arma::accu(w) - 1.0 >= std::pow(10, -8)){
		return false; 
	}
	//if the weighted expected return is above the target return and the weights sum to 1 then we satisfy the constraints
	return true; 
}


template <typename T> 
void OptimalPortfolio::compute_optimal_weights(arma::Col<T>& w_n, const arma::Col<T>& expected_returns, T step, T eps, int max_iter){
	//if the max of the expected_returns is lower than the target then we will not be able to compute the optimal weights
	T max_er = arma::max(expected_returns); 
	if(max_er <= 0.0){
		w_n.fill(neg_exp_asset_weight_);
		std::cout << "All the assets had negative expected return and so were assigned the weight neg_exp_asset_weight_" << std::endl;
		return; 
	}else if(max_er <= target_return_){
		//reset the target_return
		arma::uvec pos_er_idx = arma::find(expected_returns > 0);
		arma::Col<T> positive_exp_rets = expected_returns.rows(pos_er_idx);
		this->update_target(arma::mean(positive_exp_rets));
		std::cout << "The target return was updated to: " << target_return_ << std::endl; 
	}

	//declare variables for the gradient of w_n, the weights vector w_{n + 1} and the gradient of w_{n + 1}
	arma::Col<T> grad_w_n(w_n.n_elem);
	arma::Col<T> w_n_1(w_n.n_elem);
	arma::Col<T> grad_w_n_1(w_n.n_elem);
	//compute the gradient of w_n	
	evaluate_gradient(w_n, grad_w_n);
	//take a step against the gradient 
	w_n_1 = w_n - (step * grad_w_n);
	//compute the gradient of w_{n+1}
	evaluate_gradient(w_n_1, grad_w_n_1);
	//counter for the number of iterations 
	int n = 0; 
	//check if the constraints are satisfied 
	if(!satisfies_constraints(w_n_1, expected_returns)){
		//compute the projection
		constraint_projection(w_n_1, expected_returns); 
	}
	while(arma::norm2est(w_n_1 - w_n) >= eps && n < max_iter){
		//compute the step size
		step = step_size(w_n_1, w_n, grad_w_n_1, grad_w_n); 
		//shift w_{n+1} and its gradient
		w_n = w_n_1;
		grad_w_n = grad_w_n_1; 
		//take a step against the gradient to compute the next iterations weights vector
		w_n_1 = w_n - (step * grad_w_n);
		//check if w_n_1 satisfies the constraints (if not project)
		if(!satisfies_constraints(w_n_1, expected_returns)){
			constraint_projection(w_n_1, expected_returns);
		}
		//compute the gradient at the new point
		evaluate_gradient(w_n_1, grad_w_n_1); 
		//increment the number of iterations
		n++; 
	}
	//assign w_n_1 to the output w_n 
	w_n = w_n_1; 
}



