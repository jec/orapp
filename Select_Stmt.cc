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

#include "Select_Stmt.h"
#include "Connection.h"
#include "Varchar.h"
#include <cstdio>
#include <oci.h>


Oracle::Select_Stmt::Select_Stmt(OCIStmt* stmt_hdl, char* stmt_ptr, OCISvcCtx* svc_hdl, OCIError* err_hdl) throw()
	: Stmt(stmt_hdl, stmt_ptr, svc_hdl, err_hdl), nc(0), cnamem_(0), cnamev_(0), row_(0)
{
}


Oracle::Select_Stmt::Select_Stmt(Connection& db) throw(Oracle::Error)
	: Stmt(db), nc(0), cnamem_(0), cnamev_(0), row_(0)
{
}


Oracle::Select_Stmt::Select_Stmt(Connection& db, const std::string& sql) throw(Oracle::Error)
	: Stmt(db, sql), nc(0), cnamem_(0), cnamev_(0), row_(0)
{
	// make sure this is actually a SELECT statement
	if (get_type() != OCI_STMT_SELECT)
	{
		OCIHandleFree((dvoid *)stmt_h, (ub4)OCI_HTYPE_STMT);
		delete stmt_p;
		st = Invalid;
		Type_Error e("Select_Stmt::Select_Stmt(Connection&, const std::string&)", "Statement is not a SELECT statement");
		e.desc << "statement = {" << sql << "}";
		throw e;
	}
}


Oracle::Select_Stmt::~Select_Stmt() throw()
{
	if (st > Closed)
		close();
}


void
Oracle::Select_Stmt::exec() throw(Oracle::Error)
{
	// Stmt::do_exec will validate state and set state to Executed if successful
	// state will only be changed if it is not already Executed or higher
	Stmt::do_exec(0);

	// if column name setup has already been done, we're finished
	if (cnamem_)
		return;

	// get number of columns returned
	ub4 col_count;
	if (OCIAttrGet(	(dvoid *) stmt_h,
			(ub4) OCI_HTYPE_STMT,
			(dvoid *) &col_count,
			(ub4 *) 0,
			(ub4) OCI_ATTR_PARAM_COUNT,
			err_h))
	{
		OCI_Error e("Select_Stmt::exec()", err_h);
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}
	nc = col_count;

	// set up vector of column names and map of column name -> number
	cnamem_ = new std::map<std::string, int>;		// new map for col name -> number
	cnamev_ = new std::vector<std::string>(nc);	// new vector of nc strings
	OCIParam* parm_h;
	text* col_name;
	ub4 col_name_len;

	for (int i=0; i < nc; i++)
	{
		// initialize handle pointer
		parm_h = (OCIParam*) 0;

		// get parameter
		if (OCIParamGet(stmt_h,
				(ub4) OCI_HTYPE_STMT,
				err_h,
				(dvoid **) &parm_h,
				(ub4) i + 1))							// position (one-based)
		{
			OCI_Error e("Select_Stmt::exec()", err_h);
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
		}

		// get column name and add it to col name vector and to col name -> number map
		if (OCIAttrGet(	(dvoid*) parm_h,
				(ub4) OCI_DTYPE_PARAM, 
				(dvoid*) &col_name,
				(ub4 *) &col_name_len,
				(ub4) OCI_ATTR_NAME, 
				err_h))
		{
			OCI_Error e("Select_Stmt::exec()", err_h);
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
		}
		(*cnamev_)[i] = std::string((char*)col_name, col_name_len);
		(*cnamem_)[std::string((char*)col_name, col_name_len)] = i;
	}
}


void
Oracle::Select_Stmt::bind_col(Oracle::Nullable& bindobj) throw(Oracle::Error)
{
	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
			throw State_Error("Select_Stmt::bind_col(Nullable&)", "No statement text has been specified");
	}
	if (st < Executed) // be consistent with other bind_col()
		exec();
	else if (st > Defined)
	{
		State_Error e("Select_Stmt::bind_col(Nullable&)", "Too late to bind a column");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// define by position based on size of bind handle list
	OCIDefine* def_h;
	if (OCIDefineByPos(
			stmt_h,						// stmt handle
			&def_h,						// define handle returned
			err_h,						// error handle
			def_l.size() + 1,				// position (one-based)
			(dvoid*) bindobj.data(),			// output buffer
			(sb4) bindobj.maxsize(),			// output buffer size
			(ub2) bindobj.type(),				// external data type
			(dvoid*) bindobj.ind_addr(),			// indicator
			(ub2*) 0,					// array of length values
			(ub2*) 0,					// array of return codes
			(ub4) OCI_DEFAULT))
	{
		OCI_Error e("Select_Stmt::bind_col(Nullable&)", err_h);
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// save the define handle
	def_l.push_back(def_h);

	// update the state
	st = Defined;
}


void
Oracle::Select_Stmt::bind_col(Oracle::Nullable* n ...) throw(Oracle::Error)
{
	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
			throw State_Error("Select_Stmt::bind_col(Nullable* ...)", "No statement text has been specified");
	}
	if (st < Executed) // must be executed so we know the # of columns
		exec();
	else if (st > Defined)
	{
		State_Error e("Select_Stmt::bind_col(Nullable* ...)", "Too late to bind a column");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	va_list varg;
	va_start(varg, n);
	OCIDefine* def_h;
	Nullable* bindobj(n);
	for (int i = 0; i < nc; i++)
	{
		if (OCIDefineByPos(
				stmt_h,					// stmt handle
				&def_h,					// define handle returned
				err_h,					// error handle
				def_l.size() + 1,			// position (one-based)
				(dvoid*) bindobj->data(),		// output buffer
				(sb4) bindobj->maxsize(),		// output buffer size
				(ub2) bindobj->type(),			// external data type
				(dvoid*) bindobj->ind_addr(),		// indicator
				(ub2*) 0,				// array of length values
				(ub2*) 0,				// array of return codes
				(ub4) OCI_DEFAULT))
		{
			OCI_Error e("Select_Stmt::bind_col(Nullable* ...)", err_h);
			e.desc << "statement = {" << stmt_p << "}; position = " << def_l.size() + 1;
			throw e;
		}

		// save the define handle
		def_l.push_back(def_h);
		
		// get next arg
		bindobj = va_arg(varg, Nullable*);
	}

	// update the state and clean up
	st = Defined;
	va_end(varg);
}


void
Oracle::Select_Stmt::bind_col(Oracle::Rowtype& row) throw(Oracle::Error)
{
	// check state
	if (st == Initialized)
	{
		if (std::ostringstream::str().length())
			prepare(std::ostringstream::str());
		else
			throw State_Error("Select_Stmt::bind_col(Nullable&)", "No statement text has been specified");
	}
	if (st < Executed) // must be executed so we know the columns
		exec();
	else if (st > Defined)
	{
		State_Error e("Select_Stmt::bind_col", "Too late to bind a column");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	// initialize Rowtype object if necessary
	if (row.col_vec == 0)
		row.init_data(*this);

	// define by position for each column
	OCIDefine* def_h;
	for (int i=0; i < nc; i++)
	{
		def_h = 0;
		if (OCIDefineByPos(
				stmt_h,						// stmt handle
				&def_h,						// define handle returned
				err_h,						// error handle
				i + 1,						// position (one-based)
				(dvoid*) row[i].data(),				// output buffer
				(sb4) row[i].maxsize(),				// output buffer size
				(ub2) row[i].type(),				// external data type
				(dvoid*) row[i].ind_addr(),			// indicator
				(ub2*) 0,					// array of length values
				(ub2*) 0,					// array of return codes
				(ub4) OCI_DEFAULT))
		{
			OCI_Error e("Select_Stmt::bind_col(Rowtype&)", err_h);
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
		}

		// save the define handle
		def_l.push_back(def_h);
	}

	// update the state
	st = Defined;
}


bool
Oracle::Select_Stmt::fetch() throw(Oracle::Error)
{
	// execute statement if not already done
	if (st < Executed)
		exec();

	// if columns not defined yet, create Rowtype object and bind to it
	// bind_col() will set the state to Defined if successful
	if (st == Executed)
	{
		row_ = new Rowtype(*this);
		bind_col(*row_);
	}

	switch (OCIStmtFetch(
			stmt_h,								// stmt handle
			err_h,								// error handle
			(ub4) 1,							// #rows to fetch
			(ub4) OCI_FETCH_NEXT,						// orientation
			(ub4) OCI_DEFAULT))						// mode
	{
		case OCI_SUCCESS:
		case OCI_SUCCESS_WITH_INFO:
			st = Fetched;
			return true;
		case OCI_NO_DATA:
			return false;
		default:
			OCI_Error e("Select_Stmt::fetch()", err_h);
			e.desc << "statement = {" << stmt_p << "}";
			throw e;
	}
}


void
Oracle::Select_Stmt::throw_subscript_error(const std::string& module) const throw(Oracle::Error)
{
	State_Error e(module, "Columns not defined internally");
	e.desc << "statement = {" << str() << "}";
	throw e;
}


std::string
Oracle::Select_Stmt::colname(const int n) const throw(Oracle::Error)
{
	if (st < Executed)
	{
		State_Error e("Select_Stmt::colname(const int)", "Statement not yet executed");
		e.desc << "statement = {" << stmt_p << "}";
		throw e;
	}

	if (n>=0 && n<nc)
		return (*cnamev_)[n];
	else
	{
		Value_Error e("Select_Stmt::colname(const int)", "Subscript out of range");
		e.desc << "statement = {" << stmt_p << "}; subscript = " << n;
		throw e;
	}
}


void
Oracle::Select_Stmt::close() throw()
{
	// ignore if already closed
	if (st == Closed)
		return;

	// release Rowtype object if necessary
	delete row_;

	// release the column name vector 
	delete cnamev_;

	// clear the define handle list
	def_l.clear();

	// release the column name -> number map
	if (cnamem_)
	{
		cnamem_->clear();
		delete cnamem_;
	}

	// release inherited objects; update state
	Stmt::close();
}
