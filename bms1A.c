/*
 * Projekt do predmetu BMS - Bezdratove a mobilni site
 * Subor: bms1A.c
 * Autor: Frantisek Matecny, xmatec00
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ecc.h"

//velkost kodovaneho bloku
#define NDATA 139


/**
 * Hlavne telo programu
 **/
int main (int argc, char *argv[])
{
	
	FILE* inputFile;
	FILE* outputFile;
	
	//kontrola argumentov
	if (argc != 2)
	{
		fprintf(stderr, "Chybne argumenty\n");
		exit(-1);	
	}
	
	//otvorenie vstupneho suboru
	if ((inputFile = fopen (argv[1],"r")) == NULL)
	{
		fprintf(stderr,"Vstupny subor sa nepodarilo otvorit\n");
		exit(-1);
	}
	
	//vytvorenie mena vystupneho suboru
	char outputFileName[strlen(argv[1])];
	strcpy(outputFileName, argv[1]);
	strcat(outputFileName, ".out");
	
	//otvorenie vystupneho suboru
	if ((outputFile = fopen (outputFileName,"wb")) == NULL)
	{
		fprintf(stderr,"Vystupny subor sa nepodarilo otvorit\n");
		fclose(inputFile);
		exit(-1);
	}
	
	
	unsigned long file_size;
	size_t result;	
	unsigned char input_data[NDATA+1];
	unsigned char output_data[NDATA+NPAR];
	
	//velkost suboru
	fseek (inputFile , 0 , SEEK_END);
	file_size = ftell (inputFile);
	rewind (inputFile);
	
	//sirka matice
	int matrix_x = (file_size)/NDATA+1;
	
	//matica zakodovanych dat	
	unsigned char** matrix;
	
	matrix = (unsigned char**) malloc(sizeof(unsigned char*) * (NDATA+NPAR));
	if (matrix == NULL){
		fprintf(stderr, "Chyba alokacie\n");
		exit (1);
	}

	for (int i = 0; i < NDATA+NPAR; i++){
		matrix[i] = (unsigned char*) malloc(sizeof(unsigned char) * matrix_x);
		if (matrix[i] == NULL){
			fprintf(stderr, "Chyba alokacie\n");
			exit (1);}	
	}
	
	
	//inicializacia ECC kniznice
	initialize_ecc ();


	//naplnenie matice zakodovanymi datami
	for (int col = 0; col < matrix_x; col++)
	{
		
		//vymazanie blokov dat
		memset (input_data, 0, NDATA+1);		
		memset (output_data, 0, NDATA+NPAR);

		//kopirovanie bloku dat zo subora
		result = fread (input_data,1,NDATA,inputFile);
		if (result == 0) {break;}

		//printf("Loading data is: \"%s\"\n", input_data);

		//zakodovanie dat
		encode_data (input_data, result, output_data);
		
		//printf("Encoded data is: \"%s\"\n", output_data);
		
		//ulozenie dat do stlpca matice
		for (int row = 0; row < result+NPAR; row++)
		{
			matrix[row][col] = output_data[row];
		}
	}
	
	//ulozenie zakodovanych dat do subora z matice po riadkoch
	for (int row = 0; row < NDATA+NPAR; row++)
	{
		if(row >= result+NPAR)
		{
			fwrite (matrix[row] , sizeof(char), matrix_x-1, outputFile);		
		}
		else{
			fwrite (matrix[row] , sizeof(char), matrix_x, outputFile);
			}
	}

	//uvolnenie pamati
	free(matrix);
	//zatvorenie suborov
	fclose(inputFile);
	fclose(outputFile);
	
	exit(0);
}
