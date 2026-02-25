#include "Edgar.h"

//path is the path to where you want to store the zip file (zip file is unzipped in the same directory)
Edgar::Edgar(const std::string& request_header, const std::string& ticker_mappings_path) : request_header_{request_header}, veb_{ValidEquitiesBonds(ticker_mappings_path)}{}; 

Edgar::Edgar(const std::string& request_header, const std::string& archive_path, const std::string& ticker_mappings_path) 
	: request_header_{request_header}, archive_path_{archive_path}, veb_{ValidEquitiesBonds(ticker_mappings_path)} {}; 

void Edgar::download_archive(const std::string& archive_path){
	std::string bulk_data_url = "https://www.sec.gov/Archives/edgar/daily-index/bulkdata/submissions.zip";
	utility::Curl curl_obj(bulk_data_url, request_header_, archive_path);
	curl_obj.send_request(); 
	auto it = std::find(archive_path.rbegin(), archive_path.rend(), '/'); 
	std::string unzip_command = "unzip " + archive_path + " -d " + std::string(archive_path.begin(), --(it.base())); 
	std::system(unzip_command.c_str()); 
}

std::string Edgar::download_primary_document(const std::string& data_folder, const Filing& filing){
	std::string filing_url = Edgar::make_primary_doc_url(filing);
	//destination for where to download the filing
	std::string an_wo = boost::regex_replace(filing.accession_number, boost::regex("-"), "");
	std::string directory = data_folder + "/" + std::to_string(filing.cik) + "/" + an_wo;  
	std::string download_path = (directory + "/" + filing.primary_document);
	if(filing.primary_document == ""){
		download_path = (directory + "/" + filing.accession_number + ".txt"); 
	}
	//ensure the directory exists before downloading the file
	std::filesystem::create_directories(directory);
	utility::Curl curl_obj(filing_url, request_header_, download_path);
	curl_obj.send_request();
	return download_path; 
}
std::string Edgar::download_report(const std::string& data_folder, const int& cik, boost::regex re, const FilingSummary& fs){
	std::string url = fs.get_url(re);
	if(url != ""){
		std::string directory = data_folder + "/" + std::to_string(cik); 
		auto p = std::filesystem::path(url);
		std::string download_path = (directory + "/" + p.filename().string());
		utility::Curl curl_obj(url, request_header_, download_path);
		curl_obj.send_request();
		return download_path; 
	}else{
		return ""; 
	}
}
std::string Edgar::make_filing_summary_url(const int& cik, const std::string& accession_number){ 
	std::string an_wo = boost::regex_replace(accession_number, boost::regex("-"), "");
	return ("https://www.sec.gov/Archives/edgar/data/" + std::to_string(cik) + "/" + an_wo + "/FilingSummary.xml"); 
}

FilingSummary Edgar::parse_filing_summary(const std::string& data_folder, const Filing& filing){
	std::string an_wo = boost::regex_replace(filing.accession_number, boost::regex("-"), "");
	std::string directory = data_folder + "/" + std::to_string(filing.cik);  
	std::string download_path = (directory + "/" + filing.accession_number + "/FilingSummary.xml");
	std::string filing_summary_url = Edgar::make_filing_summary_url(filing.cik, filing.accession_number); 
	std::filesystem::create_directories(directory);
	utility::Curl curl_obj(filing_summary_url, request_header_, download_path);
	curl_obj.send_request();
	pugi::xml_document doc;
	bool loaded = doc.load_file(download_path.c_str());
	pugi::xpath_node_set reports = doc.select_nodes("//Report[child::ShortName and child::HtmlFileName]"); 
	
	FilingSummary fs;
	for(auto rep : reports){
		std::string sn = rep.node().child("ShortName").text().as_string();
		std::string html_fn = rep.node().child("HtmlFileName").text().as_string();
		fs.fs_map[sn] = ("https://www.sec.gov/Archives/edgar/data/" + std::to_string(filing.cik) + "/" + an_wo + "/" + html_fn); 
	}
	//std::string rm_file_summary = ("rm " + download_path); 
	//std::system(rm_file_summary.c_str());
	return fs; 
}
void Edgar::parse_balance_sheet(std::string& bs_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& bs_map){
	utility::parse::clean_table(bs_table);
	pugi::xml_document doc; 
	pugi::xml_parse_result res = doc.load_string(bs_table.c_str());
	std::string title;
	std::vector<std::string> col_headers;
	utility::parse::extract_balance_sheet_headers(doc, title, col_headers);
	utility::parse::parse_table_data(doc, col_headers, bs_map); 
}
void Edgar::parse_income_statement(std::string& is_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& is_map){
	utility::parse::clean_table(is_table); 
	pugi::xml_document doc; 
	pugi::xml_parse_result res = doc.load_string(is_table.c_str());
	std::string title;
	std::vector<std::string> col_headers;
	utility::parse::extract_income_statement_headers(doc, title, col_headers);
	utility::parse::parse_table_data(doc, col_headers, is_map); 
}
void Edgar::parse_cash_flow_statement(std::string& cfs_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& cfs_map){
	utility::parse::clean_table(cfs_table);
	pugi::xml_document doc; 
	pugi::xml_parse_result res = doc.load_string(cfs_table.c_str());
	std::string title;
	std::vector<std::string> col_headers;
	utility::parse::extract_cash_flow_statement_headers(doc, title, col_headers);
	utility::parse::parse_table_data(doc, col_headers, cfs_map); 
}

void Edgar::make_filing_map(const std::string& symbol, std::unordered_map<std::string, std::vector<Filing>>& filing_map){
	int cik = veb_.get_cik(symbol);
	//call the overload which takes a cik 
	make_filing_map(cik, filing_map); 
}
void Edgar::make_filing_map(const int& cik, std::unordered_map<std::string, std::vector<Filing>>& filing_map){
	
	auto process_file = [&]<typename T>(const T& json_obj){
		auto it_filing_date = json_obj["filingDate"].get_array().begin();
		auto it_accepted_date = json_obj["acceptanceDateTime"].get_array().begin(); 
		auto it_form = json_obj["form"].get_array().begin();
		auto it_accession_number = json_obj["accessionNumber"].get_array().begin();
		auto it_primary_doc = json_obj["primaryDocument"].get_array().begin(); 
		auto it_inline_xbrl= json_obj["isInlineXBRL"].get_array().begin();
		auto it_xbrl= json_obj["isXBRL"].get_array().begin();
		for(auto it = it_filing_date; it != json_obj["filingDate"].get_array().end(); it++){
			int n = std::distance(it_filing_date, it); 
			filing_map[std::next(it_form, n)->template as<std::string>()].push_back(
					Filing(cik, std::next(it_accession_number, n)->template as<std::string>(),
						std::next(it_primary_doc, n)->template as<std::string>(), 
						it->template as<std::string>(),
						std::next(it_accepted_date, n)->template as<std::string>(),  
						std::next(it_form, n)->template as<std::string>(), 
						std::next(it_inline_xbrl, n)->template as<bool>(), 
						std::next(it_xbrl, n)->template as<bool>())
					);
		}
	}; 

	std::string cik_str = std::to_string(cik); 
	std::string path = archive_path_ + "/CIK" + std::string(10 - cik_str.length(), '0') + cik_str + ".json";
	std::ifstream ifs;
	utility::fio_open(ifs, path);
	glz::basic_istream_buffer buffer(ifs);
	glz::generic json{};
	auto ec = glz::read_json(json, buffer);
	ifs.close(); 
	if(!ec){
		process_file(json.at("filings").at("recent")); 
	}
	for(auto it = json["filings"]["files"].get_array().begin(); it != json["filings"]["files"].get_array().end(); it++){
		path = archive_path_ + "/" + it->at("name").as<std::string>();
		utility::fio_open(ifs, path);
		glz::basic_istream_buffer buffer(ifs);
		auto ec = glz::read_json(json, buffer);
		ifs.close(); 
		if(!ec){
			process_file(json); 
		}
	}
}
std::string Edgar::make_primary_doc_url(const Filing& filing){
	std::string an_wo = boost::regex_replace(filing.accession_number, boost::regex("-"), "");
	if(filing.primary_document == ""){
		return ("https://www.sec.gov/Archives/edgar/data/" + std::to_string(filing.cik) + "/" + an_wo + "/" + filing.accession_number + ".txt"); 
	}else{
		return ("https://www.sec.gov/Archives/edgar/data/" + std::to_string(filing.cik) + "/" + an_wo + "/" + filing.primary_document);  
	}
}

void Edgar::get_10Q_and_10K(const int& cik, std::vector<Filing>& vec_of_10Q_and_10K){
	//make the filing map
	std::unordered_map<std::string, std::vector<Filing>> filing_map; 
	make_filing_map(cik, filing_map);
	vec_of_10Q_and_10K = filing_map.at("10-Q"); 
	vec_of_10Q_and_10K.insert(vec_of_10Q_and_10K.end(), filing_map.at("10-K").begin(), filing_map.at("10-K").end()); 	
}
void Edgar::get_latest_10Q_or_10K(const int& cik, std::vector<Filing>& latest_10Q_or_10K, int n){
	std::vector<Filing> vec_of_10Q_and_10K;
	get_10Q_and_10K(cik, vec_of_10Q_and_10K);
	//sort the filings by dt_accepted so that the most recent filings come first
	std::sort(vec_of_10Q_and_10K.begin(), vec_of_10Q_and_10K.end(), [](const Filing& f1, const Filing& f2){return f1.dt_accepted >= f2.dt_accepted;});
	for(int i = 0; i < n; i++){latest_10Q_or_10K.push_back(vec_of_10Q_and_10K[i]);}
}
void Edgar::get_latest_10Q_or_10K(const std::string& symbol, std::vector<Filing>& latest_10Q_or_10K, int n){
	int cik = veb_.get_cik(symbol);
	get_latest_10Q_or_10K(cik, latest_10Q_or_10K, n); 
}

void Edgar::get_10Q_and_10K(const std::string& symbol, std::vector<Filing>& vec_of_10Q_and_10K){
	int cik = veb_.get_cik(symbol);
	get_10Q_and_10K(cik, vec_of_10Q_and_10K); 
}
void Edgar::parse_10Q_or_10K(const std::string& data_folder, const Filing& filing, std::vector<std::pair<double, double>>& mult_pairs, 
		std::vector<std::unordered_map<std::string, std::unordered_map<std::string, double>>>& vec_of_maps){
	if(filing.form_type != "10-Q" && filing.form_type != "10-K"){
		throw std::invalid_argument("parse_10Q_or_10K: The filing argument must be a 10Q or 10K"); 
	}else{
		if(filing.has_filing_summary()){
			//if the filing summary exists parse it 
			FilingSummary fs = parse_filing_summary(data_folder, filing);
			boost::regex bs_re = boost::regex("((CONSOLIDATED(\\s+))?BALANCE(\\s+)(SHEETS))(?!((\\s+)(.*?)PARENTHETICAL))", boost::regex::icase);  
			boost::regex is_re("(CONSOLIDATED(\\s+))?STATEMENT(S?)(\\s+)OF(\\s+)(COMPREHENSIVE\\s+)?INCOME(S?)(?!((\\s+)(.*?)PARENTHETICAL))", boost::regex::icase); 
			boost::regex cfs_re("(CONSOLIDATED(\\s+))?STATEMENT(S?)(\\s+)OF(\\s+)CASH(\\s+)FLOW(S?)(?!((\\s+)(.*?)PARENTHETICAL))", boost::regex::icase); 
			std::string bs_path = download_report(data_folder, filing.cik, bs_re, fs);
			if(bs_path == ""){
				boost::regex sfp_re("((CONSOLIDATED(\\s+))?STATEMENT(S?)(\\s+)(OF)(\\s+)FINANCIAL(\\s+)(POSITION))(?!((\\s+)(.*?)PARENTHETICAL))", boost::regex::icase);  
				bs_path = download_report(data_folder, filing.cik, sfp_re, fs);          
			}
			std::string is_path = download_report(data_folder, filing.cik, is_re, fs);
			std::string cfs_path = download_report(data_folder, filing.cik, cfs_re, fs);
			std::ifstream ifs1, ifs2, ifs3; 
			utility::fio_open(ifs1, bs_path);
			utility::fio_open(ifs2, is_path);
			utility::fio_open(ifs3, cfs_path);
			std::ostringstream ss1, ss2, ss3;
			ss1 << ifs1.rdbuf(); 
			ss2 << ifs2.rdbuf(); 
			ss3 << ifs3.rdbuf(); 
			std::string bs_file_contents = ss1.str(); 
			std::string is_file_contents = ss2.str(); 
			std::string cfs_file_contents = ss3.str(); 
			ifs1.close(); ss1.clear();
			ifs2.close(); ss2.clear();
			ifs3.close(); ss3.clear();
			auto bs_p = utility::parse::find_balance_sheet(bs_file_contents, mult_pairs[0], true);
			auto is_p = utility::parse::find_income_statement(is_file_contents, mult_pairs[1], true);
			auto cfs_p = utility::parse::find_cash_flow_statement(cfs_file_contents, mult_pairs[2], true);
			std::string bs_str = utility::substr(bs_file_contents, bs_p); 
			std::string is_str = utility::substr(is_file_contents, is_p);  
			std::string cfs_str = utility::substr(cfs_file_contents, cfs_p);  
			parse_balance_sheet(bs_str, vec_of_maps[0]); 
			parse_income_statement(is_str, vec_of_maps[1]); 
			parse_cash_flow_statement(cfs_str, vec_of_maps[2]); 
		}else{
			//if the filing summary does not exist then use the primary document url 
			std::string pd_path = download_primary_document(data_folder, filing);
			std::ifstream ifs;
			utility::fio_open(ifs, pd_path);
			std::ostringstream ss;
			ss << ifs.rdbuf();
			std::string file_contents = ss.str();
			ifs.close(); ss.clear(); 
			auto bs_p = utility::parse::find_balance_sheet(file_contents, mult_pairs[0], false);
			auto is_p = utility::parse::find_income_statement(file_contents, mult_pairs[1], false);
			auto cfs_p = utility::parse::find_cash_flow_statement(file_contents, mult_pairs[2], false);
			std::string bs_str = utility::substr(file_contents, bs_p);  
			std::string is_str = utility::substr(file_contents, is_p);  
			std::string cfs_str = utility::substr(file_contents, cfs_p);  
			parse_balance_sheet(bs_str, vec_of_maps[0]); 
			parse_income_statement(is_str, vec_of_maps[1]); 
			parse_cash_flow_statement(cfs_str, vec_of_maps[2]); 
		}
	}
}


CompanyInfo::CompanyInfo(const std::string& archive_path, const int& cik){
	std::string cik_str = std::to_string(cik); 
	std::string path = archive_path + "/CIK" + std::string(10 - cik_str.length(), '0') + cik_str + ".json";
	std::ifstream ifs;
	utility::fio_open(ifs, path);
	glz::basic_istream_buffer buffer(ifs);
	glz::generic json{};
	auto ec = glz::read_json(json, buffer);
	ifs.close(); 
	if(!ec){
		this->entity_type_ = json["entityType"].as<std::string>();  
		this->description_ = json["sicDescription"].as<std::string>();
		this->insider_transaction_for_issuer_exists_ = json["insiderTransactionForIssuerExists"].as<bool>(); 
		this->insider_transaction_for_owner_exists_ = json["insiderTransactionForOwnerExists"].as<bool>();  
		this->name_ = json["name"].as<std::string>(); 
		this->tickers_ = *glz::get<std::vector<std::string>>(json, "/tickers");  
		this->exchanges_ = *glz::get<std::vector<std::string>>(json, "/exchanges");	
		this->website_ = json["website"].as<std::string>();  
		this->investor_website_ = json["investorWebsite"].as<std::string>();
		this->category_ = json["category"].as<std::string>(); 
		this->phone_ = json["phone"].as<std::string>(); 
		for(auto it = json["formerNames"].get_array().begin(); it != json["formerNames"].get_array().end(); it++){
			FormerName fn;
			auto ec = glz::read<glz::opts{.null_terminated = false, .error_on_unknown_keys = false,.partial_read = true}>(fn, it->dump()->c_str());
			if(!ec){
				this->former_names_.push_back(fn); 	
			}
		}
	
		auto scrape_address = [&](auto& add, const std::string& bus_or_mail){
			if(!json["addresses"][bus_or_mail].is_null()){
				add.street1 = json["addresses"][bus_or_mail]["street1"].as<std::string>(); 
			}
			if(!json["addresses"][bus_or_mail]["street2"].is_null()){
				add.street2 = json["addresses"][bus_or_mail]["street2"].as<std::string>(); 
			}
			if(!json["addresses"][bus_or_mail]["city"].is_null()){
				add.city = json["addresses"][bus_or_mail]["city"].as<std::string>(); 
			}
			if(!json["addresses"][bus_or_mail]["zipCode"].is_null()){
				add.zip_code = json["addresses"][bus_or_mail]["zipCode"].as<std::string>(); 
			}
			if(!json["addresses"][bus_or_mail]["stateOrCountry"].is_null()){
				add.state_or_country = json["addresses"][bus_or_mail]["stateOrCountry"].as<std::string>(); 
			}
			if(!json["addresses"][bus_or_mail]["isForeignLocation"].is_null()){
				add.is_foreign_location = json["addresses"][bus_or_mail]["isForeignLocation"].as<bool>(); 	
			}else{
				add.is_foreign_location = false; 
			}
		}; 
		scrape_address(this->business_address_, "business");
		scrape_address(this->mailing_address_, "mailing");
		
	}else{
		std::string error_message = glz::format_error(ec, std::string{});
		std::cout << error_message << std::endl; 
		throw std::runtime_error("CompanyInfo Constructor: Error parsing the json file."); 
	}
};
void CompanyInfo::display() const{
	auto head = std::make_tuple("Company", name_);
	auto string_vec_to_string = [](const std::vector<std::string>& string_vec){
		std::string out = ""; 
		if(string_vec.empty()){
			out = "None"; 
		}else{
			out = string_vec[0];
			for(int i = 1; i < string_vec.size(); i++){
				out += (", " + string_vec[i]); 
			}
		}
		return out; 
	};
	auto fn_to_string = [](const std::vector<FormerName>& fnv){
		std::string out = "";  
		if(fnv.empty()){
			out = "None"; 
		}else{
			out = (fnv[0].name);
			for(int i = 1; i < fnv.size(); i++){
				out += (", " + fnv[i].name); 
			}
		}
		return out; 
		
	};
	auto format_address = [](const Address& a){
		return (a.street1 + " " + a.street2 + ", " + a.city + ", " + a.state_or_country + " " + a.zip_code);
	}; 
	std::vector<std::tuple<std::string, std::string>> rows{
		std::make_tuple("Phone Number", phone_), 
		std::make_tuple("Description", description_), 
		std::make_tuple("Category", category_), 
		std::make_tuple("Website", website_), 
		std::make_tuple("Investor Website", investor_website_), 
		std::make_tuple("Has Insider Transaction for Issuer", utility::to_string(insider_transaction_for_issuer_exists_)), 
		std::make_tuple("Has Insider Transaction for Owner", utility::to_string(insider_transaction_for_owner_exists_)), 
		std::make_tuple("Tickers", string_vec_to_string(tickers_)), 
		std::make_tuple("Exchanges", string_vec_to_string(exchanges_)), 
		std::make_tuple("Former Names", fn_to_string(former_names_)), 
		std::make_tuple("Business Address", format_address(business_address_)), 
		std::make_tuple("Mailing Address", format_address(mailing_address_)), 
		std::make_tuple("Is Foreign Located", utility::to_string(business_address_.is_foreign_location))
	};
	auto s_con = [](const std::string& s){return s;};
	auto h_fcns = std::make_tuple(s_con, s_con); 
	auto row_fcns = std::make_tuple(s_con, s_con);
	std::vector<int> col_widths{45, 90};
	utility::table_print(head, h_fcns, rows, row_fcns, col_widths); 
}

Filing::Filing(const int& cik_in, const std::string& accession_number_in, const std::string& primary_document_in, const std::string dt_filed_in, const std::string dt_accepted_in, 
		const std::string form_type_in, bool is_inline_xbrl_in, bool is_xbrl_in) : 
	cik{cik_in}, accession_number{accession_number_in}, primary_document{primary_document_in}, form_type{form_type_in}, is_inline_xbrl{is_inline_xbrl_in}, is_xbrl{is_xbrl_in} 
{
	//dt_accepted is in the format YYYY-MM-DDTHH:MM:SS.000Z
	//dt_filed is in the format YYYY-MM-DD
	boost::regex re("([\\d]{4})-([\\d]{2})-([\\d]{2})T([\\d]{2}:[\\d]{2}:[\\d]{2}).[\\d]{3}Z"); 
	boost::smatch m;
	dt_accepted = Datetime(boost::regex_replace(dt_accepted_in, re, "$1.$2.$3 $4"));
	re = boost::regex("([\\d]{4})-([\\d]{2})-([\\d]{2})"); 
	dt_filed = Datetime(boost::regex_replace(dt_filed_in, re, "$1.$2.$3 00:00:00"));
}; 

//Note: Be careful to not exceed Edgar's request limit when using this function
bool Filing::has_filing_summary() const{
	std::string an_wo = boost::regex_replace(accession_number, boost::regex("-"), "");
	std::string fs_url = ("https://www.sec.gov/Archives/edgar/data/" + std::to_string(cik) + "/" + an_wo + "/FilingSummary.xml");
	utility::Curl curl_obj;
	return curl_obj.url_exists(fs_url);
}

void Filing::display() const{
	std::cout << "cik: " << cik << std::endl; 
	std::cout << "Filed Datetime: "; 
	dt_filed.display();
	std::cout << std::endl << "Accepted Datetime: "; 
	dt_accepted.display();
	std::cout << std::endl << "Form Type: " << form_type << std::endl;
	std::cout << "Accession Number: " << accession_number << std::endl;
	std::cout << "Primary Document: " << primary_document << std::endl; 
	std::cout << "Is Inline XBRL: " << is_inline_xbrl << std::endl;
	std::cout << "Is XBRL: " << is_xbrl << std::endl;
}; 


std::string FilingSummary::get_url(boost::regex re) const{
	boost::smatch m; 
	for(auto p : fs_map){
		if(boost::regex_search(p.first, m, re)){
			return p.second; 
		}
	}
	return ""; 
}
void FilingSummary::display(){
	for(auto p : fs_map){
		std::cout << "Name: " << p.first << "  Url: " << p.second << std::endl;  
	}
}
