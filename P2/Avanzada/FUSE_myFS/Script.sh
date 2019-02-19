#!/bin/bash

vDif1="$(diff -q src/fuseLib.c src/fuseLib.c)"

if test -z "$vDif1" ; then
echo "Son iguales"
else
echo "Son diferentes"
fi


if test -d tmp ; then 
	echo "Directorio tmp encontrado borrando contenido "
	rm -r tmp
else
	echo "Directorio tmp no encontrado, se creara uno nuevo"
fi

mkdir tmp
cp src/fuseLib.c tmp/fuseLib.c 
cp src/myFS.h tmp/myFS.h

if test -d mount-point ; then 
	echo "Directorio mount-point encontrado borrando contenido "
	rm -r mount-point
else
	echo "Directorio mount-point no encontrado, se creara uno nuevo"
fi

mkdir mount-point
cp src/fuseLib.c mount-point/fuseLib.c 
cp src/myFS.h mount-point/myFS.h
