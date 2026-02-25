#include "CandlePtr.h"

#ifdef _NVHPC_STDPAR_GPU 
__host__ __device__
#endif 

//Parameterized Constructor
CandlePtr::CandlePtr(const Datetime& dt, const double& o, const double& h, const double& l, const double& c, double&& v, const double& b, const double& a){
	dt_ptr_ = std::make_shared<Datetime>(dt); 
	o_ptr_ = std::make_shared<double>(o); 
	h_ptr_ = std::make_shared<double>(h); 
	l_ptr_ = std::make_shared<double>(l); 
	c_ptr_ = std::make_shared<double>(c);
	v_ = std::move(v); 
	b_ptr_ = std::make_shared<double>(b); 
	a_ptr_ = std::make_shared<double>(a); 
	validate_(); 
}
//Move Constructor 
CandlePtr::CandlePtr(CandlePtr &&rhs){
	dt_ptr_ = std::make_shared<Datetime>(std::move(*rhs.dt_ptr_)); 
	o_ptr_ = std::make_shared<double>(std::move(*rhs.o_ptr_)); 
	h_ptr_ = std::make_shared<double>(std::move(*rhs.h_ptr_)); 
	l_ptr_ = std::make_shared<double>(std::move(*rhs.l_ptr_)); 
	c_ptr_ = std::make_shared<double>(std::move(*rhs.c_ptr_)); 
	v_ = std::move(rhs.v_); 
	b_ptr_ = std::make_shared<double>(std::move(*rhs.b_ptr_)); 
	a_ptr_ = std::make_shared<double>(std::move(*rhs.a_ptr_)); 
}
//Copy Constructor 
CandlePtr::CandlePtr(const CandlePtr &rhs){
	//std::cout << "CandlePtr Copy Constructor Called" << std::endl; 
	dt_ptr_ = rhs.dt_ptr_; 
	o_ptr_ = rhs.o_ptr_; 
	h_ptr_ = rhs.h_ptr_; 
	l_ptr_ = rhs.l_ptr_; 
	c_ptr_ = rhs.c_ptr_; 
	v_ = rhs.v_; 
	b_ptr_ = rhs.b_ptr_;
	a_ptr_ = rhs.a_ptr_; 
}
//Destructor 

std::shared_ptr<Datetime> CandlePtr::dt() const{
	return dt_ptr_; 
}
double CandlePtr::o() const{
	return *o_ptr_; 
}
double CandlePtr::h() const{
	return *h_ptr_; 
}
double CandlePtr::l() const{
	return *l_ptr_; 
}
double CandlePtr::c() const{
	return *c_ptr_; 
}
double CandlePtr::v() const{
	return v_; 
}
double CandlePtr::b() const{
	return *b_ptr_; 
}
double CandlePtr::a() const{
	return *a_ptr_; 
}

double CandlePtr::hl2() const{
	//dereference the ptrs and return hl2 price
	return (*h_ptr_ + *l_ptr_) / 2; 
}
double CandlePtr::oc2() const{
	//deref and return oc2 price
	return (*o_ptr_ + *c_ptr_) / 2; 
}
double CandlePtr::hlc3() const{
	return (*h_ptr_ + *l_ptr_ + *c_ptr_) / 3; 
}
double CandlePtr::ohlc4() const{
	return (*o_ptr_ + *h_ptr_ + *l_ptr_ + *c_ptr_) / 4; 
}
//compute & return high minus low 
double CandlePtr::hml() const{
	return *h_ptr_ - *l_ptr_;  
}
//bull and bear boolean functions 
//Move Assignment Operator 
CandlePtr& CandlePtr::operator=(CandlePtr &&rhs){
	dt_ptr_ = std::make_shared<Datetime>(std::move(*rhs.dt_ptr_)); 
	o_ptr_ = std::make_shared<double>(std::move(*rhs.o_ptr_)); 
	h_ptr_ = std::make_shared<double>(std::move(*rhs.h_ptr_)); 
	l_ptr_ = std::make_shared<double>(std::move(*rhs.l_ptr_)); 
	c_ptr_ = std::make_shared<double>(std::move(*rhs.c_ptr_)); 
	v_ = std::move(rhs.v_); 
	b_ptr_ = std::make_shared<double>(std::move(*rhs.b_ptr_)); 
	a_ptr_ = std::make_shared<double>(std::move(*rhs.a_ptr_)); 
	return *this; 
}
//Copy Assignement Operator 
CandlePtr& CandlePtr::operator=(const CandlePtr &rhs){
	//std::cout << "Copy Assignement Operator for CandlePtr Called" << std::endl; 
	dt_ptr_ = rhs.dt_ptr_; 
	o_ptr_ = rhs.o_ptr_; 
	h_ptr_ = rhs.h_ptr_; 
	l_ptr_ = rhs.l_ptr_; 
	c_ptr_ = rhs.c_ptr_; 
	v_ = rhs.v_; 
	b_ptr_ = rhs.b_ptr_;
	a_ptr_ = rhs.a_ptr_; 
	return *this; 	
}
//Display function
void CandlePtr::display() const{
	(this->dt())->display(); 	
	std::cout << " Open: " << *o_ptr_ << " High: " << *h_ptr_; 
	std::cout << " Low: " << *l_ptr_ << " Close: " << *c_ptr_; 
	std::cout << " Volume: " << v_ << " Bid: " << *b_ptr_; 
	std::cout << " Ask: " << *a_ptr_ << std::endl; 	
}
//validate function
void CandlePtr::validate_() const{
	if(*h_ptr_ < *l_ptr_ || *h_ptr_ < *c_ptr_ || *h_ptr_ < *o_ptr_ || *l_ptr_ > *c_ptr_ || *l_ptr_ > *o_ptr_ || v_ < 0){
		/*invalid candles have either negative volume or 
		highs that are not larger than o, l, c or lows not less 
		than o, c, h*/ 
		throw std::invalid_argument("CandlePtr Constructor: Invalid Argument");  
	}
}
