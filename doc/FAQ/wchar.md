# `"wchar.h" not found`

## Symptom
When you build the package, your compiler complains that the standard library "wchar.h" is not found.

## Potential cause
It is possible that your cmake cache needs to be cleared.  For CLion users, see [here](https://intellij-support.jetbrains.com/hc/en-us/community/posts/360000919219-Clion-can-t-compile-because-of-wchar-h-not-found-after-updated-Mojave-in-mac-).