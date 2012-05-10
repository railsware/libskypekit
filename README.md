## Libskypekit

Libskypekit is thread-safe C library that provides *synchronous* SkypeKit API for *asynchronous* C++ SkypeKit SDK.

## Description

The goal of Libskypekit is catch all asynchronous callbacks of SkypeKit SDK and push they as events into one queue.

Then regular synchronous application can pull queue, retrieve each event and do own business logic.

Currently implemented events:

* Account status changed
* Chat message received

## Memory management

For each new event Libskypekit dynamically allocates memory for event structure and for event data and then push event into internal events queue. The queue size is only limited by size of operating system available memory.
So after processing certain event you are responsible to free event memory otherwise you will have memory leak. See corresponding API call.

## Dependencies

* SkypeKit SDK 4.x.x

## Building

**Note.** Before build libskypekit you must obtain, unpack and compile SkypeKit SDK first!

    $ git clone git@github.com:railsware/libskypekit.git
    $ cd libskypekit   
    $ DEBUG=1 SKYPEKIT_SDK=path/to/compliled/sdk ./build.sh

`DEBUG=1` is optional but gives your opportunity to see what is going in library.

## Issues on x86_64

When you have error like:

    relocation R_X86_64_32S against `vtable for SEClientSession' can not be used when making a shared object; recompile with -fPIC

Try to recompile SDK with `-fPIC` :

    export CC="gcc -fPIC"
    export CXX="g++ -fPIC"
    ./BuildWithCmake.sh

## Installation

To install bin, include, lib files into `/usr/local` just type:

    $ sudo ./install.sh

Probably it's better to create package for your OS and install it via package manager.

## API

See [skypekit.h](https://github.com/railsware/libskypekit/blob/master/include/skypekit.h)

## Examples

In `examples` directory you may find out how you may use libevent.

Build it with:

    $ ./build_examples.sh

Run ping_pong example:

    $ ./bin/skypekit_ping_pong my.pem 127.0.0.1 8963 my_skypename my_password

## Author

* Andriy Yanko

## Contributors

* Volodymyr Bezobiuk
* Alexey Vasiliev

## License

* Copyright (c) 2012 Railsware (www.railsware.com)
* [MIT](www.opensource.org/licenses/MIT)
