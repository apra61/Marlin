CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc
BUILD_DIR ?= build/host

.PHONY: test clean

test: $(BUILD_DIR)/test_morse_keyer
	./$(BUILD_DIR)/test_morse_keyer

$(BUILD_DIR)/test_morse_keyer: src/morse_keyer.cpp test/test_morse_keyer.cpp src/morse_keyer.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) src/morse_keyer.cpp test/test_morse_keyer.cpp -o $@

clean:
	rm -rf build
