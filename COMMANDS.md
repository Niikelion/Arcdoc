# Basics
Every command available in the application is registered in [core.h file](include/core.h.md). Commands in Arcdoc are modelled on Windows Cmd commands.
Arguments are separated with spaces. Flags have to be preceded with `-` character.
## Examples
```
selfcheck

parse test_header.h

activate cpp -l
```
# Available commands
> Note, that  this list is only up to date with master branch and will not be updated for any other branch. 
To see commands in certain branch, view core.h file from that branch.

| Command   | Description |
| --------- | ----------- |
| install   | Not implemented yet, but already registered in core. |
| show      | Takes `languages` or `formats` as an argument and lists available languages/output formats. |
| activate  | Takes name of module as an argument and `-l` or `of` flag to acttivate the module with name from argument as language or format module respectively. |
| list      | Created to access parsed content, takes `updated` as an argument and shows content marked as updated. |
| parse     | Takes file path as an argument and parses this file as source code with currently active module or project if `-p` flag is specified. Adds parsed content to the global scope. |
| generate  | Takes path as an argument and generates output file or files using currently activate format module. `-n name` changes name of output, but keep in mind that it is handled by the format module. |
| selfcheck | Performs checks to ensure that the application runs correctly. |
