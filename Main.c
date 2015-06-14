/*
 * Main.c
 *
 *  Created on: 14/06/2015
 *      Author: lucas
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include "Benchmark.h"

//The number Bytes to write (2^SIZE)
//TO DO is 16 here
#define SIZE 16
//Useful constants
#define _64KB 65536

//8GB = 8388608KB
//4GB = 4194304KB
//64KB = 65536B

char* makeChunkBuffer(int chunkSize){
	char *BUFF;
	BUFF = (char*) calloc (chunkSize,sizeof(char));

	if(BUFF == NULL){
		puts("Buffer allocation failed!");
		exit(-1);
	}

	int i = 0;
	for(i=0;i<chunkSize;i++){
		BUFF[i] = '0';
	}
	return BUFF;
}

int main(void){

	puts("Started.");

	int file = open("bench.txt", O_WRONLY | O_CREAT, 0644);

	//Check if the file opened correctly
	if(file < 0){
		perror("open");
		return -1; //Close the program
	}

	puts("File ready.");

	//Write all chunks
	int n = 0;
	int totalSize = (int)powf(2.0,SIZE);
	char *Buffer;
	while(n<SIZE){
		int chunkSize = (int)powf(2.0,n);
		int times = totalSize/chunkSize;

		printf("SW of %d bytes %d times. (%d bytes)\n",chunkSize, times, totalSize);
		Buffer = makeChunkBuffer(chunkSize);

		float startTime = getTime();
		int i = 0;
		for(i=0;i<times;i++){
			if(write(file, &Buffer, chunkSize) < 0){
				puts("Write error.");
				perror("write");
				return -1; //Failed to write
			}
		}
		float endTime = getTime();
		float timeElapsed = endTime - startTime;
		printf("Sequential write time: %f s\n", timeElapsed);
		free(Buffer);
		lseek(file,0, SEEK_SET);
		n++;
	}

	//End
	puts("Ended.");

	//Close the file 'handle'
	close(file);

	//return correctly
	return 0;
}
