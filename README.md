This Windows API Table control was originally written for package ui (https://github.com/andlabs/ui), my portable GUI library for Go, to replace use of the list view common control, which was becoming too restrictive for my use. It was always intended to be turned into a standalone control; now it exists as a standalone repository. However, it is far from ready to be used as a standalone control. Stay tuned.

As of November 28, 2015 it requires Microsoft's toolchain to build. MinGW support (and thus cross-compiling) will return once MinGW-w64 adds UI Automation and isolation awareness.
