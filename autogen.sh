#!/bin/sh

libtoolize -c --force
aclocal
autoheader
autoconf
automake -c --add-missing
