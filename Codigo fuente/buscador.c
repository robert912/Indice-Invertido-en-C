#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"


//main Menu Principal
int main(int argc, char *argv[])
{
	code *statusCode;
	statusCode = (code*)malloc(sizeof(code));
	srand(time(NULL));
	int id, opcion, idload;
	id  = adquirirID();
	int a;
	Index *i;
	Ranking *r;
	StopWords *varStopWords;

	
	char *pathStopWordsFile;
	char *pathDocumentsFile;
	pathDocumentsFile = "TestCollection.txt";
	char palabra[30];// = "wing skin wing";
	
	do
	{
		printf("\n\tBuscador de palabras\n\n");
		printf("1) Cargar StopWords\n");
		printf("2) Crear Indice\n");
		printf("3) guardar Indice\n");
		printf("4) Cargar Indice\n");
		printf("5) Hacer Consulta de Palabra\n");
		printf("6) Mostrar Resultados por Ranking\n");
		printf("7) Mostrar Documentos por Ranking\n");
		printf("8) Salir\n\n");
		
		printf("Ingrese Opcion: ");
		scanf("%d",&opcion);
		fflush(stdin);
		printf("\n");
		
		switch(opcion)
		{
			case 1:	pathStopWordsFile = "StopWords.txt";
					varStopWords = loadStopWords(pathStopWordsFile, statusCode);//llama la funcion LoadStopWords
					break;
			case 2:	
					i = createIndex(pathDocumentsFile, varStopWords, statusCode);//llama la funcion CreateIndex
					break;
			case 3:	saveIndex(i,&id, statusCode);//llama la funcion saveIndex
					break;
			case 4:	printf("Ingrese el ID que desea buscar: ");
					scanf("%d",&idload);
					fflush(stdin);
					i = loadIndex(idload, statusCode);//llama la funcion loadIndex
					break;
			case 5: printf("Ingrese la palabra que desea buscar: ");
					gets(palabra);
					fflush(stdin);
					r = query(i, varStopWords, palabra, statusCode);//llama la funcion query
					break;
			case 6:	displayResults(r, a, statusCode);//llama la funcion displayResults
					break;
			case 7:	displayDoc(r, i,statusCode,pathDocumentsFile);//llama la funcion displayDcc
					break;
			default: return 0;
				break;
		}
	}while(opcion != 8);
	
	return 0;
}
