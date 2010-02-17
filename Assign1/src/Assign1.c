/*
 ============================================================================
 Name        : Assign1.c
 Author      : Edward Thiele
 Version     :
 Copyright   : Your copyright notice
 Description : CS416 Assign1
 ============================================================================
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void *sorting_thread(void *threadNumber);

void writeTimestamp()
{

	long time_now = clock();
	printf("%ld ", time_now);
}

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
	//puts(data);
	fclose(pFile);
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
		//printf("%s\n", tok);
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
	int i, j;
	for (i = 0; i < count; i++)
	{
		for (j = i; j < count; j++)
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
	int i;
	for (i = 0; i < size; i++)
	{
		printf("%d\n", arr[i]);
	}
}

void writeCDF(int*arr, int size, char* path)
{
	FILE *file;
	file = fopen(path, "w");
	int i;
	for (i=0; i<size-1; i++)
	{
		fprintf(file, "%d,", arr[i]);
	}
	fprintf(file, "%d", arr[size-1]);
	fclose(file);
}

typedef struct chuck_data_t
{
	int chuckNumber;
	struct chuck_data_t* next;
} chuck_data_t;

int* sortArray;
int sortArray_size;
int readElementsCount;
chuck_data_t* chucks;
int chucks_size;
pthread_mutex_t chucks_lock;

int initArrayAndChuckTbl(int itemsCount)
{
	int i;
	int chuckCount;
	readElementsCount = itemsCount;
	if (itemsCount % 1024 == 0)
	{
		chuckCount = itemsCount / 1024;
	}
	else
	{
		chuckCount = (itemsCount / 1024) + 1;
	}
	chucks_size = chuckCount;
	sortArray_size = chuckCount * 1024;
	sortArray =  (int*) calloc (sizeof(int), chuckCount * 1024);
	chucks =  (chuck_data_t*) calloc (sizeof(chuck_data_t), chuckCount);
	pthread_mutex_init(&chucks_lock, NULL);
	for (i = 0; i < chuckCount; i++)
	{
		chucks[i].chuckNumber = i;
		chucks[i].next = (chuck_data_t*)(chucks + i + 1);
	}
	chucks[chuckCount-1].next = NULL;
	printf("Read %d data chucks...\n", chuckCount);
	return chuckCount;
}


pthread_barrier_t sortSync;

int* mergeAlgo(char* filepath1, char* filepath2)
{
	char* data1 = readFileAsString(filepath1);
	char* data2 = readFileAsString(filepath2);
	int count1 = countCommas(data1)+1;
	int count2 = countCommas(data2)+1;
	int* arr1 = (int*) calloc(sizeof(int),count1);
	int* arr2 = (int*) calloc(sizeof(int),count2);
	splitStringIntoArray(data1, arr1, count1);
	splitStringIntoArray(data2, arr2, count2);
	int arraySize = count1 + count2;

	int* arr3  = (int*)calloc(sizeof(int), arraySize);
	int i = 0;
	int index1 = 0;
	int index2 = 0;
	for(i = 0; i < arraySize; ++i){
		if((arr1[index1] >= arr2[index2])||(index2==count2)){
			arr3[i] = arr1[index1];
			++index1;
			}
		else if ((arr2[index2] > arr1[index1])||(index1==count1)){
			arr3[i] = arr2[index2];
			++index2;
			};
	};

	return arr3;
}

int celDivide(int num, int den){
	if (num % den == 0)
		{return num / den;}
	else
		{return num / den + 1;}
}

int* mergeThread(int i, int p){
	char* chunkName1 =  (char*) calloc (sizeof(char), 15);
	char* chunkName2 =  (char*) calloc (sizeof(char), 15);
	char* tempName =  (char*) calloc (sizeof(char), 15);
	sprintf(chunkName1, "chuck%d", i);
	sprintf(chunkName2, "chuck%d", i+1);
	sprintf(tempName, "chuck%d", i/2);

	int* temp = mergeAlgo(chunkName1, chunkName2);
	remove(chunkName1);
	remove(chunkName2);
	writeCDF(temp, 1024*p, tempName);
	return temp;

}

void oddChunkCorrection(int chunkCount){
	//if odd number of chunks, will create an empty chunk
	//for merge to work
	if (chunkCount % 2 != 0 ){
		char* tempName =  (char*) calloc (sizeof(char), 15);
		sprintf(tempName, "chuck%d", chunkCount);
		FILE *fp = fopen(tempName, "w");
		fclose(fp);
	};
}

void merge(int mergeCount){
	int i = 0;
	int j = 0;
	int p = 2;
	for(j = mergeCount; j>1 ;j=celDivide(j,2)){
		oddChunkCorrection(j);
		for(i = 0; i<j; i += 2){
			mergeThread(i, p);
		};
		p = p*2;
	};
	oddChunkCorrection(j);
	int* result = mergeThread(0, mergeCount);
	writeCDF(result, sortArray_size, "data.out");
	remove("chuck0");
}

int main(void) {
	int numOfThreads = 5;
	int i;
	int count = 0;
	char* data = readFileAsString("data.in");
	count = countCommas(data) + 1;
	int chunkCount = initArrayAndChuckTbl(count);
	splitStringIntoArray(data, sortArray, count);
	pthread_barrier_init(&sortSync, NULL, numOfThreads + 1);
	pthread_t threads[numOfThreads];
	writeTimestamp();
	printf("Create %d threads\n", numOfThreads);
	for (i = 0; i < numOfThreads; i++)
	{
		pthread_create(&threads[i], NULL, sorting_thread, (void *)i);
	}
	pthread_barrier_wait(&sortSync);
	puts("All Threads Done");

	puts("\nStart of merge\n");
	merge(chunkCount);
	puts("End of merge\n");


	return EXIT_SUCCESS;
}

chuck_data_t* getNextChuck()
{
	chuck_data_t* temp;
	pthread_mutex_lock(&chucks_lock);
	temp = chucks;
	if (temp != NULL)
	{
		chucks = temp->next;
	}
	pthread_mutex_unlock(&chucks_lock);
	return temp;
}

void *sorting_thread(void *threadNumber)
{
	int thread_num = (int)(threadNumber);
	printf("Thread %d starting\n", thread_num);
	chuck_data_t* chuck = getNextChuck();
	while (chuck != NULL)
	{
		writeTimestamp();
		printf("Thread %d starts work on chuck %d\n", thread_num, chuck->chuckNumber);
		char* fileName =  (char*) calloc (sizeof(char), 15);
		sprintf(fileName, "chuck%d", chuck->chuckNumber);

		crapSort((int*)(sortArray + 1024*chuck->chuckNumber), 1024);
		writeCDF((int*)(sortArray + 1024*chuck->chuckNumber), 1024, fileName);
		chuck = getNextChuck();
	}

	pthread_barrier_wait(&sortSync);
	pthread_exit(NULL);
}
