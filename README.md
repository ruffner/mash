```
 __  __       ____  _               __  __       _   _   _       ____  _          _ _ 
|  \/  | __ _/ ___|| |__           |  \/  | __ _| |_| |_( )___  / ___|| |__   ___| | |
| |\/| |/ _` \___ \| '_ \   _____  | |\/| |/ _` | __| __|// __| \___ \| '_ \ / _ \ | |
| |  | | (_| |___) | | | | |_____| | |  | | (_| | |_| |_  \__ \  ___) | | | |  __/ | |
|_|  |_|\__,_|____/|_| |_|         |_|  |_|\__,_|\__|\__| |___/ |____/|_| |_|\___|_|_|
```
A basic shell.

April 2016, for CS485G at the University of Kentucky

---------------
-- Usage
---------------
MaSh can be created by running make 

---------------
-- Limitations
---------------
- MaSh supports a maximum of 30 arguments per command line
- The maximum input buffer size is 512 bytes
- Only stdin and stdout of the child process are redirected


---------------
-- Implementation Choices
---------------
Due to time limitations, the verbosity of error notifications 
is minimalistic


---------------
-- Provided Files
---------------
Makefile
parser.c	command parser that detects redirects
shell.h			the parsed command structure as well as other definitions
run-tests		script to run provided test cases
tests/			       the provided test cases

---------------
-- Modified Files
---------------
Makefile	changed to support a c++ shell
shell.h			added custom defines
tests/			      edited expected output of various commands

---------------
-- Original Files
---------------
shell.cc	the implemented MaSh shell
