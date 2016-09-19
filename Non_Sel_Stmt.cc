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

#include "Non_Sel_Stmt.h"
#include "Connection.h"
#include <oci.h>


Oracle::Non_Sel_Stmt::Non_Sel_Stmt(OCIStmt* stmt_hdl, char* stmt_ptr, OCISvcCtx* svc_hdl, OCIError* err_hdl) throw()
	: Stmt(stmt_hdl, stmt_ptr, svc_hdl, err_hdl)
{
}


Oracle::Non_Sel_Stmt::Non_Sel_Stmt(Connection& db) throw(Oracle::Error)
	: Stmt(db)
{
}


Oracle::Non_Sel_Stmt::Non_Sel_Stmt(Connection& db, const std::string& sql) throw(Oracle::Error)
	: Stmt(db, sql)
{
	// make sure this is actually a non-SELECT statement
	if (oci_type() == OCI_STMT_SELECT)
	{
		OCIHandleFree((dvoid *)stmt_h, (ub4)OCI_HTYPE_STMT);
		delete stmt_p;
		st = Invalid;
		Type_Error e("Non_Sel_Stmt::Non_Sel_Stmt", "Statement is not a non-SELECT statement");
		e.desc << "statement = {" << sql << "}";
		throw e;
	}
}


void
Oracle::Non_Sel_Stmt::exec() throw(Oracle::Error)
{
	do_exec(1);
}
