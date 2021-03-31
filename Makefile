FILE=skeleton
GLMDIR=include/glm/
SDLDIR=include/sdl2/
OCLDIR=include/cl/
CLDIR=../

########
#   Directories
S_DIR=Source/src
B_DIR=build

########
#   Output
EXEC=$(B_DIR)/$(FILE)
TEST_EXEC=$(B_DIR)/$(FILE)_TEST

# default build settings
CC_OPTS=-c -pipe -Wno-switch -ggdb -g3 -fopenmp -lpthread -Wfatal-errors -lm
LN_OPTS= -fopenmp -lpthread  -lm
CC=g++-8

PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	CC_OPTS += -framework OpenCL
	LN_OPTS += -framework OpenCL
else
	CC_OPTS += -lOpenCL
	LN_OPTS += -lOpenCL
endif

########
#       SDL options
SDL_CFLAGS := $(shell sdl2-config --cflags)
GLM_CFLAGS := -I$(GLMDIR) -I${SDLDIR} -I${OCLDIR}
CL_CFLAGS := -I$(CLDIR)
SDL_LDFLAGS := $(shell sdl2-config --libs)
DEPENDENCY_FLAGS = $(SDL_CFLAGS) $(GLM_CFLAGS) $(CL_CFLAGS)

########
#   This is the default action
all:Build


######
# Get all the source files 4 directories deep
CPPS = $(wildcard ${S_DIR}/*.cpp) $(wildcard ${S_DIR}/*/*.cpp) $(wildcard ${S_DIR}/*/*/*.cpp) $(wildcard ${S_DIR}/*/*/*/*.cpp)
OLD_HEADERS = $(wildcard ${S_DIR}/*.h) $(wildcard ${S_DIR}/*/*.h) $(wildcard ${S_DIR}/*/*/*.h) $(wildcard ${S_DIR}/*/*/*/*.h)
HEADERS = $(wildcard ${S_DIR}/*.hpp) $(wildcard ${S_DIR}/*/*.hpp) $(wildcard ${S_DIR}/*/*/*.hpp) $(wildcard ${S_DIR}/*/*/*/*.hpp)
SOURCES = ${CPPS} Source/Main.cpp
TEST_SOURCES = ${CPPS} Source/Test.cpp

########
#   Object list
OBJECTS = $(patsubst Source%.cpp,${B_DIR}%.o,$(SOURCES))
TEST_OBJECTS = $(patsubst Source}%.cpp,${B_DIR}%.o,$(TEST_SOURCES))


${B_DIR}/%.o: Source/%.cpp ${OLD_HEADERS} ${HEADERS}
	@echo ${OLD_HEADERS}
	@echo build $@ from $<
	mkdir -p ${dir $@}
	${CC} ${CC_OPTS} $< ${DEPENDENCY_FLAGS} -o $@


########
#   Main build rule
Build : ${OBJECTS} Makefile
	$(CC) $(LN_OPTS) -o $(EXEC) $(OBJECTS) $(SDL_LDFLAGS)

BuildTest : ${TEST_OBJECTS} Makefile
	$(CC) $(LN_OPTS) -o $(TEST_EXEC) $(TEST_OBJECTS) $(SDL_LDFLAGS)

clean:
	rm -rf $(B_DIR)/*
