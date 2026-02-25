#pragma once
#ifndef MLPACK_USE_SYSTEM_STB
#define MLPACK_USE_SYSTEM_STB
#endif
#include <mlpack/methods/neighbor_search.hpp>
#include <mlpack/methods/kde.hpp>

//templated based on the distance metric to use when performing neighbor search
template <typename D> 
class KDEPTNPKNN{
	public: 
		KDEPTNPKNN() = default;
		KDEPTNPKNN(int k);
		//Build Reference Tree & store perc to next pivot values 
		void Build(arma::mat& data, arma::mat& ptnp); 
		//given a data point build the KDE of the pntp of its k neighbors & return the percent to next pivot c for which 
		//CDF(c) = p (==> p is between 0 and 1) (n is the number of points to include in the discretization) 
		double ptnp_given_prob(const arma::vec& obs, double p, int n); 
	private:
		//defaulted to a large number of neighbors ==> suitable number of points in the KDE
		int k_= 60;
		//perc to next pivots 1 x n matrix 
		arma::mat ptnp_; 
		//Nearest Neighbor Search object (reference tree built during build)
		mlpack::NeighborSearch<mlpack::NearestNeighborSort, D> nns_;
};

template <typename D> 
KDEPTNPKNN<D>::KDEPTNPKNN(int k) : k_{k}
{
	if(k <= 0){
		throw std::invalid_argument("KDEPTNPKNN Constructor: The number of neighbors k must be larger than 0."); 
	}
};

template <typename D> 
void KDEPTNPKNN<D>::Build(arma::mat& data, arma::mat& ptnp){
	//store the percent to next pivots 
	ptnp_ = ptnp; 
	//build the reference tree
	nns_.Train(data);
}

template <typename D> 
double KDEPTNPKNN<D>::ptnp_given_prob(const arma::vec& obs, double p, int n){
	if(p <= 0 || p >= 1){
		throw std::invalid_argument("ptnp_given_prob: Enter a valid probability p in (0, 1)."); 
	}
	if(n <= 0){
		throw std::invalid_argument("ptnp_given_prob: The number of points in the discretization must be > 0"); 
	}
	//neigbors and distances matrices
	arma::uvec nbs; 
	arma::vec dists;
	//search for the k_ nearest neighbors of obs)
	nns_.Search(obs, k_, nbs, dists);
	//get the perc to next pivots of the k_ negihbors
	arma::mat neighbor_ptnp = ptnp_.cols(nbs);
	//create the mlpack KDE object 
	mlpack::KDE<mlpack::EpanechnikovKernel> kde;
	//train the kde 
	kde.Train(neighbor_ptnp);
	//create a discretization
	double min = *std::min_element(neighbor_ptnp.begin_col(0), neighbor_ptnp.end_col(k_ - 1)) - 2.0;  
	double max = *std::max_element(neighbor_ptnp.begin_col(0), neighbor_ptnp.end_col(k_ - 1)) + 2.0; 
	double dx = (max - min) / n; 	

	arma::vec disc = arma::linspace<arma::vec>(min, max, n);

	//arma::vec for the estimation 
	arma::vec est; 
	auto comp_pdf = [&](const arma::vec& c){
		kde.Evaluate(c, est);
		return est.at(0); 
	};
	double auc = 0.0;
	//compute the area under the pdf so we can normalize pdf values
	for(int i = 0; i < n; i++){
		double pdf = comp_pdf(arma::mat{disc[i]}.as_col());
		auc += (pdf * dx); 
	}
	double cdf = 0.0;
	int i = 0;
	while(cdf <= p && i < n){
		double pdf = comp_pdf(arma::mat{disc[i]}.as_col());
		cdf += (pdf / auc) * dx; 
		i++; 		
	}
	return disc[i];  
}
