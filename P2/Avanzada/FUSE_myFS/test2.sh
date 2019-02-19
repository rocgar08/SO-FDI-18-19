#!/bin/bash

rm -Rf tmp
echo "Creando tmp \n"
mkdir tmp

echo "Copiando ficheros a tmp \n"
cp src/fuseLib.c tmp
cp src/myFS.h    tmp

if test -d mount-point ; then 
	echo "Directorio mount-point encontrado borrando contenido \n"
	rm -r mount-point/*
else
	echo "Directorio mount-point no encontrado, se creara uno nuevo \n"
fi
mkdir mount-point

echo "Copiando fuseLib.c y myFs a mount-point \n"
cp src/fuseLib.c mount-point
cp src/myFS.h    mount-point


#Audita el disco.
./my-fsck-static-64\  virtual-disk

vDif1="$(diff -q tmp/fuseLib.c mount-point/fuseLib.c)"
echo "Comparando fuseLib.c \n"

if test -z "$vDif1" ; then
echo "------------------>>>>>>>>>>>>>>>>>>>> Son iguales \n"
else
echo "------------------>>>>>>>>>>>>>>>>>>>> Son diferentes \n"
fi

echo "Truncando fuseLib.c, un bloque menos \n"

truncate mount-point/fuseLib.c -s -4096
truncate tmp/fuseLib.c -s -4096

#Audita el disco.
./my-fsck-static-64\  virtual-disk

vDif1="$(diff -q tmp/fuseLib.c mount-point/fuseLib.c)"
echo "Comparando fuseLib.c con un bloque menos \n"


if test -z "$vDif1" ;then
echo "------------------>>>>>>>>>>>>>>>>>>>> Son iguales \n"
else
echo "------------------>>>>>>>>>>>>>>>>>>>> Son diferentes \n"
fi
 
touch fichero.txt
echo "Holothuroidea(Pepino de Mar)" > fichero.txt 
echo "Copiando fichero.txt \n"
cp fichero.txt mount-point

#Audita el disco.
./my-fsck-static-64\  virtual-disk

vDif1="$(diff -q fichero.txt mount-point/fichero.txt)"
echo "Comparando fichero.txt \n"

if test -z "$vDif1" ; then
echo "------------------>>>>>>>>>>>>>>>>>>>> Son iguales \n"
else
echo "------------------>>>>>>>>>>>>>>>>>>>> Son diferentes \n"
fi

echo "Truncando myFS.h, ocupa un bloque más (+4096B)  \n"
truncate mount-point/myFS.h -s +4096
truncate tmp/myFS.h -s +4096

#Audita el disco.
./my-fsck-static-64\  virtual-disk

vDif1="$(diff -q tmp/myFS.h mount-point/myFS.h)"
echo "Comparando myFS.h, ahora tiene un bloque más \n"

if test -z "$vDif1" ; then
echo "------------------>>>>>>>>>>>>>>>>>>>> Son iguales \n"
else
echo "------------------>>>>>>>>>>>>>>>>>>>> Son diferentes \n"
fi


