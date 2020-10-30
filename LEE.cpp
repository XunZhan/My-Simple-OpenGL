#include "stdafx.h"
#include "LEE.h"
#include "math.h"
#include "Gz.h"
#include "Operation.h"
#include <sstream>

using namespace std;

LEE::LEE(GzCoord* tri, GzColor* cc, GzCoord* nn, GzTextureIndex* tt, int m, int xres, int yres)
{
	row = xres;
	col = yres;
	mode = m;

	BoundingBox(tri);
	LineCheck(tri);



	if (!isLine)
	{
		ClockWisePosition(tri, cc, nn, tt);
		EdgeEquation();
		PlaneEquation();
	}
	else
	{
		LineEquation(tri);
	}

}

LEE::~LEE()
{

}

void LEE::LineCheck(GzCoord* tri)
{
	// if 3 vertex in same line
	// line: f(x) = a*x + b
	float a = (tri[0][1] - tri[1][1]) / (tri[0][0] - tri[1][0]);
	float b = tri[0][1] - a * tri[0][0];

	if (abs(tri[2][1] - a * tri[2][0] - b) < EPSILON)
	{
		isLine = true;
		return;
	}
	isLine = false;
	return;
}

void LEE::ClockWisePosition(GzCoord* tri, GzColor* cc, GzCoord* nn, GzTextureIndex* tt)
{
	// find top point
	int i = 0;
	float y_tmp_min = min(tri[0][1], min(tri[1][1], tri[2][1]));
	for (i = 0; i < 3; i++)
	{
		if (tri[i][1] == y_tmp_min)
		{
			UpdateAttribute(0, i, tri, cc, nn, tt);
			break;
		}
	}

	// select one from other two and decide left or right
	int j = (i + 1) % 3;
	int k = (i + 2) % 3;

	// line pi-pk: f(x) = s*x + b;
	float s = (tri[i][1] - tri[k][1]) / (tri[i][0] - tri[k][0]);
	float b = tri[i][1] - s * tri[i][0];

	// pj intersect with line
	float x = (tri[j][1] - b) / s;
	if (x < tri[j][0])
	{
		// pj is right
		UpdateAttribute(1, j, tri, cc, nn, tt);
		// pk is left
		UpdateAttribute(2, k, tri, cc, nn, tt);
	}
	else
	{
		// pk is right
		UpdateAttribute(1, k, tri, cc, nn, tt);
		// pj is left
		UpdateAttribute(2, j, tri, cc, nn, tt);
	}
}

void LEE::UpdateAttribute(int class_idx, int org_idx, GzCoord* tri,  GzColor* cc, GzCoord* nn, GzTextureIndex* tt)
{
	// update positions 
	for (int d = 0; d < 3; d++)
		p[class_idx][d] = tri[org_idx][d];

	// put org_idx color or norm to class_idx color or norm
	if (mode == GZ_COLOR)
	{
		for (int d = 0; d < 3; d++)
			color[class_idx][d] = cc[org_idx][d];
	}
	else if (mode == GZ_NORMALS)
	{
		for (int d = 0; d < 3; d++)
			norm[class_idx][d] = nn[org_idx][d];
	}

	// update UV
	if (tt != NULL)
	{
		uv[class_idx][0] = tt[org_idx][0];
		uv[class_idx][1] = tt[org_idx][1];
	}
	
	
}

void LEE::BoundingBox(GzCoord* tri)
{
	xmin = (int)min(tri[0][0], min(tri[1][0], tri[2][0]));
	xmax = (int)max(tri[0][0], max(tri[1][0], tri[2][0]));
	ymin = (int)min(tri[0][1], min(tri[1][1], tri[2][1]));
	ymax = (int)max(tri[0][1], max(tri[1][1], tri[2][1]));

	xmin = (xmin < 0) ? 0 : ((xmin < row) ? xmin : (row - 1));
	xmax = (xmax < 0) ? 0 : ((xmax < row) ? xmax : (row - 1));
	ymin = (ymin < 0) ? 0 : ((ymin < col) ? ymin : (col - 1));
	ymax = (ymax < 0) ? 0 : ((ymax < col) ? ymax : (col - 1));
}

void LEE::EdgeEquation()
{
	for (int i = 0; i < 3; i++)
	{
		// head and tail index
		int head = i;
		int tail = (i + 1) % 3;

		// LEE Edge Parameter: [X, Y, dx, dy]
		e[i][0] = p[tail][0];
		e[i][1] = p[tail][1];
		e[i][2] = p[head][0] - p[tail][0];
		e[i][3] = p[head][1] - p[tail][1];
	}
}

void LEE::PlaneEquation()
{
	// position plane equation
	// vector p1p2 and p1p3
	float vec1[3];
	for (int d = 0; d < 3; d++)
	{
		vec1[d] = p[1][d] - p[0][d];
	}

	float vec2[3];
	for (int d = 0; d < 3; d++)
	{
		vec2[d] = p[2][d] - p[0][d];
	}

	// cross product of vec1 vec2
	pos_plane[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	pos_plane[1] = -(vec1[0] * vec2[2] - vec1[2] * vec2[0]);
	pos_plane[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
	pos_plane[3] = -pos_plane[0] * p[0][0] - pos_plane[1] * p[0][1] - pos_plane[2] * p[0][2];

	// UV plane
	for (int d = 0; d < 2; d++)
	{
		float vec1[3];
		vec1[0] = p[1][0] - p[0][0];
		vec1[1] = p[1][1] - p[0][1];
		vec1[2] = uv[1][d] - uv[0][d];

		float vec2[3];
		vec2[0] = p[2][0] - p[0][0];
		vec2[1] = p[2][1] - p[0][1];
		vec2[2] = uv[2][d] - uv[0][d];

		uv_plane[d][0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
		uv_plane[d][1] = -(vec1[0] * vec2[2] - vec1[2] * vec2[0]);
		uv_plane[d][2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
		uv_plane[d][3] = -uv_plane[d][0] * p[0][0] - uv_plane[d][1] * p[0][1] - uv_plane[d][2] * uv[0][d];
	}

	// color plane equation
	if (mode == GZ_COLOR)
	{
		// for every color
		for (int d = 0; d < 3; d++)
		{
			float vec1[3];
			vec1[0] = p[1][0] - p[0][0];
			vec1[1] = p[1][1] - p[0][1];
			vec1[2] = color[1][d] - color[0][d];

			float vec2[3];
			vec2[0] = p[2][0] - p[0][0];
			vec2[1] = p[2][1] - p[0][1];
			vec2[2] = color[2][d] - color[0][d];

			color_plane[d][0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
			color_plane[d][1] = -(vec1[0] * vec2[2] - vec1[2] * vec2[0]);
			color_plane[d][2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
			color_plane[d][3] = -color_plane[d][0] * p[0][0] - color_plane[d][1] * p[0][1] - color_plane[d][2] * color[0][d];
		}
	}

	// normal plane equation
	else if (mode == GZ_NORMALS)
	{
		for (int d = 0; d < 3; d++)
		{
			float vec1[3];
			vec1[0] = p[1][0] - p[0][0];
			vec1[1] = p[1][1] - p[0][1];
			vec1[2] = norm[1][d] - norm[0][d];

			float vec2[3];
			vec2[0] = p[2][0] - p[0][0];
			vec2[1] = p[2][1] - p[0][1];
			vec2[2] = norm[2][d] - norm[0][d];

			norm_plane[d][0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
			norm_plane[d][1] = -(vec1[0] * vec2[2] - vec1[2] * vec2[0]);
			norm_plane[d][2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
			norm_plane[d][3] = -norm_plane[d][0] * p[0][0] - norm_plane[d][1] * p[0][1] - norm_plane[d][2] * norm[0][d];
		}
	}

	
}

void LEE::LineEquation(GzCoord* tri)
{
	// find line segment start and end point
	// line exression: origin + t*direction, t=[0,1]
	int min_idx, max_idx;
	float x_tmp_min = min(tri[0][0], min(tri[1][0], tri[2][0]));
	for (int i = 0; i < 3; i++)
	{
		if (tri[i][0] == x_tmp_min)
		{
			min_idx = i;
			break;
		}
	}

	float x_tmp_max = max(tri[0][0], max(tri[1][0], tri[2][0]));
	for (int i = 0; i < 3; i++)
	{
		// in case all the x have same value
		if (tri[i][0] == x_tmp_max && i != min_idx)
		{
			max_idx = i;
			break;
		}
	}

	float d_len = 0.0;
	for (int d = 0; d < 3; d++)
	{
		org[d] = tri[min_idx][d];
		dir[d] = tri[max_idx][d] - tri[min_idx][d];
		d_len += dir[d] * dir[d];
	}

	// normalize d
	d_len = sqrt(d_len);
	tmax = d_len;
	for (int d = 0; d < 3; d++)
		dir[d] /= d_len;
}

bool LEE::OnLine(int i, int j)
{
	if ((float)i < org[0] || (float)i >(org[0] + tmax * dir[0]))
		return false;

	tcur = ((float)i - org[0]) / dir[0];
	if (abs((float)j - org[1] - tcur * dir[1]) > EPSILON)
		return false;

	return true;
}

bool LEE::InsideTriangle(int i, int j)
{
	if (isLine)
	{
		return OnLine(i, j);
	}

	float x = (float)i;
	float y = (float)j;

	float prev = 0;
	// 3 edges should be all positive/negative
	for (int i = 0; i < 3; i++)
	{
		// eage equation result = dy*(x - X) - dx*(y - Y)
		float cur = e[i][3] * (x - e[i][0]) - e[i][2] * (y - e[i][1]);
		if (i != 0)
		{
			if (prev * cur < 0)
				return false;
		}
		prev = cur;
	}
	return true;
}

float LEE::InterpolateDepth(int i, int j)
{
	// TODO: Debug Point Here.
	/*if (l[2] == 0)
	{
		return 1.0;
	}*/

	if (isLine)
	{
		return (org[2] + tcur * dir[2]);
	}

	return (-pos_plane[0] * (float)(i)-pos_plane[1] * (float)(j)-pos_plane[3]) / pos_plane[2];
}

float* LEE::InterpolateUV(int i, int j)
{
	float* ans = new float[2];
	if (isLine)
	{

	}
	else
	{
		for (int d = 0; d < 2; d++)
		{
			ans[d] = (-uv_plane[d][0] * (float)(i)-uv_plane[d][1] * (float)(j)-uv_plane[d][3]) / uv_plane[d][2];
		}
	}
	return ans;
}


float* LEE::InterpolateAttrib(int i, int j)
{
	// interpolate color
	if (mode == GZ_COLOR)
	{
		float* ans = new float[3];
		if (isLine)
		{

		}
		else
		{
			for (int d = 0; d < 3; d++)
			{
				ans[d] = (-color_plane[d][0] * (float)(i)-color_plane[d][1] * (float)(j)-color_plane[d][3]) / color_plane[d][2];
			}
		}
		return ans;
	}

	// interpolate normal
	else if (mode == GZ_NORMALS)
	{
		float* ans = new float[3];
		Vec3 tmp;
		if (isLine)
		{

		}
		else
		{
			for (int d = 0; d < 3; d++)
			{
				tmp.v[d] = (-norm_plane[d][0] * (float)(i)-norm_plane[d][1] * (float)(j)-norm_plane[d][3]) / norm_plane[d][2];
			}
		}

		tmp.normlize();
		for (int d = 0; d < 3; d++)
			ans[d] = tmp.v[d];
		return ans;
	}
}