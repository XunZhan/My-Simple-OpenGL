#pragma once
#include	"gz.h"

#define EPSILON 1e-5

class LEE
{
public:
	// Constructors
	LEE(GzCoord* tri, GzColor* color, GzCoord* norm, GzTextureIndex* tt, int m, int xres, int yres);
	~LEE();

	int row, col;
	int xmin, xmax, ymin, ymax; // bounding box position

	bool InsideTriangle(int i, int j);
	float InterpolateDepth(int i, int j);
	float* InterpolateAttrib(int i, int j);
	float* InterpolateUV(int i, int j);

private:
	float p[3][3];  // clock wise position
	float color[3][3]; 
	float norm[3][3];
	float uv[3][2];
	float e[3][4];  // edge equation parameters 

	float pos_plane[4];     // position plane equation parameters
	float color_plane[3][4];     // color plane equation parameters
	float norm_plane[3][4];     // norm plane equation parameters
	float uv_plane[2][4];     // UV plane equation parameters

	int mode;
	bool isLine;
	float org[3];
	float dir[3];
	float tmax;
	float tcur;
	bool OnLine(int i, int j);

	void ClockWisePosition(GzCoord* tri, GzColor* color, GzCoord* norm, GzTextureIndex* uv);
	void BoundingBox(GzCoord* tri);
	void LineCheck(GzCoord* tri);
	void LineEquation(GzCoord* tri);
	void EdgeEquation();
	void PlaneEquation();
	void UpdateAttribute(int class_idx, int org_idx, GzCoord* tri, GzColor* cc, GzCoord* nn, GzTextureIndex* tt);
};

