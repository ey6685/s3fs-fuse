#include <openssl/rc4.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstddef>
#include <cstring>

#include "jstoddart.h"

using namespace std;

const char *Hash::hashKey = "password text entered here";

int transform_XOR(int fd){//function takes file description as input and
						//performs an XOR transformation using a user supplied
						//hash key with the openssl RC4 function
	const char *hashKey = "password text entered here";//declare the password hash key
	size_t hashLength = strlen(hashKey);//get size of the password hash key

	//int hashLength = line.length();
	//const unsigned char* hashKey = reinterpret_cast<const unsigned char*>(line.c_str());

	off_t offset = lseek(fd, 0, SEEK_END);//get length of fd buffer

	if (offset < 0){//if buffer is zero then exit
		perror("Cannot obtain file offset");
		exit(EXIT_FAILURE);
	}

	lseek(fd, 0, SEEK_SET);//set iterator back to beginning of buffer

	unsigned char input_buffer[offset];//declare new char buffer for input
	unsigned char *output_buffer = (unsigned char*)malloc(offset+1);//set size of output buffer

	if (pread(fd, &input_buffer, offset, 0) == -1){//read 'offset' bytes from fd buffer into
												//input buffer starting at beginning
		perror("Could not read file");			//else exit
		exit(EXIT_FAILURE);
	}
	
	RC4_KEY key;//initialize hash key
	RC4_set_key(&key, hashLength, (unsigned char *)hashKey);
		//initialize stream cipher into key using hashKey and hashLength
	RC4(&key, offset, input_buffer, output_buffer);
		//XOR offset bytes of data from input buffer into output buffer
	if (pwrite(fd, output_buffer, offset, 0) == -1){//write XOR data from output buffer
												//back into file descriptor buffer
		perror("Could not write file");			//else exit
		exit(EXIT_FAILURE);
	}

	free(output_buffer);//release output buffer
	return 0;
};