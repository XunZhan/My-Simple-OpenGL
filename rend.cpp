/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    "LEE.h"
#include    <sstream>
#include    "Operation.h"
using namespace std;

#define PI (float) 3.14159265358979323846

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0.0;

	float rad = degree * PI / 180.0;
	mat[0][0] = 1.0;
	mat[1][1] = cos(rad);
	mat[1][2] = -sin(rad);
	mat[2][1] = sin(rad);
	mat[2][2] = cos(rad);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0.0;

	float rad = degree * PI / 180.0;
	mat[1][1] = 1.0;
	mat[0][0] = cos(rad);
	mat[0][2] = -sin(rad);
	mat[2][0] = sin(rad);
	mat[2][2] = cos(rad);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0.0;

	float rad = degree * PI / 180.0;
	mat[2][2] = 1.0;
	mat[0][0] = cos(rad);
	mat[0][1] = -sin(rad);
	mat[1][0] = sin(rad);
	mat[1][1] = cos(rad);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = (i == j) ? 1.0 : 0.0;

	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0.0;

	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = xRes;
	yres = yRes;
	framebuffer = (char*)malloc(3 * sizeof(char) * xres * yres);
	pixelbuffer = (GzPixel*)malloc(sizeof(GzPixel) * xres * yres);
	for (int s = 0; s < AAKERNEL_SIZE; s++)
	{
		pixelbufferlist[s] = (GzPixel*)malloc(sizeof(GzPixel) * xres * yres);
	}

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;

	m_camera.worldup[0] = 0.0;
	m_camera.worldup[1] = 1.0;
	m_camera.worldup[2] = 0.0;

	m_camera.FOV = DEFAULT_FOV;
	for (int i = 0; i < 3; i++)
	{
		m_camera.lookat[i] = 0.0;
	}

	matlevel = 0;
	Mat4 identity = Mat4(true);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			Ximage[0][i][j] = identity.m[i][j];
			Xnorm[0][i][j] = identity.m[i][j];
		}

	/* HW 4 Added by Xun
	- default light setting
	*/
	numlights = 0;
	/* HW 6 offset for sub pixels*/
	float filterTmp[AAKERNEL_SIZE][3]{ -0.52, 0.38, 0.128,    0.41, 0.56, 0.119,    0.27, 0.08, 0.294,
										  -0.17, -0.29, 0.249, 0.58, -0.55, 0.104, -0.31, -0.71, 0.106 };

	for (int i = 0; i < AAKERNEL_SIZE; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AAFilter[i][j] = filterTmp[i][j];
		}
	}
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	free(framebuffer);
	free(pixelbuffer);
	for (int s = 0; s < AAKERNEL_SIZE; s++)
		free(pixelbufferlist[s]);
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < xres; i++)
	{
		for (int j = 0; j < yres; j++)
		{
			pixelbuffer[ARRAY(i, j)].red = 3000;
			pixelbuffer[ARRAY(i, j)].green = 3000;
			pixelbuffer[ARRAY(i, j)].blue = 3000;
			pixelbuffer[ARRAY(i, j)].alpha = 1;
			pixelbuffer[ARRAY(i, j)].z = INT_MAX;
		}
	}

	for (int s = 0; s < AAKERNEL_SIZE; s++)
	{
		for (int i = 0; i < xres; i++)
		{
			for (int j = 0; j < yres; j++)
			{
				pixelbufferlist[s][ARRAY(i, j)].red = 3000;
				pixelbufferlist[s][ARRAY(i, j)].green = 3000;
				pixelbufferlist[s][ARRAY(i, j)].blue = 3000;
				pixelbufferlist[s][ARRAY(i, j)].alpha = 1;
				pixelbufferlist[s][ARRAY(i, j)].z = INT_MAX;
			}
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	GzMatrix Xsp;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m_camera.Xiw[i][j] = 0.0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m_camera.Xpi[i][j] = 0.0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Xsp[i][j] = 0.0;

	GzDefault();

	// calc camera axis
	Vec3 camera_direction = (Vec3(m_camera.lookat) - Vec3(m_camera.position)).normlize();
	Vec3 camera_up = (Vec3(m_camera.worldup) - (Vec3(camera_direction) * (Vec3(m_camera.worldup).dot(camera_direction)))).normlize();
	Vec3 camera_right = (camera_up.cross(camera_direction)).normlize();


	// calc Xiw
	for (int i = 0; i < 3; i++)
	{
		m_camera.Xiw[2][i] = camera_direction.v[i];
		m_camera.Xiw[1][i] = camera_up.v[i];
		m_camera.Xiw[0][i] = camera_right.v[i];
	}
	m_camera.Xiw[0][3] = -camera_right.dot(Vec3(m_camera.position));
	m_camera.Xiw[1][3] = -camera_up.dot(Vec3(m_camera.position));
	m_camera.Xiw[2][3] = -camera_direction.dot(Vec3(m_camera.position));
	m_camera.Xiw[3][3] = 1.0;

	// calc Xpi
	for (int i = 0; i < 4; i++)
		m_camera.Xpi[i][i] = 1.0;
	float d = tan((m_camera.FOV / 2) * PI / 180.0);
	m_camera.Xpi[2][2] = d;
	m_camera.Xpi[3][2] = d;

	// clac Xsp
	Xsp[0][0] = xres / 2.0;
	Xsp[0][3] = xres / 2.0;
	Xsp[1][1] = -yres / 2.0;
	Xsp[1][3] = yres / 2.0;
	Xsp[2][2] = INT_MAX;
	Xsp[3][3] = 1.0;

	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw, false);

	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera = camera;
	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix, bool ignoreNorm)
{
/* HW 3.9 
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (matlevel + 1 >= MATLEVELS)
	{
		AfxMessageBox(_T("Trnsformation Stack is already full!\n"));
		return GZ_FAILURE;
	}


	Mat4 mat_img = Mat4(Ximage[matlevel]) * Mat4(matrix);
	Mat4 mat_norm = Mat4(Xnorm[matlevel]);

	if (!ignoreNorm)
	{
		mat_norm = Mat4(Xnorm[matlevel])* Mat4(matrix);
		// delete translation and normalize vector
		for (int j = 0; j < 3; j++)
			mat_norm.m[j][3] = 0.0;

		for (int j = 0; j < 3; j++)
		{
			Vec3 vec = Vec3(mat_norm.m[j][0], mat_norm.m[j][1], mat_norm.m[j][2]);
			vec.normlize();
			mat_norm.m[j][0] = vec.v[0];
			mat_norm.m[j][1] = vec.v[1];
			mat_norm.m[j][2] = vec.v[2];
		}
	}

	matlevel++;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			Ximage[matlevel][i][j] = mat_img.m[i][j];
			Xnorm[matlevel][i][j] = mat_norm.m[i][j];
		}

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel - 1 < 0)
	{
		AfxMessageBox(_T("Ximage Stack is already empty!\n"));
		return GZ_FAILURE;
	}

	matlevel--;
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int subPixel)
{
/* HW1.4 write pixel values into the buffer */
	if (i < 0 || i >= xres || j < 0 || j >= yres)
	{

	}
	else
	{
		pixelbufferlist[subPixel][ARRAY(i, j)].red = CLAMP(r, (GzIntensity)0, (GzIntensity)4095);
		pixelbufferlist[subPixel][ARRAY(i, j)].green = CLAMP(g, (GzIntensity)0, (GzIntensity)4095);
		pixelbufferlist[subPixel][ARRAY(i, j)].blue = CLAMP(b, (GzIntensity)0, (GzIntensity)4095);
		pixelbufferlist[subPixel][ARRAY(i, j)].alpha = a;
		pixelbufferlist[subPixel][ARRAY(i, j)].z = z;
	}

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	ApplyFilter();
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	stringstream s;
	string head = "P6 " + to_string(xres) + " " + to_string(yres) + " 255\r";
	string content = "";

	for (int j = 0; j < yres; j++)
	{
		for (int i = 0; i < xres; i++)
		{
			content += (char)(pixelbuffer[ARRAY(i, j)].red >> 4);
			content += (char)(pixelbuffer[ARRAY(i, j)].green >> 4);
			content += (char)(pixelbuffer[ARRAY(i, j)].blue >> 4);
		}
	}

	string str = head + content;
	int success = fwrite(str.data(), str.size(), 1, outfile);
	if (success != 1)
	{
		MessageBox(NULL, (LPCTSTR)_T("Write output file error!"), (LPCTSTR)_T("Error"), MB_OK);
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;
}

void GzRender::ApplyFilter()
{
	int numPixels = xres * yres;

	for (int p = 0; p < numPixels; p++)
	{
		float r = 0.0, g = 0.0, b = 0.0;
		for (int s = 0; s < AAKERNEL_SIZE; s++)
		{
			r += pixelbufferlist[s][p].red * AAFilter[s][2];
			g += pixelbufferlist[s][p].green * AAFilter[s][2];
			b += pixelbufferlist[s][p].blue * AAFilter[s][2];
		}
		pixelbuffer[p].red = (GzIntensity)CLAMP(r, 0.0f, 4096.0f);
		pixelbuffer[p].green = (GzIntensity)CLAMP(g, 0.0f, 4096.0f);
		pixelbuffer[p].blue = (GzIntensity)CLAMP(b, 0.0f, 4096.0f);
	}
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int i = 0; i < xres; i++)
	{
		for (int j = 0; j < yres; j++)
		{
			framebuffer[3 * ARRAY(i, j) + 0] = (char)(pixelbuffer[ARRAY(i, j)].blue >> 4);
			framebuffer[3 * ARRAY(i, j) + 1] = (char)(pixelbuffer[ARRAY(i, j)].green >> 4);
			framebuffer[3 * ARRAY(i, j) + 2] = (char)(pixelbuffer[ARRAY(i, j)].red >> 4);
		}
	}
	return GZ_SUCCESS;
}

template <class T> T GzRender::CLAMP(T value, T low, T high)
{
	if (value < low)
		return low;
	if (value > high)
		return high;
	return value;
}

/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/

/*
- GzPutAttribute() must accept the following tokens/values:

- GZ_RGB_COLOR					GzColor		default flat-shade color
- GZ_INTERPOLATE				int			shader interpolation mode
- GZ_DIRECTIONAL_LIGHT			GzLight
- GZ_AMBIENT_LIGHT            	GzLight		(ignore direction)
- GZ_AMBIENT_COEFFICIENT		GzColor		Ka reflectance
- GZ_DIFFUSE_COEFFICIENT		GzColor		Kd reflectance
- GZ_SPECULAR_COEFFICIENT       GzColor		Ks coef's
- GZ_DISTRIBUTION_COEFFICIENT   float		spec power
*/
	for (int i = 0; i < numAttributes; i++)
	{
		switch (nameList[i])
		{
		case GZ_RGB_COLOR:
		{
			GzColor* vl = (GzColor*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				flatcolor[j] = vl[0][j];
			}
			break;
		}

		case GZ_DIRECTIONAL_LIGHT:
		{
			GzLight* dl = (GzLight*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				lights[numlights].direction[j] = dl->direction[j];
				lights[numlights].color[j] = dl->color[j];
			}
			numlights++;
			break;
		}

		case GZ_AMBIENT_LIGHT:
		{
			GzLight* al = (GzLight*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				ambientlight.direction[j] = al->direction[j];
				ambientlight.color[j] = al->color[j];
			}
			break;
		}

		case GZ_AMBIENT_COEFFICIENT:
		{
			GzColor* ac = (GzColor*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				Ka[j] = ac[0][j];
			}
			break;
		}

		case GZ_DIFFUSE_COEFFICIENT:
		{
			GzColor* dc = (GzColor*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				Kd[j] = dc[0][j];
			}
			break;
		}

		case GZ_SPECULAR_COEFFICIENT:
		{
			GzColor* sc = (GzColor*)valueList[i];
			for (int j = 0; j < 3; j++)
			{
				Ks[j] = sc[0][j];
			}
			break;
		}

		case GZ_DISTRIBUTION_COEFFICIENT:
		{
			spec = *((float*)valueList[i]);
			break;
		}

		case GZ_INTERPOLATE:
		{
			interp_mode = *((int*)valueList[i]);
		}

		case GZ_TEXTURE_MAP:
		{
			tex_fun = *(GzTexture)valueList[i];
		}
		default:
			break;
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Return error code
*/
/*
-- Xform positions of verts using matrix on top of stack 
-- Clip - just discard any triangle with any vert(s) behind view plane 
		- optional: test for triangles with all three verts off-screen (trivial frustum cull)
-- invoke triangle rasterizer  
*/
// TODO: && can be ignored?
	switch (numParts)
	{
		case 1:
		{
			for (int subPixel = 0; subPixel < AAKERNEL_SIZE; subPixel++)
				PutTriangle_Flat(1, nameList, valueList, subPixel);
			break;
		}

		case 2:
		{
			for (int subPixel = 0; subPixel < AAKERNEL_SIZE; subPixel++)
				PutTriangle_Shading(2, nameList, valueList, subPixel);
			break;
		}

		case 3:
		{
			for (int subPixel = 0; subPixel < AAKERNEL_SIZE; subPixel++)
				PutTriangle_Texture(3, nameList, valueList, subPixel);
			break;
		}
	}

	return GZ_SUCCESS;
}

void GzRender::PutTriangle_Texture(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel)
{
	GzCoord newPos[3];
	GzApplyTransform_Pos((GzCoord*)valueList[0], newPos);

	GzCoord norm_list[3];
	GzTextureIndex uv_list[3];
	GzCoord* norm_tmp = (GzCoord*)valueList[1];
	GzTextureIndex* uv_tmp = (GzTextureIndex*)valueList[2];

	if (tex_fun == 0)
	{
		PutTriangle_Shading(2, nameList, valueList, subPixel);
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		for (int d = 0; d < 3; d++)
		{
			norm_list[i][d] = norm_tmp[i][d];
		}

		// uv in perspective space
		uv_list[i][0] = UVImage2Perspective(uv_tmp[i][0], newPos[i][2]);
		uv_list[i][1] = UVImage2Perspective(uv_tmp[i][1], newPos[i][2]);

	}

	for (int i = 0; i < 3; i++)
	{
		newPos[i][0] -= AAFilter[subPixel][0];
		newPos[i][1] -= AAFilter[subPixel][1];
	}

	if (interp_mode == GZ_COLOR)
	{
		//Gouraud shading
		// interpolate color
		GzColor color_list[3];
		

		for (int i = 0; i < 3; i++)
		{
			Vec3 curNorm = GzApplyTransform_Norm(norm_list[i]); 
			GzColor constK = { 1.0, 1.0, 1.0 };
			Vec3 curColor = GzCalcColor(Vec3(newPos[i]), curNorm, constK, constK, constK);
			for (int d = 0; d < 3; d++)
				color_list[i][d] = curColor.v[d];
		}

		LEE* lee = new LEE(newPos, color_list, NULL, uv_list, GZ_COLOR, xres, yres);

		for (int i = lee->xmin; i <= lee->xmax; i++)
		{
			for (int j = lee->ymin; j <= lee->ymax; j++)
			{
				if (lee->InsideTriangle(i, j))
				{
					float z = lee->InterpolateDepth(i, j);
					if ((float)(pixelbufferlist[subPixel][ARRAY(i, j)].z) >= z)
					{
						float* newColor = lee->InterpolateAttrib(i, j);

						float* tmpUV = lee->InterpolateUV(i, j); // perspective
						float image_u = UVPerspective2Image(tmpUV[0], z);
						float image_v = UVPerspective2Image(tmpUV[1], z);
						GzColor curK;
						tex_fun(image_u, image_v, curK);

						GzIntensity r = ctoi(newColor[0] * curK[0]);
						GzIntensity g = ctoi(newColor[1] * curK[1]);
						GzIntensity b = ctoi(newColor[2] * curK[2]);

						GzPut(i, j, r, g, b, 1, (GzDepth)z, subPixel);
					}
				}
			}
		}

		delete lee;
	}
	else if (interp_mode == GZ_NORMALS)
	{
		// Phong shading
		// interpolate normal
		for (int i = 0; i < 3; i++)
		{
			Vec3 curNorm = GzApplyTransform_Norm(norm_list[i]);
			for (int d = 0; d < 3; d++)
				norm_list[i][d] = curNorm.v[d];
		}
		LEE* lee = new LEE(newPos, NULL, norm_list, uv_list, GZ_NORMALS, xres, yres);
		for (int i = lee->xmin; i <= lee->xmax; i++)
		{
			for (int j = lee->ymin; j <= lee->ymax; j++)
			{
				if (lee->InsideTriangle(i, j))
				{
					float z = lee->InterpolateDepth(i, j);

					if ((float)(pixelbufferlist[subPixel][ARRAY(i, j)].z) >= z)
					{
						
						float* tmpNorm = lee->InterpolateAttrib(i, j);

						float* tmpUV = lee->InterpolateUV(i, j); // perspective

						float image_u = UVPerspective2Image(tmpUV[0], z);
						float image_v = UVPerspective2Image(tmpUV[1], z);

						GzColor curK;
						tex_fun(image_u, image_v, curK);

						Vec3 curColor = GzCalcColor(Vec3(newPos[i]), Vec3(tmpNorm), curK, curK, Ks);

						GzIntensity r = ctoi(curColor.v[0]);
						GzIntensity g = ctoi(curColor.v[1]);
						GzIntensity b = ctoi(curColor.v[2]);
						GzPut(i, j, r, g, b, 1, (GzDepth)z, subPixel);
					}
				}
			}
		}
		delete lee;
	}
}

void GzRender::PutTriangle_Flat(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel)
{
	GzCoord newPos[3];
	GzApplyTransform_Pos((GzCoord*)valueList[0], newPos);
	for (int i = 0; i < 3; i++)
	{
		newPos[i][0] -= AAFilter[subPixel][0];
		newPos[i][1] -= AAFilter[subPixel][1];
	}
	LEE* lee = new LEE(newPos, NULL, NULL, NULL, -1, xres, yres);

	// pixels in bounding box
	for (int i = lee->xmin; i <= lee->xmax; i++)
	{
		for (int j = lee->ymin; j <= lee->ymax; j++)
		{
			if (lee->InsideTriangle(i, j))
			{
				float z = lee->InterpolateDepth(i, j);
				if ((float)(pixelbufferlist[subPixel][ARRAY(i, j)].z) >= z)
				{
					GzIntensity r = ctoi(flatcolor[0]);
					GzIntensity g = ctoi(flatcolor[1]);
					GzIntensity b = ctoi(flatcolor[2]);
					GzPut(i, j, r, g, b, 1, (GzDepth)z, subPixel);
				}
			}
		}
	}
	delete lee;
}

void GzRender::PutTriangle_Shading(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel)
{
	GzCoord newPos[3];
	GzApplyTransform_Pos((GzCoord*)valueList[0], newPos);

	GzCoord norm_list[3];
	GzCoord* tmp = (GzCoord*)valueList[1];
	for (int i = 0; i < 3; i++)
	{
		for (int d = 0; d < 3; d++)
			norm_list[i][d] = tmp[i][d];
	}

	for (int i = 0; i < 3; i++)
	{
		newPos[i][0] -= AAFilter[subPixel][0];
		newPos[i][1] -= AAFilter[subPixel][1];
	}

	if (interp_mode == GZ_COLOR)
	{
		// interpolate color
		GzColor color_list[3];

		for (int i = 0; i < 3; i++)
		{
			Vec3 curNorm = GzApplyTransform_Norm(norm_list[i]);
			Vec3 curColor = GzCalcColor(Vec3(newPos[i]), curNorm, Ka, Kd, Ks);
			for (int d = 0; d < 3; d++)
				color_list[i][d] = curColor.v[d];
		}

		LEE* lee = new LEE(newPos, color_list, NULL, NULL, GZ_COLOR, xres, yres);

		for (int i = lee->xmin; i <= lee->xmax; i++)
		{
			for (int j = lee->ymin; j <= lee->ymax; j++)
			{
				if (lee->InsideTriangle(i, j))
				{
					float z = lee->InterpolateDepth(i, j);
					if ((float)(pixelbufferlist[subPixel][ARRAY(i, j)].z) >= z)
					{
						float* newColor = lee->InterpolateAttrib(i, j);
						GzIntensity r = ctoi(newColor[0]);
						GzIntensity g = ctoi(newColor[1]);
						GzIntensity b = ctoi(newColor[2]);
						GzPut(i, j, r, g, b, 1, (GzDepth)z, subPixel);
					}
				}
			}
		}

		delete lee;
	}

	else if (interp_mode == GZ_NORMALS)
	{

		for (int i = 0; i < 3; i++)
		{
			Vec3 curNorm = GzApplyTransform_Norm(norm_list[i]);
			for (int d = 0; d < 3; d++)
				norm_list[i][d] = curNorm.v[d];
		}
		LEE* lee = new LEE(newPos, NULL, norm_list, NULL, GZ_NORMALS, xres, yres);
		for (int i = lee->xmin; i <= lee->xmax; i++)
		{
			for (int j = lee->ymin; j <= lee->ymax; j++)
			{
				if (lee->InsideTriangle(i, j))
				{
					float z = lee->InterpolateDepth(i, j);

					if ((float)(pixelbufferlist[subPixel][ARRAY(i, j)].z) >= z)
					{
						float* tmpNorm = lee->InterpolateAttrib(i, j);
						Vec3 curColor = GzCalcColor(Vec3(newPos[i]), Vec3(tmpNorm), Ka, Kd, Ks);

						GzIntensity r = ctoi(curColor.v[0]);
						GzIntensity g = ctoi(curColor.v[1]);
						GzIntensity b = ctoi(curColor.v[2]);
						GzPut(i, j, r, g, b, 1, (GzDepth)z, subPixel);
					}
				}
			}
		}
		delete lee;
	}
}

void GzRender::GzApplyTransform_Pos(GzCoord *org, GzCoord *ans)
{
	int a = 0;
	Mat4 trans = Mat4(Ximage[matlevel]);

	for (int i = 0; i < 3; i++)
	{
		Mat4 after = trans * Vec3(org[i]);
		for (int j = 0; j < 3; j++)
		{
			ans[i][j] = after.m[j][0] / after.m[3][0];
		}
	}

	return;
}


Vec3 GzRender::GzApplyTransform_Norm(float *org)
{
	int a = 0;
	Mat4 trans = Mat4(Xnorm[matlevel]);

	Mat4 after = trans * Vec3(org);
	Vec3 ans = Vec3();
	for (int j = 0; j < 3; j++)
	{
		ans.v[j] = after.m[j][0] / after.m[3][0];
	}

	return ans.normlize();
}

Vec3 GzRender::GzCalcColor(Vec3 pos, Vec3 norm, float *ambient_coeff, float *diffuse_coeff, float *specular_coeff)
{
	Vec3 ambient, diffuse, specular;

	Vec3 color = Vec3();
	Vec3 eyeDir = Vec3(0.0, 0.0, -1.0);

	// ambient color
	ambient += Vec3(ambient_coeff) % Vec3(ambientlight.color);

	for (int i = 0; i < numlights; i++)
	{
		// diffuse color
		// reflectDir, eyeDir in image space
		Vec3 reflectDir = GzReflect(Vec3(lights[i].direction), Vec3(norm));

		float nl = norm.dot(Vec3(lights[i].direction));
		float ne = norm.dot(eyeDir);

		if (nl < 0 && ne < 0)
		{
			diffuse -= Vec3(lights[i].color) * nl;
		}
		else if (nl > 0 && ne > 0)
		{
			diffuse += Vec3(lights[i].color) * nl;
		}

		// specular color
		float re = reflectDir.dot(eyeDir);
		if (re > 0)
		{
			Vec3 tmp = Vec3(lights[i].color) * pow(re, spec);
			for (int i = 0; i < 3; i++)
			{
				tmp.v[i] = CLAMP(tmp.v[i], 0.0f, 1.0f);
			}
			specular += tmp;
		}

	}

	diffuse = diffuse % Vec3(diffuse_coeff);
	specular = specular % Vec3(specular_coeff);

	for (int d = 0; d < 3; d++)
	{
		diffuse.v[d] = CLAMP(diffuse.v[d], 0.0f, 1.0f);
		specular.v[d] = CLAMP(specular.v[d], 0.0f, 1.0f);
	}

	color = ambient + diffuse + specular;
	return color;
}

Vec3 GzRender::GzReflect(const Vec3 lightDir, const Vec3 norm)
{
	return (norm * (2 * lightDir.dot(norm)) - lightDir).normlize();
}

float GzRender::UVImage2Perspective(float val, float vs)
{

	float vz = vs / ((float)INT_MAX - vs);
	return  (val / (vz + 1.0));
}

float GzRender::UVPerspective2Image(float val, float vs)
{
	float vz = vs / ((float)INT_MAX - vs);

	return (val * (vz + 1.0));
}