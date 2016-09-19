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

#include "Oracle.h"
#include "Rowtype.h"
#include "Nullable.h"
#include "Varchar.h"
#include "Number.h"
#include "Date.h"
#include "Select_Stmt.h"
#include <oci.h>


Oracle::Rowtype::Rowtype(const Select_Stmt& stmt) throw(Oracle::OCI_Error, Oracle::Type_Error)
	: stmt_h(stmt.stmt_handle()),
	err_h(stmt.err_handle()),
	col_vec(new std::vector<Nullable*>(stmt.ncols())),		// vector of ptrs to Nullables
	col_name_m(new std::map<std::string, int>(*stmt.cnamem_)),	// map of column name -> position
	col_name(new std::vector<std::string>(stmt.ncols()))		// vector of column names
{
	// create map of column names and array of column values for fetch
	for (std::map<std::string, int>::const_iterator i = col_name_m->begin(); i != col_name_m->end(); i++)
		(*col_name)[i->second] = i->first;

	OCIParam* parm_h;
	//OCIDefine* def_h;
	short precision(0);
	short scale(0);
	ub2 col_type;
	ub2 col_size;

	for(int i=0; i < stmt.ncols(); i++)
	{
		// initialize handle pointers
		parm_h = (OCIParam*) 0;
		//def_h = (OCIDefine*) 0;

		// get parameter
		if(OCIParamGet(	stmt_h,
				(ub4) OCI_HTYPE_STMT,
				err_h,
				(dvoid **) &parm_h,
				(ub4) i + 1))							// position (one-based)
			throw OCI_Error("Rowtype::Rowtype", err_h);

		// get column type
		if(OCIAttrGet(	(dvoid*) parm_h,
				(ub4) OCI_DTYPE_PARAM, 
				(dvoid*) &col_type,
				(ub4 *) 0,
				(ub4) OCI_ATTR_DATA_TYPE, 
				err_h))
			throw OCI_Error("Rowtype::Rowtype", err_h);

		// get column scale and precision for numbers; else get size
		if (col_type == 2)
		{
			if(OCIAttrGet(	(dvoid*) parm_h,
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &precision,
					(ub4 *) 0,
					(ub4) OCI_ATTR_PRECISION, 
					err_h))
				throw OCI_Error("Rowtype::Rowtype", err_h);
			if(OCIAttrGet(	(dvoid*) parm_h,
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &scale,
					(ub4 *) 0,
					(ub4) OCI_ATTR_SCALE, 
					err_h))
				throw OCI_Error("Rowtype::Rowtype", err_h);
		}
		else
			if(OCIAttrGet(	(dvoid*) parm_h,
					(ub4) OCI_DTYPE_PARAM, 
					(dvoid*) &col_size,
					(ub4 *) 0,
					(ub4) OCI_ATTR_DATA_SIZE, 
					err_h))
				throw OCI_Error("Rowtype::Rowtype", err_h);

		// create an appropriate Nullable object
		switch(col_type)
		{
			case 1:		// VARCHAR2
			case 11:	// ROWID
			case 96:	// CHAR
				(*col_vec)[i] = new Varchar(col_size);
				break;

			case 2:		// NUMBER
				(*col_vec)[i] = new Number;
				break;

			case 12:	// DATE
				// dates are length 9 when fetched into a string (dd-mmm-yy)
				//(*col_vec)[i] = new Varchar(9);
				(*col_vec)[i] = new Date;
				break;

			default:
				throw Type_Error("Rowtype::Rowtype", "Unsupported Oracle internal data type");
		}
	}
}


Oracle::Rowtype::~Rowtype() throw()
{
	// release any allocated Nullable objects
	if (col_vec)
	{
		for (int i=0; i<col_vec->size(); i++)
			delete (*col_vec)[i];
		delete col_vec;
	}

	// release the column name map
	if (col_name_m)
	{
		col_name_m->clear();
		delete col_name_m;
	}
}


void
Oracle::Rowtype::set_null() throw()
{
	if (col_vec)
		for (int i=0; i < col_vec->size(); i++)
			(*col_vec)[i]->set_null();
}


Oracle::Nullable&
Oracle::Rowtype::operator[](const int i) throw(Oracle::Value_Error)
{
	if (col_vec && i >= 0 && i < col_vec->size())
		return *(*col_vec)[i];
	else
	{
		std::ostringstream s;
		s << "Subscript out of range [" << i << "]";
		throw Value_Error("Rowtype::operator[]", s.str());
	}
}


Oracle::Nullable&
Oracle::Rowtype::operator[](const std::string& s) throw(Oracle::Value_Error)
{
	if (col_name_m->count(s))
		return *(*col_vec)[(*col_name_m)[s]];
	else
		throw Value_Error("Rowtype::operator[]", "Column does not exist [" + s + "]");
}


const Oracle::Nullable&
Oracle::Rowtype::operator[](const int i) const throw(Oracle::Value_Error)
{
	if (col_vec && i >= 0 && i < col_vec->size())
		return *(*col_vec)[i];
	else
	{
		std::ostringstream s;
		s << "Subscript out of range [" << i << "]";
		throw Value_Error("Rowtype::operator[]", s.str());
	}
}


const Oracle::Nullable&
Oracle::Rowtype::operator[](const std::string& s) const throw(Oracle::Value_Error)
{
	if (col_name_m->count(s))
		return *(*col_vec)[(*col_name_m)[s]];
	else
		throw Value_Error("Rowtype::operator[]", "Subscript out of range");
}


int
Oracle::Rowtype::ncols() const throw()
{
	if (col_vec)
		return col_vec->size();
	else
		return 0;
}


std::string
Oracle::Rowtype::colname(const int i) const throw(Oracle::Value_Error)
{
	if (col_name && (i >= 0) && (i < col_name->size()))
		return (*col_name)[i];
	else
		throw Value_Error("Rowtype::colname", "Subscript out of range");
}


std::ostream&
Oracle::operator<<(std::ostream& os, const Oracle::Rowtype& rt) throw()
{
	bool first_flag = false;
	for (int i=0; i < rt.ncols(); i++)
	{
		if (first_flag)
			os << ", ";
		else
			first_flag = true;
		os << rt.colname(i) << "=" << rt[i].str("<NULL>");
	}

	return os;
}
