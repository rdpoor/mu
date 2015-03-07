# Download, unpack, compile and install all external packages required by Mu
#
MU_ROOT = /Users/r/Projects/Mu/usr
GMOCK_DIR ?= /Users/r/Projects/Mu/packages/gmock-1.7.0

TAR=/usr/bin/tar

WGET-TARGET := $(MU_ROOT)/bin/wget
WGET-SOURCES := ./wget-1.15
WGET-TARBALL := ./wget-1.15.tar.tz
WGET-DISTRO := http://ftp.gnu.org/gnu/wget/wget-1.15.tar.gz

GSL-TARGET := $(MU_ROOT)/lib/libgsl.a
GSL-TARBALL := ./gsl-1.16.tar.gz
GSL-SOURCES := ./gsl-1.16
GSL-DISTRO := http://mirrors.zerg.biz/gnu/gsl/gsl-1.16.tar.gz

STK-TARGET := $(MU_ROOT)/lib/libstk.dylib
STK-TARBALL := ./stk-4.5.0.tar.gz
STK-SOURCES := ./stk-4.5.0
STK-DISTRO := https://ccrma.stanford.edu/software/stk/release/stk-4.5.0.tar.gz

FFTW-TARGET := $(MU_ROOT)/lib/libfftw3.a
FFTW-TARBALL := ./fftw-3.3.4.tar.gz
FFTW-SOURCES := ./fftw-3.3.4
FFTW-DISTRO := http://www.fftw.org/fftw-3.3.4.tar.gz

HIREDIS-TARGET := $(MU_ROOT)/lib/libhiredis.dylib
HIREDIS-TARBALL := ./hiredis-0.11.0.tar.gz
HIREDIS-SOURCES := ./hiredis-0.11.0
HIREDIS-DISTRO := https://github.com/redis/hiredis/archive/v0.11.0.tar.gz

JANSSON-TARGET := $(MU_ROOT)/lib/libjansson.a
JANSSON-SOURCES := ./jansson-2.7
JANSSON-TARBALL := ./jansson-2.7.tar.gz
JANSSON-DISTRO := http://www.digip.org/jansson/releases/jansson-2.7.tar.gz

CMAKE-TARGET := $(MU_ROOT)/bin/cmake
CMAKE-SOURCES := ./cmake-3.1.0
CMAKE-TARBALL := ./cmake-3.1.0.tar.gz
CMAKE-DISTRO := http://www.cmake.org/files/v3.1/cmake-3.1.0.tar.gz

GMOCK-TARGET := $(MU_ROOT)/lib/libgmock.a
GMOCK-MAIN-TARGET := $(MU_ROOT)/lib/gmock_main.a
GMOCK-SOURCES := ./gmock-1.7.0
GMOCK-TARBALL := ./gmock-1.7.0.zip
GMOCK-DISTRO := https://googlemock.googlecode.com/files/gmock-1.7.0.zip

# ================================================================
# standard targets

all:  wget gsl stk fftw hiredis jansson

# all:  wget gsl stk fftw hiredis jansson gmock

# remove build artifacts, but keep tarballs
clean:
	(test -e $(WGET-SOURCES) && cd $(WGET-SOURCES) && make clean)
	(test -e $(GSL-SOURCES) && cd $(GSL-SOURCES) && make clean)
	(test -e $(STK-SOURCES) && cd $(STK-SOURCES) && make clean)
	(test -e $(FFTW-SOURCES) && cd $(FFTW-SOURCES) && make clean)
	(test -e $(GMOCK-SOURCES) && cd $(GMOCK-SOURCES) && make clean)

# reset everything.
scour:
	make clean
	rm -rf $(WGET-TARBALL) $(GSL-TARBALL) $(STK-TARBALL) $(FFTW-TARBALL) $(GMOCK-TARBALL)

# ================================================================
# MU_ROOT is the install directory

$(MU_ROOT):
	mkdir -p $(MU_ROOT)

# ================================================================
# wget

wget: $(WGET-TARGET)

$(WGET-TARGET) : $(WGET-SOURCES) | $(MU_ROOT)
	(cd $< && ./configure --prefix=$(MU_ROOT) --with-ssl=openssl && make && make install && make clean && touch -c $(WGET-TARGET))

$(WGET-SOURCES) : $(WGET-TARBALL)
	$(TAR) xf $(WGET-TARBALL)

$(WGET-TARBALL) :
	curl $(WGET-DISTRO) -o $(WGET-TARBALL)

# ================================================================
# gnu scientific library

gsl : 	$(GSL-TARGET)

# final "touch -c" is to make sure target has newer date than the source directory
$(GSL-TARGET) : $(GSL-SOURCES) | $(MU_ROOT)
	(cd $< && ./configure --prefix=$(MU_ROOT) && make && make install && make clean && touch -c $(GSL-TARGET))

$(GSL-SOURCES) : $(GSL-TARBALL)
	$(TAR) xf $(GSL-TARBALL)

$(GSL-TARBALL) : | wget
	wget -O $(GSL-TARBALL) $(GSL-DISTRO)

# ================================================================
# stk: synthesis tool kit
# 
# BUG: As written, fails with:
# install -m 644 libstk.dylib.4.4.4 /Users/r/Projects/Mu/usr/lib
# install: libstk.dylib.4.4.4: No such file or directory
# (should be install -m libstk.4.4.4.dylib /Users/r/Projects/Mu/usr/lib)

stk : 	$(STK-TARGET)

# final "touch -c" is to make sure target has newer date than the source directory
# monkey patch the Makefile after creating it to create an absolute path in the dylib
$(STK-TARGET) : $(STK-SOURCES) | $(MU_ROOT)
	(cd $(<) && ./configure --prefix=$(MU_ROOT) --enable-debug)
	# (cd $(STK-SOURCES)/src && cp -p Makefile Makefile.orig && sed -f ../../patch-stk-src-makefile.sed Makefile.orig > Makefile )
	(cd $(<) && make && make install && make clean && touch -c $(STK-TARGET))

# monkey patch configure.ac to get the right name in the .dylib file
$(STK-SOURCES) : $(STK-TARBALL)
	$(TAR) xf $(STK-TARBALL)
	# cd $(STK-SOURCES) && cp -p configure.ac configure.ac.orig && sed -f ../patch-stk-configure.sed configure.ac.orig > configure.ac

$(STK-TARBALL) : | wget
	wget -O $(STK-TARBALL) $(STK-DISTRO)

# ================================================================
# Fastest Fourier Transform in the West.

fftw : 	$(FFTW-TARGET)

# final "touch -c" is to make sure target has newer date than the source directory
$(FFTW-TARGET) : $(FFTW-SOURCES) | $(MU_ROOT)
	(cd $< && ./configure --prefix=$(MU_ROOT) && make && make install && make clean && touch -c $(FFTW-TARGET))

$(FFTW-SOURCES) : $(FFTW-TARBALL)
	$(TAR) xf $(FFTW-TARBALL)

$(FFTW-TARBALL) : | wget
	wget -O $(FFTW-TARBALL) $(FFTW-DISTRO)

# ================================================================
# Redis for pub/sub ipc

hiredis : 	$(HIREDIS-TARGET)

# final "touch -c" is to make sure target has newer date than the source directory
$(HIREDIS-TARGET) : $(HIREDIS-SOURCES) | $(MU_ROOT)
	(cd $< && \
	PREFIX=$(MU_ROOT) make && \
	PREFIX=$(MU_ROOT) make install && \
	PREFIX=$(MU_ROOT) make clean && \
	touch -c $(HIREDIS-TARGET))

$(HIREDIS-SOURCES) : $(HIREDIS-TARBALL)
	$(TAR) xf $(HIREDIS-TARBALL)

$(HIREDIS-TARBALL) : | wget
	wget -O $(HIREDIS-TARBALL) $(HIREDIS-DISTRO)

# ================================================================
# JANSSON JSON->C parser

jansson : 	$(JANSSON-TARGET)

$(JANSSON-TARGET) : $(JANSSON-SOURCES) | $(MU_ROOT)
	(cd $< && ./configure --prefix=$(MU_ROOT) && make && make install && make clean && touch -c $(JANSSON-TARGET))

$(JANSSON-SOURCES) : $(JANSSON-TARBALL)
	$(TAR) xf $(JANSSON-TARBALL)

$(JANSSON-TARBALL) : | wget
	wget -O $(JANSSON-TARBALL) $(JANSSON-DISTRO)

# ================================================================
# CMAKE neo-make facility

cmake : 	$(CMAKE-TARGET)

$(CMAKE-TARGET) : $(CMAKE-SOURCES) | $(MU_ROOT)
	(cd $< && ./bootstrap --prefix=$(MU_ROOT) && make && make install && make clean && touch -c $(CMAKE-TARGET))

$(CMAKE-SOURCES) : $(CMAKE-TARBALL)
	$(TAR) xf $(CMAKE-TARBALL)

$(CMAKE-TARBALL) : | wget
	wget -O $(CMAKE-TARBALL) $(CMAKE-DISTRO)

# ================================================================
# GMOCK Google Test unit test framework

# BUG: Fails as written with 
# clang: error: no such file or directory: '/Users/r/Projects/Mu/Packages/gtest-1.7.0/src/gtest-all.cc'
#
# Slightly nonstandard.  Ends up creating:
#   $(MU_ROOT)/lib/libgmock.a
#   $(MU_ROOT)/lib/gmock_main.a
#   $(MU_ROOT)/include/gmock/*.h
#   $(MU_ROOT)/include/gtest/*.h
#
# To compile a test (note inclusion of gmock_main.a from lib directory):
#
# g++ -isystem ${MU_ROOT}/include -L${MU_ROOT}/lib -g -Wall -Wextra -pthread \
# 	my_test.cc -o my_test -lgmock ${MU_ROOT}/lib/gmock_main.a
#
gmock : 	$(GMOCK-TARGET) $(GMOCK-MAIN-TARGET)

$(GMOCK-TARGET) : $(GMOCK-SOURCES) | $(MU_ROOT)
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -isystem $(GMOCK_DIR)/include -I$(GMOCK_DIR) \
		-pthread -c $(GTEST_DIR)/src/gtest-all.cc -o /tmp/gtest-all.o
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -isystem $(GMOCK_DIR)/include -I$(GMOCK_DIR) \
		-pthread -c $(GMOCK_DIR)/src/gmock-all.cc -o /tmp/gmock-all.o
	cp -rp $(GTEST_DIR)/include/gtest $(MU_ROOT)/include
	cp -rp $(GMOCK_DIR)/include/gmock $(MU_ROOT)/include
	ar -rv $@ /tmp/gtest-all.o /tmp/gmock-all.o

$(GMOCK-SOURCES) : $(GMOCK-TARBALL)
	$(TAR) xf $(GMOCK-TARBALL)

$(GMOCK-TARBALL) : | wget
	wget -O $(GMOCK-TARBALL) $(GMOCK-DISTRO)

$(GMOCK-MAIN-TARGET) : $(GMOCK-SOURCES) | $(MU_ROOT)
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -isystem $(GMOCK_DIR)/include -I$(GMOCK_DIR) \
		-pthread -c $(GMOCK_DIR)/src/gmock_main.cc -o /tmp/gmock_main.o
	ar -rv $@ /tmp/gmock_main.o

# ================================================================
# GTEST Google Test unit test framework

gtest : 	$(GTEST-TARGET)

$(GTEST-TARGET) : $(GTEST-SOURCES) | $(MU_ROOT)
	(cd $< && g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc && ar -rv libgtest.a gtest-all.o)

$(GTEST-SOURCES) : $(GTEST-TARBALL)
	$(TAR) xf $(GTEST-TARBALL)

$(GTEST-TARBALL) : | wget
	wget -O $(GTEST-TARBALL) $(GTEST-DISTRO)

