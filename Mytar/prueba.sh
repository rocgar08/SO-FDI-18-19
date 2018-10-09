#!/bin/sh

#1.Comprobar que el programa Mytar está en el directorio actual y es ejecutable.
#En caso contrario mostrará un mensaje informátivo por pantalla y terminará
#-x fichero		Tienes permiso de ejecucion en fichero (o de busqueda si es un directorio)
if [-x mytar] ;then
	echo "el fichero es ejecutable"
else
	echo "el fichero no es ejecutable o no se ha podido encontrar el directorio"
fi
# Comprobará si existe un directorio tmp dentro del directorio actual. 
#Si existe lo borrará, incluyendo todo lo que haya dentro de él (mirar la opción -r del comando rm).
#-d fichero existe y es un directorio
#-r, -R, --recursive remove directories and their contents recursively
if [-d tmp]; then 
	rm -r tmp
fi

#3.Creará un nuevo directorio temporal 
#tmp dentro del directorio actual y
# cambiará a este directorio. 
mkdir tmp
cd tmp
#4. Creará tres ﬁcheros (dentro del directorio): 
 #file1.txt:conelcontenido“Helloworld!”,utilizandolaorden echo y redirigiendo la salida al ﬁchero.
 touch file1.txt 
 echo "Hello World" > file1.txt
 #file2.txt: con una copia de las 10 primeras líneas del ﬁchero /etc/passwd. 
 	#Se hace fácil utilizando el programa head y redirigiendo la salida al ﬁchero. 
 	touch file2.txt
	# Sacado del man , -n, --lines=[-]K
	# print the first K lines instead of the first 10; 
	#with the leading '-', print all but the last K lines of each file 
	head /etc/passwd > file2.txt 
 #file3.dat:con un contenido binario aleatoriode 1024 bytes,tomado del dispositivo /dev/urandom. 
	 #De nuevo conviene utilizar head con la opción -c. 
	 touch file3.dat
	 head -c 1024 /dev/urandom
# comando > fichero 	Envía la salida de comando a fichero; sobreescribe cualquier cosa de fichero

#5. Invocará el programa  que hemos desarrollado,
#para crear un ﬁchero filetar.mtar con el contenido de los tres ﬁcheros anteriores. 
#Sacado de las transpas de ayuda
 ../Mytar -c -f filetar.mtar file1.txt file2.txt file3.dat
#6 Creará un directorio out (dentro del directorio actual, que debe ser tmp)
# y copiará el ﬁchero filetar.mtar al nuevo directorio.
mkdir out
cp  filetar.mtar out
#7 Cambiará al directorio out 
#y ejecutará el programa Mytar para extraer el contenido del tarball. 
cd out
../Mytar -x -f filetar.mtar file1.txt file2.txt file3.dat
#8 Usará el programa diff para comparar los ﬁchero extraídos con los originales, 
#que estarán en el directorio anterior (..). 
#esta en las transpas
diff file1.txt  ../file1.txt
diff file2.txt  ../file2.txt
diff file3.dat  ../file3.dat

#9 Si los tres ﬁcheros extraídos son iguales que los originales, 
#volverá al directorio original (../..), 
#mostrará el mensaje “Correct” por pantalla y devolverá 0. 
#Si hay algún error, volverá al directorio original, mostrará un mensaje descriptivo por pantalla y devolverá 1.
let c=1
let i=0
if [diff -s  file1.txt  ../file1.txt && 
    diff -s  file2.txt  ../file2.txt &&
    diff -s  file3.dat  ../file3.dat];then

    echo "Correct"
    echo $c
 else
 	cd tmp
 	echo "Los ficheros no son originales"
 	echo $i




