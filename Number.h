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

#ifndef ORAPP_NUMBER_H
#define ORAPP_NUMBER_H

#include "Nullable.h"
#include "Env.h"

class OCINumber;

namespace Oracle
{
	class Number: public Nullable
	{
		public:
			// constructors/destructor
			Number()						throw();
			Number(const int)					throw(Error);
			Number(const long)					throw(Error);
			Number(const double)					throw(Error);
			Number(const Number&)					throw(Error);
			virtual ~Number()					throw();

			// accessors
			virtual std::string str() const				throw(Error);	// return a string
			virtual std::string str(const std::string&) const	throw(Error);	// return a string or given string if null
			virtual std::string str(						// return a string of given format
				const std::string&,						
				const std::string&) const			throw(Error);
			virtual std::string sql_str() const			throw();	// return a string
			virtual long lng() const				throw(Error);	// return a long
			virtual long lng(const long) const			throw(Error);	// return a long or given long if null
			virtual double dbl() const				throw(Error);	// return a double
			virtual double dbl(const double) const			throw(Error);	// return a double or given double if null
			virtual int sqlt() const				throw();	// Oracle type
			virtual int maxsize() const				throw();


			// operators
			Number& operator=(const int)				throw(Error);
			Number& operator=(const long)				throw(Error);
			Number& operator=(const double)				throw(Error);
			Number& operator=(const Number&)			throw(Error);
			Number& operator+=(const Number&)			throw(Error);
			Number& operator-=(const Number&)			throw(Error);
			Number& operator*=(const Number&)			throw(Error);
			Number& operator/=(const Number&)			throw(Error);
			bool operator==(const Number&)				throw(Error);
			bool operator!=(const Number&)				throw(Error);
			bool operator<(const Number&)				throw(Error);
			bool operator<=(const Number&)				throw(Error);
			bool operator>(const Number&)				throw(Error);
			bool operator>=(const Number&)				throw(Error);

		protected:
			// data members
			OCINumber* num;								// value
			static Env env;								// initializes OCI environment
			const static std::string default_fmt;					// default conversion format to/from text
			
			// implementors
			virtual void* data() const { return (void*)num; }			// ptr to data
	};

	inline std::ostream& operator<<(std::ostream& o, const Number& n)
	{ return o << n.str("<NULL>"); }

	const bool operator==(const Number&, const Number&);
}

Oracle::Number operator+(const Oracle::Number&, const Oracle::Number&);
Oracle::Number operator-(const Oracle::Number&, const Oracle::Number&);
Oracle::Number operator*(const Oracle::Number&, const Oracle::Number&);
Oracle::Number operator/(const Oracle::Number&, const Oracle::Number&);

#endif
