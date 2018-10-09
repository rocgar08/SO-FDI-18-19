/**
*
*Fer Roci
* head -c 1024 /dev/urandom
*
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"
#include <sys/types.h> //Para el stat
#include <sys/stat.h>

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	// Complete the function
	int ok=1;
	int car;
	unsigned int total=0;
	while((car=getc(origin))!=EOF && (total<nBytes) && ok){
		ok=( EOF!=putc((unsigned char)car,destination) );
		total++;
	}

	return ok ? total : -1;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	// Complete the function
	int n,size=0;
	do{
		n = getc(file);
		size++;
	}while((n != (int)'\0') && (n != EOF));

	if(n==EOF)
		return NULL;

	char *buf = (char*)malloc(size);

	//Desplaza el puntero del fichero
	fseek(file, -size, SEEK_CUR);
	fread(buf,1,size,file); //¿Te cambia la posicion del descriptor de fichero?
	return buf;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an header that stores
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	// Complete the function
	int i,ok=1;
	fread(nFiles,sizeof(int),1,tarFile);
	stHeaderEntry *header =(stHeaderEntry*) malloc(sizeof(stHeaderEntry)*(*nFiles));

	for(i = 0; i < *nFiles && ok;i++){
		ok=( (header[i].name=loadstr(tarFile)) !=NULL);
		if(ok)fread(&header[i].size,sizeof(unsigned int),1,tarFile);
	}

	return ok ? header : NULL;
}


/*Funciones parte extra*/

int
searchFile(char *fileName, stHeaderEntry *header, int nFiles, int *pos){
	int ok=0,i=0;
	
	while(i < nFiles && !ok)
		ok=( strcmp(header[i++].name,fileName)== 0 );
	
	*pos=--i;

	return ok;
}


void
copyHeader(stHeaderEntry *header, FILE * destination, int nFiles, int pos){
	int nFilesToWrite=nFiles-1;

	fwrite(&nFilesToWrite,sizeof(int),1,destination);
	for(int i=0;i < nFiles;i++){
		if(i!=pos){
			fwrite(header[i].name,1,strlen(header[i].name) + 1,destination);//+1 por que strlen devuelve el tamaño de la cadena sin contar /0.
			fwrite(&(header[i].size),sizeof(header[i].size),1,destination);
		}
	}
}



int
copyDataForDeleteProcess(FILE * origin, FILE * destination, stHeaderEntry *header ,int nFiles, int pos){
	int ok=1;

	for (int i = 0; i < nFiles && ok; ++i){
		 if(i!=pos){
			 ok=( copynFile(origin,destination,header[i].size) !=-1);
			 fseek(origin, -1, SEEK_CUR);
		 }
		 else
			 fseek(origin, (int)(header[i].size), SEEK_CUR);
	}

	return ok;
}


int
copyFile(char *origin, char *destination){
	int ok=0,car;
    struct stat st;						/** Structure with file's info */
    unsigned int fileSize;
    FILE *originfd, *destinationfd;

    if(origin != NULL || destination != NULL){
		ok=1;
    	originfd = fopen(origin,"r");
		destinationfd = fopen(destination,"w"); //Me trunca a 0 el valor del archivo.
		stat(origin, &st);
		fileSize = st.st_size;

		for(int i=0; i < fileSize && ok;i++){
			ok=( (car=getc(originfd)) !=EOF);
			ok=( ok && EOF!=putc((unsigned char)car,destinationfd) );
		}

		fclose(originfd);
		fclose(destinationfd);
    }

   return ok;
}



/*------------------------------------------Fin funciones parte extra-------------------------------------------------------------------*/




/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: header with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *tarFile, *enterFile;
	stHeaderEntry *header;
	unsigned int headerSize;
	if(nFiles<=0){
		fprintf(stderr, "%s\n", use);
		return EXIT_FAILURE;
	}

	tarFile=fopen(tarName,"wx");
	headerSize = sizeof(int);
	header = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * nFiles);

	for (int i = 0; i < nFiles; ++i){
		int nameSize = strlen(fileNames[i]);//Longitud neta sin el /0
		nameSize++;//Le añado el /0
		header[i].name = (char*) malloc(nameSize);
		strcpy(header[i].name, fileNames[i]);
		headerSize += nameSize + sizeof(header->size);
	}

	//SEEK_SET desde el principio.
	fseek(tarFile,headerSize, SEEK_SET);

	for (int i = 0; i < nFiles; ++i){
		enterFile = fopen(fileNames[i], "r");
		header[i].size = copynFile(enterFile, tarFile, INT_MAX);
		fclose(enterFile);
	}

	rewind(tarFile);//Va al principio.
	fwrite(&nFiles,sizeof(int),1,tarFile);//Espera leer un header, por eso la dirección.
	for (int i = 0; i < nFiles; ++i){
		fwrite(header[i].name,1,strlen(header[i].name) + 1,tarFile);//+1 por que strlen devuelve el tamaño de la cadena sin contar /0.
		fwrite(&(header[i].size),sizeof(header[i].size),1,tarFile);
	}

	fprintf(stdout, "mtar file created successfully\n");
	for (int i = 0; i < nFiles; ++i){
		free(header[i].name);
	}
	free(header);
	fclose(tarFile);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	// Complete the function
	FILE *tarFile, *compressedFiles;
	stHeaderEntry *header;
	int nFiles,ok=1;

	tarFile = fopen(tarName,"r");

	if(tarFile ==NULL || ( (header = readHeader(tarFile,&nFiles))==NULL) ){
		fprintf(stderr, "%s\n", use);
		return EXIT_FAILURE;
	}

	for(int i = 0; i < nFiles && ok; i++){
		compressedFiles = fopen(header[i].name,"w");
		copynFile(tarFile,compressedFiles,header[i].size);
		fclose(compressedFiles);
		fseek(tarFile, -1, SEEK_CUR);
	}

	fprintf(stdout, "Successfull extraction from mtar file  \n");
	free(header);
	fclose(tarFile);

	return EXIT_SUCCESS;
}

int
deleteOneFile(char tarName[],char *fileName){
	
	FILE *tarFile, *copyAux;
	stHeaderEntry *header;
	int nFiles,pos,ok=1;

	tarFile = fopen(tarName,"r+");

	if(tarFile ==NULL || ( (header = readHeader(tarFile,&nFiles))==NULL) ){
		fprintf(stderr, "%s\n", use);
		return EXIT_FAILURE;
	}

	if(searchFile(fileName,header,nFiles,&pos)){
		copyAux = fopen(NAME_ARCHIVE_AUX,"w+");

		copyHeader(header, copyAux, nFiles, pos);

		ok=copyDataForDeleteProcess(tarFile, copyAux, header, nFiles,pos);

		fclose(tarFile);
		fclose(copyAux);

		if(ok)
			ok=copyFile(NAME_ARCHIVE_AUX,tarName);

		free(header);

		if(unlink(NAME_ARCHIVE_AUX)==-1 && ok){
			fprintf(stdout, "Deliting process fails after in the latest steps\n");
			return EXIT_FAILURE;
		}

		fprintf(stdout, "Deleting file process done!!!. \n");
	}
	else
		fprintf(stdout, "The filename doesn't exist in the mtar file. \n");
	
	return EXIT_SUCCESS;		
}
int appendFiles(char tarName[], int nFiles, char *fileName,char copyTar[]){
	/**
	 * Necesitamos tarFile actual y la copia
	 * La cabecera del tarFIle actual y una cabecera auxiliar para copiar los datos de la actual
	 * Necesitamos el nAfiles numero de ficheros almacenados,
	 * y el nTFiles es el numero de ficheros nuevos que van a ser al macenado en el tar
	**/
	   FILE *enter , *tarFile,*copy;
	   stHeaderEntry *header= NULL;
	   stHeaderEntry *newHeader= NULL;

	   int nRFiles =0,newNFiles = 0,pos,nBytes = 0;


	   if((tarFile = fopen(tarName, "r")) == NULL)
	      return (EXIT_FAILURE);

	   if((copy = fopen(copyTar, "a+")) == NULL)
	      return (EXIT_FAILURE);

	   fread(&nRFiles, sizeof(int), 1, tarFile);
	   rewind(tarFile);

	   header =readHeader(tarFile,&nRFiles);

	   newNFiles = nFiles + nRFiles;

	   newHeader = (stHeaderEntry*) malloc(sizeof(stHeaderEntry) * (newNFiles));

	   for(int i = 0; i < nRFiles; i++) {
		  copyHeader(header,copy,nRFiles,pos);
	   }

	   for(int i = 0; i < nFiles; i++) {
		   copyHeader(header,copy,nFiles,pos);
	   }




	   for(int i = 0; i < nRFiles; i++) {
	      nBytes += searchFile(fileName,header,nRFiles,&pos);
	   }

	   for(int i = 0; i < nFiles; i++) {
		   nBytes += searchFile(fileName,header,nFiles,&pos);
	   }


	   nBytes += (nFiles + nRFiles + 1)+ newNFiles;


	   fseek(copy, nBytes, SEEK_SET);


	   for(int i = 0; i < nRFiles; i++) {
	      copynFile(tarFile, copy, newHeader[i].size);
	   }

	   for(int i = 0; i < nFiles; i++) {
	      enter = fopen(fileName[i], "r");
	      nBytes = copynFile(enter, copy, INT_MAX);
	      newHeader[i+nRFiles].size = nBytes;
	      fclose(enter);
	   }


	   rewind(copy);


	   fwrite(&newNFiles, sizeof(int), 1, copy);


	   for(int i = 0; i < newNFiles;i++) {
	      fwrite(newHeader[i].name,strlen(newHeader[i].name), 1, copy);
	      fwrite(&newHeader[i].size, sizeof(unsigned int), 1, copy);
	      free(newHeader[i].name);
	   }

	   for(int i = 0; i < nRFiles; i++) {
	      free(header[i].name);
	   }

	   printf("Fichero mitar creado con exito\n");

	   free(header);
	   free(newHeader);
	   fclose(copy);
	   fclose(tarFile);

	   return (EXIT_SUCCESS);

}
