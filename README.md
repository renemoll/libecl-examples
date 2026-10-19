# Embedded components library (ECL) example

An example application showcasing the usage of the Embedded Components Library (ECL).

## Local development environment

You can use the provided devcontainer or setup your own.

```bash
pip install -r requirements.txt
pre-commit install --hook-type commit-msg
```

## Compiling & testing

This project provides CMake workflows to easy building from source code.

List available workflows:

```bash
cmake --workflow --list-presets
```

Workflow | Command
--- | ---
Unit tests | ```cmake --workflow --preset unittest-gcc-debug```
<!-- Test coverage | ```cmake --workflow --preset coverage``` -->
