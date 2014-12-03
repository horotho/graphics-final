EXE=final

# Main target
all: $(EXE)

#  Linux/Unix/Solaris
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm

#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a

# Dependencies
final.o: final.c
print.o: print.c
object.o: object.c
loadtex.o: loadtex.c 
fatal.o: fatal.c 
errcheck.o: errcheck.c 
particles.o: particles.c 
lodepng.o: lodepng.c

#  Create archive
CSCIx229.a:fatal.o loadtex.o print.o errcheck.o object.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
final: final.o lodepng.o particles.o CSCIx229.a
	gcc -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
