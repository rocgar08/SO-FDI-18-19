#!/bin/bash

if diff src/fuseLib.c src/fuseLib.h; then
echo "Son iguales"
else
echo "Son diferentes"
fi