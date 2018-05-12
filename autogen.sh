#!/bin/bash

libtoolize --quiet

aclocal 

autoconf

automake --add-missing

