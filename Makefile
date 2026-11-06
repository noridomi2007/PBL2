CXX = g++

CXXFLAGS = -Wall -std=c++17 -I./include
LDFLAGS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

# Phần lõi dùng chung (app thật và bản test đều cần).
# wildcard: thư mục nào chưa có .cpp thì ra rỗng, không báo lỗi
LIB_SRC = $(wildcard item/*.cpp) \
          $(wildcard Shop/*.cpp) \
          $(wildcard doanhthu/*.cpp) \
          $(wildcard customer/*.cpp) \
          $(wildcard staff/*.cpp) \
          $(wildcard Hoadon/*.cpp) \
          $(wildcard dialog/*.cpp)

# App thật = phần lõi + giao diện
SRC = $(LIB_SRC) \
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
