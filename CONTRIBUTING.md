/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Contributing Guidelines
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Guidelines for contributing to the AIMLAB haptic application project.
 *   Includes coding standards, documentation requirements, and workflow.
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial contributing guidelines
 * 
 ****************************************************************************/

# Contributing to AIMLAB-Unity-Chai3D-VR

Thank you for your interest in contributing to this project! This document provides guidelines for contributing code, documentation, and other improvements.

---

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Workflow](#development-workflow)
4. [Coding Standards](#coding-standards)
5. [Documentation Requirements](#documentation-requirements)
6. [Commit Guidelines](#commit-guidelines)
7. [Pull Request Process](#pull-request-process)
8. [Testing](#testing)

---

## Code of Conduct

This project follows academic research ethics and professional collaboration standards:

- Be respectful and constructive in all communications
- Focus on technical merit and scientific rigor
- Credit original work and cite sources appropriately
- Share knowledge and help others learn

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:
1. Read `docs/SETUP_INSTRUCTIONS.md` and set up the development environment
2. Successfully built and run the starter application
3. Familiarity with CHAI3D framework basics
4. Access to Haply Inverse3 hardware (for haptic features)

### Repository Setup

```powershell
# Fork the repository on GitHub
# Clone your fork
git clone https://github.com/YOUR_USERNAME/AIMLAB-Unity-Chai3D-VR.git
cd AIMLAB-Unity-Chai3D-VR

# Add upstream remote
git remote add upstream https://github.com/ORIGINAL_OWNER/AIMLAB-Unity-Chai3D-VR.git

# Initialize submodules
git submodule update --init --recursive
```

---

## Development Workflow

### 1. Create a Feature Branch

```powershell
git checkout -b feature/your-feature-name
```

Branch naming conventions:
- `feature/feature-name` - New features
- `fix/bug-description` - Bug fixes
- `docs/topic` - Documentation updates
- `refactor/component-name` - Code refactoring
- `test/test-description` - Test additions

### 2. Make Your Changes

- Write clean, well-documented code
- Follow the coding standards below
- Test your changes thoroughly

### 3. Commit Your Changes

```powershell
git add .
git commit -m "Brief description of changes"
```

See [Commit Guidelines](#commit-guidelines) below.

### 4. Push and Create Pull Request

```powershell
git push origin feature/your-feature-name
```

Then create a pull request on GitHub.

---

## Coding Standards

### C++ Style Guide

**File Headers:**
All `.cpp` and `.h` files must include a header comment block:

```cpp
/****************************************************************************
 * AIMLAB - [Brief Module Description]
 * 
 * Author: Your Name (your.email@domain.com)
 * Date: DD Month YYYY
 * Version: vX.X
 * 
 * Description:
 *   Detailed description of what this file does.
 * 
 * Changelog:
 *   vX.X - DD Month YYYY - Change description
 * 
 ****************************************************************************/
```

**Function Documentation:**

```cpp
/**
 * @brief Brief description of function
 * 
 * Detailed description if needed.
 * 
 * @param param1 Description of parameter 1
 * @param param2 Description of parameter 2
 * @return Description of return value
 * 
 * Performance Notes:
 *   - Any performance-critical information
 */
ReturnType functionName(Type1 param1, Type2 param2) {
    // Implementation
}
```

**Naming Conventions:**

```cpp
// Classes: PascalCase
class MyHapticObject { };

// Functions: camelCase
void updateForceField() { }

// Variables: camelCase
double springStiffness = 1000.0;

// Constants: UPPER_SNAKE_CASE
const double MAX_FORCE = 8.0;

// Member variables: camelCase with m_ prefix (CHAI3D convention)
class MyClass {
    double m_stiffness;
    cVector3d m_position;
};
```

**Indentation and Formatting:**

- Use **4 spaces** for indentation (no tabs)
- Opening braces on same line for functions and control structures
- Maximum line length: 100 characters (soft limit)
- Use blank lines to separate logical sections

**Comments:**

```cpp
// Single-line comments for brief explanations

/* 
 * Multi-line comments for longer explanations
 * that span multiple lines.
 */

//-----------------------------------------------------------------------
// SECTION HEADERS (use for major code sections)
//-----------------------------------------------------------------------
```

### CMake Style

```cmake
# Comments start with #
# Use lowercase for commands: add_executable, not ADD_EXECUTABLE
# Indent with 4 spaces
# Use clear variable names

set(MY_SOURCES
    src/file1.cpp
    src/file2.cpp
)
```

---

## Documentation Requirements

### Required Documentation

1. **Code Comments:**
   - All public functions must have docstring comments
   - Complex algorithms should include inline explanatory comments
   - Magic numbers should be explained or extracted to named constants

2. **README Updates:**
   - Update `README.md` if you add new features
   - Add to changelog section

3. **API Documentation:**
   - Document new public APIs in `docs/DEVELOPMENT.md`
   - Include usage examples for new features

### Markdown Style

- Use ATX-style headers (`#`, `##`, `###`)
- Include table of contents for long documents
- Use code fences with language tags: ` ```cpp `, ` ```powershell `
- Keep line length reasonable (80-100 chars) for readability

---

## Commit Guidelines

### Commit Message Format

```
Brief summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain the problem this commit solves and why this approach was chosen.

- Bullet points are okay
- Use present tense: "Add feature" not "Added feature"
- Reference issues: "Fixes #123"
```

### Good Commit Examples

```
Add viscosity effect to haptic sphere

Implements velocity-based damping force to simulate fluid resistance.
Uses device linear velocity and damping coefficient to compute force.

Add support for loading OBJ 3D models

Includes example code in DEVELOPMENT.md and error handling for
failed file loads. Models are scaled and positioned automatically.

Fix haptic thread race condition

Synchronizes access to shared device pointer using mutex to prevent
crashes during cleanup. Closes #42.
```

### Commit Best Practices

- Make atomic commits (one logical change per commit)
- Commit often, push when feature is complete
- Don't commit build artifacts (already in `.gitignore`)
- Run tests before committing

---

## Pull Request Process

### Before Submitting

- [ ] Code builds successfully in Release mode
- [ ] All new code is documented
- [ ] Changes are tested with actual hardware (if applicable)
- [ ] No linter errors introduced
- [ ] `README.md` updated if needed
- [ ] Changelog updated

### Pull Request Template

When creating a PR, include:

```markdown
## Summary
Brief description of what this PR does.

## Changes
- Bullet list of specific changes
- Files modified and why

## Testing
How you tested these changes:
- [ ] Built successfully on Windows
- [ ] Tested with Inverse3 hardware
- [ ] No regression in existing features

## Screenshots/Videos
(If applicable, especially for visual or haptic changes)

## Related Issues
Fixes #123
Relates to #456
```

### Review Process

1. Automated checks will run (if configured)
2. Maintainer will review code and documentation
3. Address any requested changes
4. Once approved, PR will be merged

---

## Testing

### Manual Testing

For haptic features:
1. Build in **Release mode** (Debug is too slow for haptics)
2. Connect Inverse3 hardware
3. Run application and verify:
   - Device is detected
   - Forces are smooth (no jitter)
   - No crashes during interaction
   - Performance >= 1 kHz haptic rate

### Testing Checklist

When adding new features:

- [ ] Feature works as intended
- [ ] No console errors or warnings
- [ ] Haptic feedback is stable and smooth
- [ ] Graphics render correctly
- [ ] No memory leaks (use valgrind or similar)
- [ ] Performance impact is acceptable

### Automated Testing

(Future: Unit tests for non-haptic components)

---

## Areas for Contribution

### High Priority

- [ ] Unity integration layer (shared memory or network protocol)
- [ ] VR headset integration (OpenXR support)
- [ ] Data logging and analysis tools
- [ ] Additional haptic effects library
- [ ] Example applications and demos

### Documentation

- [ ] Video tutorials for setup process
- [ ] API reference improvements
- [ ] Troubleshooting guide expansion
- [ ] Code examples repository

### Research Features

- [ ] Force field simulation algorithms
- [ ] Haptic texture synthesis
- [ ] Multi-modal feedback integration
- [ ] User study data collection framework

---

## Versioning

This project uses Semantic Versioning (SemVer):

- **MAJOR.MINOR.PATCH** (e.g., v1.2.3)
- **MAJOR:** Breaking changes, major architecture updates
- **MINOR:** New features, backwards-compatible
- **PATCH:** Bug fixes, documentation updates

---

## Questions?

- Open an issue on GitHub
- Email: pi.ko@nyu.edu
- Check existing documentation in `docs/`

---

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see LICENSE file). Ensure your contributions are compatible with the BSD and GPLv3 licenses used by dependencies.

---

**Thank you for contributing to haptic research and education!**
