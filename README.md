zookeeper - the Apache Zookeeper C/C++ client library
==================================================

Copyright (c) 2016-2017, [Shun Tan](http://www.shuntan.se/).

[https://github.com/shuntan/libzookeeper_master](https://github.com/shuntan/libzookeeper_master)

[![Gitter chat](https://badges.gitter.im/edenhill/librdkafka.png)](https://gitter.im/shuntan/libzookeeper_master) [![Build status](https://doozer.io/badge/edenhill/libzookeeper_master/buildstatus/master)](https://doozer.io/user/shuntan/libzookeeper)


**libzookeeper** is a C library implementation of the
[Apache Zookeeper](http://zookeeper.apache.org/) protocol, ZooKeeper is a centralized service for maintaining configuration information, naming, providing distributed synchronization, and providing group services. All of these kinds of services are used in some form or another by distributed applications. Each time they are implemented there is a lot of work that goes into fixing the bugs and race conditions that are inevitable. Because of the difficulty of implementing these kinds of services, applications initially usually skimp on them ,which make them brittle in the presence of change and difficult to manage. Even when done correctly, different implementations of these services lead to management complexity when the applications are deployed.

**libzookeeper** is licensed under the 2-clause BSD license.

For an introduction to the performance and usage of libzookeeper_master, see
[INTRODUCTION.md](https://github.com/shuntan/libzookeeper_master/docs/INTRODUCTION.md)

See the [wiki](https://cwiki.apache.org/confluence/display/ZOOKEEPER/Index) for a FAQ.

**NOTE**: The `master` branch is actively developed, use latest release for production use.


#Overview#
  * Support Session Expired automatic reconnection
  * Synchronous and asynchronous mechanism
  * High-level balanced ZooKeeper (requires version >= 3.4.5)
  * Simple (legacy) client
  * Simple package for monitor
  * Stable C & C++ APIs (ABI safety guaranteed for C)
  * [Statistics](https://github.com/shuntan/libzookeeper_master/wiki/Statistics) metrics
  * Debian package: libzookeeper and libzookeeper-dev in Debian and Ubuntu
  * RPM package: libzookeeper and libzookeeper-devel
  * Portable: runs on Linux, OSX, Win32, Solaris, FreeBSD, ...


#Language bindings#

  * Java: [zookeeper](https://github.com/apache/zookeeper)
  * C#/.NET: [RabbitTeam/zookeeper-client](https://github.com/RabbitTeam/zookeeper-client)
  * D (C-like): [andreiz/php-zookeeper](https://github.com/andreiz/php-zookeeper)
  * D (C++-like): [baozh/zookeeper_cpp_client](https://github.com/baozh/zookeeper_cpp_client)
  * Go: [samuel/go-zookeeper](https://github.com/samuel/go-zookeeper)
  * PHP: [Gutza/php-zookeeper-recipes](https://github.com/Gutza/php-zookeeper-recipes)
  * Python: [python-zk/kazoo](https://github.com/python-zk/kazoo)
  * Ruby: [zk-ruby](https://github.com/zk-ruby/zookeeper)

#Users of libzookeeper#

  * [jayyi](https://github.com/jayyi/) - mooon
  * [NULL](https://github.com/NULL/) - Varnish cache 


# Usage

## Requirements
	The GNU toolchain
	GNU make
	CMAKE 
   	pthreads
	libzookeeper_mt (main)
	zlib (optional, for gzip compression support)
	libsasl2-dev (optional, for SASL GSSAPI support)
	liblz4-dev (optional, for LZ4 compression support)

## Instructions

### Building

	Source:
		cmake -DCMAKE_INSTALL_PREFIX=<installation directory> .
		make && make install
	example:
		cmake .
		make

**NOTE**: See [README.win32](README.win32) for instructions how to build
          on Windows with Microsoft Visual Studio.

### Usage in code

See [examples/example.cpp](https://github.com/shuntan/libzookeeper_master/examples/example.cpp) for an example master slave switch.

Link your program with `-llibzookeeper -lz -lpthread -lrt`.


## Documentation

The public APIs are documented in their respective header files:
 * The **C/C++** API is documented in [cpp/zookeeper_helper.h](cpp/zookeeper_helper.h)

To generate Doxygen documents for the API, type:

    make docs

For a zookeeper introduction, see
[INTRODUCTION.md](http://zookeeper.apache.org/)


## Examples

See the `examples/`sub-directory.


## Tests

See the `tests/`sub-directory.


## Support

File bug reports, feature requests and questions using
[GitHub Issues](https://github.com/shuntan/libzookeeper_master/issues)


Questions and discussions are also welcome on irc.freenode.org, #apache-zookeeper,
nickname Snaps.


### Commercial support

Commercial support is available from [Domain services](http://www.shuntan.se)