ifeq ($(OS), Windows_NT)
    PLATFORM = Win32
else
    PLATFORM = $(shell uname -s)
endif
include $(PLATFORM).mk

TARGETS = button contextMenu

print-%: ; @echo $*=$($*)
