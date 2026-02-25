#include "Asset.h"
#include "ValidEquitiesBonds.h"

class Bond : public Asset{
	public: 
		Bond(const std::string& symbol, Datetime maturity, const std::string& clean_fn, const std::string& iso, ValidEquitiesBonds& veb); 
		int get_cik() const; 
		std::string get_entity() const;
		Datetime get_maturity() const;
		int get_isin() const; 
	private: 
		int cik_ = -1;
		int isin_ = -1; //international security identification number 
		std::string entity_ = "";
		Datetime maturity_ = Datetime(); 
		void validate_(ValidEquitiesBonds& veb);
}; 
