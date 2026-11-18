# glfw-sdl-shim

Implements the GLFW API on top of SDL. Specifically, SDL3.

GLFW is not involved at all. The targeted version is compatibility with GLFW 3.5.1.

AI contributions are not welcome here, without exception.

## Coding style

Use `inline` instead of `extern` when managing global statics. Don't be afraid 
to require a newer C++ version; it already makes use of C++ 23 features.

GLib coding style is generally preferred.
