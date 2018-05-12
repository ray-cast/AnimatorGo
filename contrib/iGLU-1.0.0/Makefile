TOP = .

include $(TOP)/configs/darwin-iphone

GLU_MAJOR = 1
GLU_MINOR = 3
GLU_TINY = 0$(MESA_MAJOR)0$(MESA_MINOR)0$(MESA_TINY)

INCDIRS = -Iinclude

C_SOURCES = \
	libutil/error.c		\
	libutil/glue.c		\
	libutil/project.c	\
	libutil/registry.c	\
	libtess/dict.c		\
	libtess/geom.c		\
	libtess/memalloc.c	\
	libtess/mesh.c		\
	libtess/normal.c	\
	libtess/priorityq.c	\
	libtess/render.c	\
	libtess/sweep.c		\
	libtess/tess.c		\
	libtess/tessmono.c

SOURCES = $(C_SOURCES)

C_OBJECTS = $(C_SOURCES:.c=.o)
OBJECTS = $(C_OBJECTS)


##### RULES #####

.c.o:
	$(CC) -c $(INCDIRS) $(CFLAGS) -DNDEBUG -DLIBRARYBUILD $< -o $@


##### TARGETS #####

.PHONY: install clean

$(GLU_LIB_NAME): $(OBJECTS)
	$(AR) -ru $(GLU_LIB_NAME) $(OBJECTS)

install: $(GLU_LIB_NAME)
	mkdir -p $(INSTALL_DIR)/include
	cp include/glu.h $(INSTALL_DIR)/include
	mkdir -p $(INSTALL_DIR)/lib
	cp $(GLU_LIB_NAME) $(INSTALL_DIR)/lib

clean:
	-rm -f */*.o $(GLU_LIB_NAME)
