# Ora++

Ora++ is a set of C++ classes wrapped around the Oracle Call Interface (OCI).
They provide an object-oriented interface to Oracle similar to other OO
interfaces like JDBC or PERL.

A simple query using Ora++ looks like this:

```C++
Connection db("user/pass@db");
Varchar name("Fred");
Select_Stmt st(db);
st << "SELECT City, State \
       FROM Friends \
       WHERE Name = " << bind(name);
while(st.fetch())
    cout << st["CITY"] << ", " << st["STATE"] << endl;
```

Here is a slightly more elaborate--but no less contrived--example:

```C++
// create connection
Connection db("/");

// prepare billing query
Select_Stmt sel_billing(db, "\
   SELECT Customer_ID, Amount \
   FROM Billings \
   WHERE Status = 'PAID'");
Rowtype row;
sel_billing.bind_col(row);

// prepare update to account
Non_Sel_Stmt upd_account(db);
upd_account << "\
   UPDATE Account \
   SET Paid_To_Date = NVL(Paid_To_Date, 0) + " << bind(row["AMOUNT"]) << " \
   WHERE Customer_ID = " << bind(row["CUSTOMER_ID"]);

// for each billing record that is paid...
while (sel_billing.fetch())
   // ...update the customer record
   upd_account.exec();
```

I wrote the initial version of this library over a weekend after wrangling with
Pro*C in a C++ environment. Being somewhat of an idealist, I was frustrated by
being forced into an anti-OO way of doing things. Since I could not find
something similar on the Internet, I decided to write my own.

## Status

This library has not been developed since 2001, and it probably won't even
compile on a modern system. I'm uploading this to GitHub for archiving
purposes.

There is at least one other library out there called Ora++, but it is
unrelated.

## Features

* NULL-aware variable classes Number, Varchar and Date are provided. The Number
  and Date classes are implemented using the OCI data types.
* Common arithmetic and comparison operators are defined for the variable
  classes.
* Date arithmetic is provided using OCI functions, including such functions as
  ADD_MONTHS(), MONTHS_BETWEEN() and LAST_DAY().
* Variable classes include member functions for conversion to long integers,
  double floats and strings. Default values may be provided for NULLs. Standard
  Oracle formats can be used to convert Date and Number objects to strings.
* A collection class (Rowtype) is provided for the variable classes. Objects in
  the collection may be created manually or automatically according to the
  columns in a SELECT list.
* Statements may be built as output streams, and host variables may be bound
  using stream manipulators.
* Cursor references are fully supported.

## Limitations

* User-defined object types are not yet supported, but full OCI object support
  is planned.
* LOBs, CLOBs and BLOBs are not supported but are planned.
* The library was not written with multithreading in mind. As a result, it is
  not known whether it is thread-safe. This will be addressed eventually.
* I also plan to support arrays soon.

## Known Bugs

* The Number class currently uses a default format string of "TM" to convert a
  value to a string. This is used either when the str() method is called with
  no format or when a Number object is output to a stream. Unfortunately, the
  "TM" format is not available in versions of Oracle prior to 8.1.5. The
  workaround is to either specify your own format string or use the lng() or
  dbl() methods.

## Requirements

* An Oracle8 client library is required for building the library and any client
  programs which use it.
* This library has been developed on GNU/Linux and SunOS/Solaris platforms. The
  Makefiles that are provied are adequate for these two platforms.
  Unfortunately, for now, those building on other platforms are on their own.
* A modern, (nearly) ISO-standard C++ implementation is required, including the
  STL. In particular, the std::ostringstream class is used quite a bit,
  something that was missing from the GNU library until recently. I am currently
  using GNU release 2.95.3, and it works well.

## User Guide

If you would like more information about how to use Ora++, see the user guide.
It has been updated for version 1.2.0 but is still missing some details, such
as information about the bind() manipulator's support classes and functions. I
also plan to add example code as time allows.

## Licenses

The Ora++ Library is licensed under the GNU Lesser General Public License, and
the documentation is licensed under the GNU Free Documentation License.

## Change Log

You may view the current change log.
