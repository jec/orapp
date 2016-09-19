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
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "Oracle.h"
#include "Varchar.h"
#include <oci.h>


Oracle::Varchar::Varchar() throw()
	: Nullable(), vc(0), max_sz(0)
{
}


Oracle::Varchar::Varchar(const int n) throw()
	: Nullable(), vc(new char[n + 1]), max_sz(n + 1)
{
}


Oracle::Varchar::Varchar(const char* s) throw()
	: Nullable(), vc(new char[std::strlen(s) + 1]), max_sz(std::strlen(s) + 1)
{
	// copy s into new array of same size
	std::strcpy(vc, s);
	ind = 0;
}


Oracle::Varchar::Varchar(const std::string& s) throw()
	: Nullable(), vc(new char[s.length() + 1]), max_sz(s.length() + 1)
{
	// copy into new array of same size
	s.copy(vc, s.length());
	vc[s.length()] = '\0';
	ind = 0;
}


Oracle::Varchar::Varchar(const Varchar& v) throw()
	: Nullable()
{
	// if v is not null, initialize this Varchar with v's string
	if (v.ind == 0)
	{
		vc = new char[strlen(v.vc) + 1];
		strcpy(vc, v.vc);
		max_sz = strlen(v.vc) + 1;
		ind = 0;
	}
}


Oracle::Varchar::~Varchar() throw()
{
	// free any space that was allocated
	if (vc)
		delete [] vc;
	max_sz = 0;
	ind = -1;
}


inline
int
Oracle::Varchar::type() const throw()
{
	return SQLT_STR;
}


std::string
Oracle::Varchar::str() const throw(Oracle::Value_Error)
{
	if (ind == -1)
		return Nullable::str();
	return std::string(vc);
}


std::string
Oracle::Varchar::str(const std::string& s) const throw()
{
	if (ind == -1)
		return s;
	return std::string(vc);
}


std::string
Oracle::Varchar::str(const std::string& s, const std::string& f) const throw()
{
	// ignore format
	if (ind == -1)
		return s;
	return std::string(vc);
}


std::string
Oracle::Varchar::sql_str() const throw()
{
	if (ind == -1)
		return Nullable::sql_str();
	return "'" + sqlquote(vc) + "'";
}


long
Oracle::Varchar::lng() const throw(Oracle::Value_Error)
{
	if (ind == -1)
		return Nullable::lng();

	char *p;
	long l = std::strtol(vc, &p, 10);
	if(p == vc)
		throw Value_Error("Varchar::lng()", "The object does not contain a valid long");
	else
		return l;
}


long
Oracle::Varchar::lng(const long n) const throw(Oracle::Value_Error)
{
	if (ind == -1)
		return n;

	char *p;
	long l = std::strtol(vc, &p, 10);
	if(p == vc)
		throw Value_Error("Varchar::lng(const long)", "The object does not contain a valid long");
	else
		return l;
}


double
Oracle::Varchar::dbl() const throw(Oracle::Value_Error)
{
	if (ind == -1)
		return Nullable::dbl();

	char *p;
	double d = std::strtod(vc, &p);
	if(p == vc)
		throw Value_Error("Varchar::dbl()", "The object does not contain a valid double");
	else
		return d;
}


double
Oracle::Varchar::dbl(const double n) const throw(Oracle::Value_Error)
{
	if (ind == -1)
		return n;

	char *p;
	double d = std::strtod(vc, &p);
	if(p == vc)
		throw Value_Error("Varchar::dbl(const double)", "The object does not contain a valid double");
	else
		return d;

}


Oracle::Varchar&
Oracle::Varchar::operator=(const char* s) throw()
{
	if (strlen(s) >= max_sz)
	{
		if (vc)
			delete [] vc;
		vc = new char[strlen(s) + 1];
		max_sz = strlen(s) + 1;
	}

	strcpy(vc, s);
	ind = 0;
	return *this;
}


Oracle::Varchar&
Oracle::Varchar::operator=(const std::string& s) throw()
{
	this->operator=(s.c_str());
	return *this;
}


Oracle::Varchar&
Oracle::Varchar::operator=(const Oracle::Varchar& rhs) throw()
{
	this->operator=((char*)rhs.data());
	return *this;
}


Oracle::Varchar&
Oracle::Varchar::operator=(const long rhs) throw()
{
	std::ostringstream ost;
	ost << rhs;
	this->operator=(ost.str());
	return *this;
}


Oracle::Varchar&
Oracle::Varchar::operator=(const double rhs) throw()
{
	std::ostringstream ost;
	ost << rhs;
	this->operator=(ost.str());
	return *this;
}


std::ostream&
Oracle::operator<<(std::ostream& o, const Oracle::Varchar& v)
{
	if (v.is_null())
		o << "<NULL>";
	else
		o << v.str();
	return o;
}


const bool
Oracle::operator==(const Oracle::Varchar& v1, const Oracle::Varchar& v2)
{
	if (v1.is_null() || v2.is_null())
		return false;
	else if (&v1 == &v2)
		return true;
	else if (v1.str() == v2.str())
		return true;
	else
		return false;
}
