#pragma once
#include <functional> 
#include "../model.h" 

//T is the type stored in the arma::Mat data matrices 
//L is the type stored in the arma::Row labels rows 
//M is the model type (needs a .Train method)
//LabItPair is a pair of iterators to Timestamp objects used for constructing the labels (assumes second element in lab_it_pair is a .end iterator)
//LabFcn is the unary which converts a Timestamp object to a label
//PointTypes is the parameter pack of the types in the hyperparameter tuples
template <typename T, typename L, typename M, typename LabItPair, typename LabFcn, typename ...PointTypes> 
class GridSearch{
	public: 
		GridSearch() = default;
		//parameterized constructor  
		GridSearch(LabItPair lab_it_pair, LabFcn lab_fcn, int k_train_splits, int k_test_splits, double test_split_ratio, const std::vector<std::tuple<PointTypes...>>& grid);
		//Tune the hyperparameters (f is a function which takes the parameter tuple, builds a feature & constructs the test and train splits)
		void Tune(std::function<void (std::tuple<PointTypes...>& param_tuple, LabItPair lab_it_pair, LabFcn lab_fcn, std::vector<arma::Mat<T>>& train_mat_splits, 
					std::vector<arma::Mat<T>>& test_mat_splits, std::vector<arma::Row<L>>& train_lab_splits, std::vector<arma::Row<L>>& test_lab_splits,
					int k_train_splits, int k_test_splits, double test_ratio_splits)> f);
		//path is where to save the TuningReport file (just a txt file)
		//description should detail what the hyperparameters are for (prints to the console each parameter tuple and its averaged AUROC)
		void TuningReport(const std::string& path, const std::string& description); 
	private:
		//start and end datetimes for the the hyperparameter tuning
		LabItPair lab_it_pair_;
		LabFcn lab_fcn_; 
		int k_train_splits_ = 1;
		int k_test_splits_ = 3;
		//the ratio of data to include in the test split
		double test_split_ratio_ = .3; 
		std::vector<std::tuple<PointTypes...>> grid_;
		//AUROC averaged over each split combination for each parameter tuple 
		std::vector<std::tuple<std::tuple<PointTypes...>, double>> tuning_results_; 
}; 
	
template <typename T, typename L, typename M, typename LabItPair, typename LabFcn, typename ...PointTypes> 
GridSearch<T, L, M, LabItPair, LabFcn, PointTypes...>::GridSearch(
		LabItPair lab_it_pair, LabFcn lab_fcn, int k_train_splits, int k_test_splits, double test_split_ratio, const std::vector<std::tuple<PointTypes...>>& grid) : 
		lab_it_pair_{lab_it_pair}, lab_fcn_{lab_fcn}, k_train_splits_{k_train_splits}, k_test_splits_{k_test_splits}, test_split_ratio_{test_split_ratio}, grid_{grid} {};  
	
template <typename T, typename L, typename M, typename LabItPair, typename LabFcn, typename ...PointTypes> 
void GridSearch<T, L, M, LabItPair, LabFcn, PointTypes...>::Tune(
		std::function<void (std::tuple<PointTypes...>& param_tuple, LabItPair lab_it_pair, LabFcn lab_fcn, std::vector<arma::Mat<T>>& train_mat_splits, 
        std::vector<arma::Mat<T>>& test_mat_splits, std::vector<arma::Row<L>>& train_lab_splits, std::vector<arma::Row<L>>& test_lab_splits, int k_train_splits, int k_test_splits, double test_ratio_splits)> f){
	//initialize tuning_results_ with zero scores
	tuning_results_.resize(grid_.size());
	auto first_it = tuning_results_.begin(); 
	auto eval_param_tuple = [first_it, f, this](int k){
		auto pt = grid_[k];
		//Note: stratified_k_split calls push_back ==> dont need to size these 
		std::vector<arma::Mat<T>> train_ms;
		std::vector<arma::Mat<T>> test_ms;
		std::vector<arma::Row<L>> train_ls; 
		std::vector<arma::Row<L>> test_ls;
		//call the function f on the parameter tuple 
		f(pt, lab_it_pair_, lab_fcn_, train_ms, test_ms, train_ls, test_ls, k_train_splits_, k_test_splits_, test_split_ratio_);
		//get the minority class 
		L mc = model::minority_class(train_ls[0]); 
		double avg_auroc = 0.0;
		for(int i = 0; i < train_ms.size(); i++){
			//create a model of type M and train it on the ith train split 
			M mod;
			mod.Train(train_ms[i], train_ls[i], 2);
			//compute the auroc
			for(int j = 0; j < test_ms.size(); j++){
				arma::Row<L> preds; 
				arma::mat probs;
				mod.Classify(test_ms[j], preds, probs);
				//scores are the probabilities for the positive class (==> minority class); 
				avg_auroc += mlpack::ROCAUCScore<1>::Evaluate(test_ls[j], probs.row(mc));
			}
		}
		avg_auroc /= (train_ms.size() * test_ms.size());
		utility::tuple_print(pt); 
		std::cout << " Averaged AUROC: " << avg_auroc << std::endl; 
		*std::next(first_it, k) = std::make_tuple(pt, avg_auroc); 
	};
	std::vector<int> indices(grid_.size());
	std::iota(indices.begin(), indices.end(), 0);
	//Note: it does not seem to speed things up using either par_unseq or executing eval_param_tuple on separate threads
	std::for_each(std::execution::seq, indices.begin(), indices.end(), eval_param_tuple); 
}
template <typename T, typename L, typename M, typename LabItPair, typename LabFcn, typename ...PointTypes> 
void GridSearch<T, L, M, LabItPair, LabFcn, PointTypes...>::TuningReport(const std::string& path, const std::string& description){
	std::string text = "Grid Search Hyperparameter Tuning \n";   
	text += ("Parameter Description: " + description + "\n");
	text += ("Hyperparameter Tuning Start Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", lab_it_pair_.first->dt().zt()) + "\n"); 
	text += ("Hyperparameter Tuning End Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", std::prev(lab_it_pair_.second)->dt().zt()) + "\n"); 
	for(auto tp : tuning_results_){
		text += ("Parameter Tuple: " + utility::tuple_string(std::get<0>(tp)) + " Averaged AUROC: " + std::to_string(std::get<1>(tp)) + "\n"); 
	}
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << text;
	ofs.close(); 
}

