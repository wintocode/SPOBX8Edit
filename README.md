# SPOBX8Edit - OBX8 Hardware Editor Plugin

A CLAP plugin for controlling the Oberheim OBX8 synthesizer via MIDI NRPNs.

## Features

- **30 OBX8 Parameters** mapped to hardware controls
- **MIDI Device Selection** with auto-detection
- **Bidirectional Sync** between plugin and hardware
- **NRPN Support** for all parameter changes
- **Real-time Control** of your OBX8 from your DAW

## Installation

### Option 1: Quick Install (Recommended)
```bash
cd SPOBX8Edit
./install.sh
```

### Option 2: Manual Build
```bash
# Install dependencies
brew install cmake

# Clone repository with submodules
git clone --recursive https://github.com/your-username/SPOBX8Edit.git
cd SPOBX8Edit

# Build
mkdir build && cd build
cmake ..
make

# Install manually
mkdir -p ~/Library/Audio/Plug-Ins/CLAP/SPOBX8Edit.clap/Contents/MacOS
cp SPOBX8Edit.clap ~/Library/Audio/Plug-Ins/CLAP/SPOBX8Edit.clap/Contents/MacOS/
```

### Option 3: Transfer from Another Mac
```bash
# On source Mac - create package
tar -czf spobx8edit-plugin.tar.gz -C ~/Library/Audio/Plug-Ins/CLAP SPOBX8Edit.clap

# On target Mac - extract
cd ~/Library/Audio/Plug-Ins/CLAP
tar -xzf spobx8edit-plugin.tar.gz
```

## Usage

1. **Load Plugin** in your CLAP-compatible DAW (Bitwig, Reaper, FL Studio)
2. **Select MIDI Device** - Choose your OBX8's MIDI interface
3. **Control Parameters** - All changes sync to your hardware via NRPN
4. **Hardware Changes** sync back to the plugin automatically

## Parameters

### Oscillators
- Frequency, Waveform, Pulse Width, Sync, Level

### Filter
- Frequency, Resonance, Tracking, Pole Selection

### Envelopes
- Attack, Decay, Sustain, Release (x2)

### LFOs
- Rate, Shape, Amount (x2)

### Master
- Volume, Tune, MIDI Device Selection

## Requirements

- **macOS** 10.14 or later
- **CLAP-compatible DAW**
- **Oberheim OBX8** synthesizer
- **MIDI interface** connecting Mac to OBX8

## Supported DAWs

- Bitwig Studio
- Reaper
- FL Studio 
- Any DAW with CLAP support

## Troubleshooting

### Plugin Not Appearing
- Restart your DAW completely
- Check plugin is in `~/Library/Audio/Plug-Ins/CLAP/`
- Verify DAW supports CLAP format

### MIDI Not Working
- Select correct MIDI device in plugin
- Try "Auto-detect OBX8" option
- Check MIDI cables and interface
- Verify OBX8 MIDI settings

### Build Issues
- Install Xcode Command Line Tools: `xcode-select --install`
- Install CMake: `brew install cmake`
- Check macOS version compatibility

## License

This plugin is provided as-is for controlling Oberheim OBX8 hardware.

## Version

1.0.0 - Initial release with MIDI device selection and NRPN support