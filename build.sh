
#!/bin/sh -eu

# building project files.

echo "[INFO] : Creating build folder if it does not exist."
mkdir -p build
cd build

echo "[INFO] : Compiling C files."
# That's becoming unbearable.
gcc -std=c99 -o ./out ../src/main.c ../src/parser.c ../src/request_handler.c ../src/server.c ../src/socket.c ../src/utils/str.c ../src/utils/os.c

echo "[INFO] : Running."
./out && cd ..

