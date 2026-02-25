#pragma once
#define MLPACK_USE_SYSTEM_STB
#include <mlpack/methods/random_forest.hpp>
#include <execution>

/*
	Note: This feature selector is meant for selecting features in binary classification problems (regardless of 
	the presence of class imbalances). The AUROC metric can be computed for multiclass classification models, however 
	(given the function call in Select) it computes the performance of the model in classifying the class whose 
	probabilities are in the 2nd row of	the probs matrix outputted by the .Classify() call. 
*/

//RFFS is templated based on the fitness function 
template <typename F>
class RFFS{
	public:
		RFFS() = default;
		//k is the number of features to retain, c is the number of classes, t is the number of trees to include in the RF
		//mgs is the minimum gini gain for a split, md is the maximum depth for the trees in the random forest
		//imp thresh is the importance threshold for features to be kept (if not used then k are kept)
		RFFS(int k); 
		RFFS(double imp_thresh); 
		RFFS(int k, int c, int t, double mgs, int md);
		RFFS(double imp_thresh, int c, int t, double mgs, int md);
		//Perform the feature selection
		void Select(arma::mat train, arma::Row<size_t> train_lab, arma::mat test, arma::Row<size_t> test_lab);
		//display the feature importances 
		void display_fi() const; 
		//getter for the vector of indices of the k most important features
		arma::uvec get_import() const; 
		//return the latex code to be used in reports 
		std::string report() const; 
	private:
		int k_ = 1;
		int c_ = 2; 
		int t_ = 20;
		double mgs_ = std::pow(10, -7);
		int md_ = 0;
		//importance threshold (used if specified in the constructor)
		double imp_thresh_ = -1.0; 
		//arma vector to store the feature importances
		//multimap to store the feature importances and feature indices in sorted order 
		std::multimap<double, int> mmp_; 
		//arma::uvec to store the row indices of the most important features
		arma::uvec indic_;  
};
template <typename F> 
RFFS<F>::RFFS(int k) : k_{k} {}; 

template <typename F> 
RFFS<F>::RFFS(double imp_thresh) : imp_thresh_{imp_thresh} 
{
	if(imp_thresh <= 0 || imp_thresh_ >= 1){
		throw std::invalid_argument("RFFS Constructor: imp_thresh must be strictly between 0 and 1."); 	
	}
}; 

template <typename F>
RFFS<F>::RFFS(int k, int c, int t, double mgs, int md) : k_{k}, c_{c}, t_{t}, mgs_{mgs}, md_{md} 
{
	if(c <= 0){
		throw std::invalid_argument("RFFS Constructor: The number of classes c should be > 0"); 
	}
	if(t <= 0){
		throw std::invalid_argument("RFFS Constructor: The number of trees t should be > 0"); 
	}
	if(md <= 0){
		throw std::invalid_argument("RFFS Constructor: The maximum tree depth should be > 0"); 
	}
	if(mgs <= 0){
		throw std::invalid_argument("RFFS Constructor: The minimum gini gain should be > 0"); 
	}
};

template <typename F>
RFFS<F>::RFFS(double imp_thresh, int c, int t, double mgs, int md) : imp_thresh_{imp_thresh}, c_{c}, t_{t}, mgs_{mgs}, md_{md}
{
	if(imp_thresh <= 0 || imp_thresh_ >= 1){
		throw std::invalid_argument("RFFS Constructor: imp_thresh must be strictly between 0 and 1."); 	
	}
	if(c <= 0){
		throw std::invalid_argument("RFFS Constructor: The number of classes c should be > 0"); 
	}
	if(t <= 0){
		throw std::invalid_argument("RFFS Constructor: The number of trees t should be > 0"); 
	}
	if(md <= 0){
		throw std::invalid_argument("RFFS Constructor: The maximum tree depth should be > 0"); 
	}
	if(mgs <= 0){
		throw std::invalid_argument("RFFS Constructor: The minimum gini gain should be > 0"); 
	}

}; 

template <typename F>
void RFFS<F>::Select(arma::mat train, arma::Row<size_t> train_lab, arma::mat test, arma::Row<size_t> test_lab){
	if(k_ > train.n_rows){
		throw std::invalid_argument("Select: The number of features to select must be less than or equal to the number of features."); 
	}
	//get a vector of the indices from 0 to train.n_rows - 1;
	arma::uvec ind = arma::linspace<arma::uvec>(0, train.n_rows - 1, train.n_rows);
	//compute the AUROC of the rf with all features
	mlpack::RandomForest<F> rf; 
	rf.Train(train, train_lab, c_, t_, 1, mgs_, md_);
	//assess model performance by looking at AUROC
	arma::Row<size_t> preds; 
	arma::mat probs; 
	rf.Classify(test, preds, probs);
	double auroc_all = mlpack::ROCAUCScore<1>::Evaluate(test_lab, probs.row(1)); 
	
	//lambda function to train a random forest on all the features except the feature in index i in ind 
	auto rf_lamb = [&](const arma::uword i){
		//get the indices apart from i
		arma::uvec ind_wo_i = arma::find(ind != i);
		//train the random forest 
		mlpack::RandomForest<F> rf; 
		arma::mat train_wo_i = train.rows(ind_wo_i);
		
		rf.Train(train_wo_i, train_lab, c_, t_, 1, mgs_, md_);
		//assess model performance by looking at AUROC
		arma::Row<size_t> preds; 
		arma::mat probs; 
		rf.Classify(test, preds, probs);
		//scores are the probabilities for the positive class (==> minority class); 
		double auroc = mlpack::ROCAUCScore<1>::Evaluate(test_lab, probs.row(1));
		//set the feature importance as the difference between the AUROC of the model with 
		//all the features to the AUROC of the model without the ith predictor
		
		//feat_import_.at(r) = auroc_all - auroc;
		mmp_.insert({auroc_all - auroc, i}); 	
	};
	//compute & store the feature importances 
	std::for_each(std::execution::par_unseq, ind.begin_col(0), ind.end_row(train.n_rows - 1), rf_lamb);
	//selecting k most important features (used if no threshold is specified)
	if(imp_thresh_ == -1.0){
		//resize the indic_ vector 
		indic_.resize(k_);
		//store the indices of the most important features
		auto it = mmp_.rbegin(); 
		for(int i = 0; i < k_; i++){
			indic_.at(i) = it->second;
			it++; 
		}
	}else{
		auto it = mmp_.rbegin(); 
		while(it->first >= imp_thresh_){
			arma::uvec uv{arma::uword(it->second)}; 
			indic_.insert_rows(indic_.n_rows, uv);  
			it++; 
		}
	}
}

//display the feature importances 
template <typename F>
void RFFS<F>::display_fi() const{
	for(auto it = mmp_.rbegin(); it != mmp_.rend(); it++){
		std::cout << "Feature " << it->second << " Importance: " << it->first << std::endl; 
	}
}

template <typename F>
arma::uvec RFFS<F>::get_import() const{
	return indic_; 	
}

template <typename F> 
std::string RFFS<F>::report() const{
	std::string ltx; 
	ltx += "\\sect{Random Forest Feature Selection} \n";
	if(imp_thresh_ != -1.0){
		ltx += "\\textbf{Importance Threshold:} "; 
		ltx += " The importance threshold of " + std::format("{:.2f}", imp_thresh_) + " was used \n";  
	}
	ltx += "\\textbf{Selected Features:} "; 
	ltx += "Of " + std::format("{:d}", mmp_.size()) + " features, RFFS kept " + std::format("{:d}", indic_.size()) + ".";
	ltx += "\n";
	return ltx; 
}
