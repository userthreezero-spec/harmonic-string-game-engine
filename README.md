# Harmonic String Engine

An independent game engine project whose engineering process can be assisted by WebOS.

## Status

**Rendering Foundation v0.1** — Initial rendering foundation with math library, window management, and basic OpenGL rendering.

## Building

### Prerequisites

- CMake 3.20+
- C++20 compiler (MSVC, GCC, or Clang)
- OpenGL 3.3+ capable GPU
- GLFW 3.3+
- GLEW

### Build Commands

```bash
cmake -S . -B build
cmake --build build
```

### Running Tests

```bash
cd build
ctest
```

### Running

```bash
./build/hse_main
```

Press ESC to exit.

## Architecture

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed architecture.

## WebOS Integration

This project can request cognitive engineering assistance from WebOS. See [WEBOS_INTEGRATION.md](WEBOS_INTEGRATION.md) for details.

## Project Structure

```
HarmonicStringEngine/
├── src/           # Source files
├── include/       # Header files
├── tests/         # Unit tests
├── assets/        # Shaders, textures, models
├── docs/          # Documentation
├── knowledge/     # Project knowledge model
├── webos/         # WebOS integration
├── experiments/   # Experiment artifacts
└── tools/         # Build tools, scripts
```

## License

Internal project — not yet licensed.
