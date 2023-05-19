CC = g++

CFLAGS_DEBUG = -Wall -Wextra -g -std=c++11
CFLAGS_RELEASE = -Wall -Wextra -O2

LIBRARY_PATH = ./
LIBRARY_NAME = pthread

LDLIBS = -l$(LIBRARY_NAME)
LDFLAGS = -L$(LIBRARY_PATH)

SRCS := $(wildcard *.cpp)

OBJS := $(patsubst %.cpp, debug/%.o,$(SRCS))

APP = app

APP_DEBUG=debug/$(APP)
.PHONY: debug release clean mkdir


debug:mkdir $(APP_DEBUG)
	ln -sf  $(APP_DEBUG) $(APP)
	
$(APP_DEBUG): $(OBJS)
	$(CC) $(CFLAGS_DEBUG) $(LDFLAGS) -o $@ $^ $(LDLIBS)

debug/%.o: %.cpp
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@


clean:
	rm -rf $(OBJS) $(APP) debug release
mkdir:
	mkdir debug -p
