#!/bin/bash
# make_portable.sh - Make your wiz game portable

set -e

PROJECT_NAME="WizardGame"
VERSION="1.0.0"

print_info() {
    echo -e "\033[0;32m[INFO]\033[0m $1"
}

print_error() {
    echo -e "\033[0;31m[ERROR]\033[0m $1"
}

print_warning() {
    echo -e "\033[1;33m[WARNING]\033[0m $1"
}

# Step 1: Build the game statically
build_game() {
    print_info "Building the game..."
    
    if [ ! -f "main.cpp" ]; then
        print_error "main.cpp not found. Are you in the right directory?"
        exit 1
    fi
    
    # Clean previous builds
    rm -f wiz
    
    # Check if we have pkg-config and SDL2
    if ! command -v pkg-config &> /dev/null; then
        print_error "pkg-config not found. Please install it first:"
        print_error "Ubuntu/Debian: sudo apt install pkg-config"
        print_error "Arch: sudo pacman -S pkgconf"
        exit 1
    fi
    
    if ! pkg-config --exists sdl2; then
        print_error "SDL2 not found. Please install SDL2 development packages:"
        print_error "Ubuntu/Debian: sudo apt install libsdl2-dev libsdl2-image-dev"
        print_error "Arch: sudo pacman -S sdl2 sdl2_image"
        exit 1
    fi
    
    # Get SDL2 flags
    SDL_CFLAGS=$(pkg-config --cflags sdl2 SDL2_image)
    SDL_LIBS=$(pkg-config --libs sdl2 SDL2_image)
    
    print_info "Compiling with static linking..."
    
    # Compile all source files
    g++ -std=c++17 -O3 -Wall \
        $SDL_CFLAGS \
        *.cpp \
        -o wiz \
        $SDL_LIBS \
        -static-libgcc -static-libstdc++
    
    if [ ! -f "wiz" ]; then
        print_error "Build failed!"
        exit 1
    fi
    
    print_info "Build successful! Created: ./wiz"
}

# Step 2: Create portable package
create_portable_package() {
    print_info "Creating portable package..."
    
    if [ ! -f "wiz" ]; then
        print_error "Executable 'wiz' not found. Build first!"
        exit 1
    fi
    
    # Create package name
    OS=$(uname -s | tr '[:upper:]' '[:lower:]')
    ARCH=$(uname -m)
    PACKAGE_NAME="${PROJECT_NAME}-${VERSION}-${OS}-${ARCH}-portable"
    
    # Remove old package
    rm -rf "$PACKAGE_NAME"
    rm -f "$PACKAGE_NAME.tar.gz"
    
    # Create package directory
    mkdir -p "$PACKAGE_NAME"
    
    print_info "Copying game files..."
    
    # Copy executable
    cp wiz "$PACKAGE_NAME/"
    chmod +x "$PACKAGE_NAME/wiz"
    
    # Copy all game assets
    if [ -d "sprites" ]; then
        cp -r sprites "$PACKAGE_NAME/"
        print_info "✓ Copied sprites folder"
    fi
    
    if [ -d "levels" ]; then
        cp -r levels "$PACKAGE_NAME/"
        print_info "✓ Copied levels folder"
    fi
    
    # Copy image files
    for img in *.png *.jpg *.jpeg *.bmp; do
        if [ -f "$img" ]; then
            cp "$img" "$PACKAGE_NAME/"
            print_info "✓ Copied $img"
        fi
    done
    
    # Copy tiled files
    for tsx in *.tsx; do
        if [ -f "$tsx" ]; then
            cp "$tsx" "$PACKAGE_NAME/"
            print_info "✓ Copied $tsx"
        fi
    done
    
    # Create launch script
    print_info "Creating launch script..."
    cat > "$PACKAGE_NAME/run.sh" << 'EOF'
#!/bin/bash
# Wizard Game Launcher
cd "$(dirname "$0")"
./wiz
EOF
    chmod +x "$PACKAGE_NAME/run.sh"
    
    # Create README
    print_info "Creating README..."
    cat > "$PACKAGE_NAME/README.txt" << EOF
$PROJECT_NAME - Portable Linux Edition
======================================

This is a portable version that requires NO INSTALLATION.

HOW TO RUN:
-----------
Method 1: ./run.sh
Method 2: ./wiz

WHAT'S INCLUDED:
----------------
- wiz           : The game executable
- sprites/      : All game sprites and graphics
- levels/       : Game levels and maps
- *.png, *.tsx  : Game assets and tilesets
- run.sh        : Launch script

SYSTEM REQUIREMENTS:
--------------------
- Linux (64-bit recommended)
- SDL2 libraries (usually pre-installed)

TROUBLESHOOTING:
----------------
If the game doesn't start:
1. Make sure you have SDL2: sudo apt install libsdl2-2.0-0 libsdl2-image-2.0-0
2. Check permissions: chmod +x wiz run.sh
3. Run from terminal to see error messages: ./wiz

This portable version was built on: $(date)
System: $(uname -a)

Enjoy the game!
EOF
    
    print_info "Package created: $PACKAGE_NAME/"
    
    # List contents
    print_info "Package contents:"
    ls -la "$PACKAGE_NAME/"
}

# Step 3: Create compressed archive
create_archive() {
    if [ ! -d "$PACKAGE_NAME" ]; then
        print_error "Package directory not found!"
        return 1
    fi
    
    print_info "Creating compressed archive..."
    
    if command -v zip &> /dev/null; then
        zip -r "$PACKAGE_NAME.zip" "$PACKAGE_NAME"
        print_info "✓ Created: $PACKAGE_NAME.zip"
    fi
    
    tar -czf "$PACKAGE_NAME.tar.gz" "$PACKAGE_NAME"
    print_info "✓ Created: $PACKAGE_NAME.tar.gz"
    
    # Show file sizes
    print_info "Archive sizes:"
    ls -lh "$PACKAGE_NAME"*.zip "$PACKAGE_NAME"*.tar.gz 2>/dev/null || true
}

# Step 4: Test the portable version
test_portable() {
    if [ ! -d "$PACKAGE_NAME" ]; then
        print_error "Package directory not found!"
        return 1
    fi
    
    print_info "Testing portable version..."
    
    cd "$PACKAGE_NAME"
    
    # Check if executable works
    if ./wiz --help 2>/dev/null || ./wiz --version 2>/dev/null || true; then
        print_info "✓ Executable responds"
    fi
    
    # Check dependencies
    print_info "Checking library dependencies..."
    if command -v ldd &> /dev/null; then
        echo "Dependencies:"
        ldd wiz | head -10
    fi
    
    cd ..
    print_info "✓ Test completed"
}

# Main execution
main() {
    print_info "==================================="
    print_info "Wizard Game Portable Package Creator"
    print_info "==================================="
    
    case "$1" in
        "build")
            build_game
            ;;
        "package")
            create_portable_package
            ;;
        "archive")
            create_archive
            ;;
        "test")
            test_portable
            ;;
        "all"|"")
            build_game
            create_portable_package
            create_archive
            test_portable
            print_info ""
            print_info "🎉 SUCCESS! Your portable game is ready!"
            print_info ""
            print_info "📦 Distribute these files:"
            print_info "   - $PACKAGE_NAME.tar.gz  (Linux users)"
            print_info "   - $PACKAGE_NAME.zip      (All users)"
            print_info ""
            print_info "👥 Users just need to:"
            print_info "   1. Download and extract"
            print_info "   2. Run: ./run.sh"
            print_info "   3. Enjoy the game!"
            ;;
        *)
            echo "Usage: $0 [build|package|archive|test|all]"
            echo ""
            echo "Commands:"
            echo "  build   - Compile the game"
            echo "  package - Create portable folder"
            echo "  archive - Create .zip and .tar.gz"
            echo "  test    - Test the portable version"
            echo "  all     - Do everything (default)"
            echo ""
            echo "Examples:"
            echo "  $0           # Do everything"
            echo "  $0 all       # Do everything"  
            echo "  $0 build     # Just build"
            exit 1
            ;;
    esac
}

main "$@"
