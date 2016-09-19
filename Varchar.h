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

#ifndef ORAPP_VARCHAR_H
#define ORAPP_VARCHAR_H

#include "Nullable.h"

namespace Oracle
{
	class Varchar: public Nullable
	{
		public:
			// constructors/destructor
			Varchar()					throw();
			Varchar(const int)				throw();
			Varchar(const char*)				throw();
			Varchar(const std::string&)			throw();
			Varchar(const Varchar&)				throw();
			virtual ~Varchar()				throw();

			// accessors
			virtual std::string str() const			throw(Error);	// return a string
			virtual std::string str(const std::string&) const throw();	// return a string or given string if null
			virtual std::string str(					// return a string of given format
				const std::string&,						
				const std::string&) const		throw();
			virtual std::string sql_str() const		throw();	// return a string
			virtual long lng() const			throw(Error);	// return a long
			virtual long lng(const long) const		throw(Error);	// return a long or given long if null
			virtual double dbl() const			throw(Error);	// return a double
			virtual double dbl(const double) const		throw(Error);	// return a double or given double if null
			virtual int sqlt() const			throw();	// Oracle null-terminated STRING
			virtual int maxsize() const			throw()
				{ return max_sz; };

			// operators
			Varchar& operator=(const char*)			throw();
			Varchar& operator=(const std::string&)		throw();
			Varchar& operator=(const Varchar&)		throw();
			Varchar& operator=(const long)			throw();
			Varchar& operator=(const double)		throw();

		protected:
			// data members
			char* vc;							// value
			int max_sz;							// max size

			// implementors
			virtual void* data() const 			throw()
				{ return (void*)vc; };
	};

	inline std::ostream& operator<<(std::ostream& o, const Varchar& v)
	{ return o << v.str("<NULL>"); }
	const bool operator==(const Varchar&, const Varchar&);
}

#endif
