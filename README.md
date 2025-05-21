# CStack
In lack of a better name yet to be decided, **CStack** is a Stack-Based Programming Language made in C.

This project is made to practice C before I make my bigger project.

Until I update this readme, you can find examples in [./examples/](https://github.com/Kirdow/cstack/blob/master/examples/).<br>
You can also find docs/dev-log explaining each above example, located in [./docs/00-examples.md](https://github.com/Kirdow/cstack/blob/master/docs/00-examples.md).

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

## Testing
First read Building section

This project uses [Unity](https://github.com/ThrowTheSwitch/Unity) for Unit Testing.

Then run this command
```bash
$ ./build.sh --build --test
```

You should see all tests passing.

## Running
First read Building section.

Then run this command
```bash
$ ./build/cstack <file>
```
This will interpret the program you input

If you want to compile, instead run
```bash
$ ./build/cstack -o <executable_file> <file>
```

## License
CStack is released and the source is available under [MIT License](https://github.com/Kirdow/cstack/blob/master/LICENSE).
