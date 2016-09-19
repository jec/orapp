//////////////////////////////////////////////////////////////////////////////
//
// Ora++ -- a C++ interface to Oracle based on the Oracle Call Interface
// Copyright (C) 2000-1 James Edwin Cain <me@jimcain.net>
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
#include "Number.h"
#include <oci.h>

#define ORAPP_MAX_NUM_LEN 100

const std::string Oracle::Number::default_fmt = "TM";

Oracle::Env Oracle::Number::env;

Oracle::Number::Number() throw()
	: Nullable(), num(new OCINumber)
{
	OCINumberSetZero(
			env.err(),						// error handle
			num);							// OCINumber
}


Oracle::Number::Number(const int n) throw (Oracle::Error)
	: Nullable(), num(new OCINumber)
{
	if (OCINumberFromInt(
			env.err(),						// error handle
			(CONST dvoid*) &n,					// input integer
			(uword) sizeof(int),					// input integer size
			OCI_NUMBER_SIGNED,					// signed/unsigned
			num))							// OCINumber
		throw OCI_Error("Number::Number(const int)", env.err());
	ind = 0;
}


Oracle::Number::Number(const long n) throw (Oracle::Error)
	: Nullable(), num(new OCINumber)
{
	if (OCINumberFromInt(
			env.err(),						// error handle
			(CONST dvoid*) &n,					// input integer
			(uword) sizeof(long),					// input integer size
			OCI_NUMBER_SIGNED,					// signed/unsigned
			num))							// OCINumber
		throw OCI_Error("Number::Number(const long)", env.err());
	ind = 0;
}


Oracle::Number::Number(const double n) throw (Oracle::Error)
	: Nullable(), num(new OCINumber)
{
	if (OCINumberFromReal(
			env.err(),						// error handle
			(CONST dvoid*) &n,					// input integer
			(uword) sizeof(double),					// input integer size
			num))							// OCINumber
		throw OCI_Error("Number::Number(const double)", env.err());
	ind = 0;
}


Oracle::Number::Number(const Oracle::Number& n) throw (Oracle::Error)
	: Nullable(), num(new OCINumber)
{
	// if n is not null, initialize this Number with n's value;
	// else initialize OCINumber to zero
	if ((ind = n.ind) == 0)
	{
		if (OCINumberAssign(
				env.err(),					// error handle
				n.num,						// from OCINumber
				num))						// to OCINumber
			throw OCI_Error("Number::Number(const Number&)", env.err());
	}
	else
		OCINumberSetZero(
				env.err(),					// error handle
				num);						// OCINumber
}


Oracle::Number::~Number() throw()
{
	delete num;
	ind = -1;
}


int
Oracle::Number::sqlt() const throw()
{
	return SQLT_VNU;
}


int
Oracle::Number::maxsize() const throw()
{
	return sizeof(OCINumber);
}


std::string
Oracle::Number::str() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::str();
	char buf[ORAPP_MAX_NUM_LEN];
	ub4 buflen(ORAPP_MAX_NUM_LEN);
	if (OCINumberToText(
			env.err(),						// error handle
			num,							// input OCINumber
			(CONST text*) default_fmt.c_str(),			// format string
			(ub4) default_fmt.length(),				// format string length
			(CONST text*) 0,					// NLS parameters (default)
			(ub4) 0,						// NLS parameters length
			&buflen,						// output buffer length			
			(CONST text*) buf))					// output buffer
		throw OCI_Error("Number::str()", env.err());
	return std::string(buf);
}


std::string
Oracle::Number::str(const std::string& s) const throw(Oracle::Error)
{
	if (ind == -1)
		return s;
	char buf[ORAPP_MAX_NUM_LEN];
	ub4 buflen(ORAPP_MAX_NUM_LEN);
	if (OCINumberToText(
			env.err(),						// error handle
			num,							// input OCINumber
			(CONST text*) default_fmt.c_str(),			// format string
			(ub4) default_fmt.length(),				// format string length
			(CONST text*) 0,					// NLS parameters (default)
			(ub4) 0,						// NLS parameters length
			&buflen,						// output buffer length			
			(CONST text*) buf))					// output buffer
		throw OCI_Error("Number::str(const std::string&)", env.err());
	return std::string(buf);
}


std::string
Oracle::Number::str(const std::string& s, const std::string& f) const throw(Oracle::Error)
{
	if (ind == -1)
		return s;
	char buf[ORAPP_MAX_NUM_LEN];
	ub4 buflen(ORAPP_MAX_NUM_LEN);
	if (OCINumberToText(
			env.err(),						// error handle
			num,							// input OCINumber
			(CONST text*) f.c_str(),				// format string
			(ub4) f.length(),					// format string length
			(CONST text*) 0,					// NLS parameters (default)
			(ub4) 0,						// NLS parameters length
			&buflen,						// output buffer length			
			(CONST text*) buf))					// output buffer
		throw OCI_Error("Number::str(const std::string&, const std::string&)", env.err());
	return std::string(buf);
}


std::string
Oracle::Number::sql_str() const throw()
{
	if (ind == -1)
		return Nullable::sql_str();
	return str();
}


long
Oracle::Number::lng() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::lng();
	long i;
	if (OCINumberToInt(
			env.err(),						// error handle
			num,							// input OCINumber
			(uword) sizeof(long),					// desired output size
			(uword) OCI_NUMBER_SIGNED,				// signed/unsigned result
			(dvoid*) &i))						// result
		throw OCI_Error("Number::lng()", env.err());
	return i;
}


long
Oracle::Number::lng(const long n) const throw(Oracle::Error)
{
	if (ind == -1)
		return n;
	long i;
	if (OCINumberToInt(
			env.err(),						// error handle
			num,							// input OCINumber
			(uword) sizeof(long),					// desired output size
			(uword) OCI_NUMBER_SIGNED,				// signed/unsigned result
			(dvoid*) &i))						// result
		throw OCI_Error("Number::lng(const long)", env.err());
	return i;
}


double
Oracle::Number::dbl() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::dbl();
	double d;
	if (OCINumberToReal(
			env.err(),						// error handle
			num,							// input OCINumber
			(uword) sizeof(double),					// desired output size
			(dvoid*) &d))						// result
		throw OCI_Error("Number::dbl()", env.err());
	return d;
}


double
Oracle::Number::dbl(const double n) const throw(Oracle::Error)
{
	if (ind == -1)
		return n;
	double d;
	if (OCINumberToReal(
			env.err(),						// error handle
			num,							// input OCINumber
			(uword) sizeof(double),					// desired output size
			(dvoid*) &d))						// result
		throw OCI_Error("Number::dbl(const double)", env.err());
	return d;
}


Oracle::Number&
Oracle::Number::operator=(const int rhs) throw(Oracle::Error)
{
	if (OCINumberFromInt(
			env.err(),						// error handle
			(CONST dvoid*) &rhs,					// input integer
			(uword) sizeof(int),					// input integer size
			OCI_NUMBER_SIGNED,					// signed/unsigned
			num))							// OCINumber
		throw OCI_Error("Number::operator=(const int)", env.err());
	ind = 0;
	return *this;
}


Oracle::Number&
Oracle::Number::operator=(const long rhs) throw(Oracle::Error)
{
	if (OCINumberFromInt(
			env.err(),						// error handle
			(CONST dvoid*) &rhs,					// input integer
			(uword) sizeof(long),					// input integer size
			OCI_NUMBER_SIGNED,					// signed/unsigned
			num))							// OCINumber
		throw OCI_Error("Number::operator=(const long)", env.err());
	ind = 0;
	return *this;
}


Oracle::Number&
Oracle::Number::operator=(const double rhs) throw(Oracle::Error)
{
	if (OCINumberFromReal(
			env.err(),						// error handle
			(CONST dvoid*) &rhs,					// input integer
			(uword) sizeof(double),					// input integer size
			num))							// OCINumber
		throw OCI_Error("Number::operator=(const double)", env.err());
	ind = 0;
	return *this;
}


Oracle::Number&
Oracle::Number::operator=(const Oracle::Number& rhs) throw(Oracle::Error)
{
	if (&rhs != this)
		if ((ind = rhs.ind) == 0)
			if (OCINumberAssign(
					env.err(),				// error handle
					rhs.num,				// from OCINumber
					num))					// to OCINumber
				throw OCI_Error("Number::operator=(const Number&)", env.err());
	return *this;
}


Oracle::Number&
Oracle::Number::operator+=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == 0 && (ind = n.ind) == 0)
		if (OCINumberAdd(
				env.err(),					// error handle
				num,						// first OCINumber
				n.num,						// second OCINumber
				num))						// sum
			throw OCI_Error("Number::operator+=(const Number&)", env.err());
	return *this;
}


Oracle::Number&
Oracle::Number::operator-=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == 0 && (ind = n.ind) == 0)
		if (OCINumberSub(
				env.err(),					// error handle
				num,						// first OCINumber
				n.num,						// second OCINumber
				num))						// first - second
			throw OCI_Error("Number::operator-=(const Number&)", env.err());
	return *this;
}


Oracle::Number&
Oracle::Number::operator*=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == 0 && (ind = n.ind) == 0)
		if (OCINumberMul(
				env.err(),					// error handle
				num,						// first OCINumber
				n.num,						// second OCINumber
				num))						// product
			throw OCI_Error("Number::operator*=(const Number&)", env.err());
	return *this;
}


Oracle::Number&
Oracle::Number::operator/=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == 0 && (ind = n.ind) == 0)
		if (OCINumberDiv(
				env.err(),					// error handle
				num,						// first OCINumber
				n.num,						// second OCINumber
				num))						// first / second
			throw OCI_Error("Number::operator/=(const Number&)", env.err());
	return *this;
}


bool
Oracle::Number::operator==(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator==(const Number&)", env.err());
		return result == 0;
	}
}


bool
Oracle::Number::operator!=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator!=(const Number&)", env.err());
		return result != 0;
	}
}


bool
Oracle::Number::operator<(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator<(const Number&)", env.err());
		return result < 0;
	}
}


bool
Oracle::Number::operator<=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator==(const Number&)", env.err());
		return result <= 0;
	}
}


bool
Oracle::Number::operator>(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator>(const Number&)", env.err());
		return result > 0;
	}
}


bool
Oracle::Number::operator>=(const Oracle::Number& n) throw(Oracle::Error)
{
	if (ind == -1 || n.ind == -1)
		return false;
	else if (this == &n)
		return true;
	else
	{
		sword result;
		if (OCINumberCmp(
				env.err(),				// error handle
				num,					// first OCINumber
				n.num,					// second OCINumber
				&result))				// -1, 0 or 1
			throw OCI_Error("Number::operator>=(const Number&)", env.err());
		return result >= 0;
	}
}


Oracle::Number
operator+(const Oracle::Number& n1, const Oracle::Number& n2)
{
	Oracle::Number n(n1);
	return n += n2;
}


Oracle::Number
operator-(const Oracle::Number& n1, const Oracle::Number& n2)
{
	Oracle::Number n(n1);
	return n -= n2;
}


Oracle::Number
operator*(const Oracle::Number& n1, const Oracle::Number& n2)
{
	Oracle::Number n(n1);
	return n *= n2;
}


Oracle::Number
operator/(const Oracle::Number& n1, const Oracle::Number& n2)
{
	Oracle::Number n(n1);
	return n /= n2;
}


const bool
Oracle::operator==(const Oracle::Number& n1, const Oracle::Number& n2)
{
	if (n1.is_null() || n2.is_null())
		return false;
	else if (&n1 == &n2)
		return true;
	else if (n1.str() == n2.str())
		return true;
	else
		return false;
}
