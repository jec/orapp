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

#ifndef ORAPP_SELECT_STMT_H
#define ORAPP_SELECT_STMT_H

#include "Stmt.h"
#include "Rowtype.h"
#include <vector>
#include <map>

class OCIDefine;


namespace Oracle
{
	class Nullable;

	class Select_Stmt: public Stmt
	{
		public:
			// constructors/destructor
			Select_Stmt(Connection&)			throw(Error);	// use this Connection
			Select_Stmt(
				Connection&,						// use this Connection
				const std::string&)			throw(Error);	// statement text
			virtual ~Select_Stmt()				throw();

			// implementors
			virtual void exec()				throw(Error);
			virtual void bind_col(Nullable&)		throw(Error);
			virtual void bind_col(Nullable* ...)		throw(Error);
			virtual void bind_col(Rowtype&)			throw(Error);
			virtual bool fetch()				throw(Error);	// get rows
			virtual void close()				throw();
			inline Nullable& operator[](const int i)	throw(Error)	// get column data
				{ if (row_) return (*row_)[i];
				else throw_subscript_error("Select_Stmt::operator[](const int)"); }
			inline Nullable& operator[](const std::string& s) throw(Error)	// get column data
				{ if (row_) return (*row_)[s];
				else throw_subscript_error("Select_Stmt::operator[](const std::string&)"); }

			// accessors
			const inline Nullable& operator[](const int i) const throw(Error) // get column data
				{ if (row_) return (*row_)[i];
				else throw_subscript_error("Select_Stmt::operator[](const int)"); }
			const inline Nullable& operator[](const std::string& s) const throw(Error) // get column data
				{ if (row_) return (*row_)[s];
				else throw_subscript_error("Select_Stmt::operator[](const std::string&)"); }
			virtual std::string colname(const int) const	throw(Error);	// get column name
			virtual stmt_t type() const					// statement type
				{ return Select; }
			virtual int ncols() const					// number of columns
				{ return nc; }

		protected:
			// protected constructor
			Select_Stmt(
				OCIStmt* stmt_hdl,					// statement handle
				char* stmt_ptr,						// statement text
				OCISvcCtx* svc_hdl,					// service context handle
				OCIError* err_hdl)			throw();	// error handle
			
			// protected implementors
			void throw_subscript_error(const std::string&) const throw(Error);
			
			// data members
			int nc;								// number of columns returned
			Rowtype* row_;
			std::list<OCIDefine*> def_l;					// list of define handles
			std::vector<std::string>* cnamev_;				// vector of column names
			std::map<std::string, int>* cnamem_;				// map of col name to number

		friend class Connection;
		friend class Rowtype;
	};
}

#endif
