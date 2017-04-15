# addo

[![Build status](https://img.shields.io/appveyor/ci/jonnyptn/addo.svg?label=build)](https://ci.appveyor.com/project/JonnyPtn/sfml-doom)

## Administrator do

This is a program for windows which lets you run terminal commands in the current window with elevated permissions.

UAC is still required, Depending on your settings

Check [here](https://github.com/JonnyPtn/addo/releases) for the latest releases

## Compiling
I've included the visual studio solution I used, but the thing is only one source file, so you should be able to work it out if you choose not to use visual studio. 

## Installing
It's a single executable. so just put it somewhere in PATH so you can use it anywhere (personally I just bung it in system32, because i'm a loose cannon). If you use cli and need administrator permission to put it in the folder, my tool [here](https://github.com/JonnyPtn/sudo/) might help

## Usage
Whenever you want to **do** something as **ad**ministrator, presuming you've installed to a directory in your PATH, simply prepend addo:

`> addo someCommandWhichProbablyBreaksStuff`

Useful for installing projects generated via cmake (or otherwise)

`> addo msbuild install.vcxproj`

`> addo mingw-make install`

Or whatever other crazy stuff you want to do
