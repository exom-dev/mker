<p align="center">
  <img src="public/logo.png" alt="marker">
</p>

# About <a href="https://cmake.org/cmake/help/v3.0/release/3.0.0.html"><img align="right" src="https://img.shields.io/badge/CMake-3.0-BA1F28?logo=CMake" alt="CMake 3.0" /></a><a href="https://en.wikipedia.org/wiki/C%2B%2B17"><img align="right" src="https://img.shields.io/badge/C%2B%2B-17-00599C?logo=C%2B%2B" alt="C++ 17" /></a><a href="https://nodejs.org/api/n-api.html"><img align="right" src="https://img.shields.io/badge/N--API-3-339933?logo=Node.js&logoColor=FFFFFF" alt="N-API 3" /></a>

**mker** (pronounced *marker*) is a native Markdown-like document to HTML compiler for [Node.js](https://nodejs.org).

There are differences between Markdown documents and Marker documents. Please check the [quick documentation](#Quick-Documentation).

**This is a pre-release and, as such, may suffer from breaking changes without prior notice.**

# Getting started

You can install mker just like any other npm package.

```shell
npm i mker --save
```

> Note: this packages includes a declaration file for TypeScript.

If a prebuild is already available for your platform, you're good to go. If not, see below [compiling the package](#Compiling-the-Package).

## Quick examples

Here's a basic example.

```js
const mker = require('mker');

const result = mker('Hello, *World*!');
```

*result* will be a `Buffer` containing:

```html
<p>Hello, <strong>World</strong>!</p>
```

## Quick Documentation

Marker documents have a structure similar to Markdown documents.
The biggest difference is the list of available markers, and how they behave.

Here's a cheatsheet that shows the currently planned markers. Keep in mind that
there may be bugs, as the implementation is in its early stages.

```md
# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6

Paragraph.

Multiline
paragraph.

Inline markers like *strong*, _emphasis_, =underline=, ~strikethrough, and even *_=~nested~=_* in any order.

Multiline *markers
can exist*.

- unordered
- lists
  - with sublists
    which work
    - well
  - ## and headings

Inline <html> tags </html>  of <any type *not strong*> *strong* and also
multiline</any>.
```

## Compiling the Package

You'll need to install a C/C++ compiler, as well as [CMake](https://cmake.org).

The package will be compiled when you install it through npm, if a prebuild is not available. Please note that the `devDependencies` listed in [package.json](https://github.com/exom-dev/mker/blob/master/package.json) are required for building the package.

If you're missing either a compiler or [CMake](https://cmake.org), an error will be shown. Make sure you have both, and try again.

To manually compile the package, run:

```shell
npm run rebuild
```

...or directly run [cmake-js](https://github.com/cmake-js/cmake-js):

```shell
npx cmake-js compile
```

...or globally install [cmake-js](https://github.com/cmake-js/cmake-js) to be able to run it anywhere:

```shell
npm i -g cmake-js

cmake-js compile
```

## Scripts

- **install** - installs the package and compiles it if no (pre)build is available
- **rebuild** - compiles the package
- **prebuild** - generates prebuilds the package (you need to provide your own generator)
- **check** - checks if a build or prebuild is available (if not, exits with code `1`)

# Releases

None.

# Contributing

You can contribute by reporting bugs and requesting features on the [issue page](https://github.com/exom-dev/mker/issues).

# License <a href="https://github.com/exom-dev/mker/blob/master/LICENSE"><img align="right" src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT" /></a>

**mker** was created by The Exom Developers. It is licensed under the [MIT license](https://github.com/exom-dev/mker/blob/master/LICENSE).

This project uses first-party and third-party dependencies. They are listed below, along with their licenses.

# Dependencies

## NPM Packages (dev)

- [nodejs/**node-addon-api**](https://github.com/nodejs/node-addon-api) - wrapper classes for N-API ([MIT License](https://github.com/nodejs/node-addon-api/blob/master/LICENSE.md))
- [cmake-js/**cmake-js**](https://github.com/cmake-js/cmake-js) - native addon build tool based on CMake ([MIT License](https://github.com/cmake-js/cmake-js/blob/master/LICENSE))
- [moxystudio/**cross-spawn**](https://github.com/moxystudio/node-cross-spawn) - cross-platform alternative for spawn ([MIT License](https://github.com/moxystudio/node-cross-spawn/blob/master/LICENSE))
- [substack/**minimist**](https://github.com/substack/minimist) - argument parser ([MIT License](https://github.com/substack/minimist/blob/master/LICENSE))
- [sindresorhus/**npm-run-path**](https://github.com/sindresorhus/npm-run-path) - PATH for locally installed binaries ([MIT License](https://github.com/sindresorhus/npm-run-path/blob/master/license))

## First-Party (C/C++)

- [UnexomWid/**remem**](https://github.com/UnexomWid/remem) - memory tracking via address mapping ([MIT License](https://github.com/UnexomWid/remem/blob/master/LICENSE))
- [UnexomWid/**timerh**](https://github.com/UnexomWid/timerh) - library for measuring execution time ([MIT License](https://github.com/UnexomWid/timerh/blob/master/LICENSE))