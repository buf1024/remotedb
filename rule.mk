.PRECIOUS:%.cpp %.cc
.SUFFIXES:
.SUFFIXES: .o .cpp .cc

.cpp.o:
	$(CXXCOMPILE) $(EXT_CPPFLAGS) -c -o $@   $<

.cc.o:
	$(CXXCOMPILE) $(EXT_CPPFLAGS) -c -o $@   $<


