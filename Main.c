/*
 * Main.c
 *
 *  Created on: 14/06/2015
 *      Author: lucas
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <math.h>
#include "Benchmark.h"

//The number Bytes to write (2^SIZE)
#define SIZE 16
//The max number Bytes to write in each req (2^REQSIZE)
//A req tests writing from 2^0 to 2^REQSIZE
//All tests test each case using reqsize
#define REQSIZE 16
//Useful constants
#define _64KB 65536

//8GB = 8388608KB
//4GB = 4194304KB
//64KB = 65536B

char* makeChunkBuffer(int chunkSize, int initialize) {
	char *BUFF;
	BUFF = (char*) calloc(chunkSize, sizeof(char));

	if (BUFF == NULL) {
		puts("Buffer allocation failed!");
		exit(-1);
	}

	if(initialize != 0){
		int i = 0;
		for (i = 0; i < chunkSize; i++) {
			BUFF[i] = '0';
		}
	}
	return BUFF;
}

int main(void) {

	//Seed the random
	srand(time(NULL));

	puts("Started.");

	//Open the file to write
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
		const int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be written to fill the total size
		const int times = totalSize / chunkSize;

		//Debug
		printf("SW of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize,1);

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

		//Free current chunk buffer (we will get a bigger one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	//Inform
	puts("Sequential write complete.");

	//Reset n
	n = 0;

	//Movers
	int pos = 0;

	//Inform
	puts("Starting random write");

	//Use N to write chuks and increase by req size
	////////////////////////////////////////////////
	////              RANDOM WRITE              ////
	////////////////////////////////////////////////
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		const int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be written to fill the total size
		const int times = totalSize / chunkSize;

		//Debug
		printf("RW of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize,1);

		//Benchmark the start time
		float startTime = getTime();

		//Write each chunk to the file using write syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			//Calculate position
			if (i <= times / 2) {
				pos = i * chunkSize;
			} else {
				pos = totalSize - ((i - (times / 2)) * chunkSize);
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

		//Free current chunk buffer (we will get a bigger one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	//End inform
	puts("Ended write.");

	//Close the file for write
	close(file);

	//Open the read file
	file = open("bench.txt", O_RDONLY);

	//Inform
	puts("Read start.");

	////////////////////////////////////////////////
	////            SEQUENTIAL READ             ////
	////////////////////////////////////////////////
	//Reset n counter
	n = 0;
	//Use N to read chuks and increase by req size
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		const int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be read to fill the total size
		const int times = totalSize / chunkSize;

		//Debug
		printf("SR of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize, 0);

		//Benchmark the start time
		float startTime = getTime();

		//Read each chunk to the file using read syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			/*
			 *  On files that support seeking, the read operation commences at the
			 *	current file offset, and the file offset is incremented by the number
			 * 	of bytes read. (read man)
			 */
			int count = 0;
			do {
				count = read(file, Buffer, chunkSize);
			}while(count > 0);
		}

		//Benchmark end time
		float endTime = getTime();
		float timeElapsed = endTime - startTime;

		//Check buffer (should all be zeroes)
		/*i = 0;
		for (i = 0; i < chunkSize; i++) {
			if (Buffer[i] != '0') {
				puts("Buffer error. Non-zero returned on read.");
				printf("Current chunk size: %d\n",chunkSize);
				printf("Buffer: %s\n",Buffer);
				break;
			}
		}*/

		//Debug
		printf("Sequential read time: %f s\n", timeElapsed);

		//Free current chunk buffer (we will get a bigger one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	puts("Random read.");

	////////////////////////////////////////////////
	////              RANDOM READ               ////
	////////////////////////////////////////////////
	//Reset n counter
	n = 0;
	//Use N to read chuks and increase by req size
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		const int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be read to fill the total size
		const int times = totalSize / chunkSize;

		//Debug
		printf("RR of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize, 0);

		//Benchmark the start time
		float startTime = getTime();

		//Read each chunk to the file using read syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			//Select a place to read
			//from [0 to (totalSize - chunkSize)[
			int pos = rand() % (totalSize - chunkSize);
			//Go there
			lseek(file, pos, SEEK_SET);

			int count = 0;
			do {
				count = read(file, Buffer, chunkSize);
			}while(count < chunkSize);
		}

		//Benchmark end time
		float endTime = getTime();
		float timeElapsed = endTime - startTime;

		//Debug
		printf("Random read time: %f s\n", timeElapsed);

		//Free current chunk buffer (we will get a bigger one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	puts("Reverse read.");

	////////////////////////////////////////////////
	////             REVERSE READ               ////
	////////////////////////////////////////////////
	//Reset n counter
	n = 0;
	//Use N to read chuks and increase by req size
	while (n < REQSIZE) {
		//Calculate the size of the current chunk
		const int chunkSize = (int) powf(2.0, n);
		//Calculate how many times the chunk needs to be read to fill the total size
		const int times = totalSize / chunkSize;

		//Debug
		printf("RevR of %d bytes %d times. (%d bytes)\n", chunkSize, times,
				totalSize);
		//Build the buffer
		Buffer = makeChunkBuffer(chunkSize, 0);

		//Benchmark the start time
		float startTime = getTime();

		//Read each chunk to the file using read syscall
		int i = 0;
		for (i = 0; i < times; i++) {
			//Select a place to read
			int pos = totalSize - (i * chunkSize) - chunkSize;
			//Go there
			lseek(file, pos, SEEK_SET);

			int count = 0;
			do {
				count = read(file, Buffer, chunkSize);
			}while(count < chunkSize);
		}

		//Benchmark end time
		float endTime = getTime();
		float timeElapsed = endTime - startTime;

		//Debug
		printf("Reverse read time: %f s\n", timeElapsed);

		//Free current chunk buffer (we will get a bigger one)
		free(Buffer);
		//Rewind the file
		lseek(file, 0, SEEK_SET);
		//Increase chunk size
		n++;
	}

	//End inform
	puts("Ended read.");

	//Close the file for write
	close(file);

	//return correctly
	return 0;
}
