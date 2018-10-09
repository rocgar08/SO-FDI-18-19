#!/bin/sh

if test -x mytar ; then 
	echo "mytar es ejecutable"
else
	echo "mytar no es ejecutable"
	exit
fi

if test -d tmp ; then 
	echo "Directorio tmp encontrado borrando contenido "
	rm -r tmp
else
	echo "Directorio tmp no encontrado, se creara uno nuevo"
fi

vDir="$(pwd)"

#Creacion de las carpetas
mkdir tmp
cd tmp
mkdir out

#Creacion de ficheros de prueba.
touch file1.txt file2.txt file3.dat
echo "Hello World" > file1.txt
head /etc/passwd > file2.txt
head -c 1024 /dev/urandom > file3.dat

cp file1.txt file2.txt file3.dat $vDir

cd ..

#Da segmentation fault cuando va a crear un archvo con mismo nombre.
if test -f filetar.mtar ; then
	rm filetar.mtar
fi

./mytar -cf filetar.mtar file1.txt file2.txt file3.dat

rm file3.dat file1.txt file2.txt 

cp filetar.mtar tmp/out

./mytar -xf filetar.mtar

mv file1.txt file2.txt file3.dat tmp/out

cd tmp 
vDir="$(pwd)"
cd out 

vDif1="$(diff -q file1.txt ${vDir}/file1.txt)"
vDif2="$(diff -q file2.txt ${vDir}/file2.txt)"
vDif3="$(diff -q file3.dat ${vDir}/file3.dat)"

cd ..
cd ..

if test -z "$vDif1" && test -z "$vDif2" && test -z "$vDif3" ; then 
	echo "Correct!! "
	return 0
else
	echo "Fail.."
	if test -n "$vDif1" ; then
		echo "$vDif1"
	fi

	if test -n "$vDif2"; then
		echo "$vDif2"
	fi
	
	if test -n "$vDif3" ; then
		echo "$vDif3"
	fi

	return 1
fi
