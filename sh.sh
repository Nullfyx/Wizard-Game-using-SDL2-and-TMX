#!/bin/bash

set -e

DIST_DIR="dist"
EXEC="./wiz"

echo "Creating distribution folder..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

echo "Copying executable..."
cp "$EXEC" "$DIST_DIR/"

echo "Finding dependent shared libraries..."
libs=$(ldd "$EXEC" | grep "=> /" | awk '{print $3}')

echo "Copying libraries..."
for lib in $libs; do
    cp --parents "$lib" "$DIST_DIR"
done

echo "Strip symbols to reduce size..."
strip "$DIST_DIR/wiz"
find "$DIST_DIR" -type f -name '*.so*' -exec strip --strip-unneeded {} +

echo "Done! Your standalone folder is '$DIST_DIR'."
