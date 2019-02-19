#!/bin/bash

rutaSched="$(pwd)"

echo "Introduzca el nombre del ejemplo que desea simular"
read example
if test -f $example ; then
    echo "Existe el archivo"
else
    echo "Error al leer la direccion del archivo ejemplo, puede que no exista, finalizando..."
    exit
fi

echo "Introduzca el número de CPUS que desea simular, debe de ser menor que 8"
read numCpus
if test $numCpus -gt 8 || test $numCpus -lt 1 ; then
    echo "Error el número de cpus a simular debe ser entre 1 y 8 inclusive, finalizando..."
    exit
fi

if test -d results ; then 
	echo "Directorio results encontrado borrando contenido "
	rm -r results
else
	echo "Directorio results no encontrado, se creara uno nuevo"
fi

#Creacion de las carpetas
mkdir results

listaScheduler="FCFS RR SJF PR"
numAux=0

for nameSched in $listaScheduler ; do
    printf " %s \n" $nameSched
    
    for cpuAux in $(seq $1 $numCpus); do
        
        if test "$nameSched" = "PR" || test "$nameSched" = "RR" ; then 
            ./schedsim -i $example -n $cpuAux -s $nameSched -p
            printf " ./schedsim -i %s -n %d -s %s -p\n" $example $cpuAux $nameSched
        else
            ./schedsim -i $example -n $cpuAux -s $nameSched
            printf " ./schedsim -i %s -n %d -s %s \n" $example $cpuAux $nameSched
        fi

        for i in $(seq $1 $cpuAux); do
            
            numAux=$((i-1))
            echo $numAux
            cd ..
            cd gantt-gplot
            
            printf " ./generate_gantt_chart %s/CPU_%d.log  \n" $rutaSched $numAux 
            
            ./generate_gantt_chart $rutaSched/CPU_$numAux.log       

            cd ..
            cd schedsim
            
            printf "mv CPU_$numAux.log results/%s-CPU-%d.log \n" $nameSched $numAux
            mv CPU_$numAux.log results/$nameSched-CPU-$numAux.log
            mv CPU_$numAux.eps results/$nameSched-CPU-$numAux.eps

        done
    done
done
echo "Resultados generados!!"
exit

