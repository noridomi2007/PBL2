CXX = g++

CXXFLAGS = -Wall -std=c++17 -IRaylib/Include
LDFLAGS = -LRaylib/Lib -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

LOI_SRC = $(wildcard src/Item/*.cpp) \
          $(wildcard src/Shop/*.cpp) \
          $(wildcard src/Doanhthu/*.cpp) \
          $(wildcard src/Customer/*.cpp) \
          $(wildcard src/Staff/*.cpp) \
          $(wildcard src/Hoadon/*.cpp) \
          $(wildcard src/Bosuutap/*.cpp) \
          $(wildcard src/Boloc/*.cpp) \
          $(wildcard src/Dialog/*.cpp)

SRC = $(LOI_SRC) \
      $(wildcard src/Giaodien/*.cpp) \
      $(wildcard src/Giaodien/Khung/*.cpp) \
      src/Main.cpp
OUT = app.exe

.PHONY: all clean

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

clean:
	-del $(OUT)