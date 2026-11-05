#!/bin/bash
# Fix matplot++ unused variable warning

# Get the matplot++ source directory from environment or argument
MATPLOT_DIR="${1:-$MATPLOTPLUSPLUS_SOURCE_DIR}"

if [ -z "$MATPLOT_DIR" ]; then
    echo "Error: matplot++ source directory not specified"
    exit 1
fi

CONTOURC_FILE="$MATPLOT_DIR/source/matplot/util/contourc.cpp"

if [ ! -f "$CONTOURC_FILE" ]; then
    echo "Error: $CONTOURC_FILE not found"
    exit 1
fi

# Apply the fix
sed -i '340s#size_t i = 0;#size_t i = 0; (void)i;#' "$CONTOURC_FILE"
echo "Patched: $CONTOURC_FILE"
