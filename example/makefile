CC = gcc
CCFLAGS_D = -Darray_size_t=u32 -DLINUX -DVIOLET_NO_LIB
CCFLAGS_W = -Wall -Werror -Wshadow -Wno-missing-braces -Wno-unused-local-typedefs
CCFLAGS_I = -I../../ -I/usr/include/ -I/usr/local/include/SDL2/
CCFLAGS_DEBUG = -g -g3 -DDEBUG
CCFLAGS_RELEASE = -O2 -DNDEBUG -DPROFILE
CCFLAGS = -std=gnu99 $(CCFLAGS_DEBUG) $(CCFLAGS_D) $(CCFLAGS_W) $(CCFLAGS_I)
LFLAGS = -lGL -lGLEW -lm -lSDL2 -lsystemd

gui: gui.c ../gui.h ../sdl_gl.h
	$(CC) $(CCFLAGS) -o gui gui.c $(LFLAGS)

.PHONY: clean
clean:
	rm -f gui
