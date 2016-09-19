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

#ifndef ORAPP_CONNECTION_H
#define ORAPP_CONNECTION_H

#include "Oracle.h"
#include "Env.h"

class OCIEnv;
class OCIServer;
class OCISvcCtx;
class OCISession;
class OCIError;


namespace Oracle
{
	class Stmt;

	class Connection
	{
		public:
			// types
			enum status_t { not_connected, connected };

			// constructors/destructor
			Connection(
				const std::string&,					// username
				const std::string&,					// password
				const std::string& = "")		throw();	// database
			Connection(const std::string&)			throw();	// connect string
			virtual ~Connection()				throw(Error);

			// implementors
			virtual void open()				throw(Error);	// attach to server
			virtual Stmt* prepare(const std::string&)	throw(Error);	// execute a SQL statement
			virtual void rollback()				throw(Error);	// roll back transaction
			virtual void commit()				throw(Error);	// commit transaction
			virtual void close()				throw(Error);	// detach from server

		protected:
			// protected functions
			OCIEnv* env_handle() const			throw()
				{ return env_h; }
			OCISvcCtx* svc_handle() const			throw()
				{ return svc_h; }
			OCIError* err_handle() const			throw()
				{ return err_h; }

			// internal functions
			void init_handles()				throw(Error);
			void attach_server()				throw(Error);
			void log_on()					throw(Error);
			void log_off()					throw(Error);
			void detach_server()				throw(Error);
			void free_handles()				throw();
		
			// data members
			std::string uid;						// username
			std::string pw;							// password
			std::string sid;						// db name
			status_t stat;							// connection status
			OCIError* err_h;						// error handle
			OCIEnv* env_h;							// environment handle
			OCIServer* svr_h;						// server handle
			OCISvcCtx* svc_h;						// service context handle
			OCISession* ses_h;						// session handle

		private:
			static Env env_;						// initializes OCI environment

		friend class Stmt;
	};
}

#endif
