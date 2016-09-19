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

#ifndef ORAPP_CURSOR_H
#define ORAPP_CURSOR_H

#include "Select_Stmt.h"
#include "Nullable.h"
#include "Env.h"

namespace Oracle
{
	class Cursor: public Select_Stmt, public Nullable
	{
		public:
			// constructor/destructor
			Cursor()					throw(Error);
			virtual ~Cursor()				throw();
			virtual void exec()				throw(Error);

			// accessors
			virtual int sqlt() const			throw(Error);	// Oracle type
			virtual int maxsize() const			throw()		// max size of data
				{ return 0; }

		protected:
			// implementors
			virtual void* data() const			throw()		// ptr to data
				{ return (void*)&stmt_h; }
			virtual sb2* ind_addr()				throw()		// ptr to null indicator
				{ return (sb2*)0; }

			// data members
			static Env env;

		private:
			// disallowed methods
			virtual void set_null() throw() {}
			virtual bool is_null() const throw() { return false; }
			virtual sb2 indicator() const throw() { return 0; }
			virtual std::string str() const	 throw(Error) { return ""; }
			virtual std::string str(const std::string&) const throw() { return ""; }
			virtual std::string str(const std::string&, const std::string&) const throw() { return ""; }
			virtual std::string sql_str() const throw() { return ""; }
			virtual long lng() const throw(Error) { return 0L; }
			virtual long lng(const long) const throw() { return 0L; }
			virtual double dbl() const throw(Error) { return 0.0; }
			virtual double dbl(const double) const throw() { return 0.0; }
	};
}

#endif
