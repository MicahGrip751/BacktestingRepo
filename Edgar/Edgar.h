#pragma once
#include "../Asset/ValidEquitiesBonds.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <thread>
#include <stack>
#include "nlohmann/json.hpp"
#include <glaze/glaze.hpp>
#include <glaze/json.hpp>
#include <boost/regex.h> 
#include "../Utility/utility.h"

struct Filing{
	Filing() = default; 
	Filing(const int& cik, const std::string& acession_number, const std::string& primary_document, const std::string dt_filed, 
			const std::string dt_accepted, const std::string form_type, bool is_inline_xbrl, bool is_xbrl);
	void display() const;
	bool has_filing_summary() const; 
	int cik; 
	std::string accession_number;
	std::string primary_document; 
	Datetime dt_filed;
	Datetime dt_accepted;
	std::string form_type;
	bool is_inline_xbrl;
	bool is_xbrl;
}; 

struct FilingSummary{
	//return the url of a key which is matched by the regex 
	std::string get_url(boost::regex re) const;
	void display(); 
	//keys are ShortNames and values are the url's to the file
	std::unordered_map<std::string, std::string> fs_map;
}; 

class Edgar{
	public:
		//use this Constructor when we havent downloaded the archive yet
		Edgar(const std::string& request_header, const std::string& ticker_mappings_path);
		//use this Constructor if we have downloaded the bulk submissions archive 
		Edgar(const std::string& request_header, const std::string& archive_path_, const std::string& ticker_mappings_path); 
		//downloads the bulk submissions.zip file and unzips the file. Path should be a folder. Function will set the archive_path_ data member.  
		void download_archive(const std::string& archive_path);
		//constructs a map of filing objects for a particular symbol or cik (map keys are the form type of the filing)
		void make_filing_map(const std::string& symbol, std::unordered_map<std::string, std::vector<Filing>>& filing_map); 
		void make_filing_map(const int& cik, std::unordered_map<std::string, std::vector<Filing>>& filing_map); 
		std::string make_primary_doc_url(const Filing& filing);
	
		//download the filing's primary document (returns the path to the downloaded document) 
		std::string download_primary_document(const std::string& data_folder, const Filing& filing); 
		//download a report from a filing (returns the path to the downloaded file) 
		std::string download_report(const std::string& data_folder, const int& cik, boost::regex re, const FilingSummary& fs); 
		
		std::string make_filing_summary_url(const int& cik, const std::string& accession_number); 
		//return if the filing has a filing summary
		FilingSummary parse_filing_summary(const std::string& data_folder, const Filing& filing); 

		void parse_balance_sheet(std::string& bs_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& bs_map); 
		void parse_income_statement(std::string& is_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& is_map); 	
		void parse_cash_flow_statement(std::string& cfs_table, std::unordered_map<std::string, std::unordered_map<std::string, double>>& cfs_map); 

		//get the 10-Q and 10-K filings for a cik
		void get_10Q_and_10K(const int& cik, std::vector<Filing>& vec_of_10Q_and_10K); 
		void get_10Q_and_10K(const std::string& symbol, std::vector<Filing>& vec_of_10Q_and_10K);
		//grab the latest n 10Q or 10K filings
		void get_latest_10Q_or_10K(const int& cik, std::vector<Filing>& vec_of_10Q_or_10K, int n = 1); 
		void get_latest_10Q_or_10K(const std::string& symbol, std::vector<Filing>& vec_of_10Q_or_10K, int n = 1); 
		//parse a 10Q or a 10K (path is the location of the raw text file for the report)
		//output vector elements are in the order of balance sheet, income statement and cash flow statement
		void parse_10Q_or_10K(const std::string& data_folder, const Filing& filing, std::vector<std::pair<double, double>>& mult_pairs, 
				std::vector<std::unordered_map<std::string, std::unordered_map<std::string, double>>>& vec_of_maps);

	private:
		ValidEquitiesBonds veb_;
		//sets the request_header to use for sending curl requests to SEC websites
		std::string request_header_ = ""; 
		std::string archive_path_ = ""; 	
		//path is the directory to where you want to store the files, an is the accession number of the filing you wish to get
		void curl_command_(std::string path, std::string cik, std::string an) const;
};

struct FormerName{
	std::string name; 
	std::string from; 
	std::string to; 
}; 
struct Address{
	std::string street1; 
	std::string street2;
	std::string city; 
	std::string state_or_country; 
	std::string zip_code;
	bool is_foreign_location; 
}; 
//this class allows us to use glaze to obtain the bulk data index for a symbol
class CompanyInfo{
	public: 	
		CompanyInfo(const std::string& archive_path, const int& cik);
		//prints a table containing the company information to the console
		void display() const; 
	private:
		std::string name_; 
		std::string entity_type_; 
		std::string category_; 
		std::string description_;
		std::string phone_;
		std::string website_;
		std::string investor_website_; 
		bool insider_transaction_for_issuer_exists_; 
		bool insider_transaction_for_owner_exists_; 
		std::vector<FormerName> former_names_;
		std::vector<std::string> tickers_; 
		std::vector<std::string> exchanges_;
		Address business_address_; 
		Address mailing_address_; 
};

