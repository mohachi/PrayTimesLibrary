# Contributing to Prayer Times Library

Thank you for your interest in contributing to the Prayer Times Library! This document provides guidelines and instructions for contributing.

## Code of Conduct

- Be respectful and inclusive
- Focus on the code, not the person
- Help others learn and grow
- Report issues constructively

## Getting Started

### Prerequisites
- GCC or compatible C compiler
- GNU Make
- Git
- Basic understanding of C and astronomical calculations (for algorithm contributions)

### Setting Up Development Environment

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/PrayTimesLibrary.git
   cd PrayTimesLibrary
   ```

2. **Build and test**:
   ```bash
   make clean
   make run-test
   ```

3. **Verify everything works**:
   ```bash
   make example
   ./bin/prayer_times_demo
   ```

## Types of Contributions

### Bug Reports
- **How to Report**: Open an issue with:
  - Clear description of the bug
  - Steps to reproduce
  - Expected vs actual behavior
  - Platform and compiler information
  - Prayer time calculation details (date, location, method)

### Feature Requests
- Suggest enhancements or new features
- Explain the use case and benefits
- Reference Islamic calendar calculations or other prayer time systems if applicable

### Code Improvements
- Performance optimizations
- Code quality improvements
- Better error handling
- Additional test coverage

### Documentation
- Fix typos or unclear sections
- Improve examples
- Add API documentation
- Translate to other languages

## Development Workflow

### 1. Create a Feature Branch
```bash
git checkout -b feature/your-feature-name
# or
git checkout -b bugfix/issue-description
```

### 2. Code Style Guidelines

#### Naming Conventions
```c
// Constants: UPPER_CASE
#define MAX_YEAR 2100
#define PI 3.14159265358979323846

// Functions: snake_case
void calculate_prayer_times(void)
int get_error_code(void)

// Variables: snake_case
double solar_declination;
int prayer_count;

// Types: CamelCase
typedef struct PrayerTime { ... } PrayerTime;
typedef enum CalcMethod { ... } CalcMethod;
```

#### Comments
- Use bilingual comments (Arabic and English)
- Document the "why", not the obvious "what"
- Add @brief tags for functions
- Include examples for complex algorithms

```c
/**
 * @brief Calculate solar declination for given Julian date
 * @details حساب انحراف الشمس للتاريخ اليولياني المعطى
 *
 * This calculation is crucial for accurate prayer time computation.
 * The formula accounts for the sun's position relative to the equator.
 *
 * @param jd The Julian Date (days since epoch)
 * @return Solar declination in degrees (-23.44 to 23.44)
 */
double sun_declination(double jd)
```

#### Formatting
- Use 4-space indentation (no tabs)
- Line length: aim for < 100 characters
- Space after `if`, `while`, `for` keywords
- Braces on same line: `if (condition) { ... }`

#### Validation
- Always validate input parameters
- Use enums for method selection
- Return proper error codes
- Never assume valid input

### 3. Testing

#### Running Tests
```bash
make run-test          # Run all tests
make clean test        # Rebuild and test
make debug             # Build with debug symbols
```

#### Adding Tests
1. **Test Location**: `tests/test_libpt.c`
2. **Test Format**: Use the existing test macros
   ```c
   TEST_CASE("Test Description") {
       // Arrange
       double input = 5.5;

       // Act
       int hours, minutes;
       decimal_to_time(input, &hours, &minutes);

       // Assert
       ASSERT_EQUAL(hours, 5);
       ASSERT_EQUAL(minutes, 30);
   }
   ```

3. **Test Categories**:
   - Validation tests: Input edge cases and error conditions
   - Calculation tests: Correct computation for known inputs
   - Integration tests: Multiple methods working together
   - Global tests: Different geographic locations

#### Test Coverage
- Aim for 100% code coverage
- Test all error paths
- Include boundary conditions
- Test both calculation methods

### 4. Documentation

#### Updating README.md
- Keep examples up-to-date
- Document new methods or features
- Maintain bilingual content (Arabic/English)

#### Updating DOCUMENTATION.md
- Add technical details for algorithm changes
- Update performance analysis if applicable
- Document any mathematical changes with references

#### Commit Messages
Follow this format:
```
[Type] Brief description (50 chars max)

Detailed explanation (wrap at 72 chars)
- Point 1
- Point 2
- Related issue: #123

[Type] can be: Feature, Fix, Docs, Refactor, Test, Perf
```

Example:
```
Feature: Add DST handling utility functions

This adds helper functions to detect daylight saving time changes
for different regions. Useful for applications that need to adjust
prayer times for DST.

- Add dst_in_effect() function
- Add is_dst_transition_date() function
- Add tests for major regions (US, EU)

Closes: #45
```

### 5. Before Submitting

```bash
# 1. Update your branch
git fetch origin
git rebase origin/master

# 2. Run all checks
make clean
make ci              # Complete CI pipeline

# 3. Check code style (if available)
gcc -Wall -Wextra -pedantic tests/*.c src/*.c -lm

# 4. Review your changes
git diff

# 5. Commit
git add .
git commit -m "[Type] Your commit message"
```

## Pull Request Process

1. **Push your branch**:
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Create Pull Request**:
   - Clear title describing changes
   - Reference related issues (#123)
   - Explain why changes are needed
   - Mention any breaking changes

3. **PR Template**:
   ```markdown
   ## Description
   Brief description of changes

   ## Related Issue
   Fixes #123

   ## Changes Made
   - Change 1
   - Change 2

   ## Testing
   How was this tested?

   ## Breaking Changes
   Any breaking changes?
   ```

4. **Review Process**:
   - Wait for maintainer review
   - Respond to feedback promptly
   - Push updates to same branch
   - All tests must pass

5. **Merge**:
   - Squash commits if needed
   - Delete feature branch

## Branch Naming Convention

```
feature/description          # New features
bugfix/issue-number          # Bug fixes
refactor/improvement-area    # Code improvements
docs/what-was-updated        # Documentation
test/what-was-tested         # New tests
perf/optimization-area       # Performance improvements
```

## Commit Frequency

- Make small, logical commits
- One feature/fix per commit
- Commit frequently (don't squash until merge)
- Each commit should pass tests independently

## Areas for Contribution

### High Priority
- Performance optimizations for embedded systems
- Support for additional calculation methods
- Extended year range (pre-1900, post-2100)
- DST (Daylight Saving Time) handling utilities

### Medium Priority
- Additional languages for error messages (French, Spanish, Indonesian)
- More examples for different use cases
- Benchmark tools for comparison with other libraries
- Historical prayer time data validation

### Lower Priority
- GUI wrapper (Qt, GTK)
- Language bindings (Python, JavaScript)
- Web service wrapper
- Extended documentation with derivations

## Questions or Need Help?

- **Report Issues**: Use GitHub Issues
- **Discussions**: Use GitHub Discussions
- **Security**: See SECURITY.md (if applicable)

## License

By contributing, you agree that your contributions will be licensed under the same MIT License as the project.

---

Thank you for contributing to Prayer Times Library! Your efforts help make Islamic prayer time calculations accessible to everyone. 🕌

**Happy coding!** 🚀
