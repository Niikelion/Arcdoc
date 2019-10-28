# Arcdoc
Creates code documentation.
## Dependencies
Compiling Arcdoc requires [Nullscript](https://github.com/Niikelion/Nullscript) and Nlib libraries.
## Setup
To add a new parsing module copy module files to __langs__ folder and add name of module in __langs.txt__ file in main folder.
To add a new format module do the same, but with __formats__ folder and __formats.txt__ file.
## Usage
To run Arcdoc with activated language and format modules use `-l` and `-of` flags respectively:
```
./Arcdoc -l cpp -of debug
```
In addition, use flags `-f` or `-i` to be able to run some commands.
`-i` Enables interactive mode, which reads commands from console.
`-f somefile.ext` Reads and executes commands from file `somefile.ext`.
## Commands
To learn about Arcdoc commands see [this file](COMMANDS.md).
