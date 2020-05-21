#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Funciones.h"


//funcion que permite cargar los StopWords de un documento
StopWords* loadStopWords(char* pathStopWordsFile, code *statusCode)
{
	char temp[20];
	int  i, largo, cont;
	StopWords *words = (StopWords*)malloc(sizeof(StopWords));
	FILE *f;
	f = fopen(pathStopWordsFile,"r");//lee el erchivo de text
	
	if(f == NULL)
	{
		*statusCode = ERR_FILE_NOT_FOUND;
		printf("no se ha podido abrir el fichero!\n");
		exit(1);
	}
	
	//recorre el archivo y cuenta cuantas lineas contiene el archivo
	cont = contador_string(f);
	
	words->palabras = (char**)malloc(cont*sizeof(char*));//se asigna memoria para la cantidad de palabras
	if(words->palabras == NULL)
	{
		*statusCode = NOT_ENOUGH_MEMORY;
		printf("no se ha podido asignar memoria!\n");
		exit(1);
	}
	
	rewind(f);

	for (i = 0; i < cont; i++)
	{
		vaciar(temp);
		fgets(temp,20,f);
		largo = strlen(temp)+1;
		cambio(temp);
		words->palabras[i] = (char*)malloc(largo*sizeof(char));	//se asigna memoria para cada palabra	
		strcpy(words->palabras[i],temp);
	}
	
	fclose(f);
	words->contsw = cont;
	printf("\nPalabras cargadas Exitosamente!\n");
	return words;//retorna la estructura ddel tipo StopWords
}
//crea indice para buscar las palabras. la representacion consta de una matriz dispersa donde cada fila corresponde
//a la palabra y la columna corresponde al documento.
Index* createIndex(char* pathDocumentsFile, StopWords *sw, code *statusCode)
{
	int i,  cont = 0,cont2 = 0, j = 0, longchar, n = 0, a = 0, k;	
	char temp[80],aux;
	Index *ind = (Index*)malloc(sizeof(Index));//asigna memoria para la estructura
	
	FILE *f;
	f = fopen(pathDocumentsFile,"r");//lee el erchivo de text
	
	if(f == NULL)
	{
		*statusCode = ERR_FILE_NOT_FOUND;
		printf("no se ha podido abrir el fichero!\n");
		exit(1);
	}
	
	while (!feof(f))//cuenta la cantidad de documento que contiene el archivo
	{
		fgets(temp,80,f);
		if(temp[0] == '.' && temp[1] =='I')
			cont++;
	}
	
	ind->indIni = (int*)malloc(cont*sizeof(int));//le asigna memoria para guardar la linea de inicio de cada documento
	ind->indFin = (int*)malloc(cont*sizeof(int));//le asigna memoria para guardar la linea de fin de cada documento
	rewind(f);
		
		
	ind->contInd = cont;	
	i = 0,cont = 0;
	
	while (!feof(f))
	{
		fgets(temp,80,f);

		if(temp[0] == '.' && temp[1] =='I' || feof(f))
		{
			if (cont == 0)
				ind->indIni[i] = cont;//guardar la linea de inicio del documento
			if(feof(f))	
				ind->indFin[i-1] = cont;//guardar la linea de fin del documento
			else 
			{
				ind->indIni[i] = cont;
				ind->indFin[i-1] = cont-1;
			}	
			i++;

		}
		cont++;
	}
	
	i = 0, cont = 0;
	rewind(f);
	
	while(!feof(f))//cuenta cuantos caracteres contiene el archivo
	{
		aux = fgetc(f);
		cont++; 
	}
	
	ind->documentos = (char*)malloc(cont*sizeof(char));//asigna memoria para almacenar todo los documentos y realizar una limpieza de este.

	rewind(f);
	
	while(!feof(f))
	{
		aux = fgetc(f);
		if(aux != '.' && aux != ',' && aux != '/' && aux != 'T' && aux != 'A' && aux != 'B' && aux != 'W')//realiza una limpieza no almacenando algunos caracteres.
		{
			if(aux == '-')//cambia el caracter "-" por un espacio
				ind->documentos[i] = ' ';
			else
				ind->documentos[i] = aux;
			i++;
		}
		else
			cont--;
	}
	
	ind->documentos = (char*)realloc(ind->documentos,cont*sizeof(char));// redimensiona el vector dimnamico al limpiar el archivo
	ind->contchar = cont;
	int largOri = 0;
	for(i = 0; i < ind->contchar; i++)
	{
		largOri = ind->contchar;
		if(ind->documentos[i] == ' ' || ind->documentos[i] == '\n' || i == 0)// revisa despues de cada espacio o salto de linea la palabra para compararla con las StopWords
		{
			for(j = 0; j < sw->contsw; j++)
			{
				if(i == 0 && ind->documentos[i] == sw->palabras[j][0])// compara la palabra con las StopWords
				{
					compara(ind, sw, i, j); //llama la funcion compara
				}
				else
				{	
					compara(ind, sw, i+1, j); //llama la funcion compara
				}
			}
		}
		if(largOri > ind->contchar)
			i--;
	}

	cont = 0;
	for(i=1;i<ind->contchar;i++)
	{
		if((ind->documentos[i]== ' ' || ind->documentos[i]== '\n' || ind->documentos[i]== '\0')//comprueba si hay dos o mas espacios o saltos de linea seguidos, los elimina deja solo 1.
		&& (ind->documentos[i-1]== ' ' || ind->documentos[i-1]== '\n' || ind->documentos[i-1]== '\0'))
		{
			for(j=i;j<ind->contchar;j++)
				ind->documentos[j-1]=ind->documentos[j];
			cont++;
			i--;
		}
	}
	
	ind->contchar = ind->contchar - cont;
	ind->documentos = (char*)realloc(ind->documentos,ind->contchar*sizeof(char));// redimenciona el vector dimnamico.


///////////////////////////////////////////////////////////////////////////////////////////

	int contpalabra = 0;
	
	for (i=0;i<ind->contchar;i++)
	{
		if(ind->documentos[i]== ' ' || ind->documentos[i]== '\n' || ind->documentos[i]== '\0')
		{
			contpalabra++;//cuenta cuantas palabras contiene el archivo para almacenar
		}
	}
	
	ind->palabras = (char**)malloc((contpalabra + 1)*sizeof(char*));//asigna memoria para almacenar la direccion de memoria donde va cada palabra
		
	cont = 0;
	i = 0;
	for(j = 0;j<ind->contchar;j++)
	{
		cont++;
		if(ind->documentos[j]== ' ' || ind->documentos[j]== '\n' || ind->documentos[j]== '\0')
		{
			ind->palabras[i] = (char*)malloc((cont + 1)*sizeof(char));//asigna memoria para almacenar cada caracter de la palabra
			cont = 0;
			i++;
		}
	}
	
	cont = 0; cont2 = 0; i = 0;

	for(j=0;j < ind->contchar - 1 ;j++)
	{
		cont++;
		if(ind->documentos[j]== ' ' || ind->documentos[j]== '\n' || ind->documentos[j]== '\0')
		{
			for(k=0;k<i;k++)
			{
				if(ind->documentos[j+1] == ind->palabras[k][0])//comprueba si cada palabra nueva que almacena ya esta almacenada para no ser repetida
				{
					n = igual(ind->documentos,  j+1, ind->palabras[k]);//para comprobar si la palabra es igual llama a la funcion Igual(). 
					if (n == 1)//si la palabra completa es igual retorna 1, si no es igual retorna 0.
					{
						int largo = strlen(ind->palabras[k]);
						j = j + largo;
						for(n=i+1;n<contpalabra - 1;n++)
						{
							ind->palabras[n] = ind->palabras[n + 1];//remplaza direccion de memoria
						}
						cont2++;
						n = 0;
						i--;
					}
				}
			}

			i++;
			cont = -1;
		}
		else
		{
			ind->palabras[i][cont] = ind->documentos[j];//separa las palabras y almacena cada uno de sus caracteres			
		}
	}

	int newcontpalabra = contpalabra - cont2;//se almacena la cantidad (en numero) de palabra sin palabras repetidas.
	
	ind->matriz = (int**)malloc((newcontpalabra)*sizeof(int*));//asigna memoria para la cantidad de palabras.

	for(i=0;i<newcontpalabra;i++)
	{
		ind->matriz[i] = (int*)calloc(ind->contInd,sizeof(int));//asigna memoria para almacenar la cantidad de veces que se repite la palabra en cada documento
	}	
		
	cont = 0;
	n = 0;
	
	for(i=1;i<ind->contchar - 1 ;i++)
	{
		if((ind->documentos[i-1]== ' ' || ind->documentos[i-1]== '\n' || ind->documentos[i-1]== '\0') && (ind->documentos[i] == 'I')
			&& (ind->documentos[i+1]== ' ' || ind->documentos[i+1]== '\n' || ind->documentos[i+1]== '\0'))
		{
			cont++;//me permite ver en que documento se encuentra la palabra
		}
		if(ind->documentos[i]== ' ' || ind->documentos[i]== '\n' || ind->documentos[i]== '\0')
		{
			for(j=0;j<newcontpalabra;j++)
			{
				if(ind->documentos[i+1] == ind->palabras[j][0])
					{
						//printf("%d",cont);
						n = igual(ind->documentos,  i+1, ind->palabras[j]);//para comprobar si la palabra es igual llama a la funcion Igual().
						if (n == 1)//si la palabra completa es igual retorna 1, si no es igual retorna 0.
						{
							ind->matriz[j][cont]++;//le suma 1 a la palabra[j] en el documento[cont] -> matriz[j][cont]
							n = 0;
						}
					}
			}
		}
	}
	
	ind->contpalabras = newcontpalabra;
	
	 /////////////////////////////////////////////////////////////////////////////////////////////////////

	fclose(f);
	printf("\nIndice Creado Exitosamente!\n");
	return ind;//retorna la estructura del tipo Index
}
//funcion igual que me permite comprobar si dos palabras son iguales, si es igual retorna 1, si no es igual retorna 0.
int igual(char texto[], int n, char *palabra)
{
	int i=0;
	while((texto[n+i]==palabra[i]) && (n+i<strlen(texto))&&(i!=strlen(palabra)))
	{
		i++;
		if(strlen(palabra) == i)
			return 1;
	}
	return 0;
}

//Funcion que compara las palabras del StopWords con el arreglo de documento
void compara(Index *ind, StopWords *Sw,int N_arreglo, int N_palabra)
{
	if(Sw->palabras[N_palabra][0] == ind->documentos[N_arreglo])//si la letra de una palabra es igual a la primera letra de la palabra del StopWords la compara
	{
		int tmp1=0;
  		int tmp2=N_arreglo;

        while ((Sw->palabras[N_palabra][tmp1]==ind->documentos[tmp2])&&(tmp2<strlen(ind->documentos))&&(tmp1!=strlen(Sw->palabras[N_palabra])))//si la letra del documento es igual a la letra de la palabra, 
        {																																		//y es menor al largo del documento y distinto al largo de la palabra
        	tmp1++;
            tmp2++;

            if ((tmp1==strlen(Sw->palabras[N_palabra]) && ind->documentos[tmp2] ==' ') || (tmp1==strlen(Sw->palabras[N_palabra]) && ind->documentos[tmp2] =='\n' ))//compara que las dos palabras tengan el mismo largo
            {
        		if(ind->documentos[N_arreglo - 1] ==' ' || ind->documentos[N_arreglo - 1] =='\n')
            		quitar(ind, tmp1+1, N_arreglo - 1);//llama la funcio que elimina la palabra del documento
				else
					quitar(ind, tmp1, N_arreglo);	//llama la funcio que elimina la palabra del documento
           	}
        }
	}
}

//elimina las palabras del documento si estan dentro del StopWords
void quitar(Index *vector, int N, int v)
{

	int i,j=0;
	if(vector->documentos[v] == '\n')
	{
		v++;
		N--;		
	}
				
	for(i = 0; i < N; i++)
	{
		for(j = v ; j < vector->contchar; j++)
		{
				vector->documentos[j]  = vector->documentos[j + 1];//elimina la palabra y el espacio antes de la palabra
		}
		vector->contchar--;
	}
	
	vector->documentos = (char*)realloc(vector->documentos,vector->contchar*sizeof(char));
	
}

//guarda el Indice en un Documento
void saveIndex(Index *i,int *id, code *statusCode)
{
	FILE* salida;
	char nombre[40];
	int j,k;
	
	printf("Guarde el archivo en formato nombre.txt (ejemplo.txt): ");//el archivo guardado tiene que contener la extencion .txt (ejemplo.txt)
	scanf("%s", &nombre);
	salida = fopen(nombre, "w");
	
	time_t tiempo;//gracias a la libreria time.h podemos generar el tiempo y la fecha
	struct tm *tm;
	
	fprintf(salida, "%d\n", *id); //guarda en el archivo text el ID
	
	tiempo = time(NULL);
	tm = localtime (&tiempo);
	
	char fecha[20];
	strftime(fecha,20,"%d/%m/%Y",tm);
	fprintf(salida, "Fecha: %s\n", fecha); //guarda en el archivo text la fecha 
	
	char hora[20];
	strftime(hora,20,"%H:%M:%S",tm);
	fprintf(salida,"Hora: %s\n",hora); //guarda en el archivo text la hora 
	
	fprintf(salida,"%d\n",i->contchar); //guarda en el archivo text la cantidad de caracteres
	fprintf(salida,"%d\n",i->contInd); //guarda en el archivo text la cantidad de documentos
	fprintf(salida,"%d\n",i->contpalabras); //guarda en el archivo text la cantidad de palabras
	
	for(j = 0; j < i->contInd; j++)
	{
		fprintf(salida,"%d\n",i->indIni[j]);//guarda en el archivo text la linea inicial de cada documento
		fprintf(salida,"%d\n",i->indFin[j]);//guarda en el archivo text la linea final de cada documento
	}
	
	for (j=0; j<i->contpalabras; j++)
	{
		for (k=0; k<i->contInd; k++)
		{
			fprintf(salida,"%d ",i->matriz[j][k]);//guarda en el archivo text la matriz que contiene el Indice
		}
	}
	
	for (j=0; j<i->contpalabras; j++)
	{
		fprintf(salida,"%s\n",i->palabras[j]);//guarda en el archivo text las palabras que contiene el Indice
	}
	
	fwrite(i->documentos,sizeof(char),i->contchar,salida);//guarda en el archivo text todo el documento
	
	printf("\nIndice Guardado Exitosamente!\n");
	fclose(salida);
}
//carga el Indice desde un Documento
Index* loadIndex(int id, code *statusCode)
{
	Index *ind = (Index*)malloc(sizeof(Index));//asigna memoria para la estructura del tipo Index
	FILE* entrada;
	char *nombre = (char*)malloc(sizeof(char)*40);
	printf("Cargue el archivo con extension nombre.txt (ejemplo.txt): ");//el archivo cargado tiene que contener la extencion .txt (ejemplo.txt)
	scanf("%s", nombre);
	entrada = fopen(nombre, "r");
	if(entrada == NULL)
	{
		*statusCode = ERR_FILE_NOT_FOUND;
		printf("no se ha podido abrir el fichero!\n");
		exit(1);
	}
	
	int identif, i, j;
	
	fscanf(entrada, "%d", &identif);//recibe el Id del documento
	if (id == identif)
	{
		printf("\n");
		printf("ID = %d\n",identif);
	
		char fecha_1[15];
		char fecha_2[15];
		
		fscanf(entrada, "%s", &fecha_1); //recibe el nombre "fecha"guardado en el archivo
		fscanf(entrada, "%s", &fecha_2);//recibe la fecha del documento
		
		printf("%s ",fecha_1);
		printf("%s\n",fecha_2);
		
		char hora_1[100];
		char hora_2[100];
		
		fscanf(entrada, "%s", &hora_1); //recibe el nombre "hora" guardado en el archivo
		fscanf(entrada, "%s", &hora_2);//recibe la hora del documento
		
		printf("%s ",hora_1);
		printf("%s\n",hora_2);
	
		fscanf(entrada, "%d", &ind->contchar);//recibe la cantidad de caracteres, guardado en el archivo
		fscanf(entrada, "%d", &ind->contInd);//recibe la cantidad de documento, guardado en el archivo
		fscanf(entrada, "%d", &ind->contpalabras);//recibe la cantidad de palabras, guardado en el archivo
		
		ind->indIni = (int*)malloc(ind->contInd*sizeof(int));//asigna memoria para guardar el numero inicial de la linea de cada Documento
		ind->indFin = (int*)malloc(ind->contInd*sizeof(int));//asigna memoria para guardar el numero final de la linea de cada Documento
		
		for(i = 0; i < ind->contInd; i++)
		{
			fscanf(entrada, "%d", &ind->indIni[i]);//recibe el numero inicial de la linea de cada Documento
			fscanf(entrada, "%d", &ind->indFin[i]);//recibe el numero final de la linea de cada Documento
	
		}
		
		ind->palabras = (char**)malloc(ind->contpalabras*sizeof(char*));//asigna memoria para almacenar las palabras
		ind->matriz = (int**)malloc(ind->contpalabras*sizeof(int*));//asigna memoria por cada palabra para almacenar el Indice
		
		for (j=0; j<ind->contpalabras; j++)
			ind->matriz[j] = (int*)malloc(ind->contInd*sizeof(int));//asigna para almacenar el Indice por palabra en cada documento 
		
		for (j=0; j<ind->contpalabras; j++)
			for (i=0; i<ind->contInd; i++)
				fscanf(entrada,"%d",&ind->matriz[j][i]);//recibe la matriz completa de Indice
		
		char pal[20];
		for (j=0; j<ind->contpalabras; j++)
		{
			if(j!=0)
			{
				fscanf(entrada,"%s",&pal);
				ind->palabras[j] = (char*)malloc((strlen(pal)+1)*sizeof(char));//asigna memoria para cada caracter de la palabra
				for(i=0;i<strlen(pal);i++)
				{
					ind->palabras[j][i] = pal[i];//almacena cada caracter de la palabra				
				}
			}
			else
			{
				ind->palabras[j] = (char*)malloc(sizeof(char));
				ind->palabras[j][0] = '\n';//se alamaceno una palabra vacia que por obligacion tambien la tengo que almacenar :P
			}
		}

		ind->documentos = (char*)malloc(ind->contchar*sizeof(char));
		
		fread(ind->documentos,sizeof(char),ind->contchar,entrada);//recibe el documento completo con cada Indice guardado en el archivo
		printf("\nIndice Cargado Exitosamente!\n");
	}
	else
	{
		printf("El ID ingresado no corresponde al que desea cargar!\n");
	}
	
	
	fclose(entrada);
	return ind;//retorna la estructura Index
	
}
//funcion que ingresa una palabra, busca referencias en el indice y ordena por mayor cantidad de palabras encontradas en los docs 
Ranking* query(Index *ind, StopWords *sw, char* text, code *statusCode)
{
	Ranking *Rk = (Ranking*)malloc(sizeof(Ranking));
	Rk->result = (int*)calloc(ind->contInd,sizeof(int));//se asigna memoria para almecenar la cantidad de veces que se repite una palabra en cada Indice
	Rk->doc = ind->contInd;
	int i,j,l,cont = 0,cont2,n = 0;
	
	for(i = 0; i < strlen(text)+1; i++)
	{
		if(text[i]== ' ' || text[i] == '\0' || text[i] == '\n')//separa cada palabra por un espacio o salto de linea
			cont++;
	}
	Rk->N_pal = cont;
	char **palabra = (char**)malloc(cont*sizeof(char*));//asigna memoria para la cantidad de palabras
	cont = 0, cont2 = 0;
	
	for(i = 0; i < strlen(text)+1; i++)
	{
		if(text[i]== ' ' || text[i] == '\0' || text[i] == '\n')
		{
			palabra[cont] = (char*)malloc((cont2 + 1)*sizeof(char));//asigna memoria para el largo de cada palabra
			cont++;
			cont2 = 0;	
		}
		else
			cont2++;
	}
	
	cont = 0, cont2 = 0;
	for(i = 0; i < strlen(text)+1; i++)
	{
		
		if(text[i]== ' ' || text[i] == '\0' || text[i] == '\n')
		{
			cont++;
			cont2 = 0;	
		}
		else
		{
			palabra[cont][cont2] = text[i];//almacena cada palabra en el vector dinamico
			cont2++;			
		}
	}
	
	//de las palabras que ingresa el usuario eliminas las palabras que pertenezcan al StopWords
	for(i = 0; i < Rk->N_pal; i++)
	{
		for(j = 0; j < sw->contsw; j++)
		{
			if(sw->palabras[j][0] == palabra[i][0])//si la letra de una palabra es igual a la primera letra de la palabra del StopWords la compara
			{
				int tmp1=0;
		  		int tmp2=0;
		
		        while ((sw->palabras[j][tmp1]==palabra[i][tmp2])&&(tmp2<strlen(palabra[i]))&&(tmp1!=strlen(sw->palabras[j])))//si la letra del documento es igual a la letra de la palabra, 
		        {																																	//y es menor al largo del documento y distinto al largo de la palabra
		        	tmp1++;
		            tmp2++;
		
		            if (tmp1==strlen(palabra[i]))//compara que las dos palabras tengan el mismo largo
		            {
		        		for(l = i; l < Rk->N_pal -1; l++)
		        		{
		        			palabra[l] = palabra[l+1];
						}
						
						Rk->N_pal--;	
		           	}
		        }
			}
		}
	}

	palabra = (char**)realloc(palabra,Rk->N_pal*sizeof(char*)); //redimenciona la cantidad de palabras para la busqueda
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	for(i=0;i<Rk->N_pal;i++)
	{
		for(j = 0;j<ind->contpalabras;j++)
		{
			if(palabra[i][0] == ind->palabras[j][0])
			{
				n = igual(palabra[i],  0, ind->palabras[j]);//para comprobar si la palabra es igual llama a la funcion Igual().
				if (n == 1)//si la palabra completa es igual retorna 1, si no es igual retorna 0.
				{
					for(n=0;n<ind->contInd;n++)
					{
						Rk->result[n] = ind->matriz[j][n] + Rk->result[n];//almacena la cantidad de veces que se repite la palabra en cada documento, la Informacion la saca desde el Indice (matriz dispersa).
					}
					n = 0;
				}
			}
		}
	}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////		

	for (i = 0; i < ind->contInd; i++)
	{
		if (Rk->result[i] != 0)
		{
			printf("\nPalabra(s) encontrada(s)!\n");
			i = ind->contInd;
		}	
	}
	 OrdenarRk(Rk);//llama a la funcion que ordena el indice con mayor palabras encontradas al menor
	
	return Rk;//retorna el Ranking de resultados
}
//muestra una lista en orden de mayor a menor segun la palabra ingresada
void displayResults(Ranking *r, int TopK, code *statusCode)
{
	int i,j;
	for (i=0;i<10 && i < r->doc;i++)
		printf("Ranking N-%d el Doc %d con la catidad de %d palabras encontradas\n",i+1,r->top_k[i] +1, r->result[r->top_k[i]]);
	
}
//muestra los doc en orden de mayor a menor segun la palabra ingresada
void displayDoc(Ranking *r, Index *ind, code *statusCode, char* pathDocumentsFile)
{
	int i = 0,  cont = 0, j = 0;	
	char temp[81],aux;

	FILE *f;
	f = fopen(pathDocumentsFile,"r");//lee el erchivo de text
	
	if(f == NULL)
	{
		*statusCode = ERR_FILE_NOT_FOUND;
		printf("no se ha podido abrir el fichero!\n");
		exit(1);
	}
		
	while (!feof(f))
	{
		fgets(temp,81,f);

		if(temp[0] == '.' && temp[1] =='I' || feof(f))
		{
			if (cont == 0)
				ind->indIni[i] = cont;//guardar la linea de inicio del documento
			if(feof(f))	
				ind->indFin[i-1] = cont;//guardar la linea de fin del documento
			else 
			{
				ind->indIni[i] = cont;
				ind->indFin[i-1] = cont-1;
			}	
			i++;

		}
		cont++;
	}
	
	i = 0, cont = 0;
	rewind(f);
	
	while(!feof(f))//cuenta cuantos caracteres contiene el archivo
	{
		aux = fgetc(f);
		cont++; 
	}
	
	ind->contchar = cont;
	ind->documentos = (char*)malloc(cont*sizeof(char));//adigna memoria para el vector dinamico que contiene todo los documentos

	rewind(f);
	
	while(!feof(f))//cuenta cuantos caracteres contiene el archivo
	{
		aux = fgetc(f);
		ind->documentos[i] = aux;
		i++; 
	}
	
	////////////////////////////////////////////////////
		
	cont = 0;
	for(j = 0; j < 10 && j < r->doc; j++)
	{
		for(i = 0; i < ind->contchar ;i++)
		{
			if (cont >= ind->indIni[r->top_k[j]] && cont <= ind->indFin[r->top_k[j]])
			{
				printf("%c",ind->documentos[i]);
			}	
			if(ind->documentos[i]=='\n')
				cont++;
		}
		printf("\n");
		cont = 0;
	}
	/////////////////////////////////////////////////////////
	
	fclose(f);
	
	
}
//vacia un arreglo
void vaciar(char temp[])
{
	int i;
	for(i = 0; i < 20; i++)
		temp[i]='\0';
}
//cambia un \n por un \0
void cambio(char temp[])
{
	int i;
	
	for (i = 0; i < 20; i++)
	{
		if(temp[i] == '\n')
			temp[i] = '\0';
	}
}
//cuenta la cantidad de lineas en el documento text
int contador_string(FILE *f)
{
	int cont = 0;
	char temp[80];
	while (!feof(f))
	{
		fgets(temp,80,f);
		cont++;
	}
	return cont;
}
//generamos el ID con cualquier numero entre 1000 y 2000 atraves de random
int adquirirID() 
{
	int id;
	id = rand()%1000 + 1000;
	return id;
}
//ordena Ranking segun la cantidad de palabras que se repite en cada doc
void OrdenarRk(Ranking *Rk)
{
	
	int i, j, aux[10],aux2 = -1;
	for(i = 0; i < 10; i++)
	{
		aux[i] = 0;
		Rk->top_k[i] = 0;
	}
	
	for(i=0;i<10 && i < Rk->doc;i++)
	{
		for(j = 0; j < Rk->doc; j++ )
		{
			if(Rk->result[j] >= aux2)
			{
				aux2 = Rk->result[j];
				Rk->top_k[i] = j; //entrega el numero del documento(Indice) de mayor a menor segun la cantidad de palabras

			} 
		}
		aux2 = -1;
		aux[i] = Rk->result[Rk->top_k[i]];  
		Rk->result[Rk->top_k[i]] = -1;//cambia el valor de result para que no lo vuelva a considerar 
	}
	
	for(i=0;i<10 && i < Rk->doc;i++)
	{
		Rk->result[Rk->top_k[i]] = aux[i];//retorna todos los valores de result
	}

}
