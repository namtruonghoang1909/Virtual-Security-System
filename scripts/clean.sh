#!/bin/bash

# --- VSS: Cleanup ---
# Removes compiled binaries and build artifacts.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
BIN_DIR="$PROJECT_ROOT/bin"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}[*] Cleaning up project...${NC}"

if [ -d "$BIN_DIR" ]; then
    rm -rf "$BIN_DIR"
    echo -e "${GREEN}[SUCCESS] bin/ directory removed.${NC}"
else
    echo -e "${BLUE}[INFO] Nothing to clean.${NC}"
fi
