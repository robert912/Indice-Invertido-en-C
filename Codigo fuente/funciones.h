//Cabecera de Funciones
#ifndef FUNCIONES_H
#define FUNCIONES_H

typedef enum  {OK, ERR_FILE_NOT_FOUND, NOT_ENOUGH_MEMORY} code;

//Struct para almacenar las palabras del StopWords 
typedef struct 
{
	int contsw;
	char **palabras;
} StopWords;

//struct para almacenar Indice Invertido.
typedef struct {
	long contchar;
	int contInd;
	int contpalabras;
	int *indIni;
	int *indFin;
	char *documentos;
	int **matriz;
	char **palabras;
} Index;

//Struct para el ranking de palabras en el doc.
typedef struct {
	int N_pal;
	int top_k[10];
	int doc;
	int *result;
} Ranking;

StopWords* loadStopWords(char* pathStopWordsFile, code *statusCode);
Index* createIndex(char* pathDocumentsFile, StopWords *sw, code *statusCode);
void saveIndex(Index *i, int *id, code *statusCode);
Index* loadIndex(int id, code *statusCode);
Ranking* query(Index *i, StopWords *sw, char* text, code *statusCode);
void displayResults(Ranking *r, int TopK, code *statusCode);
void displayDoc(Ranking *r, Index *i, code *statusCode, char* pathDocumentsFile);

void vaciar(char temp[]);
void cambio(char temp[]);
int contador_string(FILE *f);
void compara(Index *ind, StopWords *Sw,int N_arreglo, int N_palabra);
void quitar(Index *vector, int N, int v);
int adquirirID();
void OrdenarRk(Ranking *Rk);
int igual(char texto[], int n, char *palabra);

#endif
