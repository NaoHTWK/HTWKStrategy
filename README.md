NaoTeam HTWK Teamstrategy Release
=================================


Introduction
------------

This is the official NaoTeam HTWK teamstrategy as used in the RoboCup 2016 in Leipzig. We included the strategy itself and an emulator to test it with up to five robots and a ball.
The emulator can be used via drag and drop of robots and the ball. The strategy itself is based on an blackboard AI idea.

Prerequisites
-------------

We tested the release with this sw packages (all from Ubuntu 16.04)

* CMake 3.5.1
* Protobuf 2.6.1
* Qt 5.6
* A C++11 compiler (GCC 5.4 or Clang 3.8)

How to Build
------------

Build it like any other CMake project. 

Nice to know facts
------------------

* DCM Time: Time in 100ms steps to you probably want to set a time > 100 (10s)
