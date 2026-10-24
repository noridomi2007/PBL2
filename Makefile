CXX = g++

CXXFLAGS = -Wall -I./include
LDFLAGS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm

# wildcard: thư mục nào chưa có .cpp (như Hoadon) thì ra rỗng, không báo lỗi
SRC = $(wildcard customer/*.cpp) \
      $(wildcard Hoadon/*.cpp) \
      $(wildcard item/*.cpp) \
      $(wildcard staff/*.cpp) \
      $(wildcard Shop/*.cpp) \
      $(wildcard main/*.cpp) \
      $(wildcard main.cpp)
OUT = app.exe

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

clean:
	del $(OUT)