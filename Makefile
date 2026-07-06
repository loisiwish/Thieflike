CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
TARGET = rogue

SRC = src/main.cpp	\
	src/window/window.cpp	\
	src/window/loop.cpp	\
	src/window/menu.cpp	\
	

OBJ = $(SRC:.cpp=.o)

SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(SFML_LIBS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)	$(OBJ)

re: fclean all

.PHONY: all fclean re
