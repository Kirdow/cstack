#!/bin/bash

# Set default flags
DO_CLEAN=false
DO_BUILD=false
DO_TEST=false
DO_RUN=false

# Check for flags
for arg in "$@"; do
    case $arg in
        --clean)
            DO_CLEAN=true
            shift
            ;;
        --build)
            DO_BUILD=true
            shift
            ;;
        --test)
            DO_TEST=true
            shift
            ;;
        --run)
            DO_RUN=true
            shift
            ;;
        --all)
            DO_CLEAN=true
            DO_BUILD=true
            DO_TEST=true
            DO_RUN=true
            shift
            ;;
    esac
done

# Enable exit on error
set -e

## Cleaning step
if [ "$DO_CLEAN" = true ]; then
    if [ -d "./build" ]; then
        echo "] rm -r build"
        rm -r build
        echo "Clean done."
    fi
fi

## Building step
if [ "$DO_BUILD" = true ]; then
    # Create build dir
    mkdir -p build

    # Enter build dir
    cd build

    # Building
    echo "] Running CMake"
    cmake ..

    echo "] Running Make"
    make

    cd ..
fi

## Testing step
if [ "$DO_TEST" = true ]; then
    echo "] Testing CStack"
    ./build/test_run $@
fi

## Running step
if [ "$DO_RUN" = true ]; then
    echo "] Running CStack"
    ./build/cstack $@
fi

