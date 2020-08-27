# liboqs-node

Node.js bindings for [liboqs](https://github.com/open-quantum-safe/liboqs) (Open Quantum Safe) through [liboqs-cpp](https://github.com/open-quantum-safe/liboqs-cpp).

**WARNING: The cryptographic algorithms used in liboqs are experimental. Do not use this module in production.**

If you still decide to ignore the above warning, then please use this module only on data that had
been encrypted with a standard and trusted algorithm beforehand.

## API Documentation

Please see https://tapucosmo.github.io/liboqs-node/index.html.

```js
const {
  Random, // Utilities for generating secure random numbers
  KEMS, // Information on supported key encapsulation mechanisms
  KeyEncapsulation, // Key encapsulation class and methods
  Sigs, // Information on supported signature algorithms
  Signature // Signature class and methods
} = require("liboqs-node");
```

## Installing

(Note: This module has only been tested on Ubuntu 20.04 and may not work on non-Linux platforms.
Please report any issues.)

You will first need to install the [liboqs dependencies](https://github.com/open-quantum-safe/liboqs#quickstart).
Follow the instructions for your OS.

Additionally, you should have OpenSSL >=v1.1.1.

Then, install the module as usual: `npm install liboqs-node`.

There are currently no prebuilt binaries.

## Issues

Please report issues at https://github.com/TapuCosmo/liboqs-node/issues.

## Pull Requests

Feel free to submit pull requests to https://github.com/TapuCosmo/liboqs-node/pulls.
