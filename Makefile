CXX = g++

CXXFLAGS = -Wall -std=c++17 -I./include
LDFLAGS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

# Phần dùng chung (app thật và bản test đều cần)
LIB_SRC = $(wildcard item/*.cpp) \
          $(wildcard Shop/*.cpp) \
          $(wildcard doanhthu/*.cpp) \
          $(wildcard dialog/*.cpp)

# wildcard: thư mục nào chưa có .cpp (như Hoadon) thì ra rỗng, không báo lỗi
SRC = $(wildcard customer/*.cpp) \
      $(wildcard Hoadon/*.cpp) \
      $(LIB_SRC) \
      $(wildcard staff/*.cpp) \
      $(wildcard main/*.cpp) \
      $(wildcard main.cpp)
OUT = app.exe

# test trùng tên thư mục test/ nên bắt buộc phải khai báo .PHONY
.PHONY: all test clean

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

test:
	$(CXX) test/test_main.cpp $(LIB_SRC) -o test.exe $(CXXFLAGS) $(LDFLAGS)

clean:
	-del $(OUT) test.exe