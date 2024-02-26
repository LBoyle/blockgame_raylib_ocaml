# Blockgame Raylib 
working title 

A simple 3d game similar to Minecraft, with a custom engine written in ReasonML (OCaml) using the Raylib library

## Setup 

### Linux (I use Debian)

Install opam globally on your workstation and run `opam init`, clone the repo

In the project root run the following commands 
```
opam switch create . 5.0.0
eval $(opam env)
opam install . --deps-only

dune build
```
Create a virtual environment for the current folder using OCaml 5.0, update your PATH for the language version, install dependancies, then run a build to see if it works


To run the game 
```
dune exec blockgame_raylib
```

Or to run the production build 
```
./_build/default/bin/main.exe
```

### Mac 

Should be exactly the same as above, Raylib does support Metal and should work properly with no additional work, Rosetta might be required for apple silicon 

### Windows (I haven't tried this) (Not recommended)

It might be challenging to install OCaml on a Windows workstation, I wouldn't attempt it right now, I would wait for 1st class support for the latest language versions with the installer

There are instructions from OCaml website https://ocaml.org/docs/installing-ocaml

Note that the Diskuv OCaml (DKML) installer does not yet support OCaml 5.0.0, this codebase should work on OCaml 4.14.0? I haven't tested it 

I will have to work this out eventually for production builds, though it might actually be easier to cross-compile for Windows instead 

## Text Editor 

The only requirement is ocaml-lsp-server, I know this works for Vim and Emacs though I haven't tried it

It's easiest with VSCode, I use VSCodium, with the ocaml-platform plugin, you might need to run `eval $(opam env)` 
and `dune build -w` before the syntax highlighting will work correctly 

## Tech choices

#### ReasonML (OCaml)

I have been using Reason to write JavaScript apps for a while, I want to get more comfortable writing native apps and the developer-experience is great. OCaml syntax is still kinda intimidating to me, I'll stick to Reason 

The performance can be near C-like if you need it to be, this way of writing OCaml is what I want to learn with this project 

#### Raylib OCaml 

https://github.com/tjammer/raylib-ocaml 

These are simple bindings to the Raylib C library, which is header-only bindings to SDL2, It's high-level and easy to use. There are not many other options for building games using OCaml, but this one is pretty great

A drawback of this library is that it cannot target wasm, support for compiling OCaml to wasm is in its infancy, and raylib-ocaml is built on CTypes which again presents issues for targeting wasm 

#### Raylib 

https://github.com/raysan5/raylib 

The Library of choice is Raylib, not only because it has OCaml bindings, it's well documented and battle-tested, it can produce builds for a variety of production environments including the browser with wasm 

Drawbacks of this are that it's OpenGL and doesn't support Vulcan, and I can't use all the features of Raylib if I'm not writing C code, which comes with its own set of drawbacks 

#### The ideal tech

I want to see a nice high-level library like Raylib that abstracts the really complex stuff, and can target Windows, Mac(nice to have), Linux and WebAssembly. Maybe even 

