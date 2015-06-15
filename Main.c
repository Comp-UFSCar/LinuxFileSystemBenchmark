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
#define SIZE 16
//The number Bytes to write in each req (2^REQSIZE)
#define REQSIZE 16
//Useful constants
#define _64KB 65536

//8GB = 8388608KB
//4GB = 4194304KB
//64KB = 65536B

char* makeChunkBuffer(int chunkSize) {
	char *BUFF;
	BUFF = (char*) calloc(chunkSize, sizeof(char));

	if (BUFF == NULL) {
		puts("Buffer allocation failed!");
		exit(-1);
	}

	int i = 0;
	for (i = 0; i < chunkSize; i++) {
		BUFF[i] = '0';
	}
	return BUFF;
}

int main(void) {

	puts("Started.");

	int file = open("bench.txt", O_WRONLY | O_CREAT, 0644);

	//Check if the file opened correctly
	if (file < 0) {
		perror("open");
		return -1; //Close the program
	}

	puts("File ready.");

	////////////////////////////////////////////////
	////            SEQUENTIAL WRITE            ////
	////////////////////////////////////////////////
	//Write all chunks
	int n = 0;
	//Calculate total size to write
	const int totalSize = (int) powf(2.0, SIZE);
	//The buffer to use for writing
	char *Buffer;
	//Use N to write chuks and increase by req size
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be written to fill the total size
		int times = totalSize / chunkSize;

		//Debug
		printf("SW of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize);

		//Benchmark the start time
		float startTime = getTime();

		//Write each chunk to the file using write syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			if (write(file, &Buffer, chunkSize) < 0) {
				puts("Write error.");
				perror("write");
				return -1; //Failed to write
			}
		}

		//Benchmark end time
		float endTime = getTime();
		float timeElapsed = endTime - startTime;

		//Debug
		printf("Sequential write time: %f s\n", timeElapsed);

		//Free current chunk buffer (we will get a bigger on of no one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	//Inform
	puts("Sequential write complete.");

	//Free the buffer
	//free(Buffer);

	//Reset n
	n = 0;

	//Movers
	int pos = 0;
	int end = totalSize;

	//Inform
	puts("Starting random write");

	//Use N to write chuks and increase by req size
	////////////////////////////////////////////////
	////              RANDOM WRITE              ////
	////////////////////////////////////////////////
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be written to fill the total size
		int times = totalSize / chunkSize;

		//Debug
		printf("RW of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize);

		//Benchmark the start time
		float startTime = getTime();

		//Write each chunk to the file using write syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			//Calculate position
			if(i<=times/2){
				pos = i*chunkSize;
			}else{
				pos = totalSize - ((i-(times/2))*chunkSize);
			}

			//Go to position
			lseek(file, pos, SEEK_SET);

			if (write(file, &Buffer, chunkSize) < 0) {
				puts("Write error.");
				perror("write");
				return -1; //Failed to write
			}
		}

		//Benchmark end time
		float endTime = getTime();
		float timeElapsed = endTime - startTime;

		//Debug
		printf("Random write time: %f s\n", timeElapsed);

		//Free current chunk buffer (we will get a bigger on of no one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	//End
	puts("Ended.");

	//Close the file
	close(file);

	//return correctly
	return 0;
}
