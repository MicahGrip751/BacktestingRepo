#pragma once
#include "../Datetime/Datetime.h" 

template <typename T> 
class Timestamp{
	public:
		//Default constructor
		Timestamp(); 
		//Parameterized constructors
		Timestamp(const Datetime& dt, T&& val, const std::string& var);
		Timestamp(const Datetime& dt, const T& val, const std::string& var); 
		Timestamp(std::shared_ptr<Datetime> dt_ptr, const T& val, const std::string& var); 
		//datetime accessor
		const Datetime& dt() const; 
		//value accessor 
		const T& val() const;
		//display function
		void display() const;
		//val setter
		void set_val(const T& t); 
		//var setter
		void set_var(const std::string& var); 
		//strong ordering <=> operator is based on val_
		std::strong_ordering operator<=>(const Timestamp& rhs) const; 
	private:
		std::shared_ptr<std::string> var_; //points to the name of the variable stored in val
		std::shared_ptr<Datetime> dt_; //points to the datetime of the timestamp 
		T val_; //value of the Timeseries at datetime 
};
template <typename T> 
Timestamp<T>::Timestamp() : 
	dt_{nullptr}, var_{nullptr}, val_{std::move(T())} {};  

template <typename T>
Timestamp<T>::Timestamp(const Datetime& dt, T&& val, const std::string& var) : 
	var_{std::make_shared<std::string>(var)}, dt_{std::make_shared<Datetime>(dt)}, val_{std::move(val)} {}; 

template <typename T> 
Timestamp<T>::Timestamp(const Datetime& dt, const T& val, const std::string& var) : 
	var_{std::make_shared<std::string>(var)}, dt_{std::make_shared<Datetime>(dt)}, val_{std::move(val)} {}; 

template <typename T> 
Timestamp<T>::Timestamp(std::shared_ptr<Datetime> dt_ptr, const T& val, const std::string& var) : 
	var_{std::make_shared<std::string>(var)}, dt_{dt_ptr}, val_{std::move(val)} {}; 


//accessors
template <typename T> 
const Datetime& Timestamp<T>::dt() const{
	return *dt_; 
}
template <typename T> 
const T& Timestamp<T>::val() const{
	return val_; 
}
//val setter
template <typename T> 
void Timestamp<T>::set_val(const T& t){
	val_ = t; 
}
//var setter
template <typename T> 
void Timestamp<T>::set_var(const std::string& var){
	var_ = std::make_shared<std::string>(var); 
}

//display function
template <typename T>
void Timestamp<T>::display() const{
	dt_->display(); 
	std::cout << std::setprecision(12); 
	std::cout << " " << *var_ << ": " << val_ << std::endl; 
}

template <typename T> 
std::strong_ordering Timestamp<T>::operator<=>(const Timestamp<T>& rhs) const{
	if(this->val_ < rhs.val_){
		return std::strong_ordering::less; 
	}else if(this->val_ > rhs.val_){
		return std::strong_ordering::greater; 
	}else{
		return std::strong_ordering::equivalent; 
	}
}

