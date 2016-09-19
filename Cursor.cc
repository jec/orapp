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

#include "Cursor.h"
#include <oci.h>

Oracle::Env Oracle::Cursor::env;

Oracle::Cursor::Cursor() throw(Oracle::Error)
	: Select_Stmt()
{
	// allocate statement handle
	if(OCIHandleAlloc(
			(dvoid *) env.env(),						// env handle
			(dvoid **) &stmt_h,						// stmt handle returned
			OCI_HTYPE_STMT,							// handle type to return
			(size_t) 0,							// user-def memory size
			(dvoid **) 0))							// user-def memory ptr
		throw Error("Cursor::Cursor()", "OCIHandleAlloc failed for statement");
	err_h = env.err();
	st = Prepared;
}

Oracle::Cursor::~Cursor() throw()
{
}


void
Oracle::Cursor::exec() throw(Oracle::Error)
{
	if (st < Executed)
		st = Executed;
	get_column_info();
}


int
Oracle::Cursor::sqlt() const throw(Oracle::Error)
{
	return SQLT_RSET;
}
