noinst_PROGRAMS = re-ig
re_ig_SOURCES = src/main.c src/objects/camera.c src/objects/vec3f.c \
    src/sound.c src/objects/elevator.c src/objects/ceil_light.c \
    src/objects/crate.c src/objects/solar_system.c
AM_CPPFLAGS = -I$(srcdir)/include -I/usr/include/opus
AM_LDFLAGS = -lm -lglfw -lGL -lGLU -ldl -lopenal -lopusfile
