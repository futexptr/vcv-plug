RACK_DIR ?= ../..

include $(RACK_DIR)/arch.mk

#  -I/mingw64/include
FLAGS += -Isrc/dep/include -Isrc/dep/ixwebsocket -Isrc/dep/buttplug
SOURCES += $(wildcard src/*.cpp)

SOURCES += $(wildcard src/dep/ixwebsocket/*.cpp)
SOURCES += $(wildcard src/dep/buttplug/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res

ifdef ARCH_WIN
	EXTRA_LDFLAGS = -lws2_32 
endif


include $(RACK_DIR)/plugin.mk
