# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SPOBX8Edit is a CLAP plugin for controlling the Oberheim OBX8 synthesizer via MIDI NRPNs. It provides bidirectional sync between a DAW and the OBX8 hardware, allowing real-time parameter control through 30 mapped parameters.

## Build System

### Primary Build Commands
```bash
# Quick install (recommended)
./install.sh

# Manual build
mkdir build && cd build
cmake ..
make
```

### Dependencies
- CMake 3.16+
- Clang++ (Xcode Command Line Tools)
- macOS 10.14+ (current target platform)

The project uses CMake with custom post-build scripts to create proper macOS bundle structures for CLAP plugins.

## Architecture

### Core Components
- **OBX8Plugin**: Main plugin class implementing CLAP interface (`src/obx8_plugin.{h,cpp}`)
- **OBX8ParameterManager**: Handles parameter definitions and NRPN mapping (`src/obx8_parameters.{h,cpp}`)
- **MidiHandler**: Processes MIDI I/O and NRPN conversion (`src/midi_handler.{h,cpp}`)
- **MidiDeviceManager**: Manages MIDI device selection and CoreMIDI integration (`src/midi_device_manager.{h,cpp}`)

### Plugin Architecture
The plugin implements multiple CLAP extensions:
- Parameters (30 OBX8 hardware parameters)
- Note ports (MIDI I/O)
- State (save/load)
- GUI (parameter interface)

### MIDI Communication
- Uses NRPN (Non-Registered Parameter Numbers) for bidirectional hardware sync
- CoreMIDI framework integration for macOS MIDI device management
- Real-time parameter conversion between plugin values and NRPN messages

## Installation Structure

**IMPORTANT**: Always install to system-wide CLAP folder, NEVER to user's Library folder.

**IMPORTANT**: Never run sudo commands directly. Always ask user to run sudo commands manually in their terminal.

Built plugins are installed as macOS bundles to system location:
```
/Library/Audio/Plug-Ins/CLAP/SPOBX8Edit.clap/
├── Contents/
│   ├── Info.plist
│   ├── PkgInfo
│   └── MacOS/
│       └── SPOBX8Edit
```

## Development Notes

### CLAP Integration
- Uses CLAP SDK as a git submodule in `include/clap/`
- Plugin entry point in `src/plugin_entry.cpp`
- Follows CLAP plugin descriptor pattern

### Platform Support
- Currently targets macOS exclusively
- Links against CoreMIDI and CoreFoundation frameworks
- CMake configuration supports cross-platform builds (Windows/Linux stubs present)

### Parameter System
- 30 hardware parameters mapped to OBX8 NRPN addresses
- Parameter normalization/denormalization for DAW integration
- Bidirectional sync maintains state consistency between plugin and hardware

## OBX8 Hardware Reference

**IMPORTANT**: Always use the latest OBX8 manual when working with NRPN mappings or hardware parameters:
- **Latest Manual**: https://oberheim.com/wp-content/uploads/2023/09/OB-X8-Manual-V2_2.pdf
- Contains current NRPN parameter mappings and hardware specifications
- Critical for maintaining accurate parameter synchronization

## No Testing Framework
This project does not include automated tests or linting tools. Manual testing is performed with the actual OBX8 hardware and CLAP-compatible DAWs.