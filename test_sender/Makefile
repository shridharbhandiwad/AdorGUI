# Simple Makefile for test_raw_data_sender
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2

# Platform detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS =
endif
ifeq ($(UNAME_S),Darwin)
    LDFLAGS =
endif

# Default target
TARGET = test_raw_data_sender

all: $(TARGET)

$(TARGET): test_raw_data_sender.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
