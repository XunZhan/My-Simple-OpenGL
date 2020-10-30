#include    "stdafx.h" 
#include	"Gz.h"

GzColor	*image = NULL;
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
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */

  if (u < 0.0)
	  u = 0.0;
  if (u > 1.0)
	  u = 1.0;
  if (v < 0.0)
	  v = 0.0;
  if (v > 1.0)
	  v = 1.0;

  float u_ = u * (xs - 1);
  float v_ = v * (ys - 1);

  int xmin = (int)floor(u_), xmax = (int)ceil(u_);
  int ymin = (int)floor(v_), ymax = (int)ceil(v_);

  int A_idx = ymin * xs + xmin;
  int B_idx = ymin * xs + xmax;
  int C_idx = ymax * xs + xmax;
  int D_idx = ymax * xs + xmin;

  float s = u_ - (float)xmin;
  float t = v_ - (float)ymin;
  for (int d = 0; d < 3; d++)
  {
	  color[d] = s * t * image[C_idx][d] + 
		  (1 - s)*t*image[D_idx][d] + 
		  s*(1 - t)*image[B_idx][d] + 
		  (1 - s)*(1 - t)*image[A_idx][d];
  }
  
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	if (u < 0.0)
		u = 0.0;
	if (u > 1.0)
		u = 1.0;
	if (v < 0.0)
		v = 0.0;
	if (v > 1.0)
		v = 1.0;

	float section[6] = { 1 / 6.0, 2 / 6.0, 3 / 6.0, 4 / 6.0, 5 / 6.0, 6 / 6.0 };
	int x = -1, y = -1; // -1: not chosen, 0: even, 1: odd
	if (u == 1.0)
	{
		x = 0;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if (u >= section[i] - 1 / 6.0 && u < section[i])
			{
				x = i % 2;
				break;
			}
		}
	}

	if (v == 1.0)
	{
		y = 0;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if ( v >= section[i] - 1 / 6.0 && v < section[i])
			{
				y = i % 2;
				break;
			}
		}
	}

	if ((x == 0 && y == 0) || (x == 1 && y == 1))
	{
		for (int d = 0; d < 3; d++)
			color[d] = 0.0;
	}
	else
	{
		for (int d = 0; d < 3; d++)
			color[d] = 1.0;
	}
	
	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}


