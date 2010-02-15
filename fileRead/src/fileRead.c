/*
 ============================================================================
 Name        : fileRead.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readFileAsString(char* filepath)
{
	int fileSize = 0;
	FILE *pFile = fopen(filepath, "rb");
	if (pFile == NULL)
	{
		puts("error opening file");
		//exit(1);
	}
	fseek(pFile,0,SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);

	char *data = (char*) calloc (sizeof(char), fileSize + 20);
	fread(data, 1, fileSize, pFile);
	puts(data);
	fclose(pFile);
	puts("Done");
	return data;
}

int countCommas(char* str)
{
	int count = 0;
	int pos = 0;
	while (str[pos] != '\0')
	{
		if (str[pos] == ',')
		{
			count++;
		}
		pos++;
	}
	return count;
}

void splitStringIntoArray(char* str, int* arr, int count)
{
	char* tok;
	int i = 0;

	tok = strtok(str,",\0");
	while (tok != NULL)
	{
		printf("%s\n", tok);
		if (i < count)
		{
			arr[i] = atoi(tok);
		}
		else
		{
			puts("Index out of bounds");
		}
		tok = strtok(NULL, ",\0");
		i++;
	}
}

void crapSort(int* arr, int count)
{
	for (int i = 0; i < count; i++)
	{
		for (int j = i; j < count; j++)
		{
			if (arr[j] > arr[i])
			{
				int t = arr[i];
				arr[i] = arr[j];
				arr[j] = t;
			}
		}
	}
}

void printIntArray(int* arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("%d\n", arr[i]);
	}
}

int* sortArray;

int main(void) {
	//puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	int count = 0;
	char* data = readFileAsString("data.in");
	count = countCommas(data) + 1;
	puts("splitting");
	sortArray =  (int*) calloc (sizeof(int), count);
	splitStringIntoArray(data, sortArray, count);
	puts("sorting..............");
	crapSort(sortArray, count);
	printIntArray(sortArray, count);
	printf("\n number of items: %d \n", count);
	return EXIT_SUCCESS;
}


