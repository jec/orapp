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

#ifndef ORAPP_NON_SEL_STMT_H
#define ORAPP_NON_SEL_STMT_H

#include "Stmt.h"


namespace Oracle
{
	class Connection;
	class Nullable;
	class Rowtype;

	class Non_Sel_Stmt: public Stmt
	{
		public:
			// constructors/destructor
			Non_Sel_Stmt(Connection&)			throw(Error);	// use this Connection
			Non_Sel_Stmt(
				Connection&,						// use this Connection
				const std::string&)			throw(Error);	// statement text
			virtual ~Non_Sel_Stmt() {}

			// implementors
			virtual void exec()				throw(Error);

			// accessors
			virtual stmt_t type() const					// statement type
				{ return Non_Select; }
		
		protected:
			// protected constructor
			Non_Sel_Stmt(
				OCIStmt* stmt_hdl,					// statement handle
				char* stmt_ptr,						// statement text
				OCISvcCtx* svc_hdl,					// service context handle
				OCIError* err_hdl)			throw();	// error handle
		
		friend class Connection;
	};
}

#endif
