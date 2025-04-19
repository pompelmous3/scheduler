#!/bin/bash

# Display usage instructions
usage() {
    echo "Usage: $0 [ubuntu|mac]"
    echo "Example: $0 ubuntu"
    exit 1
}

# Check if argument is provided
if [ "$#" -ne 1 ]; then
    echo "Error: Missing argument."
    usage
fi

OS="$1"

if [ "$OS" = "ubuntu" ]; then
    echo "Installing dependencies for Ubuntu..."
    sudo apt-get update

    if command -v g++ $>/dev/null; then
        echo "g++ is already installed: $(g++ --version | head -n 1)"
    else
        echo "g++ not found, installing..."
        sudo apt-get install -y g++
    fi
    sudo apt-get install -y libncurses5-dev libncursesw5-dev libsqlite3-dev
    echo "Ubuntu environment setup complete."

elif [ "$OS" = "mac" ]; then
    echo "Installing dependencies for macOS..."

    # Check for Homebrew installation
    if ! command -v brew &>/dev/null; then
        echo "Homebrew not found. Please install Homebrew first: https://brew.sh/"
        exit 1
    fi

    ## g++ check
    if command -v g++ $>/dev/null; then
        echo "g++ is already installed: $(g++ --version | head -n 1)"
    else
        echo "g++ not found, installing..."
        brew install gcc
    fi
    # Install ncurses using brew
    brew install ncurses

    # Update environment variables
    SHELL_RC="${HOME}/.zshrc"
    echo "Setting UTF-8 environment variables..."
    grep -qxF 'export LANG=en_US.UTF-8' "$SHELL_RC" || echo 'export LANG=en_US.UTF-8' >> "$SHELL_RC"
    grep -qxF 'export LC_ALL=en_US.UTF-8' "$SHELL_RC" || echo 'export LC_ALL=en_US.UTF-8' >> "$SHELL_RC"

    echo "Setting ncurses paths..."
    grep -qxF 'export PATH="/opt/homebrew/opt/ncurses/bin:$PATH"' "$SHELL_RC" || echo 'export PATH="/opt/homebrew/opt/ncurses/bin:$PATH"' >> "$SHELL_RC"
    grep -qxF 'export CXXFLAGS="-I/opt/homebrew/opt/ncurses/include $CXXFLAGS"' "$SHELL_RC" || echo 'export CXXFLAGS="-I/opt/homebrew/opt/ncurses/include $CXXFLAGS"' >> "$SHELL_RC"
    grep -qxF 'export LDFLAGS="-L/opt/homebrew/opt/ncurses/lib $LDFLAGS"' "$SHELL_RC" || echo 'export LDFLAGS="-L/opt/homebrew/opt/ncurses/lib $LDFLAGS"' >> "$SHELL_RC"

    # Reload shell configuration
    echo "Reloading shell configuration..."
    source "$SHELL_RC"

    echo "macOS environment setup complete."

else
    echo "Unsupported OS: $OS"
    usage
fi
