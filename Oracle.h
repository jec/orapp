//////////////////////////////////////////////////////////////////////////////
//
// Ora++ -- a C++ interface to Oracle based on the Oracle Call Interface
// Copyright (C) 2000 James Edwin Cain <me@jimcain.net>
// 
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or any
// later version.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
// General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ORAPP_ORACLE_H
#define ORAPP_ORACLE_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

class OCIError;


namespace Oracle
{
	// version
	const char version[] = "1.1.0";
	const char copyright[] = "Copyright (C) 2000-2001 James Edwin Cain <me@jimcain.net>";

	// helper functions
	std::string sqlquote(const std::string&);		// make string SQL safe

	// exceptions
	struct Error: public std::exception
	{
		// constructors
		Error(	const std::string&,			// module/function name
			const std::string&,			// error message
			const std::string& = "",		// file name
			const int = 0);				// line number
		Error(const Error&);

		// accessors
		std::string str() const;			// get formatted error message
		
		// data members
		std::string module;
		std::string msg;
		std::string fname;
		int line;
		std::ostringstream desc;			// additional info
	};


	struct State_Error: public Error
	{
		State_Error(
			const std::string&,			// module/function name
			const std::string&,			// error message
			const std::string& = "",		// file name
			const int = 0);				// line number
	};


	struct Value_Error: public Error
	{
		Value_Error(
			const std::string&,			// module/function name
			const std::string&,			// error message
			const std::string& = "",		// file name
			const int = 0);				// line number
	};


	struct Type_Error: public Error
	{
		Type_Error(
			const std::string&,			// module/function name
			const std::string&,			// error message
			const std::string& = "",		// file name
			const int = 0);				// line number
	};


	struct OCI_Error: public Error
	{
		OCI_Error(
			const std::string&,			// module/function name
			OCIError*,				// OCI error handle
			const std::string& = "",		// file name
			const int = 0);				// line number
		OCI_Error(const OCI_Error&);
		int ora_code;
	};

	std::ostream& operator<<(std::ostream&, const Oracle::Error&);
}

#endif
