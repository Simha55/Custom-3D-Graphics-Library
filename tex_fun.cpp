/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"



GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
  float res_x, res_y;
  if (u < 0) { u = 0; }
  if (v < 0) { v = 0; }
  if (u > 1) { u = 1; }
  if (v > 1) { v = 1; }
  res_x = (u * (xs - 1));
  res_y = (v * (ys - 1));
  int Ax = (int)res_x, Ay = (int)res_y;
  int index_A = Ax + Ay * xs;
  int index_B = (Ax + 1) + Ay * xs;
  int index_C = (Ax + 1) + (Ay + 1) * xs;
  int index_D = Ax + (Ay + 1) * xs;
  float s, t;
  s = res_x - Ax;
  t = res_y - Ay;
  color[0] = s * t * image[index_C][RED] + (1 - s) * t * image[index_D][RED] + (1 - t) * s * image[index_B][RED] + (1 - t) * (1 - s) * image[index_A][RED];
  color[1] = s * t * image[index_C][GREEN] + (1 - s) * t * image[index_D][GREEN] + (1 - t) * s * image[index_B][GREEN] + (1 - t) * (1 - s) * image[index_A][GREEN];
  color[2] = s * t * image[index_C][BLUE] + (1 - s) * t * image[index_D][BLUE] + (1 - t) * s * image[index_B][BLUE] + (1 - t) * (1 - s) * image[index_A][BLUE];
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */

  
    return GZ_SUCCESS;
}

#define PI 3.14159265358979323846

// Function to calculate the distance from the center (radial distance)
float radial_distance(float u, float v) {
    return sqrt(u * u + v * v);
}

// Function to calculate the angle (polar coordinate)
float radial_angle(float u, float v) {
    return atan2(v, u);
}

// Flower texture function
int ptex_fun(float u, float v, GzColor color) {
    // Normalize u and v to [-1, 1]
    u = u * 2.0f - 1.0f;
    v = v * 2.0f - 1.0f;

    // Calculate the distance and angle from the center
    float r = radial_distance(u, v);
    float theta = radial_angle(u, v);

    // Create the petal pattern using sine and radial distance
    int numPetals = 6; // Number of petals
    float petalPattern = 0.5f + 0.5f * sin(numPetals * theta + r * 5.0f); // Petal pattern

    // Use radial distance to fade the pattern outward
    float fadeFactor = exp(-3.0f * r * r);

    // Define the brown background color (RGB values for brown)
    GzColor brownBackground = { 0.6f, 0.3f, 0.1f }; // Dark brown

    // Set color based on the petal pattern and fade
    color[0] = petalPattern * fadeFactor + brownBackground[0] * (1.0f - fadeFactor); // Red
    color[1] = (1.0f - petalPattern) * fadeFactor + brownBackground[1] * (1.0f - fadeFactor); // Green
    color[2] = (0.7f + 0.3f * petalPattern) * fadeFactor + brownBackground[2] * (1.0f - fadeFactor); // Blue

    // Clamp color values to [0, 1]
    color[0] = fmax(0.0f, fmin(1.0f, color[0]));
    color[1] = fmax(0.0f, fmin(1.0f, color[1]));
    color[2] = fmax(0.0f, fmin(1.0f, color[2]));

    return GZ_SUCCESS;
}


/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

