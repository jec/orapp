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

#ifndef ORAPP_ROWTYPE_H
#define ORAPP_ROWTYPE_H

#include <iostream>
#include <vector>
#include <map>


class OCIStmt;
class OCIError;


namespace Oracle
{
	class Nullable;
	class Select_Stmt;

	class Rowtype
	{
		public:
			// constructors/destructor
			Rowtype()					throw(Error);	// create empty Rowtype object
			Rowtype(const Select_Stmt&)			throw(Error);	// initialize with select list types
			virtual ~Rowtype()				throw();

			// implementors
			void add(Nullable*, const std::string&)		throw();	
			void set_null()					throw();
			inline Nullable& operator[](const int i)	throw(Error)
				{ if (col_vec && i >= 0 && i < col_vec->size()) return *(*col_vec)[i];
				else throw_subscript_error("Rowtype::operator[](const int)", i); }
			Nullable& operator[](const std::string& s)	throw(Error)
				{ if (col_name_m->count(s)) return *(*col_vec)[(*col_name_m)[s]];
				else throw_subscript_error("Rowtype::operator[](const std::string&)", s); }

			// accessors
			int ncols() const				throw();
			std::string colname(const int) const		throw(Error);
			inline const Nullable& operator[](const int i) const throw(Error)
				{ if (col_vec && i >= 0 && i < col_vec->size()) return *(*col_vec)[i];
				else throw_subscript_error("Rowtype::operator[](const int)", i); }
			const Nullable& operator[](const std::string& s) const throw(Error)
				{ if (col_name_m->count(s)) return *(*col_vec)[(*col_name_m)[s]];
				else throw_subscript_error("Rowtype::operator[](const std::string&)", s); }

		protected:
			// protected implementors
			void init_data(const Select_Stmt&)		throw(Error);
			void throw_subscript_error(const std::string&, const int) const throw(Error);
			void throw_subscript_error(const std::string&, const std::string&) const throw(Error);

			// data members
			OCIStmt* stmt_h;
			OCIError* err_h;
			std::vector<Nullable*>* col_vec;
			std::vector<std::string>* col_name;
			std::map<std::string, int>* col_name_m;

		friend class Stmt;
		friend class Select_Stmt;
	};

	std::ostream& operator<<(std::ostream&, const Oracle::Rowtype&)	throw();
}

#endif
