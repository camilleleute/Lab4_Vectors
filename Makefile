#       the shell commands that are needed to build that target
# the target is imageview, and it depends on the object file
# we need C++, the object file, -o imageview tells the compiler to put the output of this build into imageview target
# the stuff underneath is the recepie we need to build this target
# we need the executable to include the object file, C++, and the libraries
# since this is the linking stage and happens after the compiling, we don't need the flags here
# adding the @ silences all the build info when i type make
imageview: $(OBJS)
        @$(CXX) $(OBJS) -o imageview $(LIBS)

# to create the object file, it depends on the c++ file and the header file
# since this is what runs at compile time, we need to include the flags
# -c tells the compiler to just compile and stop after that, don't link yet
# -o <filename> tells the compiler to put the output of this build into imageview.o
imageview.o: imageview.cpp imageview.h
        $(CXX) $(CXXFLAGS) -c imageview.cpp -o imageview.o

# for cleaning
clean:
        rm -f imageview