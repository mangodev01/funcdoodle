# FuncDoodle

Simple 2D pixel-art animation program written in C++.

## Build
To build, run `scripts/build.sh` (unix) or `scripts/build.bat` (windows). `scripts/build.sh` now works with no arguments (defaults: `debug true false true`) and still accepts optional overrides in this order: `[debug|release] [tiling] [clean] [run]`.
BTW for unix a zsh completion script is available: 
```sh
source scripts/completion/completion.zsh
```

## NOTE:
I haven't tested this on windows, i mean I have, but don't have the chance to very often.
So if you're trying to build on windows, just know that it MAY or MAY NOT actually work. 

## Cross-compile
You can cross compile using mingw32-w64 on macos/linux to windows... 
use 
```sh
source scripts/completion/completion_cross.zsh
```
for completion, and run scripts/build_cross.sh. (usage is shown if ran with no arguments)

### macOS
For macOS users, there's a scripts/pkg_app.sh script and a scripts/create_dmg.sh script. First, run scripts/build.sh to build the project. Then, ./scripts/pkg_app.sh, and scripts/create_dmg.sh to create a DMG out of the newly created app
