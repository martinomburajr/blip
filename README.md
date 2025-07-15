# blip
This is a basic llvm injector profiler (blip) - It acts as an introduction to building LLVM based profilers that inject into Rust applications

The goal with blip is to allow for targeted profiling for Rust (and eventually C/C++) applications. 

### How it Works
`blip` uses a custom LLVM pass that adds profiling logic to either a set of predetermined functions or all symbols (These include low-level compiler instructions not included in the app). 

When a user compiles their application, the resulting object file is injected with profiler metadata around specified regions of the application.

On execution of the app, the profiled data is sent to `stdout` to be viewed. Ideally we can use a flamegraph.

### Current Unknowns
1. How do we prevent optimizations from not removing/altering the profiling logic.
