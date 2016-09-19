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

#include <iostream>
#include <sstream>
#include <oci.h>
#include "Oracle.h"


std::string
Oracle::sqlquote(const std::string& s)
{
	std::string s2;
	for (int i=0; i < s.length(); i++)
		if (s[i] == '\'')
			s2 += "''";
		else
			s2 += s[i];
	return s2;
}


Oracle::Error::Error(const std::string& m, const std::string& s, const std::string& f, const int l)
	: module(m), msg(s), fname(f), line(l)
{
}


Oracle::Error::Error(const Error& e)
	: module(e.module), msg(e.msg), fname(e.fname), line(e.line)
{
	desc << e.desc.str();
}


Oracle::State_Error::State_Error(const std::string& m, const std::string& s, const std::string& f, const int l)
	: Error(m, s, f, l)
{
}


Oracle::Value_Error::Value_Error(const std::string& m, const std::string& s, const std::string& f, const int l)
	: Error(m, s, f, l)
{
}


Oracle::Type_Error::Type_Error(const std::string& m, const std::string& s, const std::string& f, const int l)
	: Error(m, s, f, l)
{
}


Oracle::OCI_Error::OCI_Error(const std::string& m, OCIError* error_h, const std::string& f, const int l)
	: Error(m, "", f, l)
{
	const int	max_size = 512;
	text		buf[max_size];

	OCIErrorGet((dvoid*) error_h,						// error handle
			(ub4) 1,						// record number
			(text*) NULL,						// reserved
			(sb4*) &ora_code,					// error code
			buf,							// error message
			(ub4) max_size,						// size of error msg buffer
			(ub4) OCI_HTYPE_ERROR);					// handle type

	msg = std::string((char*)buf, std::strlen((char*)buf) - 1);
}


std::string
Oracle::Error::str() const
{
	std::ostringstream os;
	os << "In " << module;
	if (fname != "" || line)
		os << " (";
	if (fname != "")
	{
		os << "file " << fname;
		if (line)
			os << ", ";
	}
	if (line)
		os << "line " << line;
	if (fname != "" || line)
		os << ")";
	os << ": " << msg;
	
	if (desc.str().length())
		os << std::endl << desc.str();
	return os.str();
}


std::ostream&
Oracle::operator<<(std::ostream& os, const Oracle::Error& e)
{
	os << e.str();
	return os;
}
