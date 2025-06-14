# C Node Modules Cleaner

This is a simple console program designed to help identify the node_modules folders on your system and how much storage they eat up, as we all know these folders can gorw in size very quick and often are forgotten about when we move from project to project

It is an interactive program which first scans your system for the drives and then prompts you to select those to scan, it then scans them and then outputs a sorted list into a CSV file.

This project uses the following skills:
    - Dynamic Arrays
    - Sorting Algorithms (Block Sort)
    - Multi Threading

## Build Output (Release Mode)
```cmd
>.\utils\rbuild        
* gcc -c -o obj/app.o src/app.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/error.o src/error.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/main.o src/main.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/structures/drive_list.o src/structures/drive_list.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/structures/int_list.o src/structures/int_list.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/structures/node_module.o src/structures/node_module.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/structures/sorter.o src/structures/sorter.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/utils/common.o src/utils/common.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -c -o obj/utils/uint64.o src/utils/uint64.c -std=c89 -Wall -Wextra -pedantic -Werror
* gcc -o bin/NodeModuleCleaner obj/app.o obj/error.o obj/main.o obj/structures/drive_list.o obj/structures/int_list.o obj/structures/node_module.o obj/structures/sorter.o obj/utils/common.o obj/utils/uint64.o -std=c89 -Wall -Wextra -pedantic -Werror
```