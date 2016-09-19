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

#ifndef ORAPP_STMT_H
#define ORAPP_STMT_H

#include "Oracle.h"
#include <sstream>
#include <list>
#include <queue>

class OCIEnv;
class OCISvcCtx;
class OCIStmt;
class OCIBind;
class OCIError;


namespace Oracle
{
	class Connection;
	class Nullable;
	class Rowtype;
	class Stmt;
	class Cursor;
	const char stmt_idx[] = { 106, 101, 99, 97, 105, 110, 0 };

	struct Stmt_Null_Manip
	{
		// constructor/destructor
		Stmt_Null_Manip(std::ostream& (*f)(std::ostream&, Nullable&), Nullable& o) : func(f), obj(o) {}
		~Stmt_Null_Manip() {}
		
		// data members
		std::ostream& (*func)(std::ostream&, Nullable&);	// ptr to function to invoke
		Nullable& obj;						// object to pass to function
	};

	struct Stmt_Row_Manip
	{
		// constructor/destructor
		Stmt_Row_Manip(std::ostream& (*f)(std::ostream&, Rowtype&), Rowtype& o) : func(f), obj(o) {}
		~Stmt_Row_Manip() {}
		
		// data members
		std::ostream& (*func)(std::ostream&, Rowtype&);		// ptr to function to invoke
		Rowtype& obj;						// object to pass to function
	};

	class Stmt: public std::ostringstream
	{
		public:
			// types
			enum stmt_t { Unknown, Select, Non_Select };
			enum state_t { Invalid, Closed, Initialized, Prepared, Executed, Defined, Fetched };

			// constructors/destructor
			virtual ~Stmt()					throw();

			// implementors
			virtual void prepare()				throw(Error);	// prepare stream text
			virtual void prepare(const std::string&)	throw(Error);	// prepare specified text
			virtual void bind(Nullable&)			throw(Error);	// bind by pos
			virtual void bind(Nullable&, const std::string&) throw(Error);	// bind by name
			virtual void bind_q(Nullable&)			throw(Error);	// add placeholder
			virtual void bind_q(Rowtype&)			throw(Error);	// add placeholder
			virtual void exec() 				throw(Error)= 0; // execute
			virtual void close()				throw();	// release resources

			// accessors
			int nrows() const				throw(Error);	// returns #rows affected
			std::string str() const				throw();	// statement text
			state_t state() const				throw()		// state
				{ return st; }
			virtual stmt_t type() const			throw() = 0;	// statement type
			int oci_type() const				throw(Error);	// get OCI stmt type
			int sql_fcode() const				throw(Error);	// get SQL fcn code

		protected:
			// protected constructors
			Stmt()						throw(Error);
			Stmt(Connection&)				throw(Error);	// can't use for a Stmt object directly
			Stmt(Connection&, const std::string&)		throw(Error);	// can't use for a Stmt object directly
			Stmt(	OCIStmt* stmt_hdl,					// statement handle
				char* stmt_ptr,						// statement text
				OCISvcCtx* svc_hdl,					// service context handle
				OCIError* err_hdl)			throw();	// error handle

			// protected accessors
			OCIStmt* stmt_handle() const			throw()		// get statement handle
				{ return stmt_h; }
			OCIError* err_handle() const			throw()		// get error handle
				{ return err_h; }

			// internal functions
			void do_exec(const int iter)			throw(Error);	// execute statement

			// data members
			state_t st;							// state
			char *stmt_p;							// ptr to statement text
			OCIStmt* stmt_h;						// statment handle
			OCISvcCtx* svc_h;						// service handle
			OCIError* err_h;						// error handle
			std::list<OCIBind*> bind_l;					// list of bind variables
			std::queue<Nullable*> bind_q_;					// queue of objects to bind

		friend class Rowtype;
		friend std::ostream& operator<<(std::ostream& s, const Stmt_Null_Manip& m)
			{ return m.func(s, m.obj); }
		friend std::ostream& operator<<(std::ostream& s, const Stmt_Row_Manip& m)
			{ return m.func(s, m.obj); }
	};
	
	inline std::ostream& bind_null_manip(std::ostream& s, Nullable& n)
		{ dynamic_cast<Stmt&>(s).bind_q(n); return s; }
	
	inline Stmt_Null_Manip bind(Nullable& n)
		{ return Stmt_Null_Manip(bind_null_manip, n); }
	
	inline std::ostream& bind_row_manip(std::ostream& s, Rowtype& r)
		{ dynamic_cast<Stmt&>(s).bind_q(r); return s; }
	
	inline Stmt_Row_Manip bind(Rowtype& r)
		{ return Stmt_Row_Manip(bind_row_manip, r); }
}

#endif
