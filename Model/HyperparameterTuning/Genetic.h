#pragma once
#include "../model.h" 

//T is the type stored in the arma::Mat data matrices 
//L is the type stored in the arma::Row labels rows 
//M is the model type (needs a .Train method)
//LabItPair is a pair of iterators to Timestamp objects used for constructing the labels (assumes second element in lab_it_pair is a .end iterator)
//PointTypes is the parameter pack of the types in the hyperparameter tuples
template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
class Genetic{
	public: 
		Genetic() = default;
		//parameterized constructor 
		Genetic(LabItPair lab_it_pair, std::string recombination_op, int n_gen, double scale, const std::vector<std::tuple<PointTypes...>>& initial_pop, 
				int k_train_splits, int k_test_splits, double test_split_ratio);
		//Tune function type is either discrete or intermediate & specifies which type of recombination to use
		void Tune(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads = 24);
		//Tune function which takes a tuple of extra arguments to pass to .Train
		template<typename ...TrainParams> 
		void Tune(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams...);
		
		template <typename ...TrainParams> 
		void TunePrecision(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams...); 
	
		template <typename ...TrainParams> 
		void TuneF25(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams...); 

		//write the results of each generation to a txt file at the location path & include a description of what hyperparameters are being tuned and what recombination operator is used	
		void TuningReport(const std::string& path, const std::string& description); 
		void PrecisionTuningReport(const std::string& path, const std::string& description); 
		void F25TuningReport(const std::string& path, const std::string& description); 

	private: 
		LabItPair lab_it_pair_;
		//the type of recombination operator 
		const std::string recombination_op_; 
		//the number of generations to produce 
		const int n_gen_ = 5;
		//how many individuals are in a generation
		const int gen_size_;
		const double scale_ = .75;
		//number of training and testing splits
		const int k_train_splits_ = 1;
		const int k_test_splits_ = 3; 
		//the ratio of data to include in the test split
		const double test_split_ratio_ = .3;
		//initial population
		const std::vector<std::tuple<PointTypes...>> initial_pop_; 
		//AUROC averaged over each split combination for each parameter tuple (each generation is kept in one of the vectors) 
		std::vector<std::vector<std::pair<std::tuple<PointTypes...>, double>>> tuning_results_; 
}; 
template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes>
Genetic<T, L, M, LabItPair, PointTypes...>::Genetic(LabItPair lab_it_pair, std::string recombination_op, int n_gen, double scale, const std::vector<std::tuple<PointTypes...>>& initial_pop,  
		int k_train_splits, int k_test_splits, double test_split_ratio) : 
	lab_it_pair_{lab_it_pair}, recombination_op_{recombination_op}, n_gen_{n_gen}, gen_size_{(int)initial_pop.size()}, scale_{scale}, initial_pop_{initial_pop}, k_train_splits_{k_train_splits}, 
	k_test_splits_{k_test_splits}, test_split_ratio_{test_split_ratio}
	{
		if(recombination_op != "discrete" && recombination_op != "intermediate"){
			throw std::invalid_argument("Genetic Constructor: Enter a valid type of recombination operator."); 
		}
	};  
template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
template<typename ...TrainParams> 
void Genetic<T, L, M, LabItPair, PointTypes...>::Tune(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams... tp){
	//generate the initial population of individuals
	auto generation = initial_pop_;
	//define the fitness function
	auto fit_fcn = [f, this, tp...](const std::tuple<PointTypes...>& ind){
		//declare matrices 
		arma::Mat<T> matrix, train_mat, test_mat; 
		arma::Row<L> labels, train_lab, test_lab;
		//declare vectors of matrices to hold the test and train splits
		std::vector<arma::Mat<T>> train_ms, test_ms;
		std::vector<arma::Row<L>> train_ls, test_ls;
		//call f to fill the matrix and labels
		f(ind, matrix, labels);
		//split the matrix and labels into test & train sets
		model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, test_split_ratio_);
		//clear matrix and labels 
		matrix.clear(); labels.clear();
		//call stratified_k_split 
		model::stratified_k_split(train_mat, train_lab, train_ms, train_ls, k_train_splits_); 
		model::stratified_k_split(test_mat, test_lab, test_ms, test_ls, k_test_splits_);
		//clear train_mat, train_lab, test_mat, and test_lab 
		train_mat.clear(); train_lab.clear(); test_mat.clear(); test_lab.clear();
		L mc = model::minority_class(train_ls[0]); 
		double avg_auroc = 0.0;
		for(int i = 0; i < train_ms.size(); i++){
			//create a model of type M and train it on the ith train split 
			M mod;
			mod.Train(train_ms[i], train_ls[i], 2, tp...);
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
		train_ms.clear(); train_ls.clear(); test_ms.clear(); test_ls.clear();
		return avg_auroc; 
	};
	for(int i = 0; i < n_gen_; i++){
		//evaluate the fitness of the generation 
		auto scored_gen = utility::generation_fitness(generation, fit_fcn, n_threads);
		tuning_results_.push_back(scored_gen); 
		//print the scored individuals in the generation to the console
		std::cout << "Generation #" << i << std::endl; 
		for(auto ind: scored_gen){
			std::cout << "Individual: "; 
			utility::tuple_print(std::get<0>(ind));
			std::cout << "  Averaged AUROC: " << std::get<1>(ind) << std::endl; 
		}
		//perform discrete recombination 
		auto parent_pairs = utility::selection(scored_gen, std::greater<double>(), scale_);
		//make the new generation using discrete recombination using either discrete or intermediate recombination
		if(recombination_op_ == "discrete"){
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), [](const auto& tp){return utility::discrete_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));}); 
		}else{
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), 
					[](const auto& tp){return utility::intermediate_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));});
		}
	}
	
}

template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
void Genetic<T, L, M, LabItPair, PointTypes...>::Tune(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads){
	//generate the initial population of individuals
	auto generation = initial_pop_;
	//define the fitness function
	auto fit_fcn = [f, this](const std::tuple<PointTypes...>& ind){
		//declare matrices 
		arma::Mat<T> matrix, train_mat, test_mat; 
		arma::Row<L> labels, train_lab, test_lab;
		//declare vectors of matrices to hold the test and train splits
		std::vector<arma::Mat<T>> train_ms, test_ms;
		std::vector<arma::Row<L>> train_ls, test_ls;
		//call f to fill the matrix and labels
		f(ind, matrix, labels);
		//split the matrix and labels into test & train sets
		model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, test_split_ratio_);
		//std::cout << "does matrix have NaN's" << matrix.has_nan() << std::endl; 
		arma::uvec nan_idx = arma::find_nan(matrix);
		//std::cout << "how many NaN's: " << nan_idx.n_elem << std::endl;  
		//clear matrix and labels 
		matrix.clear(); labels.clear();
		//call stratified_k_split 
		model::stratified_k_split(train_mat, train_lab, train_ms, train_ls, k_train_splits_); 
		model::stratified_k_split(test_mat, test_lab, test_ms, test_ls, k_test_splits_);
		//clear train_mat, train_lab, test_mat, and test_lab 
		train_mat.clear(); train_lab.clear(); test_mat.clear(); test_lab.clear();
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
		train_ms.clear(); train_ls.clear(); test_ms.clear(); test_ls.clear();
		return avg_auroc; 
	};
	for(int i = 0; i < n_gen_; i++){
		//evaluate the fitness of the generation 
		auto scored_gen = utility::generation_fitness(generation, fit_fcn, n_threads);
		tuning_results_.push_back(scored_gen); 
		//print the scored individuals in the generation to the console
		std::cout << "Generation #" << i << std::endl; 
		for(auto ind: scored_gen){
			std::cout << "Individual: "; 
			utility::tuple_print(std::get<0>(ind));
			std::cout << "  Averaged AUROC: " << std::get<1>(ind) << std::endl; 
		}
		//perform discrete recombination 
		auto parent_pairs = utility::selection(scored_gen, std::greater<double>(), scale_);
		//make the new generation using discrete recombination using either discrete or intermediate recombination
		if(recombination_op_ == "discrete"){
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), [](const auto& tp){return utility::discrete_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));}); 
		}else{
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), 
					[](const auto& tp){return utility::intermediate_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));});
		}
	}
}

template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
template<typename ...TrainParams> 
void Genetic<T, L, M, LabItPair, PointTypes...>::TunePrecision(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams... tp){
	//generate the initial population of individuals
	auto generation = initial_pop_;
	//define the fitness function
	auto fit_fcn = [f, this, tp...](const std::tuple<PointTypes...>& ind){
		//declare matrices 
		arma::Mat<T> matrix, train_mat, test_mat; 
		arma::Row<L> labels, train_lab, test_lab;
		//declare vectors of matrices to hold the test and train splits
		std::vector<arma::Mat<T>> train_ms, test_ms;
		std::vector<arma::Row<L>> train_ls, test_ls;
		//call f to fill the matrix and labels
		f(ind, matrix, labels);
		//split the matrix and labels into test & train sets
		model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, test_split_ratio_);
		//clear matrix and labels 
		matrix.clear(); labels.clear();
		//call stratified_k_split 
		model::stratified_k_split(train_mat, train_lab, train_ms, train_ls, k_train_splits_); 
		model::stratified_k_split(test_mat, test_lab, test_ms, test_ls, k_test_splits_);
		//clear train_mat, train_lab, test_mat, and test_lab 
		train_mat.clear(); train_lab.clear(); test_mat.clear(); test_lab.clear();
		//L mc = model::minority_class(train_ls[0]); 
		double precision = 0.0;
		for(int i = 0; i < train_ms.size(); i++){
			//create a model of type M and train it on the ith train split 
			M mod;
			mod.Train(train_ms[i], train_ls[i], 2, tp...);
			for(int j = 0; j < test_ms.size(); j++){
				precision += mlpack::Precision<mlpack::AverageStrategy::Binary, 1>::Evaluate(mod, test_ms[j], test_ls[j]);	
			}
		}
		precision /= (train_ms.size() * test_ms.size());
		train_ms.clear(); train_ls.clear(); test_ms.clear(); test_ls.clear();
		return precision; 
	};
	for(int i = 0; i < n_gen_; i++){
		//evaluate the fitness of the generation 
		auto scored_gen = utility::generation_fitness(generation, fit_fcn, n_threads);
		tuning_results_.push_back(scored_gen); 
		//print the scored individuals in the generation to the console
		std::cout << "Generation #" << i << std::endl; 
		for(auto ind: scored_gen){
			std::cout << "Individual: "; 
			utility::tuple_print(std::get<0>(ind));
			std::cout << "  Averaged Precision: " << std::get<1>(ind) << std::endl; 
		}
		//perform discrete recombination 
		auto parent_pairs = utility::selection(scored_gen, std::greater<double>(), scale_);
		//make the new generation using discrete recombination using either discrete or intermediate recombination
		if(recombination_op_ == "discrete"){
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), [](const auto& tp){return utility::discrete_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));}); 
		}else{
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), 
					[](const auto& tp){return utility::intermediate_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));});
		}
	}
}
template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
template <typename ...TrainParams> 
void Genetic<T, L, M, LabItPair, PointTypes...>::TuneF25(std::function<void (const std::tuple<PointTypes...>& param_tuple, arma::Mat<T>& matrix, arma::Row<L>& labels)> f, int n_threads, TrainParams... tp){
	//generate the initial population of individuals
	auto generation = initial_pop_;
	//define the fitness function
	auto fit_fcn = [f, this, tp...](const std::tuple<PointTypes...>& ind){
		//declare matrices 
		arma::Mat<T> matrix, train_mat, test_mat; 
		arma::Row<L> labels, train_lab, test_lab;
		//declare vectors of matrices to hold the test and train splits
		std::vector<arma::Mat<T>> train_ms, test_ms;
		std::vector<arma::Row<L>> train_ls, test_ls;
		//call f to fill the matrix and labels
		f(ind, matrix, labels);
		//split the matrix and labels into test & train sets
		model::split(matrix, labels, train_mat, train_lab, test_mat, test_lab, test_split_ratio_);
		//clear matrix and labels 
		matrix.clear(); labels.clear();
		//call stratified_k_split 
		model::stratified_k_split(train_mat, train_lab, train_ms, train_ls, k_train_splits_); 
		model::stratified_k_split(test_mat, test_lab, test_ms, test_ls, k_test_splits_);
		//clear train_mat, train_lab, test_mat, and test_lab 
		train_mat.clear(); train_lab.clear(); test_mat.clear(); test_lab.clear();
		//L mc = model::minority_class(train_ls[0]); 
		double f_beta = 0.0;
		for(int i = 0; i < train_ms.size(); i++){
			//create a model of type M and train it on the ith train split 
			M mod;
			mod.Train(train_ms[i], train_ls[i], 2, tp...);
			for(int j = 0; j < test_ms.size(); j++){
				double precision = mlpack::Precision<mlpack::AverageStrategy::Binary, 1>::Evaluate(mod, test_ms[j], test_ls[j]);	 
				double recall = mlpack::Recall<mlpack::AverageStrategy::Binary, 1>::Evaluate(mod, test_ms[j], test_ls[j]);
				if(std::isnan(precision)){
					f_beta += 0.0; 
				}else{
					f_beta += ((1.0625*precision*recall) / (.0625*precision + recall));  
				}
			}
		}
		f_beta /= (train_ms.size() * test_ms.size());
		train_ms.clear(); train_ls.clear(); test_ms.clear(); test_ls.clear();
		return f_beta; 
	};
	for(int i = 0; i < n_gen_; i++){
		//evaluate the fitness of the generation 
		auto scored_gen = utility::generation_fitness(generation, fit_fcn, n_threads);
		tuning_results_.push_back(scored_gen); 
		//print the scored individuals in the generation to the console
		std::cout << "Generation #" << i << std::endl; 
		for(auto ind: scored_gen){
			std::cout << "Individual: "; 
			utility::tuple_print(std::get<0>(ind));
			std::cout << "  Averaged F_{.25}: " << std::get<1>(ind) << std::endl; 
		}
		//perform discrete recombination 
		auto parent_pairs = utility::selection(scored_gen, std::greater<double>(), scale_);
		//make the new generation using discrete recombination using either discrete or intermediate recombination
		if(recombination_op_ == "discrete"){
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), [](const auto& tp){return utility::discrete_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));}); 
		}else{
			std::transform(parent_pairs.begin(), parent_pairs.end(), generation.begin(), 
					[](const auto& tp){return utility::intermediate_recombination(std::get<0>(tp), std::get<1>(tp), std::get<2>(tp));});
		}
	}
}

template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
void Genetic<T, L, M, LabItPair, PointTypes...>::TuningReport(const std::string& path, const std::string& description){
	std::string text = "Genetic Hyperparameter Tuning \n"; 
	text += ("Parameter & Recombination Description: " + description + "\n");
	text += ("Hyperparameter Tuning Start Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", lab_it_pair_.first->dt().zt()) + "\n"); 
	text += ("Hyperparameter Tuning End Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", std::prev(lab_it_pair_.second)->dt().zt()) + "\n");
	for(int i = 0; i < tuning_results_.size(); i++){
		text += ("Generation #" + std::to_string(i) + "\n");
		//iterate through the scored individuals in the generation
		for(auto si : tuning_results_[i]){
			text += ("Individual: " + utility::tuple_string(std::get<0>(si)) + " Averaged AUROC: " + std::to_string(std::get<1>(si)) + "\n"); 
		}
	}
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << text;
	ofs.close(); 
}

template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
void Genetic<T, L, M, LabItPair, PointTypes...>::PrecisionTuningReport(const std::string& path, const std::string& description){
	std::string text = "Genetic Hyperparameter Tuning For Precision \n"; 
	text += ("Parameter & Recombination Description: " + description + "\n");
	text += ("Hyperparameter Tuning Start Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", lab_it_pair_.first->dt().zt()) + "\n"); 
	text += ("Hyperparameter Tuning End Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", std::prev(lab_it_pair_.second)->dt().zt()) + "\n");
	for(int i = 0; i < tuning_results_.size(); i++){
		text += ("Generation #" + std::to_string(i) + "\n");
		//iterate through the scored individuals in the generation
		for(auto si : tuning_results_[i]){
			text += ("Individual: " + utility::tuple_string(std::get<0>(si)) + " Averaged Precision: " + std::to_string(std::get<1>(si)) + "\n"); 
		}
	}
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << text;
	ofs.close(); 
}

template <typename T, typename L, typename M, typename LabItPair, typename ...PointTypes> 
void Genetic<T, L, M, LabItPair, PointTypes...>::F25TuningReport(const std::string& path, const std::string& description){
	std::string text = "Genetic Hyperparameter Tuning For F_{.25} \n"; 
	text += ("Parameter & Recombination Description: " + description + "\n");
	text += ("Hyperparameter Tuning Start Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", lab_it_pair_.first->dt().zt()) + "\n"); 
	text += ("Hyperparameter Tuning End Datetime: " + std::format("{:%Y.%m.%d %H:%M:%S}", std::prev(lab_it_pair_.second)->dt().zt()) + "\n");
	for(int i = 0; i < tuning_results_.size(); i++){
		text += ("Generation #" + std::to_string(i) + "\n");
		//iterate through the scored individuals in the generation
		for(auto si : tuning_results_[i]){
			text += ("Individual: " + utility::tuple_string(std::get<0>(si)) + " Averaged F_{.25}: " + std::to_string(std::get<1>(si)) + "\n"); 
		}
	}
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << text;
	ofs.close(); 
}

