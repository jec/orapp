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

#include "Oracle.h"
#include "Nullable.h"
#include <oci.h>


Oracle::Nullable::Nullable() throw()
	: ind(-1)
{
}


std::string
Oracle::Nullable::str() const throw(Oracle::Error)
{
	throw Value_Error("Nullable::str()", "Cannot make a string out of a NULL");
}


std::string
Oracle::Nullable::str(const std::string& s) const throw()
{
	return s;
}


std::string
Oracle::Nullable::str(const std::string& s, const std::string& f) const throw()
{
	// ignore format string
	return s;
}


std::string
Oracle::Nullable::sql_str() const throw()
{
	return "NULL";
}


long
Oracle::Nullable::lng() const throw(Oracle::Error)
{
	throw Value_Error("Nullable::lng()", "Cannot make a long out of a NULL");
}


long
Oracle::Nullable::lng(const long n) const throw()
{
	return n;
}


double
Oracle::Nullable::dbl() const throw(Oracle::Error)
{
	throw Value_Error("Nullable::dbl()", "Cannot make a double out of a NULL");
}


double
Oracle::Nullable::dbl(const double d) const throw()
{
	return d;
}


int
Oracle::Nullable::type() const throw(Oracle::Error)
{
	throw Value_Error("Nullable::type", "A NULL has no type");
}
