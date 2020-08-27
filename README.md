# liboqs-node

Node.js bindings for [liboqs](https://github.com/open-quantum-safe/liboqs) (Open Quantum Safe) through [liboqs-cpp](https://github.com/open-quantum-safe/liboqs-cpp).

**WARNING: The cryptographic algorithms used in liboqs are experimental. Do not use this module in production.**

If you still decide to ignore the above warning, then please use this module only on data that had
been encrypted with a standard and trusted algorithm beforehand.

## Installing

(Note: This module has only been tested on Ubuntu 20.04 and may not work on non-Linux platforms.
Please report any issues.)

You will first need to install the [liboqs dependencies](https://github.com/open-quantum-safe/liboqs#quickstart).
Follow the instructions for your OS.

Then, install the module as usual: `npm install liboqs-node`.
