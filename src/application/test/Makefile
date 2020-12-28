DIRECTORY = $(shell realpath ${CURDIR})
include ${DIRECTORY}/Makefile

TEST_FLAGS = ${FLAGS} -fprofile-arcs -ftest-coverage -ggdb
TEST_LIBS = ${LIBS} -lgtest -lgtest_main -lgcov

TEST_DIR := t
TEST_INC := $(foreach directory, $(shell find ${DIRECTORY}/${SOURCES_DIR} -name "${TEST_DIR}" -a -type d -a -not -path "${DIRECTORY}/coverage/*"), -I${directory})
TEST_OBJS := $(shell find ${DIRECTORY}/${SOURCES_DIR} -wholename "*/${TEST_DIR}/*.cpp" -a -type f)
TEST_OBJS := $(TEST_OBJS:.cpp=.o)
TEST_OBJS := $(patsubst ${DIRECTORY}/%, ${DEPDIR}/%, ${TEST_OBJS})
TEST_OBJS += ${OBJS}
TEST_INC += ${INC}

application-test: ${DIRECTORY}/${DEPDIR}/src/application/test/main.o $(filter-out %/main.o, ${TEST_OBJS})
	@echo "Linking ${@}"
	@${CC} ${TEST_FLAGS} ${TEST_INC} -o $(if ${PREFIX}, ${PREFIX}-$@, $@) $^ ${TEST_LIBS}
