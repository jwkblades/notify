PREFIX := ${FLAVOR}
REQUIREMENTS := libnotify gtk+-3.0
DIRECTORY ?= ${CURDIR}
SOURCES_DIR := src
APPLICATION_DIR := $(SOURCES_DIR)/application
DEPDIR ?= build
INCDIR := i
SRCDIR := s
LIBS := $(shell pkg-config --libs ${REQUIREMENTS})
FLAGS = -Wall -pedantic -Wextra -Werror -std=c++20 -O2 $(shell pkg-config --cflags ${REQUIREMENTS})
ifeq (${DEBUG},1)
	FLAGS += -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage -ggdb
	LIBS += -lgcov
endif
CC := clang++
MANUAL_INC := 

EXES := $(patsubst ${APPLICATION_DIR}/%, %, $(shell find ${APPLICATION_DIR}/ -maxdepth 1 -a -type d))
INC := ${MANUAL_INC} $(foreach directory, $(shell find ${SOURCES_DIR} -name "${INCDIR}" -a -type d), -I${directory})
OBJS := $(shell find ${DIRECTORY}/${SOURCES_DIR} -wholename "*/${SRCDIR}/*.cpp" -a -type f -o \( -name main.cpp -a -type f \))
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.md=.o)
OBJS := $(patsubst ${DIRECTORY}/%, ${DEPDIR}/%, ${OBJS})
$(shell mkdir -p ${DEPDIR} >/dev/null)
DEPFLAGS = -MT $@ -MMD -MF ${DEPDIR}/$*.d
COMPILE.cc = ${CC} ${DEPFLAGS} ${FLAGS} ${INC} -c
DEPFILES := $(patsubst %, %.d, $(basename ${OBJS}))

PHONY_TARGETS = $(foreach exe, ${EXES}, $(shell if [ -e ${APPLICATION_DIR}/${exe}/Makefile ]; then echo ${exe}; fi))
.PHONY: all clean clean-flavor print-% ${PHONY_TARGETS}
all: ${EXES}
	bin/compiledb ${EXES}
	bin/lint

${DEPDIR}/%.o: %.cpp
${DEPDIR}/%.o: %.cpp ${DEPDIR}/%.d
	@mkdir -p $(shell dirname $@)
	@echo "Compiling ${@}"
	@${COMPILE.cc} ${OUTPUT_OPTION} $<

clean:
	@FLAVOR=fedora make clean-flavor
	@FLAVOR=ubuntu make clean-flavor
	@rm -rf ${DEPDIR} *.gcno *.dat
	@rm -f $(if ${PREFIX}, $(addprefix ${PREFIX}-, ${EXES}), ${EXES})

clean-flavor:
	@rm -f $(if ${PREFIX}, $(addprefix ${PREFIX}-, ${EXES}), ${EXES})

print-%:
	@echo $* = ${$*}

${DEPDIR}/%.d: ;
.PRECIOUS: ${DEPDIR}/%.d

-include ${DEPFILES}

define linkTemplate
${${1}}: %: ${DEPDIR}/${2}/%/main.o $(filter-out %/main.o,${OBJS})
	@echo "Building $${@}"
	@if [ -e $${DIRECTORY}/${2}/$${@}/Makefile ]; then \
		make -f ${2}/$${@}/Makefile $(shell basename ${2})-$${@}; \
		mv $(if ${PREFIX}, ${PREFIX}-, )$(shell basename ${2})-$${@} $(if ${PREFIX}, ${PREFIX}-, )$${@}; \
	fi
	@if [ ! -e $${DIRECTORY}/${2}/$${@}/Makefile ]; then \
		echo -e "\nLinking $${@}"; \
		${CC} ${FLAGS} ${INC} $$(shell bin/dependencyObjects $${@} $${DEPDIR} $${<} 2>/dev/null) -o $(if ${PREFIX}, ${PREFIX}-$${@}, $${@}) ${LIBS}; \
	fi
endef

$(eval $(call linkTemplate,EXES,${APPLICATION_DIR}))
