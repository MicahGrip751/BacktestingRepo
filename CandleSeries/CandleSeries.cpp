#include "CandleSeries.h" 


//function to clean the data in the txt files f_in
//write data to the json file f_out
void CandleSeries::clean(std::string f_in, std::string f_out, std::string tf, std::string asset_c, std::string tmz_i){
	std::ifstream file(f_in); 
	if(!file.is_open()){
		throw std::runtime_error("File was not opened"); 
	}
	//first line has header info (no need to store it)
	std::string line; 
	std::getline(file, line);
	file.seekg(0, std::ios::end); 
	std::streamsize size = file.tellg(); 
	file.seekg(0, std::ios::beg); 
	std::string buffer(size, '\0');
	file.read(buffer.data(), size); 
	file.close(); 

	auto find_chars = [](const std::string& s, char c, std::vector<int>& positions){
		positions.push_back(0);
		int i = 1; 
		while(s.find(c, positions[i - 1] + 1) != std::string::npos){
			positions.push_back(s.find(c, positions[i - 1] + 1));
			i++; 
		}
	}; 
	std::vector<int> positions; 
	find_chars(buffer, '\n', positions); 
	//we create a vector of the valid datetimes 
	std::vector<Datetime> valid_dt;
	valid_dt.reserve(positions.size()); 
	//get the first and last datetimes
	auto c_tmz = std::chrono::current_zone();
	auto c_tmz_string = std::string(c_tmz->name()); 
	Datetime sdt = Datetime(buffer.substr(positions[1] + 1, positions[2] - positions[1]).substr(0, 19), c_tmz);
	Datetime edt = Datetime(buffer.substr(positions[positions.size() - 2] + 1).substr(0, 19), c_tmz);  
	Datetime dt = sdt; 	
	//generate the valid datetimes
	gen_valid_dt(asset_c, tf, sdt, edt, valid_dt); 
	//store the datetimes & json objects in an undordered map 
	std::unordered_map<Datetime, nlohmann::json, DatetimeHash> um; 
	
	auto parse_line = [&](const int &i){
		int line_n = std::distance(&(*positions.cbegin()), &i); 
		auto posit = std::next(positions.cbegin(), line_n);
		auto positp = std::prev(posit);
		std::string line = buffer.substr(*positp + 1, *posit - *positp);
		std::vector<int> commas;
		commas.reserve(8); 
		find_chars(line, ',', commas);
		Datetime dt = Datetime(line.substr(commas[0], commas[1] - commas[0] - 1), std::chrono::locate_zone(tmz_i)).change_tmz(c_tmz_string);

	
		um.insert(std::make_pair(
					dt, 
				nlohmann::json{
					{"Datetime", std::format("{:%Y.%m.%d %H:%M:%S}", dt.zt())}, 
					{"Open", std::stod(line.substr(commas[1] + 1, commas[2] - commas[1] - 1))}, 
					{"High", std::stod(line.substr(commas[2] + 1, commas[3] - commas[2] - 1))}, 
					{"Low", std::stod(line.substr(commas[3] + 1, commas[4] - commas[3] - 1))}, 
					{"Close", std::stod(line.substr(commas[4] + 1, commas[5] - commas[4] - 1))},
					{"Volume", std::stod(line.substr(commas[5] + 1, commas[6] - commas[5] - 1))}, 
					{"Bid", std::stod(line.substr(commas[6] + 1, commas[7] - commas[6] - 1))}, 
					{"Ask", std::stod(line.substr(commas[7] + 1))}
				} 
			)
		); 
	};


	//execution has to be sequention since we are inserting elements into m
	std::for_each(std::execution::seq, std::next(positions.begin(), 3), positions.end(), parse_line);

	nlohmann::json::array_t json_vec(valid_dt.size()); 

	auto make_candle_json2 = [&](const Datetime& dt){
		int i = std::distance(&*(valid_dt.cbegin()), &dt);
		auto dt_it = std::next(valid_dt.cbegin(), i);
		auto json_it = std::next(json_vec.begin(), i);
		//look for the valid datetime in the map
		if(um.find(dt) == um.end()){
			//if we cant find the valid datetime in the map we copy the previous candle & change the datetime
			*json_it = *std::prev(json_it); 	
			(*json_it)["Datetime"] = std::format("{:%Y.%m.%d %H:%M:%S}", dt.zt());  
		}else{
			*json_it = um.at(dt); 
		}
	};

	//fill the json array 
	std::for_each(std::execution::par_unseq, valid_dt.begin(), valid_dt.end(), make_candle_json2); 

	nlohmann::json json_w_meta = {
		//fidelity is the percentage of real candles in the candle vector
		{"fidelity_", 1 - ((json_vec.size() - um.size()) / (double)um.size())},
		{"candle_vec_", json_vec},
		{"tf_", tf_min(tf)},
	}; 

	std::ofstream file_out(f_out);
	if(!file_out.is_open()){
		throw std::invalid_argument("clean: Unable to open the output file"); 
	}
	file_out << json_w_meta; 
	file.close(); 
}


//Function to read the cleaned json file  
//Note: clean changes times to the current zone & read_clean uses the current zone 
void CandleSeries::read_clean(std::string fn){
	CleanCandleSeriesJson cs_json;
	//choose a large reserve size
	auto ec = glz::read_file_json(cs_json, fn, std::string{});
	if(ec){
		throw std::runtime_error("read_clean: The json file was not read properly"); 
	}
	auto tmz = std::chrono::current_zone(); 
	auto create_candles = [tmz](CandleJson& cj){
		return std::move(Candle{
				std::move(Datetime(cj.Datetime, tmz)), 
				std::move(cj.Open), 
				std::move(cj.High), 
				std::move(cj.Low), 
				std::move(cj.Close), 
				std::move(cj.Volume), 
				std::move(cj.Bid), 
				std::move(cj.Ask) 
				});
	};
	//populate the vector of candle objects
	Candle c(Datetime(), -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0); 
	cs_.resize(cs_json.candle_vec_.size(), c); 
	std::transform(std::execution::seq, cs_json.candle_vec_.begin(), cs_json.candle_vec_.end(), cs_.begin(), create_candles); 
	//store the fidelity 
	fidelity_ = cs_json.fidelity_;
	tf_ = cs_json.tf_; 
}
void CandleSeries::make_clean_htf(std::string clean_ltf_fn, std::string clean_htf_fn, std::string htf, Datetime st){
	this->read_clean(clean_ltf_fn);
	this->comp_htf(htf, st);
	nlohmann::json::array_t json_vec;
	json_vec.reserve(htf_cs_.size()); 
	for(auto it = this->htf_cs_.cbegin(); it != this->htf_cs_.cend(); it++){
		json_vec.push_back(nlohmann::json{
			{"Datetime", std::format("{:%Y.%m.%d %H:%M:%S}", (it->dt())->zt())}, 
			{"Open", it->o()}, 
			{"High", it->h()}, 
			{"Low", it->l()}, 
			{"Close", it->c()},
			{"Volume", it->v()}, 
			{"Bid", it->b()}, 
			{"Ask", it->a()}
		});  
	}
	nlohmann::json json_w_meta = {
		//fidelity is the percentage of real candles in the candle vector
		{"fidelity_", -1},
		{"candle_vec_", json_vec},
		{"tf_", tf_min(htf)},
	}; 

	std::ofstream file_out(clean_htf_fn);
	if(!file_out.is_open()){
		throw std::invalid_argument("make_clean_htf: Unable to open the output file"); 
	}
	file_out << json_w_meta; 
	file_out.close(); 
}

//functions returning iterators

std::vector<Candle>::const_iterator CandleSeries::cs_it_b() const{
	return cs_.cbegin();
}
std::vector<Candle>::const_iterator CandleSeries::cs_it_e() const{
	return cs_.cend();
}
std::vector<CandlePtr>::const_iterator CandleSeries::htf_it_b() const{
	if(!htf_cs_.empty()){
		return htf_cs_.cbegin();
	}else{
		throw std::runtime_error("Cannot Return Iterator: htf_cs_ is empty."); 
	}
}
std::vector<CandlePtr>::const_iterator CandleSeries::htf_it_e() const{
	if(!htf_cs_.empty()){
		return htf_cs_.cend();
	}else{
		throw std::runtime_error("Cannot Return Iterator: htf_cs_ is empty.");
	}
}
std::vector<Candle>::const_reverse_iterator CandleSeries::cs_it_rb() const{
	return cs_.crbegin();
}
std::vector<Candle>::const_reverse_iterator CandleSeries::cs_it_re() const{
	return cs_.crend();
}
std::vector<CandlePtr>::const_reverse_iterator CandleSeries::htf_it_rb() const{
	if(!htf_cs_.empty()){
		return htf_cs_.crbegin();
	}else{
		throw std::runtime_error("Cannot Return Iterator: htf_cs_ is empty.");
	}
}
std::vector<CandlePtr>::const_reverse_iterator CandleSeries::htf_it_re() const{
	if(!htf_cs_.empty()){
		return htf_cs_.crend();
	}else{
		throw std::runtime_error("Cannot Return Iterator: htf_cs_ is empty.");
	}
}
//accessors for the timeframes in mins 
unsigned short int CandleSeries::tf() const{
	return tf_; 
}
unsigned short int CandleSeries::htf() const{
	return htf_; 
}

//compute a vector of CandlePtrs from the base timeframe 
void CandleSeries::extract_c_ptrs(std::vector<CandlePtr>& c_ptr_v){
	c_ptr_v.reserve(this->cs_.size());
	auto fcn = [&](const Candle& c){
		double v = c.v(); 
		CandlePtr c_ptr = CandlePtr(c.dt(), c.o(), c.h(), c.l(), c.c(), std::move(v), c.b(), c.a());
		return c_ptr; 
	}; 
	std::transform(cs_it_b(), cs_it_e(), std::back_inserter(c_ptr_v), fcn);  
}

//function to extract a range view 

void CandleSeries::extract_ts(std::vector<Timestamp<double>>& ts, const std::string& pt, std::string tf, Datetime st){
	/*Note: this is definitely not the best way to do this as I am making copies of all of 
	the values when creating the Timestamp objects
	*/

	auto c_ptr_ext = [&](const CandlePtr& c){
		double val = 0.0; 
		if(pt == "open"){
			val = c.o(); 
		}else if(pt == "high"){
			val = c.h();  
		}else if(pt == "low"){
			val = c.l();  
		}else if(pt == "close"){
			val = c.c();  
		}else if(pt == "hl2"){
			val = c.hl2(); 
		}else if(pt == "oc2"){
			val = c.oc2(); 
		}else if(pt == "hlc3"){
			val = c.hlc3(); 
		}else if(pt == "ohlc4"){
			val = c.ohlc4(); 
		}else{
			throw std::invalid_argument("simple_ma: Enter a valid price type."); 
		}
		return Timestamp<double>(*c.dt(), std::move(val), pt);

	};

	auto c_ext = [&](const Candle& c){
		double val = 0.0; 
		if(pt == "open"){
			val = c.o(); 
		}else if(pt == "high"){
			val = c.h();  
		}else if(pt == "low"){
			val = c.l();  
		}else if(pt == "close"){
			val = c.c();  
		}else if(pt == "hl2"){
			val = c.hl2(); 
		}else if(pt == "oc2"){
			val = c.oc2(); 
		}else if(pt == "hlc3"){
			val = c.hlc3(); 
		}else if(pt == "ohlc4"){
			val = c.ohlc4(); 
		}else{
			throw std::invalid_argument("simple_ma: Enter a valid price type."); 
		}
		return Timestamp<double>(c.dt(), std::move(val), pt);

	}; 

	if(tf_min(tf) == tf_){
		ts.reserve(this->cs_.size()); 
		std::transform(cs_it_b(), cs_it_e(), std::back_inserter(ts), c_ext); 	 
	}else{
		//Either the higher timeframe has been computed or needs to be computed 	
		if(tf_min(tf) != htf_){
			comp_htf(tf, st);
		}
		ts.reserve(this->htf_cs_.size()); 
		std::transform(htf_it_b(), htf_it_e(), std::back_inserter(ts), c_ptr_ext); 
	}
}

//version of comp_htf which uses 
void CandleSeries::comp_htf(std::string htf, Datetime st){
	unsigned short int htf_i = tf_min(htf); 
	if(htf_i == htf_ && !htf_cs_.empty()){
		//nothing to do if the timeframe we are looking to compute is already computed 
		return; 
	}else{
		if(!htf_cs_.empty()){
			//if we have a higher timeframe computed but need a different one we erase htf_cs
			htf_cs_.erase(htf_cs_.begin(), htf_cs_.end()); 
		}
		//check if we can compute the higher timeframe
		if(htf_i % tf_ == 0){
			//if we can store the higher time frame
			htf_ = htf_i; 
		}
		//check if the start datetime is present in the lower timeframe
		auto st_it =  std::find_if(cs_.begin(), cs_.end(), [st](const Candle& c){return c.dt() == st;});  
		if(st_it == cs_.end()){
			throw std::runtime_error("comp_htf: Datetime st not found in cs_"); 
		}
		int step = htf_ / tf_;
		int size = (cs_.size() - std::distance(cs_.begin(), st_it)) / (htf_ / tf_); 
		htf_cs_.reserve(size);
		auto end_it = std::next(st_it, size * step); 
		for(auto it = st_it; it != end_it; std::advance(it, step)){
			//iterator to the end of the block
			auto bei = std::next(it, step - 1);
			auto bei_oa = std::next(bei); //iterator one after the block end
			//iterators to the candles with the highest high & lowest low in the block
			auto h_it = std::max_element(it, bei_oa,  [](const Candle& c1, const Candle& c2){return c1.h() < c2.h();});
			auto l_it = std::min_element(it, bei_oa , [](const Candle& c1, const Candle& c2){return c1.l() < c2.l();});
			double v = std::accumulate(it, bei_oa, 0.0, [](double val, const Candle& c){return val + c.v();}); 	
			//bid, ask & close are taken from last candle in the block 
			//Datetime and open are taken from the first candle in the block 
			CandlePtr c_ptr = CandlePtr(it->dt(), it->o(), h_it->h(), l_it->l(), bei->c(),
										std::move(v), bei->b(), bei->a()); 
			
			//push the CandlePtr object onto the higher timeframe candle series
			htf_cs_.push_back(c_ptr); 	
		}
		
	}
}


bool CandleSeries::comp_htf_help(std::string tf, Datetime st){
	bool base_tf = true; 
	if(this->tf() != this->tf_min(tf)){
		base_tf = false; 
		if(this->tf_min(tf) != this->htf()){
			//if the higher timeframe needs to be computed
			this->comp_htf(tf, st, "serial"); 
		}
	}
	return base_tf; 
}
int CandleSeries::cs_size() const{
	return cs_.size();   
}
//return the number of candles in the higher timeframe 
int CandleSeries::htf_cs_size() const{
	return htf_cs_.size();   
}
//display functions

void CandleSeries::display_cs() const{
	for(Candle c : cs_){
		c.display();
	}
}
void CandleSeries::display_htf_cs() const{
	if(cs_.size() == 0){
		throw std::runtime_error("display_htf_cs: htf_cs_ is empty."); 
	}
	for(CandlePtr c : htf_cs_){
		c.display(); 
	}
}
void CandleSeries::cs_head(int n) const{
	int u; //head size
	(cs_.size() >= n)?(u = n):(u = cs_.size()); 
	for(int i = 0; i < u; i++){
		cs_[i].display(); 
	}
}
void CandleSeries::htf_cs_head(int n) const{
	int u; //head size
	(htf_cs_.size() >= n)?(u = n):(u = htf_cs_.size());
	if(u == 0){
		throw std::runtime_error("diplay_htf_cs: htf_cs_ is empty."); 
	}
	for(int i = 0; i < u; i++){
		htf_cs_[i].display(); 
	}
}

//Display the fidelity of the data 
void CandleSeries::display_fid() const{
	std::cout << "The percentage of missing data in cs_ was: " << fidelity_ << std::endl; 
}
//set timeframe function 
unsigned short int CandleSeries::tf_min(std::string tf) const{
	if(tf == "M1"){
		return 1; 
	}else if(tf == "M5"){
		return 5; 
	}else if(tf == "M15"){
		return 15; 
	}else if(tf == "M30"){
		return 30; 
	}else if(tf == "H1"){
		return 60; 
	}else if(tf == "H4"){
		return 240; 
	}else if(tf == "D1"){
		return 1440;   
	}else{
		throw std::runtime_error("set_tf: Enter a valid timeframe string."); 
	}
}

//generate the trading hours vector 
void CandleSeries::gen_trading_hours(std::string asset_c, int tf_in_min, std::string tmz_o, 
		std::vector<std::pair<std::chrono::hh_mm_ss<std::chrono::seconds>, std::chrono::hh_mm_ss<std::chrono::seconds>>>& v){
	using namespace std::chrono; 
	//get the current day (used to create Eastern timezone open and closing local_times)
	year_month_day ymd{year_month_day(floor<days>(system_clock::now()))};

	auto est_tmz = std::chrono::locate_zone("America/New_York"); 

	//h, m, s are hour minutes and seconds in eastern time
	auto conv_to_tmz = [&](int h, int m, int s){

		//get local time of h:m:s for ymd 
		zoned_time<seconds> est_time{est_tmz, local_days(ymd) + hours{h} + minutes{m} + seconds{s}};
		zoned_time<seconds> tmz_time{tmz_o, est_time}; 
		//return the hh_mm_ss of the h:m:s time in tmz 
		return std::chrono::hh_mm_ss(tmz_time.get_local_time() - std::chrono::floor<std::chrono::days>(tmz_time.get_local_time()));
	}; 

	if(asset_c == "FX"){
		//midnight 
		hh_mm_ss<seconds> midnight{hours{0}};
		hh_mm_ss<seconds> one_period_before_midnight{hours{24} - minutes{tf_in_min}}; 
		//sunday trading hours (open 5PM et) 
		v.push_back(std::make_pair(conv_to_tmz(17, 0, 0), one_period_before_midnight));
		//monday through friday thursday hours 
		v.push_back(std::make_pair(midnight, one_period_before_midnight));
		v.push_back(std::make_pair(midnight, one_period_before_midnight));
		v.push_back(std::make_pair(midnight, one_period_before_midnight));
		v.push_back(std::make_pair(midnight, one_period_before_midnight));
		//friday trading hours (close 5PM et) 
		v.push_back(std::make_pair(midnight, conv_to_tmz(17, -1*tf_in_min, 0)));
		//saturday trading hours (not open ==> make open after close) 
		v.push_back(std::make_pair(one_period_before_midnight, midnight));

	}else if(asset_c == "EQUITIES" || asset_c == "BONDS"){
		//complete later
	}else{
		throw std::invalid_argument("gen_trading_hours: Enter a valid asset class"); 
	}
}

//generate valid Datetimes for an asset class, timeframe between start and end dates
void CandleSeries::gen_valid_dt(std::string asset_c, std::string tf, Datetime st, Datetime end, std::vector<Datetime>& valid_dt){
	int tf_in_min = tf_min(tf);

	//return the number of seconds in an hh_mm_ss object 
	auto nsec = [](std::chrono::hh_mm_ss<std::chrono::seconds>& hms){
		return ((360*hms.hours()) + (60*hms.minutes()) + hms.seconds()); 
	}; 

	//get the timezone of st 
	auto tmz = std::chrono::locate_zone(st.tmz()); 	
	if(asset_c == "FX"){
		//get the trading hours for each day of the week 
		std::vector<std::pair<std::chrono::hh_mm_ss<std::chrono::seconds>, std::chrono::hh_mm_ss<std::chrono::seconds>>> v; 
		gen_trading_hours(asset_c, tf_in_min, std::string(st.zt().get_time_zone()->name()), v);
		//copy the start datetime 
		Datetime trading_period = st;
		int i = 0; 
		while(trading_period <= end){
			std::chrono::hh_mm_ss<std::chrono::seconds> hms; 
			trading_period.get_hms(hms);
			auto hms_in_sec = nsec(hms);
			auto open_in_sec = nsec(v[trading_period.dn()].first); 
			auto close_in_sec = nsec(v[trading_period.dn()].second); 
			if(hms_in_sec < open_in_sec && hms_in_sec < close_in_sec){
				//if we are before the open and before the close we move the trading period to the open
				trading_period.roll_time(v[trading_period.dn()].first); 
			}else if(hms_in_sec > open_in_sec && hms_in_sec > close_in_sec){
				//if we are after the open and after the close we move to the open on the next day
				trading_period.roll_time(v[(trading_period.dn() + 1) %  7].first);
				trading_period.add_days(1); 
			}
			//check if we rolled from friday to saturday (if so roll to sunday)
			if(trading_period.is_sat()){
				trading_period.roll_time(v[(trading_period.dn() + 1) %  7].first);
				trading_period.add_days(1); 
			}
			valid_dt.push_back(trading_period);
			i++; 
			trading_period.add_mins(std::move(tf_in_min));
		}

	}else if(asset_c == "EQUITIES" || asset_c == "BONDS"){
		//complete later
	}
}

