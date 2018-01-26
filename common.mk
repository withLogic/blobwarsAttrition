VERSION = 0.1
REVISION = $(shell git rev-list HEAD 2>/dev/null | wc -l)
LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

SEARCHPATH += src 
SEARCHPATH += src/combat 
SEARCHPATH += src/game 
SEARCHPATH += src/hub 
SEARCHPATH += src/system 
SEARCHPATH += src/util 
SEARCHPATH += src/widgets 
SEARCHPATH += src/world
SEARCHPATH += src/world/entities 
SEARCHPATH += src/world/entities/blobs 
SEARCHPATH += src/world/entities/boss

vpath %.c $(SEARCHPATH)
vpath %.h $(SEARCHPATH)

DEPS += defs.h structs.h

OBJS += atlas.o
OBJS += blaze.o boss.o blobBoss.o
OBJS += camera.o combat.o
OBJS += draw.o
OBJS += effects.o entities.o explosions.o eyeDroidCommander.o
OBJS += frost.o
OBJS += game.o
OBJS += hub.o hud.o
OBJS += init.o input.o io.o items.o
OBJS += lookup.o
OBJS += main.o map.o maths.o mia.o
OBJS += objectives.o
OBJS += particles.o player.o
OBJS += quadtree.o
OBJS += sound.o sprites.o
OBJS += tankCommander.o tankTrack.o teeka.o text.o textures.o title.o triggers.o
OBJS += unit.o util.o
OBJS += weapons.o widgets.o

# top-level rule to create the program.
all: $(PROG) $(LOCALE_MO)

# compiling other source files.
%.o: %.c %.h $(DEPS)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $<
	
%.mo: %.po
	msgfmt -c -o $@ $<
	
# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(PROG) $(LOCALE_MO)
