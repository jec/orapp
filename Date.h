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

#ifndef ORAPP_DATE_H
#define ORAPP_DATE_H

#include "Nullable.h"
#include "Env.h"
#include <string>

class OCIDate;

namespace Oracle
{
	class Days
	{
		public:
			Days(const int d) { days_ = d; }
			~Days() {}
			int days_;
	};
	
	class Months
	{
		public:
			Months(const int m) {months_ = m; }
			~Months() {}
			int months_;
	};
	
	class Date: public Nullable
	{
		public:
			// constructors/destructor
			Date()						throw();
			Date(const std::string&)			throw(Error);
			Date(const std::string&, const std::string&)	throw(Error);
			Date(const Date&)				throw(Error);
			virtual ~Date()					throw();

			// implementors
			Date& assign(							// assign date with given format
				const std::string&,
				const std::string&)			throw(Error);
			
			// accessors
			virtual std::string str() const			throw(Error);	// return a string
			virtual std::string str(const std::string&) const throw(Error);	// return a string or given string
			virtual std::string str(					// return a string of given format
				const std::string&,						
				const std::string&) const		throw(Error);
			virtual std::string sql_str() const		throw(Error);	// return a string
			virtual long lng() const			throw(Error);	// return a long
			virtual long lng(const long) const		throw(Error);	// return a long or given long
			virtual double dbl() const			throw(Error);	// return a double
			virtual double dbl(const double) const		throw(Error);	// return a double or given double
			virtual int sqlt() const			throw();	// Oracle type
			virtual int maxsize() const			throw();
			Date last_day() const				throw(Error);	// get last day of month
			Date next_day(const std::string&) const		throw(Error);	// get date of next given day of week

			// operators
			Date& operator=(const Date&)			throw(Error);
			Date& operator=(const char*)			throw(Error);
			Date& operator=(const std::string&)		throw(Error);
			Date& operator+=(const Days&)			throw(Error);
			Date& operator+=(const Months&)			throw(Error);
			Date& operator-=(const Days&)			throw(Error);
			Date& operator-=(const Months&)			throw(Error);
			int operator-(const Date&)			throw(Error);
			bool operator==(const Date&)			throw(Error);
			bool operator!=(const Date&)			throw(Error);
			bool operator<(const Date&)			throw(Error);
			bool operator<=(const Date&)			throw(Error);
			bool operator>(const Date&)			throw(Error);
			bool operator>=(const Date&)			throw(Error);
			
			// static functions
			static Date sysdate()				throw(Error);	// date/time on client
			
		protected:
			// constructor
			Date(OCIDate&)					throw(Error);	// internal constructor

			// implementors
			virtual void* data() const throw() { return (void*)date_; };	// ptr to data
			
			// data members
			OCIDate* date_;
			static const std::string default_fmt;
			static Env env;
	};

	inline std::ostream& operator<<(std::ostream& o, const Date& d)
	{ return o << d.str("<NULL>"); }
}
	
Oracle::Date operator+(const Oracle::Date&, const Oracle::Days&);
Oracle::Date operator+(const Oracle::Date&, const Oracle::Months&);
Oracle::Date operator-(const Oracle::Date&, const Oracle::Days&);
Oracle::Date operator-(const Oracle::Date&, const Oracle::Months&);

#endif
