#include "../Utility/utility.h" 

int utility::n_days_between(Datetime dt1, Datetime dt2){
	//get the year month day objects from the datetimes
	std::chrono::year_month_day dt1_ymd; 
	std::chrono::year_month_day dt2_ymd;
	if(dt1 <= dt2){
		dt1.get_ymd(dt1_ymd);
		dt2.get_ymd(dt2_ymd);
	}else{
		dt2.get_ymd(dt1_ymd);
		dt1.get_ymd(dt2_ymd);
	}
	int n_days = 1;
	while(dt1_ymd != dt2_ymd){
		dt1_ymd = (std::chrono::sys_days(dt1_ymd) + std::chrono::days(1));
		n_days += 1; 
	}
	return n_days; 
}
std::string utility::to_string(bool bool_in){
	if(bool_in){
		return "True"; 
	}else{
		return "False"; 
	}
}

//hash uses Szudzik Function which requires the integers to be non-negative 
std::size_t utility::PairHash::operator()(const std::pair<unsigned int, unsigned int>& p) const{
	if(p.first >= p.second){
		return std::hash<int>{}(std::pow(p.first, 2) + p.first + p.second); 
	}else{
		return std::hash<int>{}(std::pow(p.second, 2) + p.first); 
	}
}
//get the output from a command sent to the console with std::system
std::string utility::command_output(const std::string& command){
	std::string result = "";
	//" 2>&1"
	FILE* pipe = popen(command.c_str(), "r");
	if(!pipe){
		throw std::runtime_error("command_output: popen() call failed"); 
	}	
	try{
		char buffer[128];
		while(fgets(buffer, sizeof(buffer), pipe) != nullptr){
			result += buffer; 
		}
	}catch (...){
		pclose(pipe);
		throw; 
	}
	pclose(pipe);
	return result; 
}

//Implementing the Curl Class

//assumes that the url is not surrounded by quotes
utility::Curl::Curl(const std::string& url, const std::string& header) : header_{header},  
	request_{"curl -H \"" + header + "\" -L -O \"" + url + "\""} {}; 

utility::Curl::Curl(const std::string& url, const std::string& header, const std::string& download_path) : header_{header}, 
	request_{
		[&](){
			//call ensure_exists on the download_path 
			utility::ensure_exists(download_path);
			return "curl -H \"" + header + "\" -L -o \"" + download_path + "\" \"" + url + "\""; 
		}()
	} {};
bool utility::Curl::url_exists(const std::string& url){
	std::string com = "if curl -H \"" + header_ + "\" --output /dev/null --silent --head --fail " + "\"" + url + "\"; then echo \"true\"; else echo \"false\"; fi";
	std::string out = utility::command_output(com);
	//std::cout << "out string: " << out << "(end_check)" << std::endl;
	if(boost::regex_search(out, boost::regex("true"))){
		return true; 
	}else{
		return false; 
	}
}

void utility::Curl::display_request(){
	std::cout << request_ << std::endl; 
}
void utility::Curl::send_request() const{
	std::system(request_.c_str()); 
}

//Functions for generating a latex document from a the body of the latex document
void utility::generate_latex_doc(std::string& body){
	std::string ltx = "\\documentclass{article}\n"; 
	ltx += "\\newcommand{\\sect}[1]{\\begin{tcolorbox}[width = \\textwidth, colback = newgray, colframe = black, boxrule = 2pt]\\large\\textbf{#1}\\end{tcolorbox}}"; 
	ltx += "\\usepackage{tcolorbox}\n"; 
	ltx += "\\usepackage{latexsym, amsfonts, amssymb, amsthm, amsmath}\n"; 
	ltx += "\\usepackage{siunitx}\n"; 
	ltx += "\\usepackage{graphicx}\n"; 
	ltx += "\\usepackage{xcolor}\n"; 
	ltx += "\\usepackage{tabu}\n"; 
	ltx += "\\setlength{\\parindent}{0in}\n"; 
	ltx += "\\setlength{\\oddsidemargin}{0in}\n"; 
	ltx += "\\setlength{\\textwidth}{6.5in}\n"; 
	ltx += "\\setlength{\\textheight}{9.5in}\n"; 
	ltx += "\\setlength{\\topmargin}{0in}\n"; 
	ltx += "\\definecolor{newgray}{HTML}{A6A6A6}\n"; 
	ltx += "\\renewcommand{\\labelitemi}{$\\circ$}\n";
	ltx += "\\begin{document}\n";
	ltx += body + "\n"; 
	ltx += "\\end{document}\n";
	body = ltx; 
}
void utility::generate_latex_doc(std::string& body, const std::string& title){
	std::string ltx = "\\documentclass{article}\n"; 
	ltx += "\\newcommand{\\sect}[1]{\\begin{tcolorbox}[width = \\textwidth, colback = newgray, colframe = black, boxrule = 2pt]\\large\\textbf{#1}\\end{tcolorbox}}"; 
	ltx += "\\usepackage{tcolorbox}\n"; 
	ltx += "\\usepackage{latexsym, amsfonts, amssymb, amsthm, amsmath}\n"; 
	ltx += "\\usepackage{siunitx}\n"; 
	ltx += "\\usepackage{graphicx}\n"; 
	ltx += "\\usepackage{xcolor}\n"; 
	ltx += "\\usepackage{tabu}\n"; 
	ltx += "\\setlength{\\parindent}{0in}\n"; 
	ltx += "\\setlength{\\oddsidemargin}{0in}\n"; 
	ltx += "\\setlength{\\textwidth}{6.5in}\n"; 
	ltx += "\\setlength{\\textheight}{9.5in}\n"; 
	ltx += "\\setlength{\\topmargin}{0in}\n"; 
	ltx += "\\definecolor{newgray}{HTML}{A6A6A6}\n"; 
	ltx += "\\renewcommand{\\labelitemi}{$\\circ$}\n";
	ltx += "\\title{" + title + "} \n";
	ltx += "\\author{Micah Grip} \n"; 
	ltx += "\\begin{document}\n";
	ltx += "\\maketitle \n"; 
	ltx += body + "\n"; 
	ltx += "\\end{document}\n";
	body = ltx; 
}
void utility::write_latex(const std::string& path, std::string& body){
	//call generate_latex_doc 
	utility::generate_latex_doc(body);
	//call fout_open
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << body;
	ofs.close(); 
}
void utility::write_latex(const std::string& path, std::string& body, const std::string& title){
	//call generate_latex_doc 
	utility::generate_latex_doc(body, title);
	//call fout_open
	std::ofstream ofs; 
	utility::fout_open(ofs, path);
	ofs << body;
	ofs.close(); 
}
//corner character only displayed on the interior cell boundaries
void utility::row_separator_print(const std::vector<int>& col_widths, char row_sep, char corner){
	for(int i = 0; i <= col_widths.front(); i++){
		std::cout << row_sep;  
	}
	std::cout << corner; 
	for(int i = 1; i < col_widths.size() - 1; i++){
		for(int j = 0; j < col_widths[i]; j++){
			std::cout << row_sep; 
		}
		std::cout << corner; 
	}
	for(int i = 0; i <= col_widths.back(); i++){
		std::cout << row_sep;  
	}
}

std::pair<int, int> utility::parse::open_tag(const std::string& input, const std::string& name, int pos, bool prev){
	std::string re_string = ("<((\\s+)?)" + name + "(((\\s+)(.*?))?)>");
	//std::string re_string_reversed = (">(((.*?)(\\s+))?)" + std::string(name.crbegin(), name.crend()) + "((\\s+)?)<");
	std::string re_string_reversed = (">((([^>]*?)(\\s+))*?)" + std::string(name.crbegin(), name.crend()) + "((\\s+)?)<");
	std::string reversed_input; 
	boost::smatch m; 
	boost::regex re;
	if(prev){
		reversed_input = std::string(input.cbegin(), std::next(input.cbegin(), pos));
		std::reverse(reversed_input.begin(), reversed_input.end());
		re = boost::regex(re_string_reversed, boost::regex::icase);
		if(boost::regex_search(reversed_input, m, re)){
			return std::make_pair(pos - m.position() - m[0].length(), pos - m.position()); 	
		}
	}else{
		auto itb = std::next(input.cbegin(), pos);
		auto ite = input.cend();
		re = boost::regex(re_string, boost::regex::icase);
		if(boost::regex_search(itb, ite, m, re)){
			return std::make_pair(pos + m.position(), pos + m.position() + m[0].length()); 	
		}
	}
	//return -1, -1 pair if no match was found
	return std::make_pair(-1, -1); 	
}
std::pair<int, int> utility::parse::open_tag(const std::string& input, int pos, bool prev){
	return std::make_pair(1, 1); 	
}
std::pair<int, int> utility::parse::closing_tag(const std::string& input, const std::string& name, int pos, bool prev){
	//std::string re_string = ("</((\\s+)?)" + name + "(((\\s+)(.*?))?)>");
	std::string re_string = ("</" + name + ">"); 
	//std::string re_string_reversed = (">(((.*?)(\\s+))?)" + std::string(name.crbegin(), name.crend()) + "((\\s+)?)/<");
	std::string re_string_reversed = (">" + std::string(name.crbegin(), name.crend()) + "/<"); 
	std::string reversed_input; 	
	boost::smatch m; 
	boost::regex re; 
	if(prev){
		reversed_input = std::string(input.cbegin(), std::next(input.cbegin(), pos));
		std::reverse(reversed_input.begin(), reversed_input.end());
		//reversed_input = std::string(reversed_input.rbegin(), reversed_input.rend());
		re = boost::regex(re_string_reversed, boost::regex::icase);
		if(boost::regex_search(reversed_input, m, re)){
			return std::make_pair(pos - m.position() - m[0].length(), pos - m.position()); 	
		}
	}else{
		auto itb = std::next(input.cbegin(), pos);
		auto ite = input.cend();
		re = boost::regex(re_string, boost::regex::icase);
		if(boost::regex_search(itb, ite, m, re)){
			return std::make_pair(pos + m.position(), pos + m.position() + m[0].length()); 	
		}
	}
	return std::make_pair(-1, -1); 	
}
std::pair<int, int> utility::parse::closing_tag(const std::string& input, int pos, bool prev){
	return std::make_pair(1, 1); 	
}

void utility::parse::remove_self_closing_tags(std::string& input){
	//allow for whitespace between < and tag name
	boost::regex re("<((\\s+)?)(area|base|br|col|embed|hr|img|input|link|meta|param|source|track|wbr){1}(>|/>)",boost::regex::icase);
	input = boost::regex_replace(input, re, "");
	re = boost::regex("<((\\s+)?)(area|base|br|col|embed|hr|img|input|link|meta|param|source|track|wbr){1}(\\s+)(.*?)(>|/>)",boost::regex::icase); 
	input = boost::regex_replace(input, re, "");
} 
void utility::parse::remove_styling_tags(std::string& input){
	//allows for whitespace between < and tag name ==> works on malformed tags with such whitespace
	//tag names are from the list of the formatting category on tutorialrepublics html5 reference (we include the depreceated tags) 
	boost::regex re(
 "(<|</)((\\s+)?)(abbr|acronym|address|b|bdi|bdo|big|blink|blockquote|center|cite|code|data|del|dfn|em|font|i|ins|kbd|mark|output|pre|progress|q|rp|rt|ruby|s|samp|small|strike|strong|sub|sup|tt|var|u|wbr){1}(>|/>)",
	boost::regex::icase);
	input = boost::regex_replace(input, re, "");
	re = boost::regex(
 "(<|</)((\\s+)?)(abbr|acronym|address|b|bdi|bdo|big|blink|blockquote|center|cite|code|data|del|dfn|em|font|i|ins|kbd|mark|output|pre|progress|q|rp|rt|ruby|s|samp|small|strike|strong|sub|sup|tt|var|u|wbr){1}((\\s+)(.*?))(>|/>)",
			boost::regex::icase); 	
	input = boost::regex_replace(input, re, "");
}
void utility::parse::remove_tags(std::string& input, const std::string& name){
	boost::regex re_open("<((\\s+)?)" + name + "(((\\s+)(.*?))?)>", boost::regex::icase);
	boost::regex re_close("</" + name + ">", boost::regex::icase);
	input = boost::regex_replace(input, re_open, ""); 
	input = boost::regex_replace(input, re_close, ""); 
}
void utility::parse::remove_attributes(std::string& input){
	boost::regex re("((/?)<\\w+)\\s+(.*?)(>)"); 
	input = boost::regex_replace(input, re, "$1>"); 	
}
void utility::parse::remove_empty_tds(std::string& input){
	input = boost::regex_replace(input, boost::regex("<td>((&nbsp;|\\s+)?)</td>", boost::regex::icase), ""); 	
	//input = boost::regex_replace(input, boost::regex("<td></td>", boost::regex::icase), ""); 	
}
void utility::parse::remove_non_table_tags(std::string& input){
	boost::regex ntt_re("(<|</)((\\s+)?)(\\b(?!table|tr|td|th)\\b)\\w+(((\\s+)(.*?))?)>", boost::regex::icase);
	input = boost::regex_replace(input, ntt_re, ""); 
}
void utility::parse::fix_parentheses(std::string& input){
	input = boost::regex_replace(input, boost::regex("</td>\n<td>\\)(</td>)", boost::regex::icase), ")$1"); 	
}
void utility::parse::extract_balance_sheet_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers){
	std::string th = "th";
	auto th_cstr = th.c_str(); 
	auto headers = doc.select_nodes("//th | //TH");
	if(headers.size() != 0){
		title = headers[0].node().text().as_string();
		col_headers.push_back(headers[1].node().text().as_string()); 
		col_headers.push_back(headers[2].node().text().as_string()); 
	}else{
		//if there are no headers we assume that the headers are in the first two table data entries 
		auto td_nodes = doc.select_nodes("//td | //TD");
		col_headers.push_back(td_nodes[0].node().text().as_string()); 
		col_headers.push_back(td_nodes[1].node().text().as_string());
		td_nodes[0].node().set_name(th_cstr); 
		td_nodes[1].node().set_name(th_cstr); 
	}
	for(auto &h : col_headers){
		h = utility::parse::format_column_header(h); 
	}
}
void utility::parse::extract_income_statement_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers){
	std::string th = "th";
	auto th_cstr = th.c_str(); 
	auto headers = doc.select_nodes("//th | //TH");
	//get the table dimensions ==> helps us figure out if there are 2 columns with 3 headers or 4 columns with 6 headers
	if(headers.size() != 0){
		title = headers[0].node().text().as_string();
		if(headers.size() == 4){
			for(int i = 2; i < 4; i++){
				std::string head = headers[1].node().text().as_string(); 
				head +=  " "; 
				head += headers[i].node().text().as_string();  
				col_headers.push_back(head); 
			}
		}else if(headers.size() == 7){
			for(int i = 3; i < 5; i++){
				std::string head = headers[1].node().text().as_string(); 
				head +=  " "; 
				head += headers[i].node().text().as_string();  
				col_headers.push_back(head); 
			}
			for(int i = 5; i < 7; i++){
				std::string head = headers[2].node().text().as_string(); 
				head +=  " "; 
				head += headers[i].node().text().as_string();  
				col_headers.push_back(head); 
			}
		}
	}else{
		//if there are no headers we assume that the headers are in the first two table data entries 
		auto td_nodes = doc.select_nodes("//td | //TD");
		auto dims = utility::parse::table_dimensions(doc);
		if(dims.second == 2){
			for(int i = 1; i < 3; i++){
				std::string head = td_nodes[0].node().text().as_string(); 
				head +=  " "; 
				head += td_nodes[i].node().text().as_string();  
				col_headers.push_back(head);
				td_nodes[i].node().set_name(th_cstr); 
			}
			td_nodes[0].node().set_name(th_cstr);
		}else if(dims.second == 4){
			for(int i = 2; i < 4; i++){
				std::string head = td_nodes[0].node().text().as_string(); 
				head +=  " "; 
				head += td_nodes[i].node().text().as_string();  
				col_headers.push_back(head);
				td_nodes[i].node().set_name(th_cstr); 
			}
			for(int i = 4; i < 6; i++){
				std::string head = td_nodes[1].node().text().as_string(); 
				head +=  " "; 
				head += td_nodes[i].node().text().as_string();  
				col_headers.push_back(head); 
				td_nodes[i].node().set_name(th_cstr); 
			}
			td_nodes[0].node().set_name(th_cstr);
			td_nodes[1].node().set_name(th_cstr);
		}
	}
	for(auto &h : col_headers){
		h = utility::parse::format_column_header(h); 
	}
}
void utility::parse::extract_cash_flow_statement_headers(pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers){
	std::string th = "th"; 	
	auto th_cstr = th.c_str(); 
	auto headers = doc.select_nodes("//th | //TH");
	if(headers.size() != 0){
		title = headers[0].node().text().as_string();
		for(int i = 2; i < headers.size(); i++){
			std::string head = headers[1].node().text().as_string(); 
			head +=  " "; 
			head += headers[i].node().text().as_string();  
			col_headers.push_back(head); 
		}
	}else{
		//if there are no headers we assume that the headers are in the first two table data entries 
		auto td_nodes = doc.select_nodes("//td | //TD");
		for(int i = 1; i < 3; i++){
			std::string head = td_nodes[0].node().text().as_string();
			head +=  " "; 
			head += td_nodes[i].node().text().as_string(); 
			td_nodes[i].node().set_name(th_cstr); 
			col_headers.push_back(head); 
		}
		td_nodes[0].node().set_name(th_cstr);
	}
	for(auto &h : col_headers){
		h = utility::parse::format_column_header(h); 
	}
}
std::string utility::parse::format_column_header(const std::string& column_header){
	std::string formatted_col_header = boost::regex_replace(column_header, boost::regex("(&nbsp;)+"), " ");
	formatted_col_header = boost::regex_replace(formatted_col_header, boost::regex("\n"), " ");
	formatted_col_header = boost::regex_replace(formatted_col_header, boost::regex("\\s+"), " ");
	return formatted_col_header; 
}
std::string utility::parse::format_row_header(const std::string& row_header){
	std::string formatted_row_header = boost::regex_replace(row_header, boost::regex("(&nbsp;)+"), " ");
	formatted_row_header = boost::regex_replace(formatted_row_header, boost::regex("\n"), " ");
	formatted_row_header = boost::regex_replace(formatted_row_header, boost::regex("\\s+"), " ");
	formatted_row_header = boost::regex_replace(formatted_row_header, boost::regex("([A-Z])"), "\\L$1"); 
	return formatted_row_header; 
}
std::pair<int, int> utility::parse::find_enclosing(const std::string& input, boost::regex e, const std::string& name){
	//call boost::regex_search for the input expression
	boost::smatch m; 
	if(boost::regex_search(input, m, e)){
		int pos1 = m.position();
		int pos2 = pos1; 
		std::stack<bool> stk1;
		std::stack<bool> stk2;
		//push an element onto the stack to represent the opposite bracket type on the other side of position
		stk1.push(true); 	
		stk2.push(true); 	
		while(!stk1.empty()){
			//find the prior open and prior close
			auto p_open = utility::parse::open_tag(input, name, pos1, true);
			auto p_close = utility::parse::closing_tag(input, name, pos1, true);
			if((p_open.first != -1) && (p_close.first != -1)){
				if(p_open.first < p_close.first){
					stk1.push(true);
					pos1 = p_close.first; 
				}else{
					stk1.pop();
					pos1 = p_open.first; 
				}
			}else if((p_open.first != -1) && (p_close.first == -1)){
				stk1.pop(); 
				pos1 = p_open.first; 
			}else{
				//this else condition handles all cases where we did not find another opening tag ==> not finding an open tag means we have unclosed tags
				while(!stk1.empty()){
					stk1.pop(); 
				}
				pos1 = -1; 
			}
		}
		while(!stk2.empty()){
			auto n_open = utility::parse::open_tag(input, name, pos2, false);
			auto n_close = utility::parse::closing_tag(input, name, pos2, false);
			if((n_open.first != -1) && (n_close.first != -1)){
				if(n_open.second < n_close.second){
					stk2.push(true);
					pos2 = n_open.second; 
				}else{
					stk2.pop();
					pos2 = n_close.second;
				}
			}else if((n_open.first == -1) && (n_close.first != -1)){
				//we found a closing tag but not an opening tag
				stk2.pop(); 
				pos2 = n_close.second; 
			}else{
				//this else condition handles all cases where we did not find another closing tag ==> not finding an closing tag means we have unclosed tags
				while(!stk2.empty()){
					stk2.pop(); 
				}
				pos2 = -1; 
			}
		}
		return std::make_pair(pos1, pos2); 	
	}else{
		throw std::runtime_error("find_enclosing: The regular expression e was not matched in the input string."); 
	}
	return std::make_pair(1, 1); 	
}
std::pair<int, int> utility::parse::find_enclosing_lazy(const std::string& input, boost::regex re, const std::string& name){
	boost::smatch m;
	auto p = std::make_pair(-1, -1); 
	if(boost::regex_search(input, m, re)){
		int pos = m.position();  
		std::string reversed_name = name;
		std::reverse(reversed_name.begin(), reversed_name.end()); 
		boost::regex open_re("<" + name, boost::regex::icase);
		boost::regex close_re_reversed(">/" + reversed_name, boost::regex::icase);
		if(boost::regex_search(std::next(input.cbegin(), pos), input.cend(), m, open_re)){
			p.second = (m.position() + m.length()); 
		}
		std::string reversed_input = std::string(input.cbegin(), std::next(input.cbegin(), pos));
		std::reverse(reversed_input.begin(), reversed_input.end());
		if(boost::regex_search(reversed_input, m,close_re_reversed)){
			p.first = pos - m.position() - m[0].length(); 
		}
	}else{
		throw std::runtime_error("find_enclosing_lazy: The regular expression re was not matched in the input string."); 
	}
	return p; 
}
std::pair<int, int> utility::parse::find_enclosed_after(const std::string& input, boost::regex re, const std::string& name){
	boost::smatch m;
	auto p = std::make_pair(-1, -1); 
	if(boost::regex_search(input, m, re)){
		boost::regex open_re("<" + name, boost::regex::icase);
		boost::regex close_re("/" + name + ">", boost::regex::icase);
		boost::smatch m1;
		boost::smatch m2; 
		std::string after = m.suffix(); 
		if(boost::regex_search(after, m1, open_re)){
			p.first = (m.position() + m1.position()); 	
		}
		if(boost::regex_search(after, m2, close_re)){
			p.second = (m.position() + m2.position() + m2.length()); 	
		}
	}else{
		throw std::runtime_error("find_enclosed_after: regular expression re was not found in the input string"); 
	}
	return p; 
}

std::pair<int, int> utility::parse::find_balance_sheet(const std::string& input, std::pair<double, double>& multipliers, bool has_title){
	if(has_title){
		std::string bs_re_str = "((consolidated\\s+)?balance(\\s+)(sheet)(s?))(?!(parenthetical))";
		bs_re_str += "| ((consolidated\\s+)?statement(s?)(\\s+)of(\\s+)financial(\\s+)(position))(?!(parenthetical))"; 
		boost::regex bs_re(bs_re_str, boost::regex::icase);
		multipliers = std::make_pair(utility::parse::find_currency_multiple(input), utility::parse::find_share_multiple(input)); 
		return utility::parse::find_enclosing(input, bs_re, "table");
	}else{
		boost::regex bs_re("((CONSOLIDATED)?(\\s+)BALANCE(\\s+)(SHEET)(S?))(?!(PARENTHETICAL)) | ((CONSOLIDATED)?(\\s+)STATEMENT(S?)(\\s+)OF(\\s+)FINANCIAL(\\s+)POSITION)(?!(PARENTHETICAL))");
		//find the enclosing table containing a match for total current assets after the title match
		boost::regex tca_re("(total\\s+)?current(\\s+)asset", boost::regex::icase); 
		boost::smatch m;
		if(boost::regex_search(input, m, bs_re)){
			auto p = utility::parse::find_enclosing(m.suffix().str(), tca_re, "table");
			//find the currency and share multipliers
			std::string s = m.suffix().str().substr(0, p.first); 
			multipliers = std::make_pair(utility::parse::find_currency_multiple(s), utility::parse::find_share_multiple(s)); 
			return std::make_pair(m.position() + m.length() + p.first, m.position() + m.length() + p.second); 
		}
	}
	return std::make_pair(-1, -1); 
}
//finding an income statement table 
std::pair<int, int> utility::parse::find_income_statement(const std::string& input, std::pair<double, double>& multipliers, bool has_title){
	if(has_title){
		boost::regex is_re("(consolidated\\s+)?statement(s?)(\\s+)of(\\s+)(comprehensive\\s+)?income", boost::regex::icase);
		multipliers = std::make_pair(utility::parse::find_currency_multiple(input), utility::parse::find_share_multiple(input)); 
		return utility::parse::find_enclosing(input, is_re, "table"); 	
	}else{
		boost::regex is_re("(CONSOLIDATED\\s+)?(STATEMENT)(S?)(\\s+)(OF)(\\s+)(COMPREHENSIVE)?(INCOME)");
		boost::regex rev_re("(revenue) | (comprehensive income)", boost::regex::icase); 
		boost::smatch m;
		if(boost::regex_search(input, m, is_re)){
			auto p = utility::parse::find_enclosing(m.suffix().str(), rev_re, "table");
			std::string s = m.suffix().str().substr(0, p.first); 
			multipliers = std::make_pair(utility::parse::find_currency_multiple(s), utility::parse::find_share_multiple(s)); 
			return std::make_pair(m.position() + m.length() + p.first, m.position() + m.length() + p.second); 
		}
	}	
	return std::make_pair(-1, -1); 
}
//finding a cash flow statement 
std::pair<int, int> utility::parse::find_cash_flow_statement(const std::string& input, std::pair<double, double>& multipliers, bool has_title){
	if(has_title){
		boost::regex cfs_re("(consolidated\\s+)?(statement)(s?)(\\s+)(of)(\\s+)(cash)(\\s+)(flow)(s?)", boost::regex::icase);
		multipliers = std::make_pair(utility::parse::find_currency_multiple(input), utility::parse::find_share_multiple(input)); 
		return utility::parse::find_enclosing(input, cfs_re, "table"); 	
	}else{
		boost::regex cfs_re("(CONSOLIDATED\\s+)?(STATEMENT)(S?)(\\s+)(OF)(\\s+)(CASH)(\\s+)(FLOW)(S?)");
		//find the enclosing table containing a match for net income after the title match 
		boost::regex ni_re("net(\\s+)income", boost::regex::icase); 
		boost::smatch m;
		if(boost::regex_search(input, m, cfs_re)){
			auto p = utility::parse::find_enclosing(m.suffix().str(), ni_re, "table");
			std::string s = m.suffix().str().substr(0, p.first); 
			multipliers = std::make_pair(utility::parse::find_currency_multiple(s), utility::parse::find_share_multiple(s)); 
			return std::make_pair(m.position() + m.length() + p.first, m.position() + m.length() + p.second); 
		}
	}
	return std::make_pair(-1, -1); 
}

void utility::parse::parse_table(const std::string& table_input, std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table){
	//call remove_self_closing and remove_styling on the table input 
	std::string table_in = table_input;
	utility::parse::remove_self_closing_tags(table_in);
	utility::parse::remove_styling_tags(table_in);

	//parse the table using pugixml
//	pugi::xml_document doc;
//	pugi::xml_parse_result res = doc.load_string(table_in.c_str());
//	//find the <table> node 
//	pugi::xml_node table = doc.child("table");
//	if(res){
//		auto rows = table.children("tr"); 
//		for(auto row : rows){
//			auto headers = row.children("th"); 
//			for(auto header : headers){
//				std::cout << header.text() << " "; 
//			}
//			//table data elements
//			auto cells = row.children("td");
//			for(auto cell : cells){
//				std::cout << cell.text() << "  "; 
//			}
//			std::cout << std::endl; 
//		}
//	}else{
//		std::cout << "there was a parsing error" << std::endl; 
//	}
}
//Note: this excludes the row & column headers
std::pair<int, int> utility::parse::table_dimensions(const pugi::xml_document& doc){
	//assumes the table is square ==> just uses the first row to find the number of dimensions
	auto table_rows = doc.select_nodes("//tr[child::td] | //TR[child::TD]");
	auto first_row = table_rows.first().node();  
	int n_cols = 0;
	for(pugi::xml_node cell = first_row.first_child().next_sibling(); cell; cell = cell.next_sibling()){
		n_cols++;  
	}
	return std::make_pair(table_rows.size(), n_cols); 	
}
void utility::parse::construct_headers(const pugi::xml_document& doc, std::string& title, std::vector<std::string>& col_headers){
	auto dims = utility::parse::table_dimensions(doc);
	auto headers = doc.select_nodes("//th");
	std::vector<std::string> headers_vec; 
	for(auto h : headers){
		h.node().print(std::cout); 
		headers_vec.push_back(h.node().last_child().text().as_string()); 
	}
	//std::cout << "n cols" << dims.second << std::endl;
	//std::cout << "headers_vec size: " << headers_vec.size() << std::endl; 
	title = headers_vec[0]; 
	if(headers_vec.size() == (dims.second + 1)){
		col_headers = std::vector<std::string>(std::next(headers_vec.begin()), headers_vec.end()); 
	}else if(headers_vec.size() == (dims.second + 2)){
		for(int i = 2; i < headers_vec.size(); i++){
			col_headers.push_back(headers_vec[1] + " " + headers_vec[i]); 
		}
	}else if(headers_vec.size() == (dims.second + 3)){
		for(int i = 3; i < 5; i++){
			col_headers.push_back(headers_vec[1] + " " + headers_vec[i]); 
		}
		for(int i = 5; i < headers_vec.size(); i++){
			col_headers.push_back(headers_vec[2] + " " + headers_vec[i]); 
		}
	}
	//std::cout << "displaying headers" << std::endl;
	for(auto &h : col_headers){
		h = utility::parse::format_column_header(h); 
	}
}
void utility::parse::clean_table(std::string& table_input){
	utility::parse::remove_non_table_tags(table_input);
	utility::parse::remove_attributes(table_input); 
	utility::parse::remove_empty_tds(table_input); 
	utility::parse::remove_styling_tags(table_input); 
	utility::parse::remove_self_closing_tags(table_input); 
	utility::parse::fix_parentheses(table_input);
}

void utility::parse::parse_table_data(const pugi::xml_document& doc, const std::string& first_col_header, const std::string& second_col_header, 
		std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table){
	auto table_rows = doc.select_nodes("//tr[child::td]"); 
	for(pugi::xpath_node r : table_rows){
		std::string row_header = r.node().text().as_string();
		row_header = utility::parse::format_row_header(row_header); 
		bool first_col = true;  	
		for(pugi::xml_node cell = r.node().first_child().next_sibling(); cell; cell = cell.next_sibling()){
			std::string cell_text = cell.text().as_string(); 
			if(!boost::regex_match(cell_text, boost::regex("(\\D+)")) || cell_text != ""){
				if(first_col){
					first_col = false;
					parsed_table[first_col_header][row_header] = utility::parse::convert_cell(cell_text);
				}else{
					parsed_table[second_col_header][row_header] = utility::parse::convert_cell(cell_text); 
				}
			}
		}
	}
}
void utility::parse::parse_table_data(const pugi::xml_document& doc, const std::vector<std::string>& col_headers, std::unordered_map<std::string, std::unordered_map<std::string, double>>& parsed_table){
	auto table_rows = doc.select_nodes("//tr[child::td] | //TR[child::TD]"); 
	for(pugi::xpath_node r : table_rows){
		std::string row_header = r.node().first_child().text().as_string();
		row_header = utility::parse::format_row_header(row_header); 
		int i = 0; 
		for(pugi::xml_node cell = r.node().first_child().next_sibling(); cell; cell = cell.next_sibling()){
			std::string cell_text = cell.text().as_string();  
			if(!boost::regex_match(cell_text, boost::regex("(\\D+)")) && cell_text != ""){
				parsed_table[col_headers[i]][row_header] = utility::parse::convert_cell(cell_text); 
			}
			i++; 
		}
	}
}
double utility::parse::find_currency_multiple(const std::string& title){
	boost::regex re_bill("(?<!shares)(\\s+)?in(\\s+)billions", boost::regex::icase); 
	boost::regex re_mill("(?<!shares)(\\s+)?in(\\s+)millions", boost::regex::icase); 
	boost::regex re_thou("(?<!shares)(\\s+)?in(\\s+)thousands", boost::regex::icase); 
	boost::regex re_hund("(?<!shares)(\\s+)?in(\\s+)hundreds", boost::regex::icase);
	boost::smatch m; 
	if(boost::regex_search(title, m, re_bill)){
		return std::pow(10, 9); 
	}else if(boost::regex_search(title, m, re_mill)){
		return std::pow(10, 7); 
	}else if(boost::regex_search(title, m, re_thou)){
		return std::pow(10, 4); 
	}else if(boost::regex_search(title, m, re_hund)){
		return std::pow(10, 3); 
	}else{
		return 1.0; 
	}
}
double utility::parse::find_share_multiple(const std::string& title){
	boost::regex re_bill("shares(\\s+)in(\\s+)billions", boost::regex::icase); 
	boost::regex re_mill("shares(\\s+)in(\\s+)millions", boost::regex::icase); 
	boost::regex re_thou("shares(\\s+)in(\\s+)thousands", boost::regex::icase); 
	boost::regex re_hund("shares(\\s+)in(\\s+)hundreds", boost::regex::icase);
	boost::smatch m; 
	if(boost::regex_search(title, m, re_bill)){
		return std::pow(10, 9); 
	}else if(boost::regex_search(title, m, re_mill)){
		return std::pow(10, 7); 
	}else if(boost::regex_search(title, m, re_thou)){
		return std::pow(10, 4); 
	}else if(boost::regex_search(title, m, re_hund)){
		return std::pow(10, 3); 
	}else{
		return 1.0; 
	}
}
double utility::parse::convert_cell(const std::string& cell){
	std::string formatted_cell = boost::regex_replace(cell, boost::regex("[$,\\s]"), "");
	boost::smatch m;  
	formatted_cell = boost::regex_replace(formatted_cell, boost::regex("(\\d)?&nbsp;"), ""); 
	if(boost::regex_match(formatted_cell, m,  boost::regex("\\(([\\d.]+)\\)"))){
		return (-1*std::stod(std::next(m.begin(), 1)->str()));
	}
	return std::stod(formatted_cell); 
}
void utility::sync_iterators(std::pair<std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>, 
		std::pair<std::vector<CandlePtr>::const_iterator, std::vector<CandlePtr>::const_iterator>>& csitpp){
	Datetime lf_dt; 
	Datetime fl_dt;
	if(*(csitpp.first.first->dt()) > *(csitpp.second.first->dt())){
		lf_dt = *((csitpp.first.first->dt()));  
		for(auto it = csitpp.second.first; it != csitpp.second.second; it++){
			if(*(it->dt()) == lf_dt){
				csitpp.second.first = it; 
			}
		}
	}else{
		lf_dt = *((csitpp.second.first->dt()));  
		for(auto it = csitpp.first.first; it != csitpp.first.second; it++){
			if(*(it->dt()) == lf_dt){
				csitpp.first.first = it; 
			}
		}
	}
	if(*(csitpp.first.second->dt()) < *(csitpp.second.second->dt())){
		fl_dt = *((csitpp.first.second->dt()));  
		for(auto it = csitpp.second.second; it != csitpp.second.first; it--){
			if(*(it->dt()) == fl_dt){
				csitpp.second.second = it; 
			}
		}
	}else{
		fl_dt = *((csitpp.second.second->dt()));  
		for(auto it = csitpp.first.second; it != csitpp.first.first; it--){
			if(*(it->dt()) == fl_dt){
				csitpp.first.second = it; 
			}
		}
	}
}
void utility::sync_iterators(std::pair<std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>, 
			std::pair<std::vector<Candle>::const_iterator, std::vector<Candle>::const_iterator>>& csitpp){
	Datetime lf_dt; 
	Datetime fl_dt;
	if(csitpp.first.first->dt() > csitpp.second.first->dt()){
		lf_dt = (csitpp.first.first->dt());  
		for(auto it = csitpp.second.first; it != csitpp.second.second; it++){
			if(it->dt() == lf_dt){
				csitpp.second.first = it; 
			}
		}
	}else{
		lf_dt = (csitpp.second.first->dt());  
		for(auto it = csitpp.first.first; it != csitpp.first.second; it++){
			if(it->dt() == lf_dt){
				csitpp.first.first = it; 
			}
		}
	}
	if(csitpp.first.second->dt() < csitpp.second.second->dt()){
		fl_dt = (csitpp.first.second->dt());  
		for(auto it = csitpp.second.second; it != csitpp.second.first; it--){
			if(it->dt() == fl_dt){
				csitpp.second.second = it; 
			}
		}
	}else{
		fl_dt = (csitpp.second.second->dt());  
		for(auto it = csitpp.first.second; it != csitpp.first.first; it--){
			if(it->dt() == fl_dt){
				csitpp.first.second = it; 
			}
		}
	}
}

