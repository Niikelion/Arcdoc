# Basics
Every command available in application is registered in [core.h file](include/core.h.md). Commands in Arcdoc are modelled on Windows Cmd commands.
Arguments are separated with space and flags have to be preceded with `-` character.
## Examples
```
selfcheck

parse test_header.h

activate cpp -l
```
# Available commands
> Note, that  this list is only up to date with master branch and will not be updated for any other branch. 
To see commands in certains branch, follow view core.h file in that branch.

- install (Not implemented yet, but already registered in core.)
- show (Takes `languages` or `formats` as argument and lists available languages/output formats.)
- activate (Takes name of module as argument and `-l` or `of` flag to acttivate module with name from argument as language or format module respectively.)
- list (Created to access parsed content, takes updated as argument and shows content marked as updated.)
- parse (Takes file path as argument and parses this file as source code with currently active module, or project if `-p` flag is specified. Adds parsed content to global scope.)
- generate (Takes two arguments: path and name and generates output file or files using currently activate format module.)
- selfcheck (Performs checks to ensure that application runs correctly.)
