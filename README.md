# cpp-isort

This is a import sorting utility for C++ header and source files. It can be used
from the command line, or in conjunction with vim via the vim-plug plugin.

Usage:
```
cpp-isort <filename> [<filename>...]
```

## Sort Order

cpp-isort follows the following rules:
  * Builtin imports appear first. These are denoted by the syntax `#include
    <name>`
  * Third-party imports appear second. These are deonted by the syntax `#include
    <name.h>`
  * Local imports appear last. These are denoted by the syntax `#include
    "name.h"`
  * All import types are separated by a single blank line
  * Within each type of import the imports are sorted lexicographically by the
    full import path provided.
  * Groups of imports that you wish to sort separately and separate from each
    other can be denoted by putting any non-blank non-import statement between
    groups: e.g. a line containing only a comment can separate groups; or a line
    containing a `#DEFINE` can separate groups.
