CC := g++
FLAGS := -Wall -Wextra -Werror -pedantic -std=c++20 -O2
LIBS := $(shell pkg-config --cflags --libs libnotify gtk+-3.0)

.PHONY: all clean

all: notify

clean:
	@rm -f notify

notify: application/notify/main.cpp
	@echo "Building ${@}"
	@${CC} ${FLAGS} ${^} -o ${@} ${LIBS}

