PREFIX := 
DIRECTORY ?= ${CURDIR}
COMPONENTS := ${DIRECTORY}/cc
DEPDIR := ${DIRECTORY}/build
INCDIR := i
SRCDIR := s
LIBS := -lpthread -lssl -lcrypto -lstdc++fs -ldl -pthread -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
FLAGS = -Wall -Wextra -Werror -std=c++17 -O2
ifeq (${DEBUG},1)
	FLAGS += -fprofile-arcs -ftest-coverage -ggdb
	LIBS += -lgcov
else
	FLAGS += -DNO_TIMER
endif
CC := g++
MANUAL_INC := -I${DIRECTORY}  -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid



EXES := $(patsubst ${DIRECTORY}/application/%, %, $(shell find ${DIRECTORY}/application/ -type d))
MODULES := $(patsubst ${DIRECTORY}/modules/%, %, $(shell find ${DIRECTORY}/modules/ -type d))
INC := ${MANUAL_INC} $(foreach directory, $(shell find ${DIRECTORY} -name "${INCDIR}" -a -type d), -I${directory})
OBJS := $(shell find ${DIRECTORY} \( -wholename "*/${SRCDIR}/*.cpp" -or -wholename "*/*.md" \) -a -type f)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.md=.o)
OBJS := $(patsubst ${DIRECTORY}/%, ${DEPDIR}/%, ${OBJS})
$(shell mkdir -p ${DEPDIR} >/dev/null)
DEPFLAGS = -MT $@ -MMD -MF ${DEPDIR}/$*.d
COMPILE.cc = ${CC} ${DEPFLAGS} ${FLAGS} ${INC} -c
DEPFILES := $(patsubst %, %.d, $(basename ${OBJS}))

${DEPDIR}/%.o: %.cpp
${DEPDIR}/%.o: ${DIRECTORY}/%.cpp ${DEPDIR}/%.d
	@mkdir -p $(shell dirname $@)
	@echo "Compiling $<"
	@${COMPILE.cc} ${OUTPUT_OPTION} $<

${DEPDIR}/%.o: %.md
	@mkdir -p $(shell dirname $@)
	@echo "Translating $<"
	@echo "Looking for: _binary_$(subst .,_,$(subst /,_,$<))_start to replace with   $(shell basename -s .md $<)Documentation"
	@objcopy --binary i386 --input binary --output elf64-x86-64 --redefine-sym _binary_$(subst .,_,$(subst /,_,$<))_start=$(shell basename -s .md $<)Documentation $< $@

PHONY_TARGETS = $(foreach exe, ${EXES}, $(shell if [ -e application/${exe}/Makefile ]; then echo ${exe}; fi))
PHONY_TARGETS += $(foreach exe, ${MODULES}, $(shell if [ -e modules/${exe}/Makefile ]; then echo ${exe}; fi))
.PHONY: all clean print-% ${PHONY_TARGETS}
all: ${EXES} ${MODULES}

clean:
	@rm -rf ${DEPDIR} *.gcno *.dat
	@rm -f $(if ${PREFIX}, $(addprefix ${PREFIX}-, ${EXES}), ${EXES})
	@rm -f $(if ${PREFIX}, $(addprefix ${PREFIX}-, ${MODULES}), ${MODULES})

print-%:
	@echo $* = ${$*}

${DEPDIR}/%.d: ;
.PRECIOUS: ${DEPDIR}/%.d

-include ${DEPFILES}

define linkTemplate
${${1}}: %: $${DIRECTORY}/build/${2}/%/main.o $(filter-out %/main.o,${OBJS})
	@if [ -e $${DIRECTORY}/${2}/$${@}/Makefile ]; then \
		echo -e "\nChanging Directory to ${2}/$${@}"; \
		cd ${2}/$${@}; make $${MAKEFLAGS} ${2}-$${@} && sync && \
		mv ${2}-$${@} ../../$${@}; \
	fi
	@if [ ! -e $${DIRECTORY}/${2}/$${@}/Makefile ]; then \
		echo -e "\nLinking $${@}"; \
		${CC} ${FLAGS} ${INC} -o $(if ${PREFIX}, ${PREFIX}-$${@}, $${@}) ${LIBS} $$(shell bin/dependencyObjects $${@} 2>/dev/null); \
	fi
endef

$(eval $(call linkTemplate,EXES,application))
$(eval $(call linkTemplate,MODULES,modules))
