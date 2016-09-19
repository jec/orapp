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

#ifndef ORAPP_NULLABLE_H
#define ORAPP_NULLABLE_H

#include <string>
#include <iostream>

typedef signed short sb2;	// an OCI type; define here so we don't have to include oci.h


namespace Oracle
{
	class Stmt;
	class Select_Stmt;
	
	class Nullable
	{
		public:
			// constructors/destructor
			Nullable()					throw();
			virtual ~Nullable()				throw()
				{}

			// implementors
			void set_null()					throw()
				{ ind = -1; }

			// accessors
			bool is_null() const				throw()
				{ return ind == -1; }
			sb2 indicator() const				throw()		// return indicator variable
				{ return ind; }
			virtual std::string str() const			throw(Error);	// return a string
			virtual std::string str(const std::string&) const throw();	// return a string
			virtual std::string str(					// return a string of given format
				const std::string&,						
				const std::string&) const		throw();
			virtual std::string sql_str() const 		throw();	// return a proper SQL string
			virtual long lng() const			throw(Error);	// return a long
			virtual long lng(const long) const		throw();	// return a long
			virtual double dbl() const			throw(Error);	// return a double
			virtual double dbl(const double) const		throw();	// return a double
			virtual int type() const			throw(Error);	// Oracle type
			virtual int maxsize() const			throw()		// max size of data
				{ return 0; }
			
		protected:
			// data members
			sb2 ind;							// null indicator

			// implementors
			virtual void* data() const			throw()		// ptr to data
				{ return (void*)0; }
			virtual sb2* ind_addr()				throw()		// ptr to null indicator
				{ return &ind; }

		friend class Stmt;
		friend class Select_Stmt;
	};

	inline std::ostream& operator<<(std::ostream& o, const Nullable& n)
	{ return o << n.str("<NULL>"); }
}

#endif
