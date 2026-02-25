#pragma once
#define MLPACK_USE_SYSTEM_STB
#include <mlpack.hpp>
#include "../Timestamp/Timestamp.h"
#include "FeatureSelection/RFFS.h" 
#include "../Utility/utility.h" 
#include <typeinfo> 

namespace model{
	//methods for splitting an armadillo matrix 
	template <typename T, typename L> 
	void split(const arma::Mat<T>& m, const arma::Row<L>& l, arma::Mat<T>& m1, arma::Row<L>& l1, arma::Mat<T>& m2, arma::Row<L>& l2, double ratio); 
	//It first is an iterators to the first object used to create the input matrix m and input labels l, the method finds the iterator 
	//corresponding to the split point 
	template <typename T, typename L, typename It> 
	void split(arma::Mat<T>& m, arma::Row<L>& l, arma::Mat<T>& m1, arma::Row<L>& l1, arma::Mat<T>& m2, arma::Row<L>& l2, It first, It& split_it, double ratio); 
	//split data and labels into k splits
	template <typename T, typename L> 
	void k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, int k); 	
	//similar to k_splits but also returns the iterators to the split points 
	template <typename T, typename L, typename It> 
	void k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, It first, std::vector<It>& split_its, int k); 
	//perform stratified split k times to get k evenly sized splits 
	template <typename T, typename L> 
	void stratified_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, int k); 
	//call k_split on the input and labels and then perform stratified split on each split according to the ratio 	
	template <typename T, typename L, typename It> 
	void split_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_train_splits, std::vector<arma::Mat<T>>& m_test_splits, 
			std::vector<arma::Row<L>>& l_train_splits, std::vector<arma::Row<L>>& l_test_splits, int k, double ratio);
	//iterators delineate the split points of the first k_split call 
	template <typename T, typename L, typename It> 
	void split_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_train_splits, std::vector<arma::Mat<T>>& m_test_splits, 
			std::vector<arma::Row<L>>& l_train_splits, std::vector<arma::Row<L>>& l_test_splits, It first, std::vector<It>& split_its, int k, double ratio);
	
	//given a vector of labels we return the minority class
	template <typename L> 
	L minority_class(const arma::Row<L>& labels); 
	//T is the type held in the armadillo matrix, L an integral type used to store the labels
	//undersampling aglorithm
	template <typename T, typename L, typename F> 
	void undersample(arma::Mat<T>& data, arma::Row<L>& labels, F min_prop);
	//oversampling algorithm 
	template <typename T, typename L, typename F> 
	void oversample(arma::Mat<T>& data, arma::Row<L>& labels, F min_prop);
	
	//returns tuple of ncols, #minority, #majority, and proportion of minority cases
	template <typename L>
	std::tuple<int, int, int, double> binary_class_info(const arma::Row<L>& labels);
	//sampling report (k is the number of decimal places to display from the minority proportion in cia and cib) 
	std::string sampling_report(std::string sampling_type, std::tuple<int, int, int, double> cib, std::tuple<int, int, int, double> cia, int k = 4); 
	

	template <typename T, typename L, typename D>
	//D defines the metric space in which the learning occurs (data is the original data matrix, t is the transformation matrix, k is the 
	//number of neighbors to use when learing the distance, n is the number of dimensions to keep)
	mlpack::MahalanobisDistance<> lmnn_wrap(const arma::Mat<T>& data, const arma::Row<L>& labels, arma::Mat<T>& t, int k, int n, D d);   

	template <typename T, typename L, typename D>
	//D is the distance metric passed to lmnn_wrap, md is the Mahalanobis distance metric learned from lmnn, 
	//t is the transformation matrix from lmnn_wrap. 
	std::string lmnn_report(const arma::Mat<T>& data, const arma::Row<L>& labels, const arma::Mat<T>& t, D d, mlpack::MahalanobisDistance<> md);

	//data is the data matrix (w/o labels), t is the transformation matrix, rv is the amount of variance to keep, 
	//arv is the actual variance retained, dp is a templated parameter for the the decomposition policy
	template <typename T, typename DP> 
	void pca_wrap(const arma::Mat<T>& data, arma::Mat<T>& t, DP dp, double & arv, double rv = .9);
	//pca report function (prints to console & generates latex)
	template <typename T> 
	std::string pca_report(const arma::Mat<T>& m, const arma::Mat<T>& t, double arv); 

	//prints the eigenvalues of the covariance matrix (==> values in a scree plot) 
	template <typename T, typename DP>
	void scree(const arma::Mat<T>& data, arma::Mat<T>& t, DP dp, bool scale = true); 
	
	//template parameter d must be a class with a default constructor and an evaluate function (is is a distance metric)
	//k is the number of neighbors to use. 
	template <typename T, typename L, typename D>
	void tomeks_alg(arma::Mat<T>& m, arma::Row<L>& labels, int k, D& d); 
	//tomek's alg report function (generates latex & prints to console)	
	std::string tomeks_report(std::tuple<int, int, int, double> cib, std::tuple<int, int, int, double> cia);

	//smote algorithm (k is the number of neighbors used, we generate a n synthetic points interpolating between min instance & its neighbors); 
	template <typename T, typename L, typename D>
	void smote(arma::Mat<T>& m, arma::Row<L>& labels, int k, int n, D d); 
	
	//other versions of SMOTE
	template <typename D> 
	void smote_v2(arma::mat& m, int k, int n, D);
	template <typename D> 
	void smote_v3(arma::mat& m, int k, D, double alpha = .9); 

	//report functions for model performance (model M must already be trained (labels are the true labels))
	//averages performance over k splits (beta is the beta used for computing F_beta)
	//function prints to the console & returns latex 
	template <typename T, typename L, typename M> 
	std::string performance_report(const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab, M& mod, int k = 3, double beta = 1.0); 

	//prints the confusion matrix of the model for the given threshold returns the pair (precision, recall)
	template <typename T, typename L, typename M> 
	std::pair<double, double> confusion_matrix(M& model, const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab, double thresh = .5); 
	
	//prints the confusion matrices for the thresholds from 0 to 1 in .1 increments
	template <typename T, typename L, typename M> 
	void confusion_matrices(M& model, const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab); 
	

	/*
		Modelling Pipelines: 
		Common Templates & Parameters:
		Dist is a type of distance metric 
		T is the type stored in the data arma::Mat
		L is the type stored in the labels arma::Row vector 
		Mod is the type of mlpack model 
		DP is the decomposition policy used when computing the SVD (when performing PCA)
		F is the type of the fitness function which RFFS is templated on
		the path parameter is the location at which to write the generated .tex performance report
		ret_var is the amount of variance to retain when performing PCA
		ratio is the ratio of test data used when model::split is called (only in overloads which generate reports)
		k_tomeks is the number of neighbors used in Tomek's algorithm 
		k_lmnn is the number of neighbors used in learning a pseudometric during LMNN 
		d_lmnn is the number of dimensions to keep when performing LMNN
		k_smote is the number of neighbors used in the SMOTE algorithm 
		n_smote is the number of synthetic minority instance to generate for each minority instance
		min_prop is the proportion of the minority class to target when performing undersampling or oversampling 
		beta is the number used when computing the F_beta metric
		k is the number of splits of the test dataset over which we average performance metrics
		fs_thresh is the threshold used for keeping features when performing Random Forest Feature Selection
		t is the composition of any matrix transformation which occur in the modelling pipeline
		idx are the row indices of the features selected by RFFS
	*/
	
	//Pipeline #1: (split) -> tomeks -> undersample -> lmnn 
	template <typename T, typename L, typename Dist, typename Mod> 
	void tomeks_undersample_lmnn_model(Mod& m, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
			double ratio, int k_tomeks, double min_prop, int k_lmnn, int d_lmnn, int k = 3, double beta = 1.0); 	
	//version which trains the model on the whole matrix & does not generate any reports 
	template <typename T, typename L, typename Dist, typename Mod> 
	void tomeks_undersample_lmnn_model(Mod& m, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, int k_tomeks, double min_prop, int k_lmnn, int d_lmnn); 	

	//Pipeline #2: (split) -> pca -> lmnn -> smote -> tomeks
	template <typename T, typename L, typename Dist, typename Mod, typename DP> 
	void pca_lmnn_smote_tomeks_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
			double ratio, DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, int k = 3, double beta = 1.0); 
	//version which trains the model on the whole matrix & does not generate any reports 
	template <typename T, typename L, typename Dist, typename Mod, typename DP> 
	void pca_lmnn_smote_tomeks_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
			DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks); 

	//Pipeline #3: (split) -> pca -> lmnn -> tomeks -> undersample  
	template <typename T, typename L, typename Dist, typename Mod, typename DP> 
	void pca_lmnn_smote_tomeks_undersample_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
			double ratio, DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, double min_prop, int k = 3, double beta = 1.0); 
	//version which trains the model on the whole matrix & does not generate any reports 
	template <typename T, typename L, typename Dist, typename Mod, typename DP> 
	void pca_lmnn_smote_tomeks_undersample_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
			DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, double min_prop); 

	//Pipeline #4: (split) -> RFFS -> LMNN 
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, double ratio, double fs_thresh, 
			int k_lmnn, int d_lmnn, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, double fs_thresh, int k_lmnn, int d_lmnn); 	
	
	//Pipeline #5: split -> RFFS -> LMNN -> smote -> tomeks   
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_lmnn_smote_tomeks_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, double ratio, double fs_thresh, 
			int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_lmnn_smote_tomeks_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, 
			double fs_thresh, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks); 
	
	//Pipeline #6: (split) -> RFFS -> undersample -> lmnn   
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_undersample_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, double ratio, double fs_thresh,  
			double min_prop, int k_lmnn, int d_lmnn, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename Dist, typename F> 
	void rffs_undersample_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, 
			double fs_thresh, double min_prop, int k_lmnn, int d_lmnn); 

	//Pipeline #7: (split) -> smote -> RFFS 	
	template <typename T, typename L, typename Mod, typename F, typename Dist> 
	void smote_rffs_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, const std::string& path, double ratio, int k_smote, int n_smote, 
			double fs_thresh, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename F, typename Dist> 
	void smote_rffs_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, int k_smote, int n_smote, double fs_thresh); 
	
	//Pipeline #8: (split) -> smote -> lmnn -> pca
	template <typename T, typename L, typename Mod, typename Dist, typename DP>
	void smote_lmnn_pca_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t,const std::string& path, double ratio, int k_smote, int n_smote, 
			int k_lmnn, int d_lmnn, DP dp, double ret_var, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename Dist, typename DP>
	void smote_lmnn_pca_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t,int k_smote, int n_smote, int k_lmnn, int d_lmnn, DP dp, double ret_var); 
	
	//Pipeline #9 (split) -> tomeks -> pca -> lmnn
	template <typename T, typename L, typename Mod, typename Dist, typename DP> 
	void tomeks_pca_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, double ratio, int k_tomeks, 
			DP dp, double ret_var, int k_lmnn, int d_lmnn, int k = 3, double beta = 1.0); 
	template <typename T, typename L, typename Mod, typename Dist, typename DP> 
	void tomeks_pca_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, int k_tomeks, DP dp, double ret_var, int k_lmnn, int d_lmnn); 
	

	//KNN Classifier class which is templated on a voting policy class and a distance metric 
	template <typename V, typename D> 
	class KNNC{
		public:
			//Default constructor
			KNNC() = default; 
			//constructor
			KNNC(int k);
			//Training function (n is the number of classes)
			void Train(arma::mat& data, arma::Row<size_t>& labels, int n); 
			//Classify (Note: the mlpack .Classify member functions take data in by const reference) 
			void Classify(const arma::mat& data, arma::Row<size_t>& labels);
			//Classify Overload (probability of class j for data point i is accessed from p_vec[j, i]
			void Classify(const arma::mat& data, arma::Row<size_t>& labels, arma::mat& p_vec);
		private:
			//Distance metric
			D d_ = D(); 
			//voting policy is the default constructor for the voting policy class T
			V vote_policy_ = V();
			//number of neighbors is defaulted to 4
			int k_ = 4;
			//number of classes (default to zero and set to n when .Train is called)
			int n_ = 0; 
			//Nearest Neighbor Search object 
			mlpack::NeighborSearch<mlpack::NearestNeighborSort, D> nns_;
			//true class labels 
			arma::Row<size_t> labels_; 
	};
	//Majority voting policy class
	class MajorityVote{
		public:
			//use the compiler generated default constructor
			MajorityVote() = default;
			//n is the number of classes, p_vec is the class probabilities vector
			size_t vote(arma::Row<size_t>& nbs_labs, arma::mat& dists, int n); 
			size_t vote(arma::Row<size_t>& nbs_labs, arma::mat& dists, arma::vec& p_vec, int n); 
	}; 
	class InverseDistanceVote{
		public:
			//use the compiler generated default constructor
			InverseDistanceVote() = default;
			size_t vote(arma::Row<size_t>& nbs_labs, arma::vec& dists, int n); 
			size_t vote(arma::Row<size_t>& nbs_labs, arma::vec& dists, arma::vec& p_vec, int n); 
	}; 
}

//definitions of templated functions
template <typename L> 
L model::minority_class(const arma::Row<L>& labels){
	std::map<L, int> mp;
	for(auto i : labels){
		mp[i]++; 
	}
	return std::min_element(mp.begin(), mp.end(), [](const auto& p1, const auto& p2){return p1.second < p2.second;})->first; 
}
template <typename T, typename L> 
void model::split(const arma::Mat<T>& m, const arma::Row<L>& l, arma::Mat<T>& m1, arma::Row<L>& l1, arma::Mat<T>& m2, arma::Row<L>& l2, double ratio){
	//we dont shuffle data so that future data is not used before it occurs
	mlpack::data::Split(m, l, m1, m2, l1, l2, ratio, false);
}
template <typename T, typename L, typename It> 
void model::split(arma::Mat<T>& m, arma::Row<L>& l, arma::Mat<T>& m1, arma::Row<L>& l1, arma::Mat<T>& m2, arma::Row<L>& l2, It first, It& split_it, double ratio){
	mlpack::data::Split(m, l, m1, m2, l1, l2, ratio, false); 	
	split_it = std::next(first, m1.n_cols - 1); 	
}
template <typename T, typename L> 
void model::k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, int k){
	int n = m.n_cols / k;
	for(int i = 0; i < k - 1; i++){
		arma::uvec cols = arma::linspace<arma::uvec>(i*n, ((i + 1)*n) - 1, n);  
		m_splits.push_back(m.cols(cols));
		l_splits.push_back(l.cols(cols)); 
	}
	//last split gets the rest of the columns
	arma::uvec cols = arma::linspace<arma::uvec>((k - 1)*n, m.n_cols - 1, m.n_cols - ((k - 1)*n));  
	m_splits.push_back(m.cols(cols));
	l_splits.push_back(l.cols(cols)); 
}
template <typename T, typename L, typename It> 
void model::k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, It first, std::vector<It>& split_its, int k){
	int n = m.n_cols / k;
	for(int i = 0; i < k - 1; i++){
		arma::uvec cols = arma::linspace<arma::uvec>(i*n, ((i + 1)*n) - 1, n);  
		m_splits.push_back(m.cols(cols));
		l_splits.push_back(l.cols(cols));
		split_its.push_back(std::next(first, i*n)); 
	}
	//last split gets the rest of the columns
	arma::uvec cols = arma::linspace<arma::uvec>((k - 1)*n, m.n_cols - 1, m.n_cols - ((k - 1)*n));  
	m_splits.push_back(m.cols(cols));
	l_splits.push_back(l.cols(cols)); 
	split_its.push_back(std::next(first, (k - 1)*n)); 
}
template <typename T, typename L> 
void model::stratified_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_splits, std::vector<arma::Row<L>>& l_splits, int k){
	arma::Mat<T> m_temp = m;
	arma::Row<L> l_temp = l;
	arma::Mat<T> m_temp2 = m_temp;
	arma::Row<L> l_temp2 = l_temp;
	for(int i = 0; i < k - 1; i++){
		arma::Mat<T> m_split; 	
		arma::Row<L> l_split;
		mlpack::data::StratifiedSplit(m_temp2, l_temp2, m_temp, m_split, l_temp, l_split, 1.0 / (k - i), false);
		arma::Mat<T> m_temp2 = m_temp;
		arma::Row<L> l_temp2 = l_temp;
		m_splits.push_back(m_split);
		l_splits.push_back(l_split); 
	}
	m_splits.push_back(m_temp);
	l_splits.push_back(l_temp); 
}
template <typename T, typename L, typename It> 
void model::split_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_train_splits, std::vector<arma::Mat<T>>& m_test_splits, 
		std::vector<arma::Row<L>>& l_train_splits, std::vector<arma::Row<L>>& l_test_splits, int k, double ratio){
	std::vector<arma::Mat<T>> k_m_splits; 	
	std::vector<arma::Row<L>> k_l_splits;
	model::k_split(m, l, k_m_splits, k_l_splits, k);
	for(int i = 0; i < k; i++){
		arma::Mat<T> m_train;
		arma::Mat<T> m_test;
		arma::Row<L> l_train;
		arma::Row<L> l_test;
		mlpack::data::StratifiedSplit(k_m_splits[i], k_l_splits[i], m_train, m_test, l_train, l_test, ratio, false);
		m_train_splits.push_back(m_train);
		m_test_splits.push_back(m_test);
		l_train_splits.push_back(l_train);
		l_test_splits.push_back(l_test);
	}
}
template <typename T, typename L, typename It> 
void model::split_k_split(const arma::Mat<T>& m, const arma::Row<L>& l, std::vector<arma::Mat<T>>& m_train_splits, std::vector<arma::Mat<T>>& m_test_splits, 
		std::vector<arma::Row<L>>& l_train_splits, std::vector<arma::Row<L>>& l_test_splits, It first, std::vector<It>& split_its, int k, double ratio){
	//call k_split 
	std::vector<arma::Mat<T>> k_m_splits; 	
	std::vector<arma::Row<L>> k_l_splits;
	model::k_split(m, l, k_m_splits, k_l_splits, first, split_its, k);
	for(int i = 0; i < k; i++){
		arma::Mat<T> m_train;
		arma::Mat<T> m_test;
		arma::Row<L> l_train;
		arma::Row<L> l_test;
		mlpack::data::StratifiedSplit(k_m_splits[i], k_l_splits[i], m_train, m_test, l_train, l_test, ratio, false);
		m_train_splits.push_back(m_train);
		m_test_splits.push_back(m_test);
		l_train_splits.push_back(l_train);
		l_test_splits.push_back(l_test);
	}
}
template <typename T, typename L, typename F> 
void model::undersample(arma::Mat<T>& data, arma::Row<L>& labels, F min_prop){
	//checking that the min_prop passed in is greater than the min prop of the labels to start 
	auto ci = model::binary_class_info(labels); 
	if(std::get<3>(ci) >= min_prop){
		std::cout << "undersample: The starting min_prop of the labels is larger than the target min_prop." << std::endl; 
	}else{
		int n = labels.n_cols; 	
		L min_class = model::minority_class(labels);
		//get the indices for which the labels are not the minority_class 
		arma::uvec maj_class_idx = arma::find(labels != min_class);
		//get the number of majority class instance to remove
		int n_min = n - maj_class_idx.size();
		int n_maj_remove = maj_class_idx.size() - ((n_min*(1 - min_prop)) / min_prop);
		//sample the majority class instances
		arma::uvec mc_idx_sample(n_maj_remove);
		std::unordered_set<int> us; 
		int i = 0; 
		while(i < n_maj_remove){
			int ri = arma::randi<arma::uvec>(1, arma::distr_param(0, maj_class_idx.size() - 1)).at(0);
			//generate random integers until we find one which hasn't been generated yet
			while(us.find(ri) != us.end()){
				ri = arma::randi<arma::uvec>(1, arma::distr_param(0, maj_class_idx.size() - 1)).at(0);
			}
			us.insert(ri); 
			mc_idx_sample.at(i) = maj_class_idx.at(ri);
			i++; 
		}
		//shed n_maj_remove majority class instances 
		data.shed_cols(mc_idx_sample);
		labels.shed_cols(mc_idx_sample); 
	}
}
//oversampling algorithm 
template <typename T, typename L, typename F> 
void model::oversample(arma::Mat<T>& data, arma::Row<L>& labels, F min_prop){
	auto ci = model::binary_class_info(labels); 
	if(std::get<3>(ci) >= min_prop){
		std::cout << "oversample: The starting min_prop of the labels is larger than the target min_prop." << std::endl; 
	}else{
		int n = labels.n_cols; 
		L min_class = model::minority_class(labels);
		//get the indices of the minority class 
		arma::uvec min_class_idx = arma::find(labels == min_class); 	
		//find the number of min_class instances to add
		F maj_prop = 1 - min_prop; 
		int n_maj = n - min_class_idx.size();  
		int n_min_add = ((n_maj * min_prop) - (min_class_idx.size()* maj_prop)) / maj_prop;
		std::cout << "n_min_add: " << n_min_add << std::endl; 
		arma::uvec mc_idx_sample = arma::randi<arma::uvec>(n_min_add, arma::distr_param(0, min_class_idx.size() - 1));
		std::cout << "mc_idx_sample.n_rows" << mc_idx_sample.n_rows << std::endl; 
		//add the minority_class instances to data & labels
		data.resize(data.n_rows, data.n_cols + n_min_add);
		labels.resize(1, labels.n_cols + n_min_add); 
		for(int i = 0; i < mc_idx_sample.n_rows; i++){
			data.col(n + i) = data.col(min_class_idx.at(mc_idx_sample.at(i)));  
			//data.insert_cols(data.n_cols, data.col(min_class_idx.at(mc_idx_sample.at(i))));
			//labels.insert_cols(labels.n_cols, labels.col(min_class_idx.at(0)));
			labels.col(n + i) = labels.col(min_class_idx.at(0)); 
		}
	}
}
template <typename L>
std::tuple<int, int, int, double> model::binary_class_info(const arma::Row<L>& labels){
	L min_c = model::minority_class(labels); 	
	arma::uvec u = arma::find(labels == min_c); 
	int n_min = u.n_rows;  
	int n = labels.n_cols;  
	int n_maj = n - n_min; 
	double min_prop = n_min / double(n);
	return std::tuple<int, int, int, double>{n, n_min, n_maj, min_prop}; 
}

template <typename T, typename L, typename D>
mlpack::MahalanobisDistance<> model::lmnn_wrap(const arma::Mat<T>& data, const arma::Row<L>& labels, arma::Mat<T>& t, int k, int n, D){
	if(n > data.n_rows){
		throw std::invalid_argument("lmnn_wrap: The number of dimensions to keep, n, must be such that n <= data.n_rows"); 	
	}else{
		//size the transformation matrix appropriately
		t.ones(n, data.n_cols); 
	}
	mlpack::LMNN<D> lmnn(k, 1, 25);
	//learn the linear transformation 
	lmnn.LearnDistance(data, labels, t);
	//create a Mahalanobis Distance metric from t
	mlpack::MahalanobisDistance<> d{t.t() * t}; 	
	return d; 
}

//Returns a string to use in a latex file which contains average distance between points (of same & opposite class) before transformation using D, 
//distance between points using md, distance D points of the transformed dataset
template <typename T, typename L, typename D> 
std::string model::lmnn_report(const arma::Mat<T>& data, const arma::Row<L>& labels, const arma::Mat<T>& t, D d, mlpack::MahalanobisDistance<> md){
	std::vector<float> dists(6, 0.0);
	size_t lab0 = labels.at(0);
	//apply the transformation matrix to data
	arma::mat data_trans = t * data;
	//number of points with different class label from lab0
	int z = 0; 
	for(int i = 0; i < data.n_cols; i++){
		//distance under D before LMNN
		double d1 = d.Evaluate(data.col(0), data.col(i));
		//distance under Mahalanobis Metric
		double d2 = md.Evaluate(data.col(0), data.col(i));
		//distance under D after LMNN
		double d3 = d.Evaluate(data_trans.col(0), data_trans.col(i));
		if(labels.at(i) == lab0){
			//if col i is the same class as lab0
			dists[0] += d1; 
			dists[1] += d2; 
			dists[2] += d3; 
		}else{
			//if col i is not the same class as lab0
			dists[3] += d1; 
			dists[4] += d2; 
			dists[5] += d3;
			z++; 
		}
	}
	for(int i = 0; i < 3; i++){
		dists[i] /= (data.n_cols - z);
		dists[i + 3] /= z; 
	}
	std::cout << "Average distance d(data[:, 0], data[:, i]) where data[:, 0] and data[:, i] are of the same class: " << dists[0] << std::endl; 
	std::cout << "Average distance md(data[:, 0], data[:, i]) where data[:, 0] and data[:, i] are of the same class: " << dists[1] << std::endl; 
	std::cout << "Average distance d(data_trans[:, 0], data_trans[:, i]) where data_trans[:, 0] and data_trans[:, i] are of the same class: " << 
		dists[2] << std::endl; 
	std::cout << "Average distance d(data[:, 0], data[:, i]) where data[:, 0] and data[:, i] are of different classes: " << dists[3] << std::endl; 
	std::cout << "Average distance md(data[:, 0], data[:, i]) where data[:, 0] and data[:, i] are of the different classes: " << dists[4] << std::endl; 
	std::cout << "Average distance d(data_trans[:, 0], data_trans[:, i]) where data_trans[:, 0] and data_trans[:, i] are of different classes: " << 
		dists[5] << std::endl; 
	
	std::cout << "Ratio of other class differences to same class differences." << std::endl; 
	std::cout << "Before LMNN: " << dists[3] / dists[0] << std::endl; 
	std::cout << "After LMNN: " << dists[5] / dists[2] << std::endl; 
	
	std::string latex = "\\sect{Large Margin Nearest Neighbor} \n";
	latex += "Let $M$ be a matrix in $\\mathbb{R}^{n, m}$, let $l \\hspace{1mm}:\\hspace{1mm} \\mathbb{R}^n \\longrightarrow \\{0, 1\\}$ be a function which maps columns of $M$ to their associated labels, $T$ be the linear transformation obtained from LMNN, $d$ a distance metric and $d'$ be the Mahalanobis distance metric obtained from $T$."; 
	
	//generate the latex for the tables
	auto headers = std::make_tuple("", "$l(M_{:, i}) = l(M_{:, 0})$", "$l(M_{:, i}) \\neq l(M_{:, 0})$"); 	
	auto h_fcn = [](const std::string& s){
		return s; 
	};
	auto header_fcns = std::make_tuple(h_fcn, h_fcn, h_fcn);
	std::vector<std::tuple<std::string, double, double>> rows{std::make_tuple("mean($d(M_{:, 0}, M_{:, i})$)", dists[0], dists[3]), 
															  std::make_tuple("mean($md(M_{:, 0}, M_{:, i})$)", dists[1], dists[4]),
															  std::make_tuple("mean($d(T(M_{:, 0}), T(M_{:, i}))$)", dists[2], dists[5])};
	auto r_fcn = [](const double& d){
		return utility::float_cell(d, 4); 
	};
	auto row_fcns = std::make_tuple(h_fcn, r_fcn, r_fcn);
	latex += utility::table_latex(headers, header_fcns, rows, row_fcns, "center", "|| c | c | c ||"); 	
	auto headers2 = std::make_tuple("", "Ratio of Average of Same Class Distances to Different Class Distances");
	auto h_fcn2 = [](const std::string& s){
		return "\\textbf{" + s + "}"; 	
	};
	auto header_fcns2 = std::make_tuple(h_fcn2, h_fcn2); 
	std::vector<std::tuple<std::string, double>> rows2{std::make_tuple("Before LMNN", dists[3] / dists[0]), 
													   std::make_tuple("After LMNN", dists[5] / dists[2])};
	auto row_fcns2 = std::make_tuple(h_fcn2, r_fcn); 
	latex += utility::table_latex(headers2, header_fcns2, rows2, row_fcns2, "center", "|| c | c ||"); 	
	return latex; 
}

//Note: t is the transformation matrix not the transformed matrix
// t_data is the transformed data
// Note: data needs to be center before projecting a vector onto the principal components with t
template <typename T, typename DP> 
void model::pca_wrap(const arma::Mat<T>& data, arma::Mat<T>& t, DP dp, double& arv, double rv){
	mlpack::PCA<DP> pca(false); 
	arv = pca.Apply(data, t, rv); 
	arma::mat centered_data = data;
	centered_data.each_col() -= arma::mean(data, 1);
	arma::mat score; 
	arma::vec latent;
	//t is the transformation matrix
	arma::uvec rows_to_keep = arma::linspace<arma::uvec>(0, t.n_rows - 1, t.n_rows);
	arma::princomp(t, score, latent, centered_data.t());
	t = t.t(); 
	t = t.rows(rows_to_keep); 
}
template <typename T> 
std::string model::pca_report(const arma::Mat<T>& m, const arma::Mat<T>& t, double arv){
	int ndimr = m.n_rows - t.n_rows; 
	std::cout << "PCA retained " + std::format("{:.2f}", arv) + " of the variance." << std::endl;
	std::cout << "PCA reduced m by " + std::format("{:d}", ndimr) + " dimensions." << std::endl;
	std::string latex = "\\sect{Principal Component Analysis} \n"; 
	latex += "\\textbf{Dimensionality Reduction}: PCA reduced the original dataset by $" + std::format("{:d}", ndimr) + "$ dimensions. \n";
	latex += "\\textbf{Retained Variance}: PCA retained $" + std::format("{:.2f}", arv) + "$ of the variance in the original dataset. \n";
	return latex; 
}
template <typename T, typename DP> 
void model::scree(const arma::Mat<T>& data, arma::Mat<T>& t, DP dp, bool scale){
	mlpack::PCA<DP> pca(scale); 
	arma::vec eigval; 
	pca.Apply(data, t, eigval);
	for(int i = 0; i < eigval.n_rows; i++){
		std::cout << "Component " << i + 1 << " Eigenvalue: " << eigval.at(i) << std::endl; 
	}
}
template <typename T, typename L, typename D>
void model::tomeks_alg(arma::Mat<T>& m, arma::Row<L>& labels, int k, D& d){
	if(k <= 0){
		throw std::invalid_argument("tomeks_alg: Enter a valid k such that 0 < k."); 
	}
	if(m.n_cols != labels.n_cols){
		throw std::invalid_argument("tomeks_alg: m and labels must have the same number of columns."); 
	}
	//get the minority class 
	L mc = model::minority_class(labels); 
	//get the indices of the minority class instances (i.e. those with class labels of 1) 
	arma::uvec min_idx = arma::find(labels == mc); 
	mlpack::NeighborSearch<mlpack::NearestNeighborSort, D, arma::Mat<T>> nn(m);
	//unordered_set to keep track of the majority instances deleted from tomek links (using unordered_set for efficient find() calls)
	std::unordered_set<int> us; 

	//iterate through minority class instances
	for(auto i : min_idx){
		//number of neighbors which have not been previously deleted
		int num_nd = 0; 
		int num_d = 0;
		arma::Mat<L> nbs;
		arma::Mat<T> dists;
		nn.Search(m.col(i), k, nbs, dists);
		//when the neighborhood has k neighbors which havent been deleted yet. Check their classes and delete them if need be
		for(auto j : nbs){
			if((labels.at(j) != mc) && (us.find(j) == us.end())){
				us.insert(j);
			}
		}
	}
	//store the indices of majority class instances in tomek links to be deleted in an arma::uvec
	arma::uvec shed_idx(us.size());
	int l = 0; 
	for(auto i : us){
		shed_idx.at(l) = i; 
		l++; 
	}
	//shed the points which are majority instances of Tomek Links
	m.shed_cols(shed_idx);
	labels.shed_cols(shed_idx);
}

template <typename T, typename L, typename D> 
void model::smote(arma::Mat<T>& m, arma::Row<L>& labels, int k, int n, D d){
	if(k <= 0 || n <= 0){
		throw std::invalid_argument("smote: Enter a valid integers n and k such that n, k <= 0"); 
	}
	//get the minority class 
	L mc = model::minority_class(labels); 
	//get the indices of the minority class instances (i.e. those with class labels of 1) 
	arma::uvec min_idx = arma::find(labels == mc); 
	
	mlpack::NeighborSearch<mlpack::NearestNeighborSort, D, arma::Mat<T>> nn(m); 
	int n_cols_orig = m.n_cols; 	
	int n_synth = n * min_idx.n_elem; 
	m.resize(m.n_rows, m.n_cols + n_synth);
	labels.resize(1, labels.n_elem + n_synth); 
	int c = 0; 
	//iterate through minority class instances 
	for(auto i : min_idx){
		arma::Mat<size_t> nbs; 
		//(note: dists is a kx1 matrix) 
		arma::mat dists;
		nn.Search(m.col(i), k, nbs, dists);
		//randomly sample the columns in nbs n times 
		arma::uvec idx = arma::randi<arma::uvec>(n, arma::distr_param(1, k - 1)); //we only want to sample the neighbors ==> [1, k-1]
		
		arma::Row<L> new_lab{mc};
		for(auto j : idx){
			double alpha = arma::randu(arma::distr_param(0.00000001, 0.99999999));  //distr_param includes bounds ==> cant use [0, 1]
			//generate the synthetic minority class instance 
			arma::vec synth = m.col(i) + alpha*(m.col(i) - m.col(nbs.at(j))); // alpha * (difference vector)
			////insert the synthetic minority class instance
			//m.insert_cols(m.n_cols, synth);
			//labels.insert_cols(labels.n_cols, new_lab);  
			m.col(n_cols_orig + c) = (m.col(i) + alpha*(m.col(i) - m.col(nbs.at(j)))); 
			labels.at(n_cols_orig + c) = 1;
			c++; 
		}
	}
}
template <typename D> 
void model::smote_v2(arma::mat& m, int k, int n, D){
	if(k <= 0 || n <= 0){
		throw std::invalid_argument("smote_v2: Enter a valid integers n and k such that n, k <= 0"); 
	}
	//get the indices of the minority class instances (i.e. those with class labels of 1) 
	arma::uvec min_idx = arma::find(m.row(m.n_rows - 1) == 1.0); 
	//store the class labels in a vector and temporarily shed them from m 
	arma::rowvec labels = m.row(m.n_rows - 1); 
	m.shed_row(m.n_rows - 1); 	
	
	mlpack::NeighborSearch<mlpack::NearestNeighborSort, D> nn(m); 
	
	//iterate through minority instances
	for(int i : min_idx){
		arma::Mat<size_t> nbs; 
		//(note: dists is a kx1 matrix) 
		arma::mat dists;
		nn.Search(m.col(i), k, nbs, dists);
		//randomly sample the columns in nbs n times 
		arma::uvec idx = arma::randi<arma::uvec>(n, arma::distr_param(1, k - 1)); //we only want to sample the neighbors ==> [1, k-1]
		//number of majority instance neighbors
		int n_maj_nbs = 0; 
		for(int j : nbs){
			if(labels.at(j)){
				n_maj_nbs += 1; 
			}
		}
		//compute the proportion of majority instance neighbors 
		double prop = n_maj_nbs / double(k);
		for(auto j : idx){
			double alpha = arma::randu(arma::distr_param(0.0, prop)); //prop is the upperbound for alpha
			//generate the synthetic minority class instance 
			arma::vec synth = m.col(i) + alpha*(m.col(i) - m.col(nbs.at(j))); // alpha * (difference vector)
			arma::vec new_lab = {1.0};
			//insert the synthetic minority class instance
			m.insert_cols(m.n_cols,synth);
			labels.insert_cols(labels.n_cols, new_lab);  
		}
	}
	m = arma::join_cols(m, labels); 
}
template <typename D> 
void model::smote_v3(arma::mat& m, int k, D, double alpha){
	if(k <= 0){
		throw std::invalid_argument("smote_v3: Enter a valid integer k such that k <= 0"); 
	}
	//get the indices of the minority class instances (i.e. those with class labels of 1) 
	arma::uvec min_idx = arma::find(m.row(m.n_rows - 1) == 1.0); 
	//store the class labels in a vector and temporarily shed them from m 
	arma::rowvec labels = m.row(m.n_rows - 1); 
	m.shed_row(m.n_rows - 1); 	
	
	mlpack::NeighborSearch<mlpack::NearestNeighborSort, D> nn(m); 

	//iterate through minority instances
	for(int i : min_idx){
		arma::Mat<size_t> nbs; 
		//(note: dists is a kx1 matrix) 
		arma::mat dists;
		nn.Search(m.col(i), k, nbs, dists);
		//iterate through the neighbors
		for(auto j : nbs){
			//check if the neighbor is a majority class instance
			if(labels.at(j) == 0.0){
				//generate the synthetic minority class instance 
				arma::vec synth = m.col(i) + alpha*(m.col(i) - m.col(j)); // alpha * (difference vector)
				arma::vec new_lab = {1.0};
				//insert the synthetic minority class instance
				m.insert_cols(m.n_cols,synth);
				labels.insert_cols(labels.n_cols, new_lab);  
			}
		}
	}
	m = arma::join_cols(m, labels); 
}

template <typename T, typename L, typename M> 
std::pair<double, double> model::confusion_matrix(M& model, const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab, double thresh){
	arma::Row<L> preds;
	arma::Mat<T> probs;
	L mc = model::minority_class(true_lab);
	model.Classify(test_mat, preds, probs);
	arma::uvec idx = arma::find(true_lab == mc); 
	int n_pos = idx.n_elem;
	int n_neg = true_lab.n_elem - n_pos;
	int t_pos = 0;
	int t_neg = 0;
	for(int i = 0; i < true_lab.n_elem; i++){
		if(probs.at(mc, i) >= thresh && true_lab.at(i) == mc){
			t_pos++; 
		}
		if(probs.at(mc, i) < thresh && true_lab.at(i) != mc){
			t_neg++; 	
		}
	}
	int f_pos = n_neg - t_neg;
	int f_neg = n_pos - t_pos;
	auto headers = std::make_tuple("Total Population", "Predicted Positive", "Predicted Negative");
	auto rows = std::vector<std::tuple<std::string, double, double>>{std::make_tuple("Actual Positive", t_pos, f_neg), std::make_tuple("Actual Negative", f_pos, t_neg)};  
	auto header_fcn_console = [](const std::string& s){return s;};
	auto r_fcn_console = [](const double& d){return d;}; 
	auto row_fcns_console = std::make_tuple(header_fcn_console, r_fcn_console, r_fcn_console);
	auto h_fcns_console = std::make_tuple(header_fcn_console, header_fcn_console, header_fcn_console); 
	//print the confusion matrix to the console
	std::vector<int> col_widths{25, 25, 25}; 
	utility::table_print(headers, h_fcns_console, rows, row_fcns_console, col_widths); 
	//default precision to -1
	double precision = -1; 
	if(t_pos + f_pos != 0){
		precision = (double)t_pos / (t_pos + f_pos);
	}
	double recall = (double)t_pos / n_pos;
	return std::make_pair(precision, recall); 
}
template <typename T, typename L, typename M> 
void model::confusion_matrices(M& model, const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab){
	double thresh = 0.0; 
	for(int i = 0; i < 21; i++){
		std::cout << "Threshold: " << thresh << std::endl; 
		auto p = model::confusion_matrix(model, test_mat, true_lab, thresh); 
		std::cout << "Precision: " << p.first << std::endl; 
		std::cout << "Recall: " << p.second << std::endl; 
		thresh += .05; 
		std::cout << std::endl; 
	}
}

template <typename T, typename L, typename M> 
std::string model::performance_report(const arma::Mat<T>& test_mat, const arma::Row<L>& true_lab, M& mod, int k, double beta){
	L mc = model::minority_class(true_lab); 
	//perform k_split on test_mat and true_lab 
	std::vector<arma::Mat<T>> test_mat_splits; 
	std::vector<arma::Row<L>> true_lab_splits;
	model::k_split(test_mat, true_lab, test_mat_splits, true_lab_splits, k);
	std::vector<double> precisions(k);
	std::vector<double> recalls(k);
	std::vector<double> accuracies(k);
	std::vector<double> aurocs(k);
	//true positives are when we predict the minority class and the true label is the minority class
	std::vector<double> true_pos(k);
	std::vector<double> true_negs(k);
	std::vector<double> false_pos(k);
	std::vector<double> false_negs(k);
	auto split_perf_eval = [&](int i){
		arma::Row<L> preds; 
		arma::mat probs; 
		mod.Classify(test_mat_splits[i], preds, probs);
		//scores are the probabilities for the positive class (==> minority class) (mlpack requires labels to be [0, n] ==> mc is the row index) 
		aurocs[i] = mlpack::ROCAUCScore<1>::Evaluate(true_lab_splits[i], probs.row(mc)); 
		accuracies[i] = mlpack::Accuracy::Evaluate(mod, test_mat_splits[i], true_lab_splits[i]);
		//use binary classification strategy since we are doing binary classification
		recalls[i] = mlpack::Recall<mlpack::AverageStrategy::Binary, 1>::Evaluate(mod, test_mat_splits[i], true_lab_splits[i]);
		precisions[i] = mlpack::Precision<mlpack::AverageStrategy::Binary, 1>::Evaluate(mod, test_mat_splits[i], true_lab_splits[i]);
		int n_pop = test_mat_splits[i].n_cols;
		arma::uvec mc_idx = arma::find(true_lab_splits[i] == mc);  
		int n_positive = mc_idx.n_rows;
		int n_negative = n_pop - n_positive;
		true_pos[i] = recalls[i] * n_positive; 
		true_negs[i] = (accuracies[i]*n_pop) - (recalls[i]*n_positive);	
		false_pos[i] = n_negative - true_negs[i];
		false_negs[i] = n_positive - true_pos[i];
	};
	arma::uvec indices = arma::linspace<arma::uvec>(0, k - 1, k); 
	std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), split_perf_eval);
	//average the metrics for the k-splits
	double auroc = utility::mean(aurocs.cbegin(), aurocs.cend(), 0.0); 
	double acc = utility::mean(accuracies.cbegin(), accuracies.cend(), 0.0); 
	double prec = utility::mean(precisions.cbegin(), precisions.cend(), 0.0); 
	double rec = utility::mean(recalls.cbegin(), recalls.cend(), 0.0);
	double tp = utility::mean(true_pos.cbegin(), true_pos.cend(), 0.0);
	double tn = utility::mean(true_negs.cbegin(), true_negs.cend(), 0.0);
	double fp = utility::mean(false_pos.cbegin(),  false_pos.cend(), 0.0);
	double fn = utility::mean(false_negs.cbegin(), false_negs.cend(), 0.0);
	//compute the rest of the metrics
	double beta2 = std::pow(beta, 2);
	double f_beta = ((1 + beta2)*prec*rec) / ((beta2 * prec) + rec);
	//n negative = n false positive + n true negative
	double bal_acc = (rec + (tn / double(fp + tn))) / 2; // TPR + TNR / 2
	//false omission rate (negative value doesn't make sense ==> if for is undefined it will be -1.0 in the tables) 
	double fo_rate = -1.0;
	if(tn + fn != 0){
		fo_rate = 1 - double(tn) / (tn + fn); 
	}
	//Print the tables to the console & generate the latex for the table
	//confusion matrix table 
	auto headers = std::make_tuple("Total Population", "Predicted Positive", "Predicted Negative");
	auto rows = std::vector<std::tuple<std::string, double, double>>{std::make_tuple("Actual Positive", tp, fn), std::make_tuple("Actual Negative", fp, tn)};  
	auto header_fcn_console = [](const std::string& s){return s;};
	auto header_fcn = [](const std::string& s){return "\\textbf{" + s + "}";}; 
	auto r_fcn = [](const double& d){return utility::float_cell(d, 4);};
	auto r_fcn_console = [](const double& d){return d;}; 
	auto row_fcns_console = std::make_tuple(header_fcn_console, r_fcn_console, r_fcn_console);
	auto row_fcns = std::make_tuple(header_fcn, r_fcn, r_fcn);
	auto h_fcns = std::make_tuple(header_fcn, header_fcn, header_fcn);  
	auto h_fcns_console = std::make_tuple(header_fcn_console, header_fcn_console, header_fcn_console); 
	//print the confusion matrix to the console
	std::vector<int> col_widths{25, 25, 25}; 
	utility::table_print(headers, h_fcns_console, rows, row_fcns_console, col_widths); 
	//generate the latex for the table
	std::string latex = "\\sect{Confusion Matrix}\n";  
	latex += utility::table_latex(headers, h_fcns, rows, row_fcns, "center", "|| c | c | c ||"); 
	//performance metric table
	auto headers2 = std::make_tuple("Metric", "Value");
	std::vector<std::tuple<std::string, double>> rows2{std::make_tuple("Accuracy", acc), 
													  std::make_tuple("Balanced Accuracy", bal_acc),
													  std::make_tuple("False Discovery Rate", 1 - prec), 
													  std::make_tuple("False Omission Rate", fo_rate),
													  std::make_tuple("Precision", prec), 
													  std::make_tuple("Recall", rec),
													  std::make_tuple("$F_{" + std::format("{:.2f}", beta) + "}$", f_beta), 
													  std::make_tuple("AUROC", auroc)
													  };
	auto h_fcns_console2 = std::make_tuple(header_fcn_console, header_fcn_console);
	auto row_fcns_console2 = std::make_tuple(header_fcn_console, r_fcn_console);
	auto h_fcns2 = std::make_tuple(header_fcn, header_fcn); 
	auto row_fcns2 = std::make_tuple(header_fcn, r_fcn); 	
	//print the performance metric table to the console
	std::vector<int> col_widths2{25, 10}; 
	utility::table_print(headers2, h_fcns_console2, rows2, row_fcns_console2, col_widths2);
	//generate the latex for the performance metric table 
	latex += "\\sect{Performance Metrics} \n"; 
	latex += utility::table_latex(headers2, h_fcns2, rows2, row_fcns2, "center", "|| c | c ||");
	return latex; 
}

//Pipeline #1: 
template <typename T, typename L, typename Dist, typename Mod> 
void model::tomeks_undersample_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
		double ratio, int k_tomeks, double min_prop, int k_lmnn, int d_lmnn, int k, double beta){
	//split 
	arma::Mat<T> train_mat;
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//tomek's algorithm 
	auto cib = model::binary_class_info(train_lab);
	model::tomeks_alg(train_mat, train_lab, k_tomeks, d); 
	auto cia = model::binary_class_info(train_lab);
	//start accumulating the latex for the model report
	std::string latex = tomeks_report(cib, cia);
	//undersampling 
	cib = cia; 	
	model::undersample(train_mat, train_lab, min_prop);
	cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Undersampling", cib, cia);
	//LMNN 
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t, k_lmnn, d_lmnn, d);
	//LMNN report
	latex += model::lmnn_report(train_mat, train_lab, t, d, md);
	//Train the model on the transformed matrix
	train_mat = t * train_mat; 
	utility::fix_duplicates(train_mat, .001); 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = t * test_mat; 
	latex += model::performance_report(test_mat, test_lab, M, k, beta); 	
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Dist, typename Mod> 
void model::tomeks_undersample_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, int k_tomeks, double min_prop, int k_lmnn, int d_lmnn){
	//split
	arma::Mat<T> mat = matrix;
	arma::Row<L> lab = labels; 
	model::tomeks_alg(mat, lab, k_tomeks, d); 
	//undersampling 
	model::undersample(mat, lab, min_prop);
	//LMNN 
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t, k_lmnn, d_lmnn, d); ;
	//Train the model on the transformed matrix 
	mat = t * mat; 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}
//Pipeline #2 
template <typename T, typename L, typename Dist, typename Mod, typename DP> 
void model::pca_lmnn_smote_tomeks_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
		double ratio, DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, int k, double beta){
	//split 
	arma::Mat<T> train_mat;
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//PCA
	double arv; 
	model::pca_wrap(train_mat, t, dp, arv, ret_var); 
	std::string latex = model::pca_report(train_mat, t, arv);
	train_mat = (t * train_mat); 
	//LMNN
	arma::Mat<T> t2; 
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t2, k_lmnn, d_lmnn, d); 
	latex += model::lmnn_report(train_mat, train_lab, t2, d, md);
	t = (t2 * t);
	train_mat = (t2 * train_mat); 
	//SMOTE
	auto cib = model::binary_class_info(train_lab); 
	model::smote(train_mat, train_lab, k_smote, n_smote, d);
	auto cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("SMOTE", cib, cia);
	//TOMEKS
	cib = cia; 
	model::tomeks_alg(train_mat, train_lab, k_tomeks, d);
	cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Tomek's Algorithm", cib, cia);
	utility::fix_duplicates(train_mat, .001); 
	//Train the model on the transformed matrix 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = t * test_mat; 
	latex += model::performance_report(test_mat, test_lab, M, k, beta); 	
	//write the latex 
	utility::write_latex(path, latex); 

}
template <typename T, typename L, typename Dist, typename Mod, typename DP> 
void model::pca_lmnn_smote_tomeks_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
		DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks){
	arma::Mat<T> mat = matrix;
	arma::Mat<T> lab = labels; 
	//PCA
	double arv; 
	model::pca_wrap(mat, t, arv, ret_var); 
	mat = (t * mat);  
	//LMNN
	arma::Mat<T> t2; 
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t2, k_lmnn, d_lmnn, d); 
	t = (t2 * t);
	mat = (t2 * mat); 
	//SMOTE
	model::smote(mat, lab, k_smote, n_smote, d);
	//TOMEKS
	model::tomeks_alg(mat, lab, k_tomeks, d);
	//Train the model on the transformed matrix 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}
//Pipeline #3: (split) -> pca -> lmnn -> tomeks -> undersample  
template <typename T, typename L, typename Dist, typename Mod, typename DP> 
void model::pca_lmnn_smote_tomeks_undersample_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
		double ratio, DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, double min_prop, int k, double beta){
	//split 
	arma::Mat<T> train_mat;
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//PCA
	double arv; 
	model::pca_wrap(train_mat, t, dp, arv, ret_var); 
	std::string latex = model::pca_report(train_mat, t, arv);
	train_mat = (t * train_mat);  
	//LMNN
	arma::Mat<T> t2; 
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t2, k_lmnn, d_lmnn, d); 
	latex += model::lmnn_report(train_mat, train_lab, t2, d, md);
	t = (t2 * t);
	train_mat = (t2 * train_mat); 
	//SMOTE
	auto cib = model::binary_class_info(train_lab); 
	model::smote(train_mat, train_lab, k_smote, n_smote, d);
	auto cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("SMOTE", cib, cia);
	//TOMEKS
	cib = cia; 
	model::tomeks_alg(train_mat, train_lab, k_tomeks, d);
	cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Tomek's Algorithm", cib, cia);
	//Undersample 
	cib = cia; 
	model::undersample(train_mat, train_lab, min_prop); 
	cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Undersampling", cib, cia);
	utility::fix_duplicates(train_mat, .001); 
	//Train the model on the transformed matrix 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * test_mat); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta); 	
	//write the latex 
	utility::write_latex(path, latex); 
}
//version which trains the model on the whole matrix & does not generate any reports 
template <typename T, typename L, typename Dist, typename Mod, typename DP> 
void model::pca_lmnn_smote_tomeks_undersample_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, 
		DP dp, double ret_var, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, double min_prop){
	arma::Mat<T> mat = matrix;
	arma::Mat<T> lab = labels; 
	//PCA
	double arv; 
	model::pca_wrap(mat, t, arv, ret_var); 
	mat = (t * mat); 
	//LMNN
	arma::Mat<T> t2; 
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t2, k_lmnn, d_lmnn, d); 
	t = (t2 * t);
	mat = (t2 * mat); 
	//SMOTE
	model::smote(mat, lab, k_smote, n_smote, d);
	//TOMEKS
	model::tomeks_alg(mat, lab, k_tomeks, d);
	//Undersample 
	model::undersample(mat, lab, min_prop); 
	//Train the model on the transformed matrix 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}

//Pipeline #4: (split) RFFS -> LMNN 
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, double ratio, double fs_thresh, 
		int k_lmnn, int d_lmnn, int k, double beta){
	//Split
	arma::Mat<T> train_mat;
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(train_mat, train_lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	train_mat = train_mat.rows(idx);
	std::string latex = rffs.report(); 
	//LMNN
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t, k_lmnn, d_lmnn, d); 
	latex += model::lmnn_report(train_mat, train_lab, t, d, md);
	//Train the model on the preprocessed data 
	train_mat = (t * train_mat); 
	utility::fix_duplicates(train_mat, .001); 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * test_mat.rows(idx)); 
	latex += model::performance_report(test_mat,  test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, double fs_thresh, int k_lmnn, int d_lmnn){
	arma::Mat<T> mat = matrix;
	arma::Mat<T> lab = labels; 
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(mat, lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	mat = mat.rows(idx);
	//LMNN
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t, k_lmnn, d_lmnn, d); 
	//Train the model on the preprocessed data
	mat = (t * mat); 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}

//Pipeline #5: split -> RFFS -> LMNN -> smote -> tomeks   
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_lmnn_smote_tomeks_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, 
		double ratio, double fs_thresh, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks, int k, double beta){
	arma::Mat<T> train_mat;
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(train_mat, train_lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	train_mat = train_mat.rows(idx);
	std::string latex = rffs.report(); 
	//LMNN
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t, k_lmnn, d_lmnn, d); 
	latex += model::lmnn_report(train_mat, train_lab, t, d, md);
	train_mat = (t * train_mat); 
	//SMOTE
	auto cib = model::binary_class_info(train_lab); 
	model::smote(train_mat, train_lab, k_smote, n_smote, d);
	auto cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("SMOTE", cib, cia);
	//TOMEKS
	cib = cia; 
	model::tomeks_alg(train_mat, train_lab, k_tomeks, d);
	cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Tomek's Algorithm", cib, cia);
	utility::fix_duplicates(train_mat, .001); 
	//Train the model on the preprocessed data 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * (test_mat.rows(idx))); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_lmnn_smote_tomeks_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, 
		double fs_thresh, int k_lmnn, int d_lmnn, int k_smote, int n_smote, int k_tomeks){
	arma::Mat<T> mat = matrix;
	arma::Mat<T> lab = labels; 
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(mat, lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	mat = mat.rows(idx);
	//LMNN
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t, k_lmnn, d_lmnn, d); 
	mat = (t * mat); 	
	//SMOTE
	model::smote(mat, lab, k_smote, n_smote, d);
	//TOMEKS
	model::tomeks_alg(mat, lab, k_tomeks, d);
	//Train the model on the preprocessed data 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}

//Pipeline #6: (split) -> RFFS -> undersample -> lmnn   
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_undersample_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, const std::string& path, 
		double ratio, double fs_thresh, double min_prop, int k_lmnn, int d_lmnn, int k, double beta){
	arma::Mat<T> train_mat;
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(train_mat, train_lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	train_mat = train_mat.rows(idx);
	std::string latex = rffs.report(); 
	//Undersample 
	auto cib = model::binary_class_info(train_lab); 
	model::undersample(train_mat, train_lab, min_prop); 
	auto cia = model::binary_class_info(train_lab);
	latex += model::sampling_report("Undersampling", cib, cia); 
	//LMNN
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t, k_lmnn, d_lmnn, d); 
	latex += model::lmnn_report(train_mat, train_lab, t, d, md);
	//Train the model on the preprocessed data
	train_mat = (t * train_mat); 
	utility::fix_duplicates(train_mat, .001); 
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * test_mat.rows(idx)); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 

}
template <typename T, typename L, typename Mod, typename Dist, typename F> 
void model::rffs_undersample_lmnn_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, arma::Mat<T>& t, 
		double fs_thresh, double min_prop, int k_lmnn, int d_lmnn){
	arma::Mat<T> mat = matrix;
	arma::Mat<T> lab = labels; 
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(mat, lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	mat = mat.rows(idx);
	//undersample 
	model::undersample(mat, lab, min_prop); 
	//LMNN
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	utility::fix_duplicates(mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t, k_lmnn, d_lmnn, d); 
	//Train the model on the preprocessed data
	mat = (t * mat); 
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}

//Pipeline #7: (split) -> smote -> RFFS 	
template <typename T, typename L, typename Mod, typename F, typename Dist> 
void model::smote_rffs_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, const std::string& path, double ratio, int k_smote, int n_smote, 
		double fs_thresh, int k, double beta){
	arma::Mat<T> train_mat;
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//SMOTE
	auto cib = model::binary_class_info(train_lab); 
	model::smote(train_mat, train_lab, k_smote, n_smote, d);
	auto cia = model::binary_class_info(train_lab);
	std::string latex = model::sampling_report("SMOTE", cib, cia);
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(train_mat, train_lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	train_mat = train_mat.rows(idx);
	latex += rffs.report();
	utility::fix_duplicates(train_mat, .001); 
	//train the model on the preprocessed data & evaluate model performance
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = test_mat.rows(idx); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Mod, typename F, typename Dist> 
void model::smote_rffs_model(Mod& M, Dist& d, F& fit_fcn, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::uvec& idx, int k_smote, int n_smote, double fs_thresh){
	arma::Mat<T> mat = matrix;
	arma::Row<L> lab = labels; 
	arma::Mat<T> fs_train_mat;
	arma::Row<L> fs_train_lab; 
	arma::Mat<T> fs_test_mat;
	arma::Row<L> fs_test_lab; 
	//SMOTE
	model::smote(mat, lab, k_smote, n_smote, d);
	//split train_mat & train_lab so we don't assess feature importances based on the test data
	model::split(mat, lab, fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab, .35); 
	//RFFS 
	RFFS<F> rffs(fs_thresh);
	rffs.Select(fs_train_mat, fs_train_lab, fs_test_mat, fs_test_lab);
	//store the important features in train_mat
	idx = rffs.get_import(); 
	mat = mat.rows(idx);
	utility::fix_duplicates(mat, .001); 
	//train the model on the preprocessed data
	M.Train(mat, lab, 2);
}

//Pipeline #8: (split) -> smote -> lmnn -> pca
template <typename T, typename L, typename Mod, typename Dist, typename DP>
void model::smote_lmnn_pca_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, double ratio, int k_smote, int n_smote, 
		int k_lmnn, int d_lmnn, DP dp, double ret_var, int k, double beta){
	arma::Mat<T> train_mat;
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//smote algorithm 
	auto cib = model::binary_class_info(train_lab);
	model::smote(train_mat, train_lab, k_smote, n_smote, d); 
	auto cia = model::binary_class_info(train_lab);
	//start accumulating the latex for the model report
	std::string latex = sampling_report("SMOTE", cib, cia);
	//LMNN
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t, k_lmnn, d_lmnn, d);
	//LMNN report
	latex += model::lmnn_report(train_mat, train_lab, t, d, md);
	train_mat = (t * train_mat);
	arma::Mat<T> t2;
	//PCA 
	double arv; 
	model::pca_wrap(train_mat, t2, dp, arv, ret_var); 
	latex += model::pca_report(train_mat, t2, arv);
	train_mat = (t2 * train_mat); 
	t = (t2 * t); 
	utility::fix_duplicates(train_mat, .001); 
	//train the model on the preprocessed data & evaluate model performance
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * test_mat); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Mod, typename Dist, typename DP>
void model::smote_lmnn_pca_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, int k_smote, int n_smote, int k_lmnn, int d_lmnn, DP dp, double ret_var){
	arma::Mat<T> mat = matrix;
	arma::Row<L> lab = labels;
	//smote algorithm 
	model::smote(mat, lab, k_smote, n_smote, d); 
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	//LMNN 
	utility::fix_duplicates(mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t, k_lmnn, d_lmnn, d);
	//LMNN report
	mat = (t * mat);
	arma::Mat<T> t2;
	//PCA 
	double arv; 
	model::pca_wrap(mat, t2, dp, arv, ret_var); 
	t = (t2 * t);
	mat = (t2 * mat);
	utility::fix_duplicates(mat, .001); 
	//train the model on the preprocessed data
	M.Train(mat, lab, 2);
}

//Pipeline #9 (split) -> tomeks -> pca -> lmnn
template <typename T, typename L, typename Mod, typename Dist, typename DP> 
void model::tomeks_pca_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, const std::string& path, double ratio, int k_tomeks, 
		DP dp, double ret_var, int k_lmnn, int d_lmnn, int k, double beta){
	arma::Mat<T> train_mat;
	arma::Mat<T> test_mat;
	arma::Row<L> train_lab;
	arma::Row<L> test_lab;
	//Split
	model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, ratio);
	//Tomek's algorithm
	auto cib = model::binary_class_info(train_lab);
	model::tomeks_alg(train_mat, train_lab, k_tomeks, d); 
	auto cia = model::binary_class_info(train_lab);
	//start accumulating the latex for the model report
	std::string latex = tomeks_report(cib, cia);
	//PCA 
	double arv; 
	model::pca_wrap(train_mat, t, dp, arv, ret_var); 
	latex += model::pca_report(train_mat, t, arv);
	train_mat = (t * train_mat); 
	//LMNN 
	arma::Mat<T> t2;
	if(d_lmnn > train_mat.n_rows){
		d_lmnn = train_mat.n_rows; 
	}
	utility::fix_duplicates(train_mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(train_mat, train_lab, t2, k_lmnn, d_lmnn, d);
	//LMNN report
	latex += model::lmnn_report(train_mat, train_lab, t2, d, md);
	t = (t2 * t);
	train_mat = (t2 * train_mat); 
	utility::fix_duplicates(train_mat, .001); 
	//train the model on the preprocessed data & evaluate model performance
	M.Train(train_mat, train_lab, 2);
	//model reporting
	std::string modeltype = typeid(M).name();
	latex += "\\sect{$" + modeltype + "$} \n";
	test_mat = (t * test_mat); 
	latex += model::performance_report(test_mat, test_lab, M, k, beta);
	//write the latex 
	utility::write_latex(path, latex); 
}
template <typename T, typename L, typename Mod, typename Dist, typename DP> 
void model::tomeks_pca_lmnn_model(Mod& M, Dist& d, const arma::Mat<T>& matrix, const arma::Row<L>& labels, arma::Mat<T>& t, int k_tomeks, DP dp, double ret_var, int k_lmnn, int d_lmnn){
	arma::Mat<T> mat = matrix;
	arma::Row<L> lab = labels;
	//Tomeks algorithm 
	model::tomeks_alg(mat, lab, k_tomeks, d); 
	//PCA 
	double arv; 
	model::pca_wrap(mat, t, dp, arv, ret_var); 
	mat = (t * mat);
	//LMNN 
	if(d_lmnn > mat.n_rows){
		d_lmnn = mat.n_rows; 
	}
	arma::Mat<T> t2;
	utility::fix_duplicates(mat, .001); 	
	mlpack::MahalanobisDistance md = model::lmnn_wrap(mat, lab, t2, k_lmnn, d_lmnn, d);
	//LMNN report
	t = (t2 * t);
	mat = (t2 * mat); 
	//train the model on the preprocessed data
	utility::fix_duplicates(mat, .001); 
	M.Train(mat, lab, 2);
}

template <typename V, typename D> 
model::KNNC<V, D>::KNNC(int k) : k_{k} {};

//Note: We dont use the labels when building the reference tree for KNN but to match the behavior of other mlpack 
//classifiers we need to pass the labels (==> allows the same .Train() call to work for KNNC and mlpack classifiers)

template <typename V, typename D> 
void model::KNNC<V, D>::Train(arma::mat& data, arma::Row<size_t>& labels, int n){
	n_ = n; 
	//build the reference tree for the data
	nns_.Train(data);
	//store the true class labels 
	labels_ = labels; 
}

//store predicted labels in labels 
template<typename V, typename D>
void model::KNNC<V, D>::Classify(const arma::mat& data, arma::Row<size_t>& labels){
	//resize labels
	labels.resize(data.n_cols);
	//lambda to apply to each column in data
	auto vote = [&](const arma::vec& c){
		//get the column number of c
		arma::mat::const_iterator itb = data.begin();
		//note matrices are stored column by column ==> distance from first element of first col to first element of c / n_rows is the col number 
		int cn = std::distance(itb, c.begin_row(0)) / data.n_rows; 
		//neigbors and distances matrices
		arma::uvec nbs; 
		arma::vec dists;
		//search for the k_ nearest neighbors of c)
		nns_.Search(c, k_, nbs, dists);
		//store the true class labels of the neigbors in a arma::Row
		arma::Row<size_t> nbs_labs = labels_.cols(nbs);
		//vote for the class label of c (n_ is the number of classes)
		size_t lab = vote_policy_.vote(nbs_labs, dists, n_);
		//place the class label in labels
		labels.col(cn) = lab; 
	};
	data.each_col(vote); 
}

template<typename V, typename D>
void model::KNNC<V, D>::Classify(const arma::mat& data, arma::Row<size_t>& labels, arma::mat& p_vec){
	//resize labels and p_vec
	labels.resize(data.n_cols);
	//n_ classes 
	p_vec.resize(n_, data.n_cols);
	//lambda to apply to each column in data
	auto vote = [&](const arma::vec& c){
		//get the column number of c
		arma::mat::const_iterator itb = data.begin();
		//note matrices are stored column by column ==> distance from first element of first col to first element of c / n_rows is the col number 
		int cn = std::distance(itb, c.begin_row(0)) / data.n_rows; 
		//neigbors and distances matrices
		arma::uvec nbs; 
		arma::vec dists;
		//search for the k_ nearest neighbors of c)
		nns_.Search(c, k_, nbs, dists);
		//Store the true labels of the neighbors in an arma::Row 
		arma::Row<size_t> nbs_labs = labels_.cols(nbs);
		//vector for the class probabilities of column c
		arma::vec class_probs(n_); 
		//vote for the class label of c and obtain the probabilities of each class (n_ is the number of classes)
		size_t lab = vote_policy_.vote(nbs_labs, dists, class_probs, n_);
		//place the class label in labels
		labels.col(cn) = lab;
		//place the class probabilities in p_vec
		p_vec.col(cn) = class_probs; 
	};
	data.each_col(vote); 
}



