#include	"gz.h"
#include    "Operation.h"
#ifndef GZRENDER_
#define GZRENDER_


/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */
#define	AAKERNEL_SIZE	6

class GzRender{			/* define a renderer */
  

public:
	unsigned short	xres;
	unsigned short	yres;
	GzPixel		*pixelbuffer;		/* frame buffer array */
	GzPixel*	 pixelbufferlist[AAKERNEL_SIZE];		/* pixel buffer list */
	char* framebuffer;

	GzCamera		m_camera;
	short		    matlevel;	        /* top of stack - current xform */
	GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
	GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
	GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
	GzColor		flatcolor;          /* color state for flat shaded triangles */
	int			interp_mode;
	int			numlights;
	GzLight		lights[MAX_LIGHTS];
	GzLight		ambientlight;
	GzColor		Ka, Kd, Ks;
	float		    spec;		/* specular power */
	GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */

	float AAFilter[AAKERNEL_SIZE][3];

  	// Constructors
	GzRender(int xRes, int yRes);
	~GzRender();

	// Function declaration

	// HW1: Display methods
	int GzDefault();
	int GzBeginRender();
	int GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int subPixel);

	int GzFlushDisplay2File(FILE* outfile);
	int GzFlushDisplay2FrameBuffer();

	// HW2: Render methods
	int GzPutAttribute(int numAttributes, GzToken *nameList, GzPointer *valueList);
	int GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList);
	void GzApplyTransform_Pos(GzCoord *org, GzCoord *ans);
	Vec3 GzApplyTransform_Norm(float *org);

	// HW5: Flat, Shading, Texture
	void PutTriangle_Flat(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel);
	void PutTriangle_Shading(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel);
	void PutTriangle_Texture(int numParts, GzToken *nameList, GzPointer *valueList, int subPixel);
	float UVImage2Perspective(float val, float vs);
	float UVPerspective2Image(float val, float vs);

	void ApplyFilter();

	// HW3
	int GzPutCamera(GzCamera camera);
	int GzPushMatrix(GzMatrix matrix, bool ignoreNorm = true);
	int GzPopMatrix();

	// HW4: Color
	Vec3 GzCalcColor(Vec3 pos, Vec3 norm, float *ambient_coeff, float *diffuse_coeff, float *specular_coeff);
	Vec3 GzReflect(const Vec3 lightDir, const Vec3 norm);

	// Extra methods: NOT part of API - just for general assistance */
	inline int ARRAY(int x, int y){return (x+y*xres);}	/* simplify fbuf indexing */
	inline short	ctoi(float color) {return(short)((int)(color * ((1 << 12) - 1)));}		/* convert float color to GzIntensity short */
	template <class T> T CLAMP(T value, T low, T high);

	// Object Translation
	int GzRotXMat(float degree, GzMatrix mat);
	int GzRotYMat(float degree, GzMatrix mat);
	int GzRotZMat(float degree, GzMatrix mat);
	int GzTrxMat(GzCoord translate, GzMatrix mat);
	int GzScaleMat(GzCoord scale, GzMatrix mat);

};
#endif