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
#include "Date.h"
#include <oci.h>

#define ORAPP_MAX_DATE_LEN 100

const std::string Oracle::Date::default_fmt = "YYYY/MM/DD HH24:MI:SS";

Oracle::Env Oracle::Date::env;

Oracle::Date::Date() throw()
	: Nullable(), date_(new OCIDate)
{
}


Oracle::Date::Date(OCIDate& d) throw(Oracle::Error)
	: Nullable(), date_(new OCIDate)
{
	if (OCIDateAssign(
			env.err(),					// error handle
			&d,						// from OCIDate
			date_))						// to OCIDate
		throw OCI_Error("Date::Date(OCIDate&)", env.err());
	ind = 0;
}


Oracle::Date::Date(const Oracle::Date& d) throw(Oracle::Error)
	: Nullable(), date_(new OCIDate)
{
	// if n is not null, initialize this Date with d's value
	if ((ind = d.ind) == 0)
		if (OCIDateAssign(
				env.err(),				// error handle
				d.date_,				// from OCIDate
				date_))					// to OCIDate
			throw OCI_Error("Date::Date(const Date&)", env.err());
}


Oracle::Date::Date(const std::string& s) throw(Oracle::Error)
	: Nullable(), date_(new OCIDate)
{
	if (OCIDateFromText(
			env.err(),					// error handle
			(CONST text*) s.c_str(),			// input string
			(ub4) s.length(),				// input string length
			(CONST text*) default_fmt.c_str(),		// format string
			(ub1) default_fmt.length(),			// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			date_))						// output buffer
	{
		OCI_Error e("Date::Date(const std::string&)", env.err());
		e.desc << "date string = {" << s << "}; default format = {" << default_fmt << "}";
		throw e;
	}
	ind = 0;
}


Oracle::Date::Date(const std::string& s, const std::string& f) throw(Oracle::Error)
	: Nullable(), date_(new OCIDate)
{
	if (OCIDateFromText(
			env.err(),					// error handle
			(CONST text*) s.c_str(),			// input string
			(ub4) s.length(),				// input string length
			(CONST text*) f.c_str(),			// format string
			(ub1) f.length(),				// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			date_))						// output buffer
	{
		OCI_Error e("Date::Date(const std::string&, const std::string&)", env.err());
		e.desc << "date string = {" << s << "}; format = {" << f << "}";
		throw e;
	}
	ind = 0;
}


Oracle::Date::~Date() throw()
{
	delete date_;
	ind = -1;
}


int
Oracle::Date::sqlt() const throw()
{
	return SQLT_ODT;
}


int
Oracle::Date::maxsize() const throw()
{
	return sizeof(OCIDate);
}


Oracle::Date&
Oracle::Date::assign(const std::string& s, const std::string& f) throw(Oracle::Error)
{
	if (OCIDateFromText(
			env.err(),					// error handle
			(CONST text*) s.c_str(),			// input string
			(ub4) s.length(),				// input string length
			(CONST text*) f.c_str(),			// format string
			(ub1) f.length(),				// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			date_))						// output buffer
	{
		OCI_Error e("Date::Date(const std::string&, const std::string&)", env.err());
		e.desc << "date string = {" << s << "}; format = {" << f << "}";
		throw e;
	}
	ind = 0;
	return *this;
}


std::string
Oracle::Date::str() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::str();
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) default_fmt.c_str(),		// format string
			(ub1) default_fmt.length(),			// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::str()", env.err());
	return buf;
}


std::string
Oracle::Date::str(const std::string& s) const throw(Oracle::Error)
{
	if (ind == -1)
		return s;
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) default_fmt.c_str(),		// format string
			(ub1) default_fmt.length(),			// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::str(const std::string&)", env.err());
	return buf;
}


std::string
Oracle::Date::str(const std::string& s, const std::string& f) const throw(Oracle::Error)
{
	if (ind == -1)
		return s;
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) f.c_str(),			// format string
			(ub1) f.length(),				// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::str(const std::string&, const std::string&)", env.err());
	return buf;
}


std::string
Oracle::Date::sql_str() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::sql_str();
	return "TO_CHAR('" + str() + "', '" + default_fmt + "')";
}


long
Oracle::Date::lng() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::lng();
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) "J",				// format string
			(ub1) 1,					// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::lng()", env.err());
	char* p;
	long l(std::strtol(buf, &p, 10));
	if(p == buf)
		throw Value_Error("Date::lng()", "Could not convert date to long");
	else
		return l;
}


long
Oracle::Date::lng(const long n) const throw(Oracle::Error)
{
	if (ind == -1)
		return n;
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) "J",				// format string
			(ub1) 1,					// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::lng(const long)", env.err());
	char* p;
	long l(std::strtol(buf, &p, 10));
	if(p == buf)
		return n;
	else
		return l;
}


double
Oracle::Date::dbl() const throw(Oracle::Error)
{
	if (ind == -1)
		return Nullable::lng();
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) "J",				// format string
			(ub1) 1,					// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::lng()", env.err());
	char* p;
	double d(std::strtod(buf, &p));
	if(p == buf)
		throw Value_Error("Date::dbl()", "Could not convert date to double");
	else
		return d;
}


double
Oracle::Date::dbl(const double n) const throw(Oracle::Error)
{
	if (ind == -1)
		return n;
	char buf[ORAPP_MAX_DATE_LEN];
	ub4 buf_len(ORAPP_MAX_DATE_LEN);
	if (OCIDateToText(
			env.err(),					// error handle
			date_,						// OCIDate to convert
			(CONST text*) "J",				// format string
			(ub1) 1,					// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			(ub4*) &buf_len,				// output buffer size
			(text*) buf))					// output buffer
		throw OCI_Error("Date::dbl(const double)", env.err());
	char* p;
	double d(std::strtod(buf, &p));
	if(p == buf)
		return n;
	else
		return d;
}


Oracle::Date
Oracle::Date::last_day() const throw(Oracle::Error)
{
	OCIDate last;
	if (OCIDateLastDay(
			env.err(),					// error handle
			date_,						// input OCIDate
			&last))						// output OCIDate
		throw OCI_Error("Date::last_day()", env.err());
	return Date(last);
}


Oracle::Date
Oracle::Date::next_day(const std::string& day) const throw(Oracle::Error)
{
	OCIDate next;
	if (OCIDateNextDay(
			env.err(),					// error handle
			date_,						// input OCIDate
			(text*) day.c_str(),				// day of week
			(ub4) day.length(),				// day of week length
			&next))						// output OCIDate
	{
		OCI_Error e("Date::next_day(const std::string&)", env.err());
		e.desc << "date = {" << str() << "}; day = {" << day << "}";
		throw e;
	}
	return Date(next);
}


Oracle::Date&
Oracle::Date::operator=(const Oracle::Date& rhs) throw(Oracle::Error)
{
	if (&rhs != this)
	{
		ind = rhs.ind;
		if (ind == 0)
			if (OCIDateAssign(
					env.err(),			// error handle
					rhs.date_,			// from OCIDate
					date_))				// to OCIDate
				throw OCI_Error("Date::operator=(const Date&)", env.err());
	}
	return *this;
}


Oracle::Date&
Oracle::Date::operator=(const char* rhs) throw(Oracle::Error)
{
	if (OCIDateFromText(
			env.err(),					// error handle
			(CONST text*) rhs,				// input string
			(ub4) std::strlen(rhs),				// input string length
			(CONST text*) default_fmt.c_str(),		// format string
			(ub1) default_fmt.length(),				// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			date_))						// output buffer
	{
		OCI_Error e("Date::operator=(const char*)", env.err());
		e.desc << "date string = {" << rhs << "}; default format = {" << default_fmt << "}";
		throw e;
	}
	ind = 0;
	return *this;
}


Oracle::Date&
Oracle::Date::operator=(const std::string& rhs) throw(Oracle::Error)
{
	if (OCIDateFromText(
			env.err(),					// error handle
			(CONST text*) rhs.c_str(),			// input string
			(ub4) rhs.length(),				// input string length
			(CONST text*) default_fmt.c_str(),		// format string
			(ub1) default_fmt.length(),			// format string length
			(CONST text*) 0,				// default language
			(ub4) 0,					// language name length
			date_))						// output buffer
	{
		OCI_Error e("Date::operator=(const std::string&)", env.err());
		e.desc << "date string = {" << rhs << "}; default format = {" << default_fmt << "}";
		throw e;
	}
	ind = 0;
	return *this;
}


Oracle::Date&
Oracle::Date::operator+=(const Oracle::Days& days) throw(Oracle::Error)
{
	if (OCIDateAddDays(
			env.err(),					// error handle
			date_,						// input OCIDate
			(sb4) days.days_,				// number of days
			date_))						// output OCIDate
		throw OCI_Error("Oracle::operator+=(const Days&)", env.err());
	return *this;
}


Oracle::Date&
Oracle::Date::operator+=(const Oracle::Months& months) throw(Oracle::Error)
{
	if (OCIDateAddMonths(
			env.err(),					// error handle
			date_,						// input OCIDate
			(sb4) months.months_,				// number of days
			date_))						// output OCIDate
		throw OCI_Error("Oracle::operator+=(const Months&)", env.err());
	return *this;
}


Oracle::Date&
Oracle::Date::operator-=(const Oracle::Days& days) throw(Oracle::Error)
{
	if (OCIDateAddDays(
			env.err(),					// error handle
			date_,						// input OCIDate
			(sb4) -days.days_,				// number of days
			date_))						// output OCIDate
		throw OCI_Error("Oracle::operator+=(const Days&)", env.err());
	return *this;
}


Oracle::Date&
Oracle::Date::operator-=(const Oracle::Months& months) throw(Oracle::Error)
{
	if (OCIDateAddMonths(
			env.err(),					// error handle
			date_,						// input OCIDate
			(sb4) -months.months_,				// number of days
			date_))						// output OCIDate
		throw OCI_Error("Oracle::operator+=(const Months&)", env.err());
	return *this;
}


int
Oracle::Date::operator-(const Oracle::Date& d) throw(Oracle::Error)
{
	sb4 diff;
	if (OCIDateDaysBetween(
			env.err(),					// error handle
			date_,						// first date
			d.date_,					// second date
			&diff))						// difference
		throw OCI_Error("Date::operator-(const Date&)", env.err());
	return diff;
}


bool
Oracle::Date::operator==(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return true;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result == 0;
	}
}


bool
Oracle::Date::operator!=(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return false;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result != 0;
	}
}


bool
Oracle::Date::operator<(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return false;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result == -1;
	}
}


bool
Oracle::Date::operator<=(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return true;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result != 1;
	}
}


bool
Oracle::Date::operator>(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return false;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result == 1;
	}
}


bool
Oracle::Date::operator>=(const Oracle::Date& d) throw(Oracle::Error)
{
	if (ind == -1 || d.ind == -1)
		return false;
	else if (this == &d)
		return true;
	else
	{
		sword result;
		if (OCIDateCompare(
				env.err(),				// error handle
				date_,					// first date
				d.date_,				// second date
				&result))				// -1, 0 or 1
			throw OCI_Error("Date::operator==(const Date&)", env.err());
		return result != -1;
	}
}


Oracle::Date
Oracle::Date::sysdate() throw(Oracle::Error)
{
	OCIDate now;
	if (OCIDateSysDate(
			Date::env.err(),			// error handle
			&now))					// output OCIDate
		throw OCI_Error("Date::Date(const init_date_t)", Date::env.err());
	return Date(now);
}


Oracle::Date
operator+(const Oracle::Date& date, const Oracle::Days& days)
{
	Oracle::Date d(date);
	return d += days;
}


Oracle::Date
operator+(const Oracle::Date& date, const Oracle::Months& months)
{
	Oracle::Date d(date);
	return d += months;
}


Oracle::Date
operator-(const Oracle::Date& date, const Oracle::Days& days)
{
	Oracle::Date d(date);
	return d -= days;
}


Oracle::Date
operator-(const Oracle::Date& date, const Oracle::Months& months)
{
	Oracle::Date d(date);
	return d -= months;
}
