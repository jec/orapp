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
#include "Env.h"
#include <oci.h>


bool Oracle::Env::inited = false;

OCIEnv* Oracle::Env::env_h = 0;

OCIError* Oracle::Env::err_h = 0;


Oracle::Env::Env() throw(Oracle::Error)
{
	if (!inited)
	{
		// Initialize the OCI process just once.
		// Any class which calls OCI functions must contain a static Env object
		// to ensure the OCI environment is initialized.
		if (OCIInitialize(
				(ub4) OCI_OBJECT,				// mode
				(dvoid*) 0,					// user-def memory ptr
				0,						// user-def memory alloc function
				0,						// user-def memory realloc function
				0))						// user-def memory free function
			throw Error("Env::Env", "Could not initialize Oracle environment", __FILE__, __LINE__);

		// initialize the OCI environment handle
		if (OCIEnvInit(	&env_h,						// ptr to env handle inited
				(ub4) OCI_DEFAULT,				// mode
				(size_t) 0,					// user memory size
				(dvoid**) 0))					// user memory ptr
			throw Error("Env::Env", "Could not initialize Oracle environment", __FILE__, __LINE__);

		// allocate an error handle
		if (OCIHandleAlloc(
				(dvoid*) env_h,
				(dvoid**) &err_h,
				(ub4) OCI_HTYPE_ERROR,
				(size_t) 0,
				(dvoid**) 0))
			throw Error("Env::Env", "Could not initialize Oracle environment", __FILE__, __LINE__);
		inited = true;
	}
}
