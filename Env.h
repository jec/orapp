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

#ifndef ORAPP_ENV_H
#define ORAPP_ENV_H

class OCIEnv;
class OCIError;

namespace Oracle
{
	class Connection;
	class Date;
	class Number;
	
	class Env
	{
		public:
			// constructor/destructor
			Env()				throw(Error);
			~Env()				throw()
				{}
			
		private:
			// implementors
			static OCIEnv* env()		throw()
				{ return env_h; }
			static OCIError* err()		throw()
				{ return err_h; }
		
			// data members
			static bool inited;
			static OCIEnv* env_h;
			static OCIError* err_h;
			
		friend class Connection;
		friend class Date;
		friend class Number;
	};
	
	
}

#endif

