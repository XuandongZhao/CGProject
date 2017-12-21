#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<gl/glut.h>
#include <assert.h>
/* defines */
#define GLM_NONE     (0)		/* render with only vertices */
#define GLM_FLAT     (1 << 0)		/* render with facet normals */
#define GLM_SMOOTH   (1 << 1)		/* render with vertex normals */
#define GLM_TEXTURE  (1 << 2)		/* render with texture coords */
#define GLM_COLOR    (1 << 3)		/* render with colors */
#define GLM_MATERIAL (1 << 4)		/* render with materials */
/* defines */
#define T(x) model->triangles[(x)]
#define BITMAP_ID 0x4D42
using namespace std;
//材质信息
typedef struct
{
	char* name;				/* name of material */
	GLfloat diffuse[4];			/* diffuse component */
	GLfloat ambient[4];			/* ambient component */
	GLfloat specular[4];			/* specular component */
	GLfloat emmissive[4];			/* emmissive component */
	GLfloat shininess;			/* specular exponent */
} Material;

//面的信息
typedef struct {
	GLuint v[3];			/* array of triangle vertex indices */
	GLuint vn[3];			/* array of triangle normal indices */
	GLuint vt[3];			/* array of triangle texcoord indices*/
	GLuint f;			/* index of triangle facet normal */
} Triangle;

//材质相同的多个面的信息
typedef struct My_Group{
	char*             name;		/* name of this group */
	GLuint            numtriangles;	/* number of triangles in this group */
	GLuint*           triangles;		/* array of triangle indices */
	GLuint            material;           /* index to material for group */
	struct My_Group*            next;		/* pointer to next group in model */
} Group;

typedef struct {
	char texfilename[100];
	bool isTex = false;
	GLuint texture;
} Tex;

class Model{
	public:
		char*    pathname;			/* path to this model */
		char*    mtllibname;			/* name of the material library */

		GLuint   numvertices;			/* number of vertices in model */
		GLfloat* vertices;			/* array of vertices  */

		GLuint   numnormals;			/* number of normals in model */
		GLfloat* normals;			/* array of normals */

		GLuint   numtexcoords;		/* number of texcoords in model */
		GLfloat* texcoords;			/* array of texture coordinates */

		GLuint   numfacetnorms;		/* number of facetnorms in model */
		GLfloat* facetnorms;			/* array of facetnorms */

		GLuint       numtriangles;		/* number of triangles in model */
		Triangle* triangles;		/* array of triangles */

		GLuint       nummaterials;		/* number of materials in model */
		Material* materials;		/* array of materials */

		GLuint       numgroups;		/* number of groups in model */
		Group*    groups;			/* linked list of groups */

		GLfloat position[3];			/* position of the model */
		Tex tex;
		Model() : pathname(NULL), mtllibname(NULL), numvertices(0), vertices(NULL), numnormals(0), normals(NULL), numtexcoords(0), texcoords(NULL), numfacetnorms(0), facetnorms(NULL),
				  numtriangles(0), triangles(NULL), nummaterials(0), materials(NULL), numgroups(0), groups(NULL)
		{
			position[0] = 0;
			position[1] = 0;
			position[2] = 0;
			printf("Model initialized!\n");
		}
		GLvoid ReadOBJ(char* filename);
		GLvoid Draw();
		GLfloat Unitize();
		unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	private:
		void PrintInfo();
		GLvoid FirstPass(FILE* file);
		Group* FindGroup(char* name);
		Group* AddGroup(char* name);
		GLvoid ReadMTL(char* name);
		GLvoid SecondPass(FILE* file);
		GLuint FindMaterial(char* name);
		GLvoid texload();
};