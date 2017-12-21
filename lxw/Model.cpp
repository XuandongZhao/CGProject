#include"Model.h"

enum { X, Y, Z, W };

static char* DirName(char* path)
{
	char* dir;
	char* s;

	dir = strdup(path);

	s = strrchr(dir, '/');
	if (s)
		s[1] = '\0';
	else
		dir[0] = '\0';

	return dir;
}

GLvoid Model::ReadMTL(char* name)
{
	FILE* file;
	char* dir;
	char* filename;
	char  buf[128];
	GLuint nummaterials, i;

	dir = DirName(this->pathname);
	filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
	strcpy(filename, dir);
	strcat(filename, name);
	free(dir);

	/* open the file */
	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "_glmReadMTL() failed: can't open material file \"%s\".\n",filename);
		exit(1);
	}
	free(filename);

	/* count the number of materials in the file */
	nummaterials = 1;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':				/* comment */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':				/* newmtl */
			fgets(buf, sizeof(buf), file);
			nummaterials++;
			sscanf(buf, "%s %s", buf, buf);
			break;
		default:
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	rewind(file);

	/* allocate memory for the materials */
	this->materials = (Material*)malloc(sizeof(Material) * nummaterials);
	this->nummaterials = nummaterials;

	/* set the default material */
	for (i = 0; i < nummaterials; i++) {
		this->materials[i].name = NULL;
		this->materials[i].shininess = 0;
		this->materials[i].diffuse[0] = 0.8;
		this->materials[i].diffuse[1] = 0.8;
		this->materials[i].diffuse[2] = 0.8;
		this->materials[i].diffuse[3] = 1.0;
		this->materials[i].ambient[0] = 0.2;
		this->materials[i].ambient[1] = 0.2;
		this->materials[i].ambient[2] = 0.2;
		this->materials[i].ambient[3] = 1.0;
		this->materials[i].specular[0] = 0.0;
		this->materials[i].specular[1] = 0.0;
		this->materials[i].specular[2] = 0.0;
		this->materials[i].specular[3] = 1.0;
	}
	this->materials[0].name = strdup("default");

	/* now, read in the data */
	nummaterials = 0;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':				/* comment */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':				/* newmtl */
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
			nummaterials++;
			this->materials[nummaterials].name = strdup(buf);
			break;
		case 'N':
			fscanf(file, "%f", &this->materials[nummaterials].shininess);
			/* wavefront shininess is from [0, 1000], so scale for OpenGL */
			this->materials[nummaterials].shininess /= 1000.0;
			this->materials[nummaterials].shininess *= 128.0;
			break;
		case 'K':
			switch (buf[1]) {
			case 'd':
				fscanf(file, "%f %f %f",
					&this->materials[nummaterials].diffuse[0],
					&this->materials[nummaterials].diffuse[1],
					&this->materials[nummaterials].diffuse[2]);
				break;
			case 's':
				fscanf(file, "%f %f %f",
					&this->materials[nummaterials].specular[0],
					&this->materials[nummaterials].specular[1],
					&this->materials[nummaterials].specular[2]);
				break;
			case 'a':
				fscanf(file, "%f %f %f",
					&this->materials[nummaterials].ambient[0],
					&this->materials[nummaterials].ambient[1],
					&this->materials[nummaterials].ambient[2]);
				break;
			default:
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				break;
			}
			break;
		case 'm':
			fscanf(file, "%s", tex.texfilename);
			tex.isTex = true;
			break;
		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}
}

Group* Model::FindGroup(char* name)
{
	Group* group;

	//assert(model);

	group = this->groups;
	while (group) {
		if (!strcmp(name, group->name))
			break;
		group = group->next;
	}

	return group;
}

Group* Model::AddGroup(char* name)
{
	Group* group;

	group = FindGroup(name);
	if (!group) {
		group = (Group*)malloc(sizeof(Group));
		group->name = strdup(name);
		group->material = 0;
		group->numtriangles = 0;
		group->triangles = NULL;
		group->next = this->groups;
		this->groups = group;
		this->numgroups++;
	}
	return group;
}

GLvoid Model::FirstPass(FILE* file)
{
	Group*    group;			/* current group */
	unsigned  v, n, t;
	char      buf[128];

	/* make a default group */
	group = AddGroup("default");
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) 
		{
			case '#':
			{
				fgets(buf, sizeof(buf), file);
				break;
			}
			case 'v':/* v, vn, vt */
			{
				switch (buf[1]) 
				{
					case '\0':			/* vertex */
						fgets(buf, sizeof(buf), file);
						numvertices++;
						break;
					case 'n':				/* normal */
						fgets(buf, sizeof(buf), file);
						numnormals++;
						break;
					case 't':				/* texcoord */
						fgets(buf, sizeof(buf), file);
						numtexcoords++;
						break;
					default:
						printf("FirstPass(): Unknown token \"%s\".\n", buf);
						exit(1);
						break;
				}
				break;
			}
			case 'm':
			{
				fgets(buf, sizeof(buf), file);
				sscanf(buf, "%s %s", buf, buf);
				this->mtllibname = strdup(buf);
				ReadMTL(buf);
				if (tex.isTex) {
					glGenTextures(1, &(tex.texture));
					texload();
				}
				break;
			}
			case 'u':
			{
				fgets(buf, sizeof(buf), file);
				break;
			}
			case 'g':				/* group */
			{
				fgets(buf, sizeof(buf), file);
				sscanf(buf, "%s", buf);
				group = AddGroup(buf);
				break;
			}
			case 'f':				/* face */
			{
				v = n = t = 0;
				fscanf(file, "%s", buf);
				/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
				if (strstr(buf, "//")) {
					/* v//n */
					sscanf(buf, "%d//%d", &v, &n);
					fscanf(file, "%d//%d", &v, &n);
					fscanf(file, "%d//%d", &v, &n);
					numtriangles++;
					group->numtriangles++;
					while (fscanf(file, "%d//%d", &v, &n) > 0) {
						numtriangles++;
						group->numtriangles++;
					}
				}
				else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
					/* v/t/n */
					fscanf(file, "%d/%d/%d", &v, &t, &n);
					fscanf(file, "%d/%d/%d", &v, &t, &n);
					numtriangles++;
					group->numtriangles++;
					while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
						numtriangles++;
						group->numtriangles++;
					}
				}
				else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
					/* v/t */
					fscanf(file, "%d/%d", &v, &t);
					fscanf(file, "%d/%d", &v, &t);
					numtriangles++;
					group->numtriangles++;
					while (fscanf(file, "%d/%d", &v, &t) > 0) {
						numtriangles++;
						group->numtriangles++;
					}
				}
				else {
					/* v */
					fscanf(file, "%d", &v);
					fscanf(file, "%d", &v);
					numtriangles++;
					group->numtriangles++;
					while (fscanf(file, "%d", &v) > 0) {
						numtriangles++;
						group->numtriangles++;
					}
				}
				break;
			}
			default:
			{
				fgets(buf, sizeof(buf), file);
				break;
			}
		}
	}
	/* allocate memory for the triangles in each group */
	group = this->groups;
	while (group) {
		group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
		group->numtriangles = 0;
		group = group->next;
	}
};

GLvoid Model::ReadOBJ(char* filename)
{
	
	FILE*     file;
	/* open the file */
	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "ReadOBJ() failed: can't open data file \"%s\".\n",filename);
		exit(1);
	}
	
	/* allocate a new model */
	this->pathname = (char*)malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(this->pathname, filename);
	/* make a first pass through the file to get a count of the number
	of vertices, normals, texcoords & triangles */
	FirstPass(file);
	/* allocate memory */
	this->vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (this->numvertices + 1));
	this->triangles = (Triangle*)malloc(sizeof(Triangle) * this->numtriangles);
	if (this->numnormals) {
		this->normals = (GLfloat*)malloc(sizeof(GLfloat) *3 * (this->numnormals + 1));
	}
	if (this->numtexcoords) {
		this->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *2 * (this->numtexcoords + 1));
	}
	PrintInfo();
	/* rewind to beginning of file and read in the data this pass */
	rewind(file);
	SecondPass(file);
	/* close the file */
	fclose(file);
}

void Model::PrintInfo()
{
	cout << "Model path: " << pathname << endl;
	cout << "material lib name: " << mtllibname << endl;
	cout << "vertex number: "<<numvertices << endl;
	cout << "normal number: "<<numnormals << endl;
	cout << "texture coord numbers: "<<numtexcoords << endl;
	cout << "triangle numbers: "<<numtriangles << endl;
	cout << "material numbers: "<<nummaterials << endl;
	cout << "face group numbers: "<<numgroups << endl;
}

GLuint Model::FindMaterial(char* name)
{
	GLuint i;
	for (i = 0; i < this->nummaterials; i++) {
		if (!strcmp(this->materials[i].name, name))
			return i;
	}
	/* didn't find the name, so set it as the default material */
	printf("FindMaterial():  can't find material \"%s\".\n", name);
	return 0;
}

GLvoid Model::SecondPass(FILE* file)
{
	GLuint    numvertices;			/* number of vertices in model */
	GLuint    numnormals;			/* number of normals in model */
	GLuint    numtexcoords;		/* number of texcoords in model */
	GLuint    numtriangles;		/* number of triangles in model */

	Group* group;			/* current group pointer */
	GLuint    material;			/* current material */
	GLuint    v, n, t;
	char      buf[128];

	/* set the pointer shortcuts */
	group = this->groups;

	/* on the second pass through the file, read all the data into the
	allocated arrays */
	numvertices = numnormals = numtexcoords = 1;
	numtriangles = 0;
	material = 0;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':				/* comment */
			fgets(buf, sizeof(buf), file);
			break;
		case 'v':				/* v, vn, vt */
			switch (buf[1]) {
			case '\0':			/* vertex */
				fscanf(file, "%f %f %f",
					&vertices[3 * numvertices + X],
					&vertices[3 * numvertices + Y],
					&vertices[3 * numvertices + Z]);
				numvertices++;
				break;
			case 'n':				/* normal */
				fscanf(file, "%f %f %f",
					&normals[3 * numnormals + X],
					&normals[3 * numnormals + Y],
					&normals[3 * numnormals + Z]);
				numnormals++;
				break;
			case 't':				/* texcoord */
				fscanf(file, "%f %f",
					&texcoords[2 * numtexcoords + X],
					&texcoords[2 * numtexcoords + Y]);
				numtexcoords++;
				break;
			}
			break;
		case 'u':
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
			group->material = material = FindMaterial(buf);
			break;
		case 'g':				/* group */
								/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s", buf);
			group = FindGroup(buf);
			group->material = material;
			break;
		case 'f':				/* face */
			v = n = t = 0;
			fscanf(file, "%s", buf);
			/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
			if (strstr(buf, "//")) {
				/* v//n */
				sscanf(buf, "%d//%d", &v, &n);
				this->triangles[numtriangles].v[0] = v;
				this->triangles[numtriangles].vn[0] = n;
				fscanf(file, "%d//%d", &v, &n);
				this->triangles[numtriangles].v[1] = v;
				this->triangles[numtriangles].vn[1] = n;
				fscanf(file, "%d//%d", &v, &n);
				this->triangles[numtriangles].v[2] = v;
				this->triangles[numtriangles].vn[2] = n;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d//%d", &v, &n) > 0) {
					this->triangles[numtriangles].v[0] = this->triangles[numtriangles - 1].v[0];
					this->triangles[numtriangles].vn[0] = this->triangles[numtriangles - 1].vn[0];
					this->triangles[numtriangles].v[1] = this->triangles[numtriangles - 1].v[2];
					this->triangles[numtriangles].vn[1] = this->triangles[numtriangles - 1].vn[2];
					this->triangles[numtriangles].v[2] = v;
					this->triangles[numtriangles].vn[2] = n;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
				/* v/t/n */
				this->triangles[numtriangles].v[0] = v;
				this->triangles[numtriangles].vt[0] = t;
				this->triangles[numtriangles].vn[0] = n;
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				this->triangles[numtriangles].v[1] = v;
				this->triangles[numtriangles].vt[1] = t;
				this->triangles[numtriangles].vn[1] = n;
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				this->triangles[numtriangles].v[2] = v;
				this->triangles[numtriangles].vt[2] = t;
				this->triangles[numtriangles].vn[2] = n;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
					this->triangles[numtriangles].v[0] = this->triangles[numtriangles-1].v[0];
					this->triangles[numtriangles].vt[0] = this->triangles[numtriangles-1].vt[0];
					this->triangles[numtriangles].vn[0] = this->triangles[numtriangles-1].vn[0];
					this->triangles[numtriangles].v[1] = this->triangles[numtriangles-1].v[2];
					this->triangles[numtriangles].vt[1] = this->triangles[numtriangles-1].vt[2];
					this->triangles[numtriangles].vn[1] = this->triangles[numtriangles-1].vn[2];
					this->triangles[numtriangles].v[2] = v;
					this->triangles[numtriangles].vt[2] = t;
					this->triangles[numtriangles].vn[2] = n;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
				/* v/t */
				this->triangles[numtriangles].v[0] = v;
				this->triangles[numtriangles].vt[0] = t;
				fscanf(file, "%d/%d", &v, &t);
				this->triangles[numtriangles].v[1] = v;
				this->triangles[numtriangles].vt[1] = t;
				fscanf(file, "%d/%d", &v, &t);
				this->triangles[numtriangles].v[2] = v;
				this->triangles[numtriangles].vt[2] = t;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d/%d", &v, &t) > 0) {
					this->triangles[numtriangles].v[0] = this->triangles[numtriangles - 1].v[0];
					this->triangles[numtriangles].vt[0] = this->triangles[numtriangles - 1].vt[0];
					this->triangles[numtriangles].v[1] = this->triangles[numtriangles - 1].v[2];
					this->triangles[numtriangles].vt[1] = this->triangles[numtriangles - 1].vt[2];
					this->triangles[numtriangles].v[2] = v;
					this->triangles[numtriangles].vt[2] = t;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else {
				/* v */
				sscanf(buf, "%d", &v);
				this->triangles[numtriangles].v[0] = v;
				fscanf(file, "%d", &v);
				this->triangles[numtriangles].v[1] = v;
				fscanf(file, "%d", &v);
				this->triangles[numtriangles].v[2] = v;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d", &v) > 0) {
					this->triangles[numtriangles].v[0] = this->triangles[numtriangles - 1].v[0];
					this->triangles[numtriangles].v[1] = this->triangles[numtriangles - 1].v[2];
					this->triangles[numtriangles].v[2] = v;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			break;

		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}
	
}

//GLvoid
//Model::Draw(GLuint mode)
//{
//	GLuint i;
//	Group* group;
//	Model* model = this;
//	assert(model);
//	assert(model->vertices);
//
//	/* do a bit of warning */
//	if (mode & GLM_FLAT && !model->facetnorms) {
//		printf("glmDraw() warning: flat render mode requested "
//			"with no facet normals defined.\n");
//		mode &= ~GLM_FLAT;
//	}
//	if (mode & GLM_SMOOTH && !model->normals) {
//		printf("glmDraw() warning: smooth render mode requested "
//			"with no normals defined.\n");
//		mode &= ~GLM_SMOOTH;
//	}
//	if (mode & GLM_TEXTURE && !model->texcoords) {
//		printf("glmDraw() warning: texture render mode requested "
//			"with no texture coordinates defined.\n");
//		mode &= ~GLM_TEXTURE;
//	}
//	if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
//		printf("glmDraw() warning: flat render mode requested "
//			"and smooth render mode requested (using smooth).\n");
//		mode &= ~GLM_FLAT;
//	}
//	if (mode & GLM_COLOR && !model->materials) {
//		printf("glmDraw() warning: color render mode requested "
//			"with no materials defined.\n");
//		mode &= ~GLM_COLOR;
//	}
//	if (mode & GLM_MATERIAL && !model->materials) {
//		printf("glmDraw() warning: material render mode requested "
//			"with no materials defined.\n");
//		mode &= ~GLM_MATERIAL;
//	}
//	if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
//		printf("glmDraw() warning: color and material render mode requested "
//			"using only material mode\n");
//		mode &= ~GLM_COLOR;
//	}
//	if (mode & GLM_COLOR)
//		glEnable(GL_COLOR_MATERIAL);
//	if (mode & GLM_MATERIAL)
//		glDisable(GL_COLOR_MATERIAL);
//
//	glPushMatrix();
//	//glEnable(GL_TEXTURE_2D);
//	//glBindTexture(GL_TEXTURE_2D, GLuint texture);
//	glTranslatef(model->position[0], model->position[1], model->position[2]);
//
//	glBegin(GL_TRIANGLES);
//	group = model->groups;
//	while (group) {
//		if (mode & GLM_MATERIAL) {
//			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
//				model->materials[group->material].ambient);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
//				model->materials[group->material].diffuse);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
//				model->materials[group->material].specular);
//			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
//				model->materials[group->material].shininess);
//		}
//
//		if (mode & GLM_COLOR) {
//			glColor3fv(model->materials[group->material].diffuse);
//		}
//
//		for (i = 0; i < group->numtriangles; i++) {
//			if (mode & GLM_FLAT)
//				glNormal3fv(&model->facetnorms[3 * T(group->triangles[i]).f]);
//
//			if (mode & GLM_SMOOTH)
//				glNormal3fv(&model->normals[3 * T(group->triangles[i]).vn[0]]);
//			if (mode & GLM_TEXTURE)
//				glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).vt[0]]);
//			glVertex3fv(&model->vertices[3 * T(group->triangles[i]).v[0]]);
//#if 0
//			printf("%f %f %f\n",
//				model->vertices[3 * T(group->triangles[i]).v[0] + X],
//				model->vertices[3 * T(group->triangles[i]).v[0] + Y],
//				model->vertices[3 * T(group->triangles[i]).v[0] + Z]);
//#endif
//
//			if (mode & GLM_SMOOTH)
//				glNormal3fv(&model->normals[3 * T(group->triangles[i]).vn[1]]);
//			if (mode & GLM_TEXTURE)
//				glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).vt[1]]);
//			glVertex3fv(&model->vertices[3 * T(group->triangles[i]).v[1]]);
//#if 0
//			printf("%f %f %f\n",
//				model->vertices[3 * T(group->triangles[i]).v[1] + X],
//				model->vertices[3 * T(group->triangles[i]).v[1] + Y],
//				model->vertices[3 * T(group->triangles[i]).v[1] + Z]);
//#endif
//
//			if (mode & GLM_SMOOTH)
//				glNormal3fv(&model->normals[3 * T(group->triangles[i]).vn[2]]);
//			if (mode & GLM_TEXTURE)
//				glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).vt[2]]);
//			glVertex3fv(&model->vertices[3 * T(group->triangles[i]).v[2]]);
//#if 0
//			printf("%f %f %f\n",
//				model->vertices[3 * T(group->triangles[i]).v[2] + X],
//				model->vertices[3 * T(group->triangles[i]).v[2] + Y],
//				model->vertices[3 * T(group->triangles[i]).v[2] + Z]);
//#endif
//
//		}
//
//		group = group->next;
//	}
//	glEnd();
//
//	glPopMatrix();
//}

unsigned char * Model::LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char	*bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量
	char temName[100] = { 0 };
	strcat(temName, "obj/");
	strcat(temName, filename);
								// 以“二进制+读”模式打开文件filename 
	filePtr = fopen(temName, "rb");
	if (filePtr == NULL) {
		printf("Open Error!\n");
		return NULL;
	}
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}

GLvoid Model::texload() {
	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap信息头
	unsigned char*   bitmapData;                                       // 纹理数据
	unsigned char* temB;
	bitmapData = LoadBitmapFile(tex.texfilename, &bitmapInfoHeader);
	unsigned char* tem;
	BITMAPINFOHEADER bitmapInfoHeaderTem;
	glBindTexture(GL_TEXTURE_2D, tex.texture);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap层次(通常为，表示最上层) 
		GL_RGB,	//我们希望该纹理有红、绿、蓝数据
		bitmapInfoHeader.biWidth, //纹理宽带，必须是n，若有边框+2 
		bitmapInfoHeader.biHeight, //纹理高度，必须是n，若有边框+2 
		0, //边框(0=无边框, 1=有边框) 
		GL_RGB,	//bitmap数据的格式
		GL_UNSIGNED_BYTE, //每个颜色数据的类型
		bitmapData);	//bitmap数据指针  
}

GLvoid Model::Draw()
{
	GLuint i;
	Group* group;
	int boundary[3][2] = {
		{ 0,0 },
		{ 1,0 },
		{ 0,1 }
	};
	if (tex.isTex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex.texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	glPushMatrix();
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, GLuint texture);
	//glEnable(GL_COLOR_MATERIAL);
	glTranslatef(this->position[0], this->position[1], this->position[2]);

	glBegin(GL_TRIANGLES);
	group = this->groups;
	while (group) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,this->materials[group->material].ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,this->materials[group->material].diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,this->materials[group->material].specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,this->materials[group->material].shininess);
		
		for (i = 0; i < group->numtriangles; i++) {
			glNormal3fv(&this->normals[3 * triangles[group->triangles[i]].vn[0]]);
			glTexCoord2fv(&this->texcoords[2 * triangles[group->triangles[i]].vt[0]]);
			glVertex3fv(&this->vertices[3 * triangles[group->triangles[i]].v[0]]);
			glNormal3fv(&this->normals[3 * triangles[group->triangles[i]].vn[1]]);
			glTexCoord2fv(&this->texcoords[2 * triangles[group->triangles[i]].vt[1]]);
			glVertex3fv(&this->vertices[3 * triangles[group->triangles[i]].v[1]]);
			glNormal3fv(&this->normals[3 * triangles[group->triangles[i]].vn[2]]);
			glTexCoord2fv(&this->texcoords[2 * triangles[group->triangles[i]].vt[2]]);
			glVertex3fv(&this->vertices[3 * triangles[group->triangles[i]].v[2]]);

		}

		group = group->next;
	}

	glEnd();

	glPopMatrix();
	if(tex.isTex)
		glDisable(GL_TEXTURE_2D);
}

GLfloat Model::Unitize()
{
	GLuint  i;
	GLfloat maxx, minx, maxy, miny, maxz, minz;
	GLfloat cx, cy, cz, w, h, d;
	GLfloat scale;

	/* get the max/mins */
	maxx = minx = this->vertices[3 + X];
	maxy = miny = this->vertices[3 + Y];
	maxz = minz = this->vertices[3 + Z];
	for (i = 1; i <= this->numvertices; i++) {
		if (maxx < this->vertices[3 * i + X])
			maxx = this->vertices[3 * i + X];
		if (minx > this->vertices[3 * i + X])
			minx = this->vertices[3 * i + X];

		if (maxy < this->vertices[3 * i + Y])
			maxy = this->vertices[3 * i + Y];
		if (miny > this->vertices[3 * i + Y])
			miny = this->vertices[3 * i + Y];

		if (maxz < this->vertices[3 * i + Z])
			maxz = this->vertices[3 * i + Z];
		if (minz > this->vertices[3 * i + Z])
			minz = this->vertices[3 * i + Z];
	}

	/* calculate model width, height, and depth */
	w = maxx - minx;
	h = maxy - miny;
	d = maxz - minz;

	/* calculate center of the model */
	cx = (maxx + minx) / 2.0;
	cy = (maxy + miny) / 2.0;
	cz = (maxz + minz) / 2.0;

	GLfloat max;
	if (w > h)
	{
		if (w > d)
			max = w;
		else
			max = d;
	}
	else
	{
		if (h > d)
			max = h;
		else
			max = d;
	}
	/* calculate unitizing scale factor */
	scale = 2.0 / max;

	/* translate around center then scale */
	for (i = 1; i <= this->numvertices; i++) {
		this->vertices[3 * i + X] -= cx;
		this->vertices[3 * i + Y] -= cy;
		this->vertices[3 * i + Z] -= cz;
		this->vertices[3 * i + X] *= scale;
		this->vertices[3 * i + Y] *= scale;
		this->vertices[3 * i + Z] *= scale;
	}

	return scale;
}