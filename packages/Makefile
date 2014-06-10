# Download, unpack, compile and install all external packages required by Mu
#
MU_ROOT=/Users/r/Projects/Mu/usr

TAR=/usr/bin/tar

WGET-TARGET := $(MU_ROOT)/bin/wget
WGET-SOURCES := ./wget-1.15
WGET-TARBALL := ./wget-1.15.tar.tz
WGET-DISTRO := http://ftp.gnu.org/gnu/wget/wget-1.15.tar.gz

GSL-TARGET := $(MU_ROOT)/lib/libgsl.a
GSL-TARBALL := ./gsl-1.16.tar.gz
GSL-SOURCES := ./gsl-1.16
GSL-DISTRO := http://mirrors.zerg.biz/gnu/gsl/gsl-1.16.tar.gz

STK-TARGET := $(MU_ROOT)/lib/libstk.a
STK-TARBALL := ./stk-4.5.0.tar.gz
STK-SOURCES := ./stk-4.5.0
STK-DISTRO := https://ccrma.stanford.edu/software/stk/release/stk-4.5.0.tar.gz

FFTW-TARGET := $(MU_ROOT)/lib/libfftw3.a
FFTW-TARBALL := ./fftw-3.3.4.tar.gz
FFTW-SOURCES := ./fftw-3.3.4
FFTW-DISTRO := http://www.fftw.org/fftw-3.3.4.tar.gz

# ================================================================
# standard targets

all:  wget gsl stk fftw

clean:
	rm -rf $(WGET-TARBALL) $(WGET-SOURCES) $(GSL-TARBALL) $(GSL-SOURCES) $(STK-TARBALL) $(STK-SOURCES) $(FFTW-TARBALL) $(FFTW-SOURCES) 

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

stk : 	$(STK-TARGET)

# final "touch -c" is to make sure target has newer date than the source directory
$(STK-TARGET) : $(STK-SOURCES) | $(MU_ROOT)
	(cd $(<) && ./configure --prefix=$(MU_ROOT) && make && make install && make clean && touch -c $(STK-TARGET))

$(STK-SOURCES) : $(STK-TARBALL)
	$(TAR) xf $(STK-TARBALL)
	cd $(STK-SOURCES) && cp -p configure.ac configure.ac.orig && sed -f ../patch-stk-configure.sed configure.ac.orig > configure.ac
	cd $(STK-SOURCES) && autoconf

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

