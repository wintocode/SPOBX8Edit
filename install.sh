#!/bin/bash

# SPOBX8Edit CLAP Plugin Installer
# Builds and installs the OBX8 hardware editor plugin

set -e

echo "🎹 SPOBX8Edit Plugin Installer"
echo "================================"

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ This installer is for macOS only"
    exit 1
fi

# Check for required tools
echo "🔍 Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Please install it:"
    echo "   brew install cmake"
    exit 1
fi

if ! command -v clang++ &> /dev/null; then
    echo "❌ Clang++ not found. Please install Xcode Command Line Tools:"
    echo "   xcode-select --install"
    exit 1
fi

echo "✅ Dependencies found"

# Initialize submodules (for CLAP headers)
echo "📦 Updating dependencies..."
git submodule update --init --recursive

# Create build directory
echo "🏗️  Building plugin..."
mkdir -p build
cd build

# Configure and build
cmake ..
make

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "✅ Build successful"

# Install plugin
echo "📦 Installing plugin..."

# Create plugin directory structure
PLUGIN_DIR="$HOME/Library/Audio/Plug-Ins/CLAP/SPOBX8Edit.clap"
mkdir -p "$PLUGIN_DIR/Contents/MacOS"

# Copy plugin binary
cp SPOBX8Edit.clap "$PLUGIN_DIR/Contents/MacOS/"

# Create Info.plist
cat > "$PLUGIN_DIR/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>SPOBX8Edit.clap</string>
	<key>CFBundleIdentifier</key>
	<string>com.spobx8.edit</string>
	<key>CFBundleName</key>
	<string>SPOBX8Edit</string>
	<key>CFBundleVersion</key>
	<string>1.0.0</string>
	<key>CFBundleShortVersionString</key>
	<string>1.0.0</string>
	<key>CFBundlePackageType</key>
	<string>BNDL</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
</dict>
</plist>
EOF

echo "✅ Plugin installed to: $PLUGIN_DIR"
echo ""
echo "🎉 Installation complete!"
echo ""
echo "📝 Next steps:"
echo "   1. Restart your DAW (Bitwig, Reaper, etc.)"
echo "   2. Look for 'SPOBX8Edit' in your plugin list"
echo "   3. Select your OBX8 MIDI device in the plugin"
echo "   4. Start controlling your hardware!"
echo ""
echo "🔧 Features:"
echo "   • 30 OBX8 parameters with NRPN mapping"
echo "   • MIDI device selection"
echo "   • Auto-detect OBX8 hardware"
echo "   • Bidirectional parameter sync"