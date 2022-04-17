# Storm-Engine
Game engine behind [Sea Dogs](https://en.wikipedia.org/wiki/Sea_Dogs_(video_game)), [Pirates of the Caribbean](https://en.wikipedia.org/wiki/Pirates_of_the_Caribbean_(video_game)) and [Age of Pirates](https://en.wikipedia.org/wiki/Age_of_Pirates_2:_City_of_Abandoned_Ships) games.

[![Join the chat at https://discord.gg/jmwETPGFRe](https://img.shields.io/discord/353218868896071692?color=%237289DA&label=storm-devs&logo=discord&logoColor=white)](https://discord.gg/jmwETPGFRe) 
[![GitHub Actions Build Status](https://github.com/storm-devs/storm-engine/actions/workflows/cibuild.yml/badge.svg)](https://github.com/storm-devs/storm-engine/actions/workflows/cibuild.yml)

 * [GitHub Discussions](https://github.com/storm-devs/storm-engine/discussions)
 * [Discord Chat](https://discord.gg/jmwETPGFRe)

## Supported games
- [Sea Dogs: To Each His Own](https://github.com/storm-devs/sd-teho-public)
- [Sea Dogs: City of Abandoned Ships](https://store.steampowered.com/app/937940/Sea_Dogs_City_of_Abandoned_Ships/) (work in progress)
- [Pirates of the Caribbean: New Horizons](https://www.piratesahoy.net/wiki/new-horizons/) (work in progress)

<p align="center">
<img src="https://steamuserimages-a.akamaihd.net/ugc/879748394074455443/FD04CEA2434D8DACAD4886AF6A5ADAA54CDE42AA/">
</p>

## Building the project
You need to install [Conan](https://conan.io/downloads.html) and add it to the `%PATH%` environment variable. Also, make sure you have the following Visual Studio components installed:
- C++ CMake Tools for Windows
- C++ Clang Compiler for Windows
- C++ MFC for latest v142 build tools (x86 & x64)
- Windows 10 SDK
- [Rust with Cargo](https://rustup.rs/)
- [CBindgen](https://github.com/eqrion/cbindgen)
- [Corrosion installed as CMake package](https://github.com/corrosion-rs/corrosion)

Open the repo root as a CMake project in Visual Studio 2019 and select `engine.exe` as a startup item.

For running `engine.exe` you need to have [DirectX 9 runtime libraries](https://www.microsoft.com/en-us/download/details.aspx?id=8109) installed.
You will also need assets from one of the supported games. 

## Oxidation

To rewrite a library in Rust you need:

* Create new `lib` project named `library-rs` within library folder
* Set crate type to `staticlib`
* Create new `cbindgen.toml` file with `pragma_once` set to `true` and `include_guard` set to `rust_<library>_h`

Current progress can be tracked [here](https://docs.google.com/spreadsheets/d/19E9eoRhXB-EakGLOqJRBDAnl3rYTtt15ZeKNh-GJN3Q/edit?usp=sharing)

### Oxidation Workflow

Necessary steps when rewriting C++ to Rust:

* Collect live samples by playing the game (preferably a few thousands of samples)
* Make sure your code returns correct value for each sample
* Compare values returned from your Rust code with values returned by original C++ code during live gameplay
* Reduce collected live samples to a simple unit-test
* Replace calls to C++ code with calls to Rust code

**Notice:** in certain cases Step 1 can be skipped (when it slows down the game to unplayable state even in release mode).

**Notice:** Rust code assumes all strings to be in valid UTF-8 encoding. However, that is not always the case. In such cases create a separate method for each encoding used.

## Roadmap
Since our development team is small, we want to reduce the amount of code we have to maintain.
For this reason, we decided to rely on the C++ standard library or third-party libraries if possible.

Some things that we are going to do:
- Replace a custom math library with a third-party one, e.g. [glm](https://github.com/g-truc/glm).
- Replace custom rendering code with a [bgfx](https://github.com/bkaradzic/bgfx) library.
- Replace custom input handling code with a third-party library (see the [discussion](https://github.com/storm-devs/storm-engine/discussions/19)).
- Replace custom data structures with C++ standard types.
- Replace a custom scripting language with Lua.
- Replace ini config files with a standard format (see the [discussion](https://github.com/storm-devs/storm-engine/discussions/26)).
- Replace custom binary asset formats with standard ones.
- Update code using the latest C++ standard

## Contributing
If you'd like to get involved, please check [CONTRIBUTING.md](https://github.com/storm-devs/storm-engine/blob/develop/CONTRIBUTING.md).

## License
[GPL-3.0 License](https://choosealicense.com/licenses/gpl-3.0/)
