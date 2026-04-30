#!/usr/bin/env bash

set -e
root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

info()  { echo -e "${BLUE}[*]${NC} $1"; }
success() { echo -e "${GREEN}[✓]${NC} $1"; }
warn()   { echo -e "${YELLOW}[!]${NC} $1"; }
error()  { echo -e "${RED}[✗]${NC} $1"; }
# Check doxygen
if ! command -v doxygen >/dev/null 2>&1; then
    error "Doxygen not found! Install with your package manager of choice"
    exit 1
fi
info "Doxygen found: $(doxygen --version)"
# Config
DOXYFILE="${root_dir}/Doxyfile"
OUT_DIR="${root_dir}/docs/html"
# Create Doxyfile if missing
if [ ! -f "${DOXYFILE}" ]; then
    warn "Doxyfile not found, creating default..."
    cat > "${DOXYFILE}" << 'EOF'
PROJECT_NAME           = "FuncDoodle"
OUTPUT_DIRECTORY       = docs
INPUT                  = src
RECURSIVE             = YES
FILE_PATTERNS          = *.h *.cc
GENERATE_HTML          = YES
GENERATE_LATEX          = NO
GENERATE_XML           = NO
EXTRACT_ALL            = NO
WARNINGS_AS_ERROR      = NO
WARN_IF_UNDOCUMENTED = YES
QUIET                  = YES
EOF
    success "Created ${DOXYFILE}"
fi
# Run doxygen
info "Generating Doxygen documentation..."
info "Input:  ${root_dir}/src/"
info "Output: ${OUT_DIR}/"
mkdir -p "${OUT_DIR}"
doxygen "${DOXYFILE}" 2>&1 | tee /tmp/doxygen.log
if [ $? -eq 0 ]; then
    success "Documentation generated in ${OUT_DIR}/"
    info "Open: ${CYAN}${OUT_DIR}/index.html${NC}"
else
    error "Doxygen failed! Check /tmp/doxygen.log"
    exit 1
fi
