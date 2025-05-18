# CStack
In lack of a better name yet to be decided, **CStack** is a Stack-Based Programming Language made in C

## Building
You need these tools:
- Linux (maybe MinGW/Cygwin/WSL2 works but I haven't tried it)
- Clang/gcc
- CMake
- other stuff idk

Run this command
```
./build.sh --build
```
This will build the program

If something fails between builds, you may want to do the following first
```
./build.sh --clean
```

You can also save yourself time by doing
```
./build.sh --clean --build
```

## Running
First read the previous section.

Then run this command
```
./build.sh --run
```

If you want to both build and run, do `./build.sh --build --run`.
If you want to do a clean build and run, you can do `./build.sh --clean --build --run`,
but conveniently there's also `./build.sh --all` which does those three all in one.

## License
TBD
