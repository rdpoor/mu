# http://martinvseticka.eu/temp/make/presentation.html looks like a good tutorial.
#
# Strategy:
#
# ./usr is the "sandbox" for libraries, executables, etc.  These will be updated
# on a "make install" command:
# ./usr/bin     -- executables
# ./usr/include -- header files
# ./usr/lib     -- archive files
# ./usr/share   -- documentation
#
# ./doc contains documentation
# ./examples contains self-contained example files
# ./examples/binary_sources contain any binary files required by ./examples
# ./include contains the core header files (copied into ./usr/include)
# ./sketches contains programming sketches (not for distribution)
# ./src contains the core source files (that compile into ./usr/lib/libmu.a)
# ./tarballs contains external dependencies (stk, gsl)
# ./test contains unit tests
# ./test/binary_sources contain any binary files required by ./test
# ./utilities contains self-contained utility programs
#
# Some targets:
#
# make install => [src tarballs utilities docs] make install
# make run_tests => make install, make tests, 
# make examples => make install, [examples] make

export MU_ROOT=/Users/r/Projects/Mu/usr

all :
	cd packages && make all
	cd src && make all

install :
	make all
	cd src && make install


clean :
	cd examples && make clean
	cd src && make clean
	cd sketches && make clean
	cd test && make clean
	cd utilities && make clean

# Prepare to rebuild from scratch
scour :
	make clean
	cd packages && make scour
	rm -rf $(MU_ROOT)

packages :
	cd packages && make install

tests :
	cd test && make run_tests

testing :
	cd doc && make test

