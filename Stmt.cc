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

#include "Stmt.h"
#include "Connection.h"
#include "Nullable.h"
#include "Rowtype.h"
#include <oci.h>


Oracle::Stmt::Stmt(OCIStmt* stmt_hdl, char* stmt_ptr, OCISvcCtx* svc_hdl, OCIError* err_hdl) throw()
	: stmt_h(stmt_hdl), stmt_p(stmt_ptr), svc_h(svc_hdl), err_h(err_hdl), st(Prepared)
{
}


Oracle::Stmt::Stmt(Connection& db) throw(Oracle::Error)
	: svc_h(db.svc_handle()), err_h(db.err_handle()), st(Initialized), stmt_p(0)
{
	// connect to database if necessary and set handles again
	if (db.stat == Connection::not_connected)
	{
		db.open();
		svc_h = db.svc_handle();
		err_h = db.err_handle();
	}

	// allocate a statement handle
	if(OCIHandleAlloc(
			(dvoid *) db.env_handle(),					// env handle
			(dvoid **) &stmt_h,						// stmt handle returned
			OCI_HTYPE_STMT,							// handle type to return
			(size_t) 0,							// user-def memory size
			(dvoid **) 0))							// user-def memory ptr
		throw Error("Stmt::Stmt(Connection&)", "OCIHandleAlloc failed for statement");
}


Oracle::Stmt::Stmt(Connection& db, const std::string& sql) throw(Oracle::Error)
	: svc_h(db.svc_handle()), err_h(db.err_handle()), st(Initialized), stmt_p(new char[sql.length() + 1])
{
	// connect to database if necessary and set handles again
	if (db.stat == Connection::not_connected)
	{
		db.open();
		svc_h = db.svc_handle();
		err_h = db.err_handle();
	}

	// allocate a statement handle
	if(OCIHandleAlloc(
			(dvoid *) db.env_handle(),					// env handle
			(dvoid **) &stmt_h,						// stmt handle returned
			OCI_HTYPE_STMT,							// handle type to return
			(size_t) 0,							// user-def memory size
			(dvoid **) 0))							// user-def memory ptr
		throw Error("Stmt::Stmt(Connection&, const std::string&)", "OCIHandleAlloc failed for statement");

	prepare(sql);
}


Oracle::Stmt::~Stmt() throw()
{
	if (st > Closed)
		close();
}


void
Oracle::Stmt::prepare() throw(Oracle::Error)
{
	if (std::ostringstream::str().length())
		prepare(std::ostringstream::str());
	else
		throw State_Error("Stmt::prepare()", "No statement text has been specified");
}


void
Oracle::Stmt::prepare(const std::string& sql) throw(Oracle::Error)
{
	// validate state
	if (st >= Prepared)
		throw State_Error("Stmt::prepare(const std::string&)", "Too late to prepare statement");

	// copy statement text into buffer
	if (!stmt_p)
		stmt_p = new char[sql.length() + 1];
	sql.copy(stmt_p, sql.length());
	stmt_p[sql.length()] = '\0';

	// prepare the statement
	if(OCIStmtPrepare(
			stmt_h,								// statement handle
			err_h,								// error handle
			(text *) stmt_p,						// statement text
			(ub4) strlen(stmt_p),						// statement length
			(ub4) OCI_NTV_SYNTAX,						// syntax type
			(ub4) OCI_DEFAULT))						// mode
	{
		OCI_Error e("Stmt::prepare(const std::string&)", err_h);
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}
	st = Prepared;

	// if bind variables have been queued, bind them
	if (bind_q_.size())
	{
		int count(1);
		while (bind_q_.size())
		{
			std::ostringstream ost;
			ost << "bindq" << count++;
			bind(*bind_q_.front(), ost.str());
			bind_q_.pop();
		}
	}
}


int
Oracle::Stmt::get_type() const throw(Oracle::Error)
{
	// get the statement type
	ub2 stmt_type;
	if(OCIAttrGet(	(dvoid *) stmt_h,						// statement handle
			(ub4) OCI_HTYPE_STMT,						// handle type
			(dvoid *) &stmt_type,						// returned stmt type
			(ub4 *) 0,							// output size (0==default)
			(ub4) OCI_ATTR_STMT_TYPE,					// attribute to return
			err_h))								// error handle
	{
		OCI_Error e("Stmt::get_type()", err_h);
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}
	return (int)stmt_type;
}


std::string
Oracle::Stmt::str() const throw()
{
	if (stmt_p)
		return std::string(stmt_p);
	else
		return std::ostringstream::str();
}


void
Oracle::Stmt::bind(Nullable& bindvar) throw(Oracle::Error)
{
	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
			throw State_Error("Stmt::bind(Nullable&)", "No statement text has been specified");
	}
	else if (st > Prepared)
	{
		State_Error e("Stmt::bind", "Cannot bind after execution");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// do the bind
	OCIBind* bind_h = 0;
	if(OCIBindByPos(stmt_h,
			&bind_h,
			err_h,
			(ub4) bind_l.size() + 1,
			(dvoid*) bindvar.data(),
			(sb4) bindvar.maxsize(),
			(ub2) bindvar.type(),
			(dvoid*) bindvar.ind_addr(),
			(ub2*) 0,
			(ub2*) 0,
			(ub4) 0,
			(ub4*) 0,
			OCI_DEFAULT))
	{
		OCI_Error e("Stmt::bind(Nullable&)", err_h);
		e.desc << "statement = {" << stmt_p << "}; position = " << bind_l.size() + 1;
		throw e;
	}

	// add the bind handle to the list
	bind_l.push_back(bind_h);
}


void
Oracle::Stmt::bind(Nullable& bindvar, const std::string& label) throw(Oracle::Error)
{
	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
			throw State_Error("Stmt::bind(Nullable&)", "No statement text has been specified");
	}
	else if (st > Prepared)
	{
		State_Error e("Stmt::bind", "Cannot bind after execution");
		e.desc << "statement = {" << stmt_p << "}; label = {" << label << "}";
		throw e;
	}

	// do the bind
	OCIBind* bind_h = 0;
	if(OCIBindByName(stmt_h,
			&bind_h,
			err_h,
			(CONST text*) label.c_str(),
			(sb4) label.length(),
			(dvoid*) bindvar.data(),
			(sb4) bindvar.maxsize(),
			(ub2) bindvar.type(),
			(dvoid*) bindvar.ind_addr(),
			(ub2*) 0,
			(ub2*) 0,
			(ub4) 0,
			(ub4*) 0,
			OCI_DEFAULT))
	{
		OCI_Error e("Stmt::bind(Nullable&, const std::string&)", err_h);
		e.desc << "statement = {" << stmt_p << "}; label = {" << label << "}";
		throw e;
	}
}


void
Oracle::Stmt::bind_q(Nullable& bindvar) throw(Oracle::Error)
{
	// check state
	if (st != Initialized)
	{
		State_Error e("Stmt::bind_q(Nullable&)", "Cannot alter statement text in this state");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// add to the queue
	bind_q_.push(&bindvar);
	
	// add placeholder in statement text
	*this << ":bindq" << bind_q_.size();
}


void
Oracle::Stmt::bind_q(Rowtype& row) throw(Oracle::Error)
{
	// check state
	if (st != Initialized)
	{
		State_Error e("Stmt::bind_q(Rowtype&)", "Cannot alter statement text in this state");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// check Rowtype
	if (!row.col_vec)
	{
		Value_Error e("Stmt::bind_q(Rowtype&)", "Rowtype object is not initialized");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// add each Nullable to bind queue and add placeholder to statement text
	for (int i=0; i<row.col_vec->size(); i++)
	{
		bind_q_.push((*row.col_vec)[i]);
		*this << ":bindq" << bind_q_.size();
		if (i < row.col_vec->size() - 1)
			*this << ", ";
	}
}


void
Oracle::Stmt::do_exec(const int iters) throw(Oracle::Error)
{
	// The parameter indicates the following:
	// 	For SELECT statements, if iters is non-zero, then defines must have already been done,
	// 		and an appropriate number of rows will be fetched upon execute.
	// 	For non-SELECT statements, iters indicates the number of times this statement should
	// 		be executed.
	// For basic operation, iters should be 0 for SELECT statements and 1 for non-SELECT statements.

	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
		{
			State_Error e("Stmt::do_exec", "No statement text has been specified");
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
		}
	}
	if (st == Closed)
	{
		State_Error e("Stmt::do_exec", "Cannot execute a closed statement");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// execute statement
	switch(OCIStmtExecute(
			svc_h,									// service handle
			stmt_h,									// statement handle
			err_h,									// error handle
			(ub4) iters,								// # of iterations
			(ub4) 0,								// offset into bind array
			(OCISnapshot*) 0,							// input snapshot
			(OCISnapshot*) 0,							// output snapshot
			(ub4) OCI_DEFAULT))							// mode
	{
		case OCI_SUCCESS:
		case OCI_SUCCESS_WITH_INFO:
		case OCI_NO_DATA:
			if (st < Executed)
				st = Executed;
			break;

		default:
			OCI_Error e("Stmt::do_exec", err_h);
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
	}
}


void
Oracle::Stmt::close() throw()
{
	// ignore if already closed
	if (st == Closed)
		return;

	// clear the bind list
	bind_l.clear();

	// release the statement text
	delete stmt_p;
	
	// release the statement handle
	if (stmt_h)
		OCIHandleFree((dvoid *)stmt_h, (ub4)OCI_HTYPE_STMT);

	// set the state
	st = Closed;
}
