Binary clock for the eZ430 Chronos
==================================

This is a module for the [openchronos-ng-elf](https://github.com/BenjaminSoelberg/openchronos-ng-elf) operating system on the eZ430 Chronos. It shows the time in binary.

How to build
------------

You will need the openchronos-ng-elf repository in order to build this.

Copy `binclock.c` and `binclock.cfg` into the modules folder of openchronos-ng-elf.

Run `make config` to activate the module, followed by `make`.
