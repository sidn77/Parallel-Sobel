/*
	Purpose: Parralelize the Sobel Operator.
	
	Authors: Siddhant Narang
	
	References used: OpenMPI Docs,
					 Stackoverflow
*/


#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

#define MAX_BRIGHTNESS 255
#define DIM 128

int width;
int height;
int widthnew;
unsigned char header[54];
FILE *fd;
unsigned char **ia, input_array[DIM][DIM], row[DIM], output_array[DIM][DIM];
float weight[3][3] = {{-1, 0, 1},
						 {-2, 0, 2},
						 {-1, 0, 1}};

unsigned char** read_image(FILE *fd)
{	
	// Read header
	fread(header, sizeof(unsigned char), 54, fd);

	// Capture dimensions
	width = *(int*)&header[18];
	height = *(int*)&header[22];

	int padding = 0;
	// Calculate padding
	while ((width + padding) % 4 != 0) {
		padding++;
	}

	// Compute new width, which includes padding
	widthnew = width * 3 + padding;

	// Allocate memory to store image data (non-padded)
	unsigned char **texels = (unsigned char **)malloc(height * sizeof(unsigned char*));
	if (texels == NULL) {
		printf("Error: Malloc failed\n");
		exit(0);
	}

	// Allocate temporary memory to read widthnew size of data
	unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned char));
	printf("\nReading image\n");
	for (int i = 0; i < height; i++) {
		// Read widthnew length of data
		fread(data, sizeof(unsigned char), widthnew, fd);
		texels[i] = (unsigned char*)malloc(width * 3 * sizeof(unsigned char));
		for (int j = 0; j < width * 3; j++) {
			texels[i][j] = (unsigned char)data[j];
		}
	}
	fclose(fd);
	return texels;
}


void write_image(FILE *fd, unsigned char output_array[][DIM])
{
	fd = fopen("12.bmp", "wb");
	if (fd == NULL) {
		printf("Error: fopen failed\n");
		exit(0); 
	}

	unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned char));
	fwrite(header, sizeof(unsigned char), 54, fd);
	for (int i = 0; i < height; i++) {
    	for(int j = 0; j < width; j++) {
    		data[j] = output_array[i][j];
    	}
    	fwrite(data, sizeof(unsigned char), widthnew, fd);
	}
}


int main(int argc, char *argv[])
{
	int rank, size;
	double g_min = 0, g_max = 0;
	double pixel_value, min = 0, max = 0;
	

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_rank(MPI_COMM_WORLD, &size);

	if(rank == 0) {
		fd = fopen("res.bmp", "rb");    	
 		ia = read_image(fd);
 		for(int i = 0; i < height; i++) {
 			for(int j = 0; j < width * 3; j++) {
 				input_array[i][j] = ia[i][j];
 			}
 		}

 		printf("Allocating space\n");
  		printf("Now, filtering of input image is performed \n\n");
		min = 255;
		max = -255;
		for(int i = 0; i < height; i++) {
 			for(int j = 0; j < width; j++) {
 				output_array[i][j] = 100;
 			}
 		}
 	}

	MPI_Bcast(*input_array, DIM * DIM, MPI_CHAR, 0, MPI_COMM_WORLD);

	for (int y = 1; y < DIM - 1; y++) {
		pixel_value = 0.0;
		for (int j = -1; j <= 1; j++) {
	    	for (int i = -1; i <= 1; i++) {
	      		pixel_value += weight[j + 1][i + 1] * input_array[y + j][rank + i];
	    	}
      	}
      	if (pixel_value < min) min = pixel_value;
      	if (pixel_value > max) max = pixel_value;
    }

    MPI_Reduce(&min, &g_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max, &g_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0) {
		printf("Min: %lf, Max: %lf", g_min, g_max);
	}
	
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) {
		for (int y = 1; y < height - 1; y++) {
			for(int x = 1; x < width - 1; x++) {
				pixel_value = 0.0;
				for (int j = -1; j <= 1; j++) {
		    		for (int i = -1; i <= 1; i++) {
		      			pixel_value += weight[j + 1][i + 1] * input_array[y + j][x + i];
		    		}
	      		}
	     		pixel_value = MAX_BRIGHTNESS * (pixel_value - g_min) / (g_max - g_min);
	      		output_array[y][x] = (unsigned char)pixel_value; 
	      	}
	    }
		write_image(fd, output_array);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
