#!/bin/bash

# Script to create macOS bundle structure for CLAP plugin

BUILD_DIR="$1"
SOURCE_DIR="$2"

# Save the original library file
mv "${BUILD_DIR}/SPOBX8Edit.clap" "${BUILD_DIR}/SPOBX8Edit_temp.clap"

# Create bundle directory
mkdir -p "${BUILD_DIR}/SPOBX8Edit.clap/Contents/MacOS"

# Move the built library to the bundle
mv "${BUILD_DIR}/SPOBX8Edit_temp.clap" "${BUILD_DIR}/SPOBX8Edit.clap/Contents/MacOS/SPOBX8Edit"

# Create Info.plist from template
cp "${SOURCE_DIR}/Info.plist.in" "${BUILD_DIR}/SPOBX8Edit.clap/Contents/Info.plist"

# Replace version placeholders
sed -i '' 's/@PROJECT_VERSION@/1.0.0/g' "${BUILD_DIR}/SPOBX8Edit.clap/Contents/Info.plist"

# Create PkgInfo
echo "BNDL????" > "${BUILD_DIR}/SPOBX8Edit.clap/Contents/PkgInfo"

echo "macOS bundle created successfully"