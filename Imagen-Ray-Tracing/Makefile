# AgujeroNegro - Ray tracing por CPU. GNU Make + g++ (Ubuntu/Linux).
CXX := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -pthread
TARGET := bin/AgujeroNegro
SOURCES := src/main.cpp

.PHONY: all render preview clean help
all: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

render: $(TARGET)
	./$(TARGET) --width 1920 --height 1080 --samples 4 --output renders/AgujeroNegro.png

preview: $(TARGET)
	./$(TARGET) --width 640 --height 360 --samples 1 --output renders/Preview.png

help: $(TARGET)
	./$(TARGET) --help

clean:
	rm -rf bin
