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
loadtexbmp.o: loadtexbmp.c
fatal.o: fatal.c
errcheck.o: errcheck.c
lodepng.o: lodepng.c
loadtex.o: loadtex.c
particles.o: particles.c
print.o: print.c

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
final: final.o loadtex.o fatal.o errcheck.o print.o lodepng.o particles.o 
	gcc -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
