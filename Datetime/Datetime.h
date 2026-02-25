#pragma once
#include <chrono>
#include <iostream> 
#include <stdexcept>
#include <string>

class Datetime{
	public:
		//default constructor 
		Datetime(); 
		//parameterized constructor
		Datetime(std::string& s, const std::chrono::time_zone* = std::chrono::current_zone()); 
		Datetime(std::string&& s, const std::chrono::time_zone* = std::chrono::current_zone()); 
		Datetime(std::stringstream&& ss, const std::chrono::time_zone* tmz = std::chrono::current_zone()); 
		//move constructor 
		Datetime(Datetime&& dt);
		//copy constructor 
		Datetime(const Datetime& dt); 
		//accessors
		std::chrono::zoned_time<std::chrono::seconds> zt() const; 
		int year() const; 
		unsigned month() const; 
		unsigned day() const;
		unsigned int hour() const; 
		unsigned int min() const; 
		unsigned int sec() const;
		unsigned short int dn() const; 
		//String views can be printed with cout << 
		std::string_view tmz() const; 	
		unsigned eom() const; 
		void get_ymd(std::chrono::year_month_day& ymd) const; 
		void get_hms(std::chrono::hh_mm_ss<std::chrono::seconds>& hms) const; 
		//Display functions 
		void display() const;
		void display_date() const;
		void display_time() const; 
		//modifiers
		Datetime& change_tmz(std::string& tmz);
		Datetime& add_years(int&& rhs_y); 
		Datetime& add_months(int&& rhs_m); 
		Datetime& add_days(int&& rhs_d); 
		Datetime& add_hours(int&& rhs_h); 
		Datetime& add_mins(int&& rhs_m); 
		Datetime& add_secs(int&& rhs_s);
		Datetime& weekend_roll();
		Datetime& roll(short int dn, std::string time);
		Datetime& roll_time(std::chrono::hh_mm_ss<std::chrono::seconds> hms); 
		//Properties 
		bool is_sun() const;
		bool is_sat() const;
		bool is_eom() const;
		bool is_leap() const; 
		int serial_datetime() const;
		bool is_christmas() const; 
		bool is_christmas_eve() const; 
		bool is_new_years() const; 
		bool is_new_years_eve() const; 
		bool is_closed(std::string asset_c) const; 
		//operators 
		int operator -(const Datetime& rhs) const; 
		bool operator ==(const Datetime& rhs) const; 
		std::strong_ordering operator <=> (const Datetime& rhs) const; 
		//move assignment operator
		Datetime& operator=(Datetime&& dt); 
		//copy assignment operator 
		Datetime& operator=(const Datetime& dt);
	private:
		std::chrono::zoned_time<std::chrono::seconds> dt_;
		void validate_(std::string &s) const; 
};

struct DatetimeHash{
	std::size_t operator()(const Datetime& dt) const{
		return std::hash<int>{}(dt.serial_datetime()); 
	}
}; 


