#pragma once
#include <memory>
#include <math.h>
#include "../Datetime/Datetime.h" 
class CandlePtr{
	public:
		CandlePtr(const Datetime& dt, const double& o, const double& h, const double& l, const double& c, double&& v, const double& b, const double& a);
		//Move Constructor 
		CandlePtr(CandlePtr &&rhs); 
		//Copy Constructor 
		CandlePtr(const CandlePtr &rhs);
		//Destructor (Use compiler Default) 
		~CandlePtr() = default; 
		//accessors
		std::shared_ptr<Datetime> dt() const;
		double o() const; 
		double h() const; 
		double l() const; 
		double c() const;
		double b() const; 
		double a() const; 
		double v() const; 
		double hl2() const; 
		double oc2() const; 
		double hlc3() const; 
		double ohlc4() const;
		//compute high minus low 
		double hml() const; 
		//Display function
		void display() const;
		//Move assignment operator 
		CandlePtr& operator=(CandlePtr &&rhs); 
		//Copy assignement operator
		CandlePtr& operator=(const CandlePtr &rhs); 
	private:
		std::shared_ptr<Datetime> dt_ptr_; 
		std::shared_ptr<double> o_ptr_; 
		std::shared_ptr<double> h_ptr_; 
		std::shared_ptr<double> l_ptr_; 
		std::shared_ptr<double> c_ptr_;
		std::shared_ptr<double> b_ptr_; 
		std::shared_ptr<double> a_ptr_; 
		double v_; 
		//validate function 
		void validate_() const; 
};
