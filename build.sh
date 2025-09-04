
#!/bin/sh -eu

# building project files.

echo "[INFO] : Creating build folder if it does not exist."
mkdir -p build
cd build

echo "[INFO] : Compiling C files."
gcc -std=c99 -o ./out ../src/main.c ../src/parser.c

echo "[INFO] : Running."
./out && cd ..

