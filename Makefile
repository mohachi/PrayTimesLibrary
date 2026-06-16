# ============================================================================
# PrayTimes Library Makefile
# مكتبة أوقات الصلاة - ملف البناء
# ============================================================================
# This Makefile provides commands to build, test, and debug the Prayer Times
# library - a C implementation of Dr. Hamid Zarrabi-Zadeh's prayer time
# calculation algorithm.
#
# Main targets:
#   - lib       : Build the shared library
#   - test      : Build and run unit tests
#   - example   : Build example program
#   - debug     : Build with debugging symbols
#   - clean     : Remove build artifacts
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -fPIC -lm
DEBUGFLAGS = -g -O0 -DDEBUG

.PHONY: all clean lib test run-test debug help example info ci

# Default target
all: lib

# ============================================================================
# Help target - Display available commands
# ============================================================================
help:
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║           PrayTimes Library - Build Commands                   ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Available targets:"
	@echo "  make lib       - Build shared library (bin/libpt.so)"
	@echo "  make example   - Build example program (bin/prayer_times_demo)"
	@echo "  make test      - Build unit test suite (bin/test_libpt)"
	@echo "  make run-test  - Build and run all tests"
	@echo "  make debug     - Build with debug symbols (-g -O0)"
	@echo "  make ci        - Run CI pipeline (clean, build, test)"
	@echo "  make clean     - Remove all build artifacts"
	@echo "  make info      - Show project information"
	@echo "  make help      - Display this help message"
	@echo ""

# ============================================================================
# Build shared library target
# ============================================================================
# Builds the Prayer Times library as a shared object (.so file)
# Usage: make lib
lib: bin/libpt.so

bin/libpt.so: src/libpt.c include/libpt.h
	@mkdir -p bin/
	$(CC) -shared -fPIC -lm -o bin/libpt.so src/libpt.c
	@echo "✓ Library built successfully: bin/libpt.so"

# ============================================================================
# Build example program target
# ============================================================================
# Compiles the example program demonstrating library usage
# Creates: bin/prayer_times_demo
example: bin/prayer_times_demo

bin/prayer_times_demo: examples/prayer_times_demo.c bin/libpt.so include/libpt.h
	@mkdir -p bin/
	$(CC) $(CFLAGS) -o bin/prayer_times_demo examples/prayer_times_demo.c src/libpt.c
	@echo "✓ Example program built: bin/prayer_times_demo"

# ============================================================================
# Build test suite target
# ============================================================================
# Compiles comprehensive unit tests for the library
# Includes validation tests, calculation tests, and integration tests
# Creates: bin/test_libpt
test: bin/test_libpt

bin/test_libpt: tests/test_libpt.c src/libpt.c include/libpt.h
	@mkdir -p bin/
	$(CC) $(CFLAGS) -o bin/test_libpt tests/test_libpt.c src/libpt.c
	@echo "✓ Test suite built successfully: bin/test_libpt"

# ============================================================================
# Run tests target
# ============================================================================
# Builds and executes the test suite
# Displays detailed test results and pass/fail statistics
run-test: test
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║    Running Prayer Times Library - Comprehensive Unit Tests     ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo ""
	@./bin/test_libpt

# ============================================================================
# Debug build target
# ============================================================================
# Creates debug version with symbols and optimizations disabled
# Useful for GDB debugging and detailed error reporting
# Flags: -g (symbols), -O0 (no optimization), -DDEBUG (preprocessor define)
debug: CFLAGS += $(DEBUGFLAGS)
debug: clean lib test
	@echo ""
	@echo "✓ Debug build complete with symbols"
	@echo "  Use: gdb ./bin/test_libpt"

# ============================================================================
# Clean target
# ============================================================================
# Removes all build artifacts and temporary files
# Cleans: bin/ directory
clean:
	@rm -rf bin/
	@echo "✓ All build artifacts cleaned"

# ============================================================================
# CI Pipeline target
# ============================================================================
# Runs complete continuous integration pipeline:
# 1. Clean all previous builds
# 2. Build the library
# 3. Build unit tests
# 4. Run all tests
# Useful for automated testing and deployment verification
ci: clean lib test run-test
	@echo ""
	@echo "✓ CI pipeline complete - all checks passed"

# ============================================================================
# Info target
# ============================================================================
# Displays project information and structure
info:
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║                   PrayTimes Library v1.0                       ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Description:"
	@echo "  Calculates Islamic prayer times based on geographical location"
	@echo "  and astronomical calculations by Dr. Hamid Zarrabi-Zadeh"
	@echo ""
	@echo "Supported Calculation Methods:"
	@echo "  1. Umm Al-Qura University (جامعة أم القرى)"
	@echo "  2. Muslim World League (رابطة العالم الإسلامي)"
	@echo "  3. Islamic Society of North America (جمعية إسلامية)"
	@echo "  4. Egyptian General Authority (الهيئة المصرية)"
	@echo "  5. Karachi Islamic University (جامعة كراتشي)"
	@echo ""
	@echo "Juristic Schools:"
	@echo "  • Standard (Shafi'i, Maliki, Hanbali)"
	@echo "  • Hanafi"
	@echo ""
	@echo "Project Structure:"
	@echo "  include/  - Header files (libpt.h)"
	@echo "  src/      - Library source code (libpt.c)"
	@echo "  tests/    - Test suite and examples"
	@echo "  bin/      - Compiled output (created after make lib)"
	@echo ""
	@echo "Quick Start:"
	@echo "  1. make lib        - Build the library"
	@echo "  2. make example    - Compile and run example"
	@echo "  3. make run-test   - Run unit tests"
	@echo ""
	@echo "License: MIT / Public Domain"
	@echo ""
