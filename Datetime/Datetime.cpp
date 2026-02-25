#include "Datetime.h" 

//default constructor 
Datetime::Datetime(){
	std::stringstream str("1970.01.01 00:00:00"); 
	std::chrono::local_seconds src; 
	std::chrono::from_stream(str, "%Y.%m.%d %H:%M:%S", src);
	dt_ = std::chrono::zoned_time(std::chrono::current_zone(), std::move(src));	
}

//parameterized constructor using braced initialization 
Datetime::Datetime(std::string& s, const std::chrono::time_zone* tmz) : 
	dt_
	{
		[&](){
			std::stringstream str(s); 
			std::chrono::local_time<std::chrono::seconds> src;
			std::chrono::from_stream(str, "%Y.%m.%d %H:%M:%S", src);
			return std::chrono::zoned_time(tmz, std::move(src)); 
		}()  
	} {}; 

	

Datetime::Datetime(std::string&& s, const std::chrono::time_zone* tmz) : 
	dt_
	{
		[&](){
			std::stringstream str(s); 
			std::chrono::local_time<std::chrono::seconds> src;
			std::chrono::from_stream(str, "%Y.%m.%d %H:%M:%S", src);
			return std::chrono::zoned_time(tmz, std::move(src)); 
		}()  
	} {}; 

Datetime::Datetime(std::stringstream&& ss, const std::chrono::time_zone* tmz) : 
	dt_{
		[&](){
			std::chrono::local_time<std::chrono::seconds> src;
			std::chrono::from_stream(ss, "%Y.%m.%d %H:%M:%S", src);
			return std::chrono::zoned_time(tmz, std::move(src)); 
		}()  

	} {}; 

//move constructor 
Datetime::Datetime(Datetime&& dt) : dt_{std::move(dt.dt_)} {}
//Copy Constructor 
Datetime::Datetime(const Datetime& dt) : dt_{dt.dt_} {}

//Accessors 

std::chrono::zoned_time<std::chrono::seconds> Datetime::zt() const{
	return dt_; 
}


int Datetime::year() const{
	//extract local time point, convert to year_month_day object, cast as an int and return
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	return static_cast<int>(ymd.year());  
}
unsigned Datetime::month() const{
	//extract year month day object
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	return static_cast<unsigned>(ymd.month());  
}
unsigned Datetime::day() const{
	//extract year_month_day object
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	//cast Chrono day object to an unsigned int
	return static_cast<unsigned>(ymd.day());
}
unsigned int Datetime::hour() const{
	//extract the hh_mm_ss object
	std::chrono::hh_mm_ss<std::chrono::seconds> hms; 
	get_hms(hms);
	//call duration_cast, count the hours, cast to an int	
	return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::hours>(hms.hours()).count());
}
unsigned int Datetime::min() const{
	//extract the hh_mm_ss object
	std::chrono::hh_mm_ss<std::chrono::seconds> hms; 
	get_hms(hms);
	//call duration_cast, count the hours, cast to an int	
	return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::minutes>(hms.minutes()).count());
}
unsigned int Datetime::sec() const{
	//extract the hh_mm_ss object
	std::chrono::hh_mm_ss<std::chrono::seconds> hms; 
	get_hms(hms);
	//call duration_cast, count the seconds, cast to an int	
	return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(hms.seconds()).count());
}

unsigned short int Datetime::dn() const{
	using namespace std::chrono; 
	year_month_day ymd; 
	get_ymd(ymd); 
	weekday wd{sys_days(ymd)};
	return wd.c_encoding(); //grab the weekday number which is in the range [0, 6] 
}

std::string_view Datetime::tmz() const{
	return dt_.get_time_zone()->name();
}


int Datetime::serial_datetime() const{
	//extract year_month_day object
	std::chrono::year_month_day ymd;  
	get_ymd(ymd);
	//represent the the ymd_ object as an int 
	return std::chrono::sys_time<std::chrono::seconds>(dt_).time_since_epoch().count();  
}
unsigned Datetime::eom() const{
	//extract year_month_day object
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	//get the last day of the month
	std::chrono::year_month_day_last eom{ymd.year() / ymd.month() / std::chrono::last};
	//cast the eom day to an unsigned and return 
	return static_cast<unsigned>(eom.day());
}
bool Datetime::is_christmas() const{
	if(this->month() == 12 && this->day()  == 25){
		return true; 
	}
	return false; 
}
bool Datetime::is_christmas_eve() const{
	if(this->month() == 12 && this->day()  == 24){
		return true; 
	}
	return false; 
}
bool Datetime::is_new_years() const{
	if(this->month() == 1 && this->day()  == 1){
		return true; 
	}
	return false; 
}
bool Datetime::is_new_years_eve() const{
	if(this->month() == 12 && this->day()  == 31){
		return true; 
	}
	return false; 
}


//Get year_month_day object from the zoned_time 
void Datetime::get_ymd(std::chrono::year_month_day& ymd) const{
	//extract local time point object
	std::chrono::local_time<std::chrono::seconds> tp = this->dt_.get_local_time(); 
	//Convert to a year month day object
	ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::move(tp))); 
}

void Datetime::get_hms(std::chrono::hh_mm_ss<std::chrono::seconds>& hms) const{
	hms = std::chrono::hh_mm_ss(this->dt_.get_local_time() - std::chrono::floor<std::chrono::days>(this->dt_.get_local_time()));
}

Datetime& Datetime::change_tmz(std::string& tmz){
	this->dt_ = std::chrono::zoned_time<std::chrono::seconds>(tmz, this->dt_.get_sys_time());
	return *this;   
}
Datetime& Datetime::add_years(int &&rhs_y){
	using namespace std::chrono;
	year_month_day ymd; 
	get_ymd(ymd);
	hh_mm_ss<seconds> hms; 
	get_hms(hms);
	ymd += std::chrono::years(rhs_y);
	//only possible issue is if we obtain Feb 29 in a non leap year
	if(!ymd.ok()){
		ymd = ymd.year() / ymd.month() / 28;
	}
	auto tmz = this->dt_.get_time_zone()->name(); 
	this->dt_ = zoned_time<seconds>(tmz, local_days(ymd) + hms.hours() + hms.minutes() + hms.seconds()); 
	return *this; 
}
Datetime& Datetime::add_months(int&& rhs_m){
	using namespace std::chrono; 
	year_month_day ymd; 
	get_ymd(ymd);
	hh_mm_ss<seconds> hms; 
	get_hms(hms);
	ymd += std::chrono::months(rhs_m);
	//only possible issue is if we obtain Feb 29 in a non leap year
	if(!ymd.ok()){
		ymd = ymd.year() / ymd.month() / last;
	}
	auto tmz = this->dt_.get_time_zone()->name(); 
	this->dt_ = zoned_time<seconds>(tmz, local_days(ymd) + hms.hours() + hms.minutes() + hms.seconds()); 
	return *this; 
}
Datetime& Datetime::add_days(int&& rhs_d){
	using namespace std::chrono; 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), this->dt_.get_local_time() + days(rhs_d));
	return *this; 
}
Datetime& Datetime::add_hours(int&& rhs_h){
	using namespace std::chrono; 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), this->dt_.get_local_time() + hours(rhs_h));
	return *this; 
	
}
Datetime& Datetime::add_mins(int&& rhs_m){
	using namespace std::chrono; 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), this->dt_.get_local_time() + minutes(rhs_m));
	return *this; 
}
Datetime& Datetime::add_secs(int&& rhs_s){
	using namespace std::chrono; 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), this->dt_.get_local_time() + seconds(rhs_s));
	return *this; 
}
	
Datetime& Datetime::weekend_roll(){
	//grab the year month day object
	using namespace std::chrono; 
	year_month_day ymd; 
	get_ymd(ymd); 
	unsigned wdn = dn() + 1; //grab the weekday number [1, 7] 
	if(wdn > 5){
		add_days(8 - wdn); //roll to monday
	}
	if(static_cast<unsigned int>(ymd.month()) != this->month()){
		/*in the case the next monday is in the next month 
		we roll backwards three days to the last friday of the month*/
		this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), this->dt_.get_local_time() - std::chrono::days(3));
		
	}
	return *this; 
}
/*
this function rolls (back or forward) to the day specified by n 
of the current week and then changes the time to the time passed in as time
which is in the format "HH:MM:SS"

if n < 0 then roll backwards 
if n > 0 then roll forwards
*/
Datetime& Datetime::roll(short int n, std::string time){
	//grab the year month day object
	using namespace std::chrono; 
	year_month_day ymd; 
	get_ymd(ymd); 
	unsigned short int wdn = dn() + 1; //grab the weekday number [1, 7] 
	//storing time as an hh_mm_ss object 
	std::stringstream str(time); 
	std::chrono::seconds src;
	str >> std::chrono::parse("%H:%M:%S", src);
	std::chrono::hh_mm_ss<std::chrono::seconds> t_hms(src);
	//std::cout << "after hh_mm_ss object was constructed" << std::endl; 
	//changing the time 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), 
			local_days(ymd) + t_hms.hours() + t_hms.minutes() + t_hms.seconds());

	//std::cout << "after the time was changed in roll" << std::endl; 
	//rolling the day
	if(n > 0){
		if(wdn <= n){
			add_days(n - wdn); //roll forward to the correct day  
		}else{
			add_days(7 - (wdn - n)); //roll forward to the correct day 
		}
	}else{
		if(wdn >= std::abs(n)){
			add_days(std::abs(n) - wdn); //roll backward to the correct day 
		}else{
			add_days(-(7 - (std::abs(n) - wdn))); 	
		}
	}
	return *this; 	
}

Datetime& Datetime::roll_time(std::chrono::hh_mm_ss<std::chrono::seconds> hms){
	using namespace std::chrono; 
	year_month_day ymd; 
	get_ymd(ymd); 
	this->dt_ = zoned_time<seconds>(this->dt_.get_time_zone(), 
			local_days(ymd) + hms.hours() + hms.minutes() + hms.seconds());
	return *this; 
}


void Datetime::display() const{
	//use the stream operator
	std::cout << dt_; 
}
void Datetime::display_date() const{
	//Grab the year_month_day object and use the stream operator
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	std::cout << ymd; 
}
void Datetime::display_time() const{
	//Grab the hh_mm_ss object and use the stream operator
	std::chrono::hh_mm_ss<std::chrono::seconds> hms;  
	get_hms(hms); 
	std::cout << hms; 
}

//Properties 
bool Datetime::is_sat() const{
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	std::chrono::weekday dw = std::chrono::sys_days(ymd);
	return dw.iso_encoding() == 6;
}
bool Datetime::is_sun() const{
	std::chrono::year_month_day ymd; 
	get_ymd(ymd);
	std::chrono::weekday dw = std::chrono::sys_days(ymd);
	return dw.iso_encoding() == 7;
}
bool Datetime::is_eom() const{
	using namespace std::chrono;
	year_month_day ymd; 
	get_ymd(ymd);
	year_month_day_last em{std::chrono::year{ymd.year()} / std::chrono::month{ymd.month()} / last};
	return ymd == em;
}
bool Datetime::is_leap() const{
	using namespace std::chrono;
	year_month_day ymd; 
	get_ymd(ymd);
	return ymd.year().is_leap(); 
}

bool Datetime::is_closed(std::string asset_c) const{
	Datetime close = *this; 
	Datetime open = *this;
	std::string est = "America/New_York";
	std::string c_zone{this->tmz()}; //convert string view to string 
	//store the weekday number 
	short int wdn = this->dn() + 1; //in the range [1, 7] 
	//change the timezone of open and close
	//std::cout << "in is_closed prior to change_tmz call" << std::endl; 
	open.change_tmz(est); 
	close.change_tmz(est);
	//std::cout << "in is_closed post change_tmz call" << std::endl; 
	//figure out if we are in ST or DT 
	/*
	auto info = open.zt().get_info();
	bool st = true; 
	if(info.save > std::chrono::minutes{0}){
		st = false; 
	}
	*/
	if(asset_c == "FX"){
		if(wdn < 5){
			return false; 
		}else if(wdn == 5){
			//keep the day but roll the time
			close.roll(5, "17:00:00");
			//open is the upcoming sunday
			open.roll(7, "17:00:00"); 
		}else if(wdn == 6){
			//close is the previous friday
			close.roll(-5, "17:00:00");
			//open is the upcoming Sunday
			open.roll(7, "17:00:00"); 
		}else if(wdn == 7){
			close.roll(-5, "17:00:00"); 
			open.roll(-7, "17:00:00"); 
		}
	}else if(asset_c == "EQUITIES" || asset_c == "BONDS"){
		//close is the current day at 4PM EST
		close = close.roll(wdn, "16:00:00"); 
		if(wdn < 5){
			//open is the next day at 9:30 AM EST 
			open = open.roll(7, "09:30:00");
		}else if(wdn == 5){
			//on friday the next open is monday at 9:30AM EST
			open = open.roll(1, "09:30:00");  
		}
	}else{
		throw std::runtime_error("is_closed: Enter a valid asset class."); 
	}


	if(*this >= close && *this < open){
		return true; 
	}else{
		return false; 
	}
}

//operators 
int Datetime::operator -(const Datetime& rhs) const{
	//returns the difference between two datetime objects in seconds
	return this->serial_datetime() - rhs.serial_datetime(); 
}
bool Datetime::operator ==(const Datetime& rhs) const{
	return this->dt_ == rhs.dt_;
}
std::strong_ordering Datetime::operator<=>(const Datetime& rhs) const{
	if(this->serial_datetime() < rhs.serial_datetime()){
		return std::strong_ordering::less; 
	}else if(this->serial_datetime() > rhs.serial_datetime()){
		return std::strong_ordering::greater; 
	}else{
		return std::strong_ordering::equivalent; 
	}
}
//move assignment operator 
Datetime& Datetime::operator=(Datetime&& dt){
	dt_ = std::move(dt.dt_);
	return *this; 
}
//copy assignment operator 
Datetime& Datetime::operator=(const Datetime& dt){
	dt_ = dt.dt_; 
	return *this; 
}

//Validate function
void Datetime::validate_(std::string &s) const{
	//This validate function assumes s is in the format %Y.%m.%d %H:%M:%S
	std::string d = s.substr(0, 10); 
	int h = std::stoi(s.substr(11, 2));
	int m = std::stoi(s.substr(14, 2)); 
	int sc = std::stoi(s.substr(17));  
	std::stringstream ssd(d); 
	std::chrono::year_month_day ymd; 
	std::chrono::from_stream(ssd, "%Y.%m.%d", ymd); 
	
	if(!ymd.ok()){
		throw std::invalid_argument("Datetime Constructor: Invalid Date");
	}
	if((h < 0 || h > 23) || (m < 0 || m > 59) || (sc < 0 || sc > 59)){
		throw std::invalid_argument("Datetime Constructor: Invalid Time");
	}
}






