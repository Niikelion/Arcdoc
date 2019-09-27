# Arcdoc
Creates code documentation.
## Setup
To add new parsing module you have to copy module files to __langs__ folder and add name of module in __langs.txt__ file in main folder.
To add new format module you have to do the same, but with __formats__ folder and __formats.txt__ file.
## Usage
To run Arcdoc with activated language and format modules you have to use `-l` and `-of` flags respectively:
```
./Arcdoc -l cpp -of debug
```
In addition, you propably want to use flags `-f` or `-i` to be able to run some commands.
`-i` enables interactive mode, which reads commands from console.
`-f somefile.ext` reads and executes commands from file `somefile.ext`.
## Commands
To learn about Arcdoc commands see [this file](COMMANDS.md).
