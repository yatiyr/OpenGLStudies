CFLAGS = -std=c++17 -g3
LDFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
STB_INCLUDE_PATH = include
THIRD_PARTY_INCLUDES = thirdparty/include

VulkanTest: main.cpp
	g++ $(CFLAGS) -o build/VulkanTest main.cpp $(LDFLAGS) -I$(STB_INCLUDE_PATH) -I$(THIRD_PARTY_INCLUDES)

.PHONY: test clean

test: VulkanTest
	./build/VulkanTest

clean:
	rm -f VulkanTest