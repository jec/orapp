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

#include <iostream>
#include "Connection.h"
#include "Stmt.h"
#include "Select_Stmt.h"
#include "Non_Sel_Stmt.h"
#include <oci.h>


Oracle::Env Oracle::Connection::env_;


Oracle::Connection::Connection(const std::string& u, const std::string& p, const std::string& d) throw()
	: uid(u), pw(p), sid(d), stat(not_connected), ses_h(0), svc_h(0), svr_h(0)
{
	env_h = env_.env();
	err_h = env_.err();
}


Oracle::Connection::Connection(const std::string& s) throw()
	: uid("/"), pw("/"), sid(""), stat(not_connected), ses_h(0), svc_h(0), svr_h(0)
{
	env_h = env_.env();
	err_h = env_.err();

	if (s == "/")
		return;
	
	int slash(s.find('/'));
	int at(s.find('@'));
	
	if (slash == std::string::npos)
	{
		uid = s.substr(0, at == std::string::npos ? s.length() : at);
		pw = "";
	}
	else
	{
		uid = s.substr(0, slash);
		pw = s.substr(slash + 1, (at == std::string::npos ? s.length() : at) - slash - 1);
	}
	
	if (at == std::string::npos)
		sid = "";
	else
		sid = s.substr(at + 1);
}


Oracle::Connection::~Connection() throw(Oracle::Error)
{
	if (stat == connected)
	{
		// User must explicitly commit or call close before destructor is called;
		// otherwise all transactions are rolled back.
		rollback();
		close();
	}
	free_handles(); // close does this, but do it again just in case
}


void
Oracle::Connection::open() throw(Oracle::Error)
{
	if (stat == connected)
		return;

	try
	{
		init_handles();
		attach_server();
	}
	catch(Error)
	{
		free_handles();
		throw;
	}

	try
	{
		log_on();
		stat = connected;
	}
	catch(Error)
	{
		detach_server();
		free_handles();
		throw;
	}
}


Oracle::Stmt*
Oracle::Connection::prepare(const std::string& sql) throw(Oracle::Error)
{
	// connect to database if necessary
	if (stat == not_connected)
		open();

	// allocate a statement handle
	OCIStmt* stmt_h;
	if(OCIHandleAlloc(
			(dvoid *) env_h,						// env handle
			(dvoid **) &stmt_h,						// stmt handle returned
			OCI_HTYPE_STMT,							// handle type to return
			(size_t) 0,							// user-def memory size
			(dvoid **) 0))							// user-def memory ptr
		throw Error("Connection::prepare", "OCIHandleAlloc failed for statement [" + sql + "]");

	// allocate space for the statement text
	// the ptr to the statement text must be valid for the life of the statement
	char* stmt_p = new char[sql.length() + 1];
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
		throw OCI_Error("Connection::prepare", err_h);
	
	// get the statement type
	ub2 stmt_type;
	if(OCIAttrGet(	(dvoid *) stmt_h,						// statement handle
			(ub4) OCI_HTYPE_STMT,						// handle type
			(dvoid *) &stmt_type,						// returned stmt type
			(ub4 *) 0,							// output size (0==default)
			(ub4) OCI_ATTR_STMT_TYPE,					// attribute to return
			err_h))								// error handle
		throw OCI_Error("Connection::prepare", err_h);

	switch(stmt_type)
	{
		case OCI_STMT_SELECT:
			return new Select_Stmt(stmt_h, stmt_p, svc_h, err_h);
		default:	
			return new Non_Sel_Stmt(stmt_h, stmt_p, svc_h, err_h);
	}
}


void
Oracle::Connection::rollback() throw(Oracle::Error)
{
	if (stat == not_connected)
		throw State_Error("Connection::rollback", "Not connected");

	if(OCITransRollback(
			svc_h,							// service handle
			err_h,							// error handle
			(ub4) OCI_DEFAULT))					// flags
		throw OCI_Error("Connection::rollback", err_h);
}


void
Oracle::Connection::commit() throw(Oracle::Error)
{
	if (stat == not_connected)
		throw State_Error("Connection::commit", "Not connected");

	if(OCITransCommit(
			svc_h,							// service handle
			err_h,							// error handle
			(ub4) OCI_DEFAULT))					// flags
		throw OCI_Error("Connection::commit", err_h);
}


void
Oracle::Connection::close() throw(Oracle::Error)
{
	if (stat == not_connected)
		return;

	log_off();
	detach_server();
	free_handles();
	stat = not_connected;
}


void
Oracle::Connection::init_handles() throw(Oracle::Error)
{
	// allocate a service handle
	if (OCIHandleAlloc(
			(dvoid *) env_h,					// env handle
			(dvoid **) &svc_h,					// ptr to handle alloced
			(ub4) OCI_HTYPE_SVCCTX,					// handle type
			(size_t) 0,						// user memory size
			(dvoid **) 0))						// user memory ptr
		throw Error("Connection::init_handles", "Could not allocate a service handle");

	// allocate a server handle
	if (OCIHandleAlloc(
			(dvoid *) env_h,
			(dvoid **) &svr_h,
			(ub4) OCI_HTYPE_SERVER,
			(size_t) 0,
			(dvoid **) 0))
		throw Error("Connection::init_handles", "Could not allocate a server handle");

	// allocate a session handle
	if (OCIHandleAlloc(
			(dvoid *) env_h,
			(dvoid **) &ses_h,
			(ub4) OCI_HTYPE_SESSION,
			(size_t) 0,
			(dvoid **) 0))
		throw Error("Connection::init_handles", "Could not allocate a session handle");
}


void
Oracle::Connection::attach_server() throw(Oracle::Error)
{
	// attach to server
	if (OCIServerAttach(
			svr_h,							// server handle
			err_h,							// error handle
			(text *) sid.c_str(),					// connect string
			(sb4) sid.length(),					// length of connect string
			(ub4) OCI_DEFAULT))					// mode
		throw OCI_Error("Connection::attach_server", err_h, __FILE__, __LINE__);

	// set the server handle in the service handle
	if (OCIAttrSet(	(dvoid *) svc_h,					// target
			(ub4) OCI_HTYPE_SVCCTX,					// target type
			(dvoid *) svr_h,					// source
			(ub4) 0,						// size of attribute
			(ub4) OCI_ATTR_SERVER,					// attribute type
			err_h))							// error handle
		throw OCI_Error("Connection::attach_server", err_h, __FILE__, __LINE__);
}


void
Oracle::Connection::log_on() throw(Oracle::Error)
{
	// set attributes in the authentication handle
	if (OCIAttrSet(	(dvoid *) ses_h,
			(ub4) OCI_HTYPE_SESSION,
			(dvoid *) uid.c_str(),
			(ub4) uid.length(),
			(ub4) OCI_ATTR_USERNAME,
			err_h))
		throw OCI_Error("Connection::log_on", err_h, __FILE__, __LINE__);

	if (OCIAttrSet(	(dvoid *) ses_h,
			(ub4) OCI_HTYPE_SESSION,
			(dvoid *) pw.c_str(),
			(ub4) pw.length(),
			(ub4) OCI_ATTR_PASSWORD,
			err_h))
		throw OCI_Error("Connection::log_on", err_h, __FILE__, __LINE__);

	// begin session
	if (OCISessionBegin(
			svc_h,							// service handle
			err_h,							// error handle
			ses_h,							// session handle
			(ub4) OCI_CRED_RDBMS,					// type of credentials
			(ub4) OCI_DEFAULT))					// mode
		throw OCI_Error("Connection::log_on", err_h, __FILE__, __LINE__);

	// set the authentication handle in the service handle
	if (OCIAttrSet(	(dvoid *) svc_h,
			(ub4) OCI_HTYPE_SVCCTX,
			(dvoid *) ses_h,
			(ub4) 0,
			(ub4) OCI_ATTR_SESSION,
			err_h))
		throw OCI_Error("Connection::log_on", err_h, __FILE__, __LINE__);
}


void
Oracle::Connection::log_off() throw(Oracle::Error)
{
	if (OCISessionEnd(
			svc_h,							// service handle
			err_h,							// error handle
			ses_h,							// session handle
			(ub4) OCI_DEFAULT))					// mode
		throw OCI_Error("Connection::log_off", err_h);
}


void
Oracle::Connection::detach_server() throw(Oracle::Error)
{
	if (OCIServerDetach(
			svr_h,							// server handle
			err_h,							// error handle
			(ub4) OCI_DEFAULT))					// mode
		throw OCI_Error("Connection::detach_server", err_h);
}


void
Oracle::Connection::free_handles() throw()
{
	if (svr_h)
		OCIHandleFree((dvoid *)svr_h, (ub4)OCI_HTYPE_SERVER);
	if (svc_h)
		OCIHandleFree((dvoid *)svc_h, (ub4)OCI_HTYPE_SVCCTX);
	if (ses_h)
		OCIHandleFree((dvoid *)ses_h, (ub4)OCI_HTYPE_SESSION);

	// reset handles to zero
	ses_h = 0;
	svc_h = 0;
	svr_h = 0;
}
