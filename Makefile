ORAPP=libora++.so
ORAPPLIB=../../lib/$(ORAPP)

lib:	$(ORAPPLIB)

OBJS=Oracle.o \
	Env.o \
	Nullable.o \
	Number.o \
	Varchar.o \
	Date.o \
	Rowtype.o \
	Stmt.o \
	Select_Stmt.o \
	Cursor.o \
	Non_Sel_Stmt.o \
	Connection.o

$(ORAPPLIB):	$(ORAPP)
		cp -a $(ORAPP) $(ORAPPLIB)

$(ORAPP):	$(OBJS)
		g++ -shared -o $(ORAPP) $(OBJS)
#		g++ -shared -o $(ORAPP) -L/opt/STLport/lib $(OBJS) -lstlport_gcc

Oracle.o:	Oracle.cc Oracle.h

Env.o:		Env.cc Env.h Oracle.h

Nullable.o:	Nullable.cc Nullable.h Oracle.h

Varchar.o:	Varchar.cc Varchar.h Nullable.h Oracle.h

Number.o:	Number.cc Number.h Nullable.h Oracle.h

Date.o:		Date.cc Date.h Nullable.h Oracle.h Env.h

Rowtype.o:	Rowtype.cc Rowtype.h Oracle.h Nullable.h Varchar.h Number.h Stmt.h Select_Stmt.h Date.h

Connection.o:	Connection.cc Connection.h Stmt.h Select_Stmt.h Non_Sel_Stmt.h Oracle.h Env.h Rowtype.h

Stmt.o:		Stmt.cc Stmt.h Oracle.h Connection.h Nullable.h

Select_Stmt.o:	Select_Stmt.cc Select_Stmt.h Stmt.h Oracle.h Connection.h Nullable.h Varchar.h Rowtype.h

Cursor.o:	Cursor.cc Cursor.h Select_Stmt.h Stmt.h Oracle.h Connection.h Nullable.h Varchar.h Rowtype.h

Non_Sel_Stmt.o:	Non_Sel_Stmt.cc Non_Sel_Stmt.h Stmt.h Oracle.h Nullable.h Connection.h

#
# suffix rules
#
.SUFFIXES:
.SUFFIXES: .cc .o

.cc.o:
	g++ -c $(LCFLAGS) $(INCLUDE) $<

INCLUDE= -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public
#INCLUDE= -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I/opt/STLport/stlport
