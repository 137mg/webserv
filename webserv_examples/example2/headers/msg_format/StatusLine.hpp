#ifndef STATUSLINE_HPP
#define STATUSLINE_HPP

#include "../webserv.hpp"
#include "iostream"

class StatusLine
{
	private:

		/* ------------------------- ATTRIBUTES ------------------------ */
	
		int		_code;		// Code indicating the response category (ex: 200 means ok)
		std::string 	_reason;	// Reason explaing the code
		std::string 	_addInfos;	// Additionnal informations if an error occured for developpment and debug


	public:

		/* ------------------------ COPLIEN FORM ----------------------- */

		StatusLine();
		StatusLine(int code, const char* reason, const std::string& addInfos);
		StatusLine(const StatusLine& c);
		~StatusLine();
		StatusLine& operator=(StatusLine a);

		/* --------------------------- SETTERS ------------------------- */

		void setCode(int code);
		void setReason(const std::string& reason);
		void setAdditionalInfo(const std::string& addInfos);


		/* --------------------------- GETTERS ------------------------- */

		int getCode() const;
		const std::string& getReason() const;
		const std::string& getAdditionalInfo() const;


		/* --------------------------- METHODS ------------------------- */

		// Clear the StatusLine object
		void clear();
		
		// Print the StatusLine object
		void print() const;

		/* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

		friend void swap(StatusLine& a, StatusLine& b);
		
};


/* -------------------- OPERATOR OVERLOADS --------------------- */

inline std::ostream& operator<<(std::ostream& stream, const StatusLine& staLine);

#endif