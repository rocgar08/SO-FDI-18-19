#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>
#define NAME_ARCHIVE_AUX "contacts_aux.bin"

typedef enum{
  NONE,
  ERROR,
  CREATE,
  EXTRACT,
  DELETE,
  APPEND
} flags;

typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);
int deleteOneFile(char tarName[],char *fileName);
int appendFiles(char tarName[], int nFiles, char *filName,char copyTar[]);

#endif /* _MYTAR_H */
