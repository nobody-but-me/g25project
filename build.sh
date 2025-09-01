
#!/bin/sh -eu

# building project files.

rm -rf ./out
gcc -std=c99 -o ./out ../main.c

