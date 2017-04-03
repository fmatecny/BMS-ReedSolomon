/*
 * Projekt do predmetu BMS - Bezdratove a mobilni site
 * Subor: bms1B.c
 * Autor: Frantisek Matecny, xmatec00
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ecc.h"

//velkost zakodovaneho bloku
#define NDATA 243


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
	if ((inputFile = fopen (argv[1],"rb")) == NULL)
	{
		fprintf(stderr,"Vstupny subor sa nepodarilo otvorit\n");
		exit(-1);
	}
	
	//vytvorenie mena vystupneho suboru
	char outputFileName[strlen(argv[1])];
	strcpy(outputFileName, argv[1]);
	strcat(outputFileName, ".ok");
	
	//otvorenie vystupneho suboru
	if ((outputFile = fopen (outputFileName,"w")) == NULL)
	{
		fprintf(stderr,"Vystupny subor sa nepodarilo otvorit\n");
		fclose(inputFile);
		exit(-1);
	}
	
	long file_size;
	size_t result;
	//int erasures[16];
	//int nerasures = 0;
	
	//velkost suboru
	fseek (inputFile , 0 , SEEK_END);
	file_size = ftell (inputFile);
	rewind (inputFile);
	
	//sirka matice
	int matrix_x = (file_size)/NDATA+1;
	
	//buffer na blok vstupnych dat
	unsigned char input_data[matrix_x];
	
	//matica zakodovanych dat	
	unsigned char** matrix;
	
	matrix = (unsigned char**) malloc(sizeof(unsigned char*) * matrix_x);
	if (matrix == NULL){
		fprintf(stderr, "Chyba alokacie\n");
		exit (1);
		}

	for (int i = 0; i < matrix_x; i++){
		matrix[i] = (unsigned char*) malloc(sizeof(unsigned char) * NDATA);
		if (matrix[i] == NULL){
			fprintf(stderr, "Chyba alokacie\n");
			exit (1);}
	}
	
	//inicializacia ECC kniznice
	initialize_ecc ();


	//naplnenie matice zakodovanymi datami
	for (int col = 0; col < NDATA; col++)
	{		
		//vymazanie bloku dat
		memset (input_data, 0, matrix_x);

		//kopirovanie bloku dat zo subora
		if (col >= file_size%NDATA)
		{
			result = fread (input_data, 1, matrix_x-1, inputFile);
		}
		else{
			result = fread (input_data, 1, matrix_x, inputFile);
			}	
					
		if (result == 0) {break;}

		//printf("Loading data is: \"%s\"\n", input_data);


		//ulozenie dat do stlpca matice
		for (int row = 0; row < result; row++)
		{
			matrix[row][col] = input_data[row];
		}
	}

/* ulozenie dekodovanych dat do subora z matice po riadkoch	*/			
	for (int row = 0; row < matrix_x-1; row++)
	{
		//printf("Matrix data is: \"%s\"\n", matrix[row]);

		//dekodovanie dat
		decode_data(matrix[row], NDATA);

		//kontrola chyb
		if (check_syndrome () != 0)
		{
			correct_errors_erasures (matrix[row], NDATA, 0, NULL);

			//printf("Corrected codeword: \"%s\"\n", matrix[row]);
		}
		//ulozenie riadku matice do subora
		fwrite (matrix[row] , sizeof(char), NDATA-NPAR, outputFile);
	}



		
	//printf("Matrix data is: \"%s\"\n", matrix[matrix_x-1]);
	
	//dekodovanie dat
	decode_data(matrix[matrix_x-1], file_size%NDATA);

	//kontrola chyb
	if (check_syndrome () != 0)
	{
		//opravenie chyb
		correct_errors_erasures (matrix[matrix_x-1], file_size%NDATA, 0, NULL);

		//printf("Corrected codeword: \"%s\"\n", matrix[matrix_x-1]);
	}
	//ulozenie posledneho riadku matice do subora
	fwrite (matrix[matrix_x-1] , sizeof(char), file_size%NDATA-NPAR, outputFile);

  
	//uvolnenie pamati
	free(matrix);
	//zatvorenie suborov
  	fclose(inputFile);
	fclose(outputFile);
	
	exit(0);
  
}