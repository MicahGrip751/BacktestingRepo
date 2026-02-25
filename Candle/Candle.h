#pragma once
#include "../Datetime/Datetime.h"
#include "CandleJson.h"
#include <math.h>

class Candle{
	public:
		Candle(Datetime&& dt, double&& o, double&& h, double&& l, double&& c, double&& v, double&& b, double&& a);	
		//constructor from a CandleJson object 
		Candle(CandleJson& c_json); 
		//Move Constructor 
		Candle(Candle&& rhs); 
		//Copy Constructor
		Candle(const Candle& rhs);
		//Destructor 
		~Candle() = default; 
		//Accessors (return const reference to avoid obj copy)
		const Datetime& dt() const; //datetime 
		const double& o() const; //open 
		const double& h() const; //high
		const double& l() const; //low 
		const double& c() const; //close 
		const double& v() const; //volume
		const double& b() const; //bid
		const double& a() const; //ask
		//compute alternate price types
		double hl2() const; 
		double oc2() const; 
		double hlc3() const; 	
		double ohlc4() const;
		//compute high minus low 
		double hml() const; 
		//Display Function
		void display() const;
		//Move assignment operator 
		Candle& operator=(Candle&& rhs); 
		//Copy assignment operator
		Candle& operator=(const Candle& rhs);
	private:
		Datetime dt_; 
		double o_; 
		double h_; 
		double l_; 
		double c_; 
		double v_;
		double b_; 
		double a_; 
		void validate_() const; 
};
