#!/usr/bin/env bash
# Build script for Mario Builder 64 - 3DS Port
# This script sets up the devkitPro environment and builds the 3DS version

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}=================================${NC}"
echo -e "${GREEN}Mario Builder 64 - 3DS Port Build${NC}"
echo -e "${GREEN}=================================${NC}"
echo ""

# Check for devkitPro environment
if [ -z "$DEVKITPRO" ]; then
    echo -e "${YELLOW}Warning: DEVKITPRO environment variable not set${NC}"
    echo "Attempting to auto-detect devkitPro..."
    
    # Common installation paths
    if [ -d "/opt/devkitpro" ]; then
        export DEVKITPRO="/opt/devkitpro"
    elif [ -d "$HOME/devkitpro" ]; then
        export DEVKITPRO="$HOME/devkitpro"
    elif [ -d "/usr/local/devkitpro" ]; then
        export DEVKITPRO="/usr/local/devkitpro"
    else
        echo -e "${RED}Error: Could not find devkitPro installation${NC}"
        echo "Please install devkitPro or set DEVKITPRO environment variable"
        exit 1
    fi
    
    echo -e "${GREEN}Found devkitPro at: $DEVKITPRO${NC}"
fi

if [ -z "$DEVKITARM" ]; then
    export DEVKITARM="$DEVKITPRO/devkitARM"
fi

# Set up library paths
export CTRULIB="$DEVKITPRO/libctru"
export PORTLIBS="$DEVKITPRO/portlibs/3ds"

# Check for required libraries
echo -e "${BLUE}Checking for required libraries...${NC}"

if [ ! -d "$CTRULIB" ]; then
    echo -e "${RED}Error: libctru not found at $CTRULIB${NC}"
    echo "Install with: sudo dkp-pacman -S libctru"
    exit 1
fi

# citro3d is bundled with libctru in newer versions
if [ ! -f "$CTRULIB/include/citro3d.h" ]; then
    echo -e "${RED}Error: citro3d.h not found in $CTRULIB/include/${NC}"
    echo "Install with: sudo dkp-pacman -S citro3d"
    exit 1
fi

echo -e "${GREEN}All required libraries found!${NC}"
echo ""

# Generate banner and icon
echo -e "${BLUE}Generating 3DS banner and icon...${NC}"
if command -v python3 &> /dev/null; then
    python3 tools/generate_3ds_banner.py
else
    echo -e "${YELLOW}Python3 not found, skipping banner generation${NC}"
fi
echo ""

# Prepare ROMFS
echo -e "${BLUE}Preparing ROMFS...${NC}"
if [ ! -d "romfs" ]; then
    mkdir -p romfs
fi

# Copy assets to ROMFS if they exist
if [ -d "assets" ]; then
    echo "Copying assets to ROMFS..."
    cp -r assets/* romfs/ 2>/dev/null || true
fi

echo -e "${GREEN}ROMFS prepared!${NC}"
echo ""

# Clean previous build if requested
if [ "$1" == "clean" ]; then
    echo -e "${BLUE}Cleaning previous build...${NC}"
    make -f Makefile.3ds clean
    exit 0
fi

# Build the project
echo -e "${BLUE}Building Mario Builder 64 for 3DS...${NC}"
echo "This may take a few minutes..."
echo ""

if make -f Makefile.3ds "$@"; then
    echo ""
    echo -e "${GREEN}=================================${NC}"
    echo -e "${GREEN}Build successful!${NC}"
    echo -e "${GREEN}=================================${NC}"
    echo ""
    echo "Output files:"
    if [ -f "mb64.3dsx" ]; then
        echo "  - mb64.3dsx (Homebrew Launcher)"
    fi
    if [ -f "mb64.smdh" ]; then
        echo "  - mb64.smdh (Icon/metadata)"
    fi
    if [ -f "mb64.cia" ]; then
        echo "  - mb64.cia (Installable title)"
    fi
    echo ""
    echo -e "${YELLOW}To run:${NC}"
    echo "  - Copy mb64.3dsx to your SD card"
    echo "  - Run with Homebrew Launcher"
    echo ""
    echo -e "${YELLOW}Or install CIA:${NC}"
    echo "  - Copy mb64.cia to your SD card"
    echo "  - Install with FBI or similar"
else
    echo ""
    echo -e "${RED}=================================${NC}"
    echo -e "${RED}Build failed!${NC}"
    echo -e "${RED}=================================${NC}"
    exit 1
fi
