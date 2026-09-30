# Dòng này chính là lệnh đổi sang trình biên dịch C++ (g++)
CXX = g++

# Cấu hình thư viện (chỉ đường dẫn vào thư mục include và lib)
CXXFLAGS = -Wall -I./include
LDFLAGS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Gom toàn bộ file .cpp từ các thư mục của bro
SRC = customer/*.cpp Hoadon/*.cpp item/*.cpp staff/*.cpp main.cpp
OUT = app.exe

# Lệnh build chính
all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

# Lệnh dọn dẹp file cũ
clean:
	del $(OUT)