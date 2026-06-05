#!/bin/bash
# Flora Philosophica - Wayland/Hyprland Compatibility Wrapper
# Automatically manages Wayland native vs. XWayland fallback launch mechanisms.

set -e

# Path to game binary
BINARY="./build/FloraPhilosophica"

if [ ! -f "$BINARY" ]; then
    echo "Error: Game binary not found at $BINARY. Please run 'cmake --build build' first."
    exit 1
fi

echo "========================================================="
echo "Launching Flora Philosophica with Hyprland Compatibility..."
echo "========================================================="

# Detect Wayland environment
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo "Wayland session detected: $WAYLAND_DISPLAY"
    echo "Attempting native Wayland window context via GLFW..."
    
    # Try launching with Wayland platform backend.
    # If the app crashes (exit code non-zero, e.g. because GLFW wasn't built with Wayland),
    # the || operator captures the failure and triggers the XWayland fallback.
    set +e
    GLFW_PLATFORM=wayland "$BINARY" "$@"
    EXIT_CODE=$?
    set -e
    
    if [ $EXIT_CODE -ne 0 ]; then
        echo "--------------------------------------------------------"
        echo "Native Wayland launch exited with code $EXIT_CODE."
        echo "Falling back to XWayland (X11 emulation)..."
        echo "--------------------------------------------------------"
        GLFW_PLATFORM=x11 "$BINARY" "$@"
    fi
else
    echo "Standard X11 session detected. Launching natively..."
    "$BINARY" "$@"
fi
