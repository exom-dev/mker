<p align="center">
  <img src="public/logo.png" alt="marker">
</p>

# About <a href="https://cmake.org/cmake/help/v3.0/release/3.0.0.html"><img align="right" src="https://img.shields.io/badge/CMake-3.0-BA1F28?logo=CMake" alt="CMake 3.0" /></a><a href="https://en.wikipedia.org/wiki/C%2B%2B17"><img align="right" src="https://img.shields.io/badge/C%2B%2B-17-00599C?logo=C%2B%2B" alt="C++ 17" /></a><a href="https://nodejs.org/api/n-api.html"><img align="right" src="https://img.shields.io/badge/N--API-3-339933?logo=Node.js&logoColor=FFFFFF" alt="N-API 3" /></a>

**mker** (pronounced *marker*) is a native Markdown-like document parser for [Node.js](https://nodejs.org).

Currently, it can only render HTML.

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

Here's a cheatsheet that shows the currently available markers. Keep in mind that
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

Inline markers like *strong*, _emphasis_, =underline=, ~strikethrough~, and even *_=~nested~=_* in any order.

Markers can be escaped: under\_score, 5 \* 10 + 20 \* 30.

\\ is a single backslash. If \ is not followed by either a marker or \, it is rendered as-is.

Multiline * markers
can exist, even with spaces around words. *.

- unordered
- lists
  - with sublists
    which work
    - well
  - ## and headings

1. ordered lists
1. which can have arabic numerals

i. lowercase roman numerals

I. uppercase roman numerals

a. lowercase letters

A. uppercase letters

5. lists that start at a specific number

aaa. this starts at 'aaa'
aaa. this is 'aab'

9. reverse lists
8. if the second element has a number less than the first
8. this is 7

IV. same for roman numerals
III. if a different type of list is encountered
a. it is counted as a separate list

Inline <html> tags </html>  of <any type *not strong*> *strong* and also
multiline</any>.
```

### Ordered lists

An ordered list is marked by a sequence of characters followed by `.`

The sequence of characters can be made up of either:

- **any** number of characters that make up a valid **lowercase** roman numeral
- **any** number of characters that make up a valid **uppercase** roman numeral
- **max 5** lowercase letter characters (`a` -> `zzzzz`)
- **max 5** uppercase letter characters (`A` -> `ZZZZZ`)
- **max 9** digit characters (`0` -> `999999999`)

The sequence of characters will be converted to a number.

The number attached to the first list element represents the list start. The type of this number
represents the list type (*e.g.* `IV.` will give a start of 4 and an uppercase roman type).

The number attached to the second element can have a special meaning. If it is smaller than the number
attached to the first list element, the list will be reversed.

The numbers attached to the next list elements will be ignored, as long as they are of the same type.

If two adjanced list elements have different types, they will belong in different lists.

### HTML Entities

Some symbols are automatically translated to HTML entities:

- `<` - `&lt;`
- `>` - `&gt;`
- `&` - `&amp;`

The `&` symbol will not be translated if it's already part of an HTML entity.

```html
Simple & symbol      -> Simple &amp; symbol
Simple &copy; entity -> Simple &copy; entity
```

### HTML Tags

The text inside HTML tags will be left as-is. Furthermore, no entity translations will be performed.

Example input:

```md
This is *strong &* while <span attr="this is *not*, and & is not an entity"> </span>
```

Output:

```html
<p>This is <strong>strong &amp;</strong> while <span attr="this is *not*, and & is not an entity"> </span></p>
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

[0.1.0](https://github.com/exom-dev/mker/releases/tag/0.1.0) - May 9th, 2021

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