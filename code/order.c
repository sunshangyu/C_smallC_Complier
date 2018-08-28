flex fi.l &&
bison -d fi.y &&
gcc fi.tab.c -lfl -ly -o parser &&
./parser test4.1.c aim1.asm &&
./parser test4.2.c aim2.asm
