Developer Documentation
=======================
**Table of Contents**

* [Debugging](#debugging)
* [Unit tests](#unit-tests)
# Debugging
Remote debug - gdb server (program is running) + Eclipse debug (debugger w/ code)

1. `cd ~/p/hstr/github/hstr/Debug && gdbserver :9999 ./hstr`
1. Eclipse/Debug Configuration/Using GDB Manual Remote Debugger launching|Debugger tab/Connection port: 9999 

# Unit tests
HSTR unit tests are written using [Unity](http://www.throwtheswitch.org/unity/).
To run and manage unit test use scripts located in:

* `build/test-*.sh`
