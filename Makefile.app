PREFIX=/home/jec
ORAPPLIB=$(PREFIX)/lib
ORAPPINC=$(PREFIX)/src/ora++

all:	test

include $(ORACLE_HOME)/rdbms/lib/env_rdbms.mk

test:	test.o
	g++ $(LDFLAGS) -L$(ORAPPLIB) -L/opt/STLport/lib -o test test.o -lora++ -lstlport_gcc $(OCISHAREDLIBS)

#
# suffix rules
#
.SUFFIXES:
.SUFFIXES: .cc .o

.cc.o:
	g++ -I$(ORAPPINC) -c $(LCFLAGS) $(INCLUDE) $<

#
# Oracle stuff
#
RDBMSLIB=$(ORACLE_HOME)/rdbms/lib/
LDFLAGS=-L$(LIBHOME) -L$(ORACLE_HOME)/rdbms/lib
LLIBPSO=`cat $(ORACLE_HOME)/rdbms/lib/psoliblist`
INCLUDE= -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I/opt/STLport/stlport
CONFIG = $(ORACLE_HOME)/rdbms/lib/config.o
OCISHAREDLIBS=$(TTLIBS) $(LLIBTHREAD)
OCISTATICLIBS=$(STATICTTLIBS) $(LLIBTHREAD)
PSOLIBLIST=$(ORACLE_HOME)/rdbms/lib/psoliblist
CLEANPSO=rm -f $(PSOLIBLIST); $(GENPSOLIB)
DOLIB=$(ORACLE_HOME)/lib/liborcaccel.a
DUMSDOLIB=$(ORACLE_HOME)/lib/liborcaccel_stub.a
REALSDOLIB=/usr/lpp/orcaccel/liborcaccel.a
PROC=$(ORACLE_HOME)/bin/proc
PCCINCLUDE= include=$(ORACLE_HOME)/precomp/public
PCCI=-I$(ORACLE_HOME)/precomp/public
PCCPLSFLAGS= $(PCCINCLUDE) ireclen=132 oreclen=132 sqlcheck=full \
ltype=none user=$(USERID)
LLIBSQL= -lsql8
PROLDLIBS= $(LLIBSQL) $(TTLIBS)
