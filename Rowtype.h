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
			Rowtype(const Select_Stmt&)			throw(OCI_Error, Type_Error); // initialize with select list types
			virtual ~Rowtype()				throw();

			// implementors
			void set_null()					throw();
			Nullable& operator[](const int)			throw(Value_Error);
			Nullable& operator[](const std::string&)	throw(Value_Error);

			// accessors
			int ncols() const				throw();
			std::string colname(const int) const		throw(Value_Error);
			const Nullable& operator[](const int) const	throw(Value_Error);
			const Nullable& operator[](const std::string&) const	throw(Value_Error);

		protected:
			// data members
			OCIStmt* stmt_h;
			OCIError* err_h;
			std::vector<Nullable*>* col_vec;
			std::vector<std::string>* col_name;
			std::map<std::string, int>* col_name_m;
	};

	std::ostream& operator<<(std::ostream&, const Oracle::Rowtype&)	throw();
}

#endif
