#include "Candle.h" 

//constructor
/*
Candle::Candle(Datetime&& dt, double&& o, double&& h, double&& l, double&& c, double&& v, double&& b, double &&a){
	dt_ = Datetime(std::move(dt)); 
	o_ = std::move(o); 
	h_ = std::move(h); 
	l_ = std::move(l); 
	c_ = std::move(c); 
	v_ = std::move(v);
	b_ = std::move(b); 
	a_ = std::move(a); 
	//check if the candle is valid
	validate_(); 
}
*/

Candle::Candle(Datetime&& dt, double&& o, double&& h, double&& l, double&& c, double&& v, double&& b, double &&a) : 
	dt_{(std::move(dt))}, o_{std::move(o)}, h_{std::move(h)}, l_{std::move(l)}, c_{std::move(c)}, v_{std::move(v)}, b_{std::move(b)}, a_{std::move(a)} {};

/*
Candle::Candle(Datetime&& dt, double&& o, double&& h, double&& l, double&& c, double&& v, double&& b, double &&a) : 
	dt_{std::move(dt)}, o_{std::move(o)}, h_{std::move(h)}, c_{std::move(c)}, v_{std::move(v)}, b_{std::move(b)}, a_{std::move(a)} {}; 
*/

//constructor from a CandleJson object 
Candle::Candle(CandleJson& cj) : 
	dt_{std::move(Datetime{cj.Datetime})}, o_{std::move(cj.Open)}, h_{std::move(cj.High)}, l_{std::move(cj.Low)}, c_{std::move(cj.Close)}, v_{std::move(cj.Volume)}, b_{std::move(cj.Bid)}, a_{std::move(cj.Ask)} {};


//Move Constructor
Candle::Candle(Candle&& rhs) : dt_{std::move(rhs.dt_)}, o_{std::move(rhs.o_)}, h_{std::move(rhs.h_)}, l_{std::move(rhs.l_)}, c_{std::move(rhs.c_)}, v_{std::move(rhs.v_)}, b_{std::move(rhs.b_)}, a_{std::move(rhs.a_)} {}; 

//Copy Constructor 
Candle::Candle(const Candle &rhs) : dt_{rhs.dt_}, o_{rhs.o_}, h_{rhs.h_}, l_{rhs.l_}, c_{rhs.c_}, v_{rhs.v_}, b_{rhs.b_}, a_{rhs.a_} {} 
	
const Datetime& Candle::dt() const{
	return dt_; 
}
const double& Candle::o() const{
	return o_; 
}
const double& Candle::h() const{
	return h_; 
}
const double& Candle::l() const{
	return l_; 
}
const double& Candle::c() const{
	return c_; 
}
const double& Candle::v() const{
	return v_; 
}
const double& Candle::b() const{
	return b_; 
}
const double& Candle::a() const{
	return a_; 
}

double Candle::hl2() const{
	return (h_ + l_) / 2; 
}
double Candle::oc2() const{
	return (o_ + c_) / 2; 
}
double Candle::hlc3() const{
	return (h_ + l_ + c_) / 3; 
}
double Candle::ohlc4() const{
	return (o_ + h_ + l_ + c_) / 4; 
}
//compute & return high minus low 
double Candle::hml() const{
	return h_ - l_; 
}
//Copy assingment operator 
Candle& Candle::operator=(const Candle &rhs){
	dt_ = rhs.dt_; //use copy assignment operator for Datetime 
	o_ = rhs.o_; 
	h_ = rhs.h_; 
	l_ = rhs.l_; 
	c_ = rhs.c_; 
	v_ = rhs.v_;
	b_ = rhs.b_; 
	a_ = rhs.a_; 
	return *this; 
}
//Move assingment operator
Candle& Candle::operator=(Candle &&rhs){
	dt_ = std::move(rhs.dt_); //use move assingment operator for Datetime 
	o_ = std::move(rhs.o_); 
	h_ = std::move(rhs.h_); 
	l_ = std::move(rhs.l_); 
	c_ = std::move(rhs.c_); 
	v_ = std::move(rhs.v_);
	b_ = std::move(rhs.b_); 
	a_ = std::move(rhs.a_); 
	return *this; 
}

//Display function
void Candle::display() const{
	this->dt().display(); 	
	std::cout << " Open: " << this->o() << " High: " << this->h(); 
	std::cout << " Low: " << this->l() << " Close: " << this->c(); 
	std::cout << " Volume: " << this->v() << " Bid: ";
	std::cout << this->b() << " Ask: " << this->a() << std::endl; 
}


//validate function
void Candle::validate_() const{
	if(h_ < l_ || h_ < c_ || h_ < o_ || l_ > c_ || l_ > o_ || v_ < 0){
		/*invalid candles have either negative volume or 
		highs that are not larger than o, l, c or lows not less 
		than o, c, h */
		throw std::invalid_argument("Candle Constructor: Invalid Argument");  
	}
}





