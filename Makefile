GLFW = `pkg-config --cflags glfw3` `pkg-config --libs --static glfw3`
GLEW = `pkg-config --cflags glew` `pkg-config --libs glew`
LIB = -lasound

common = shader.cpp play_wav.cpp
target = point-line-triangle

all: $(target)

$(target): $(target).cpp
	clang++ $(target).cpp -o $(target) $(common) $(GLFW) $(GLEW) $(LIB)
