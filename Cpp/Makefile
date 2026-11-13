# Makefile — build dans build/, en-têtes dans include/, libs externes dans third_party/ et lib/
CXX      ?= g++
CXXFLAGS ?= -O2 -std=c++11 -Iinclude -Ithird_party
SRCS      = src/Simu_headless.cpp src/Ball.cpp src/Club.cpp src/EquationODE.cpp \
            src/EquationODEFlight.cpp src/EquationODERoll.cpp src/EquationODEEventFlight.cpp \
            src/EquationODEEventRoll.cpp src/SolverODE.cpp
FRAMEWORKS = -framework AudioToolbox -framework CoreAudio -framework AudioUnit
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS := $(FRAMEWORKS)
else
LDLIBS :=
endif

all: build/simu_headless build/spectres_html build/spectro_png

build:
	mkdir -p build

build/simu_headless: $(SRCS) $(wildcard include/*.h) | build
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LDLIBS)

build/spectres_html: src/spectres_html.cpp $(wildcard include/*.h) | build
	$(CXX) $(CXXFLAGS) -o $@ src/spectres_html.cpp $(LDLIBS)

build/spectro_png: src/spectro_png.cpp $(wildcard include/*.h) | build
	$(CXX) $(CXXFLAGS) -o $@ src/spectro_png.cpp

# simu avec image PNG via CImg (headless)
build/simu_headless_cimg: $(SRCS) $(wildcard include/*.h) | build
	$(CXX) $(CXXFLAGS) -Dcimg_display=0 -DUSE_CIMG -o $@ $(SRCS) $(LDLIBS)

clean:
	rm -f build/simu_headless build/simu_headless_cimg build/spectres_html build/spectro_png

.PHONY: all clean
