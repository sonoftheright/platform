del ./run/example.exe
del ./run/example.pdb
del ./run/example.lib
del ./run/example.exp


call clang ./example.c -O0 -D DEBUG=1 -g -fsanitize=address -Wall -Werror -Wextra -Wl,-subsystem:console  -luser32.lib -lshell32.lib -ladvapi32.lib  -lgdi32.lib -ldbghelp.lib -lOpenGL32.lib -o ./run/example.exe -v

start /d "./run/" example.exe
