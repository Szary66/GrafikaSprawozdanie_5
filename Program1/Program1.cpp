//Damian Moskała

//Program 1 - Piramida o podstawie 6-kąta i teksturze polerowanego złota z możliwością wyboru oświetlenia

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "materials.h"
#include "colors.h"
#include <iostream>

bool rescale_normal = false;

enum
{
	BRASS,
	BRONZE,
	POLISHED_BRONZE,
	CHROME,
	COPPER,
	POLISHED_COPPER,
	GOLD,
	POLISHED_GOLD,
	PEWTER,
	SILVER,
	POLISHED_SILVER,
	EMERALD,
	JADE,
	OBSIDIAN,
	PEARL,
	RUBY,
	TURQUOISE,
	BLACK_PLASTIC,
	BLACK_RUBBER,
	NORMALS_SMOOTH,
	NORMALS_FLAT,
	FULL_WINDOW,
	ASPECT_1_1,
	GLOBAL_LIGHT_OPTION,
	REFLECTOR_OPTION,
	POINT_LIGHT_OPTION,
	EXIT
};

int aspect = FULL_WINDOW;

const int N = 6;
const int VertexNumber = N + 2;
const int TrianglesNumber = N * 2;
GLfloat vertex[VertexNumber * 3];
GLint triangles[TrianglesNumber * 3];

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

const GLdouble left = -1.0;
const GLdouble right = 1.0;
const GLdouble bottom = -1.0;
const GLdouble top = 1.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;
const GLfloat *ambient = PolishedGoldAmbient;
const GLfloat *diffuse = PolishedGoldDiffuse;
const GLfloat *specular = PolishedGoldSpecular;
GLfloat shininess = PolishedGoldShininess;
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;
GLint LightMode = GLOBAL_LIGHT_OPTION;
int normals = NORMALS_FLAT;
int button_state = GLUT_UP;
int button_x, button_y;

GLfloat ambient_light[4] =
{
	0.2,0.2,0.2,1.0
};

GLfloat spot_direction[3] =
{
	0.0,0.0,-1.0
};

GLfloat spot_cutoff = 180.0;
GLfloat spot_exponent = 128.0;
GLfloat constant_attenuation = 1.0;
GLfloat linear_attenuation = 0.0;
GLfloat quadratic_attenuation = 0.0;

GLfloat light_position[4] =
{
	0.0,0.0,1.0,0.0
};

void GenerateVerticles(GLfloat* vertices, int Sides)
{
	for (auto i = 0; i < Sides; ++i)
	{
		vertices[i * 3 + 0] = sinf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
		vertices[i * 3 + 1] = -0.5f;
		vertices[i * 3 + 2] = cosf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
	}

	int l = Sides * 3;
	vertices[l + 0] = 0.f;
	vertices[l + 1] = -0.5f;
	vertices[l + 2] = 0.f;
	vertices[l + 3] = 0.f;
	vertices[l + 4] = 0.5f;
	vertices[l + 5] = 0.f;
}

void GenerateTriangles(GLint* triangles, int Sides)
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < Sides; ++j)
		{
			triangles[i*Sides * 3 + j * 3] = j;
			triangles[i*Sides * 3 + j * 3 + 1] = (j + 1) % Sides;
			triangles[i*Sides * 3 + j * 3 + 2] = Sides + i;
		}
	for (int i = 0; i < Sides * 2; ++i)
	{
		std::cout << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << std::endl;
	}
}

void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void Normalize(GLfloat *v)
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (d)
	{
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

GLfloat scale = 1.0;

void Display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -(near + far) / 2);
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);
	glScalef(scale, scale, scale);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		glEnable(GL_LIGHT0);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
	}
	else if (LightMode == REFLECTOR_OPTION)
	{
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant_attenuation);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linear_attenuation);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0, 0, -(near + far) / 2);
		glRotatef(light_rotatex, 1.0, 0, 0);
		glRotatef(light_rotatey, 0, 1.0, 0);
		glTranslatef(light_position[0], light_position[1], light_position[2]);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHT0);
		glMaterialfv(GL_FRONT, GL_EMISSION, Red);
		glutSolidSphere(0.1, 30, 20);
		glPopAttrib();
		glPopMatrix();
	}
	else if (LightMode == POINT_LIGHT_OPTION)
	{
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glRotatef(light_rotatex, 1.0, 0, 0);
		glRotatef(light_rotatey, 0, 1.0, 0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	if (rescale_normal == true)
	{
		glEnable(GL_RESCALE_NORMAL);
	}
	else
	{
		glEnable(GL_NORMALIZE);
	}

	glBegin(GL_TRIANGLES);

	if (normals == NORMALS_SMOOTH)
	{
		for (int i = 0; i < TrianglesNumber; i++)
		{
			GLfloat n[3];
			n[0] = n[1] = n[2] = 0.0;

			for (int j = 0; j < TrianglesNumber; j++)
			{
				if (3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 2])
				{
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}
			}

			if (rescale_normal == true)
			{
				Normalize(n);
			}

			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
			n[0] = n[1] = n[2] = 0.0;

			for (int j = 0; j < TrianglesNumber; j++)
			{
				if (3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 2])
				{
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}
			}

			if (rescale_normal == true)
			{
				Normalize(n);
			}

			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
			n[0] = n[1] = n[2] = 0.0;

			for (int j = 0; j < TrianglesNumber; j++)
			{
				if (3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 2])
				{
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}
			}

			if (rescale_normal == true)
			{
				Normalize(n);
			}

			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}
	}
	else
	{
		for (int i = 0; i < TrianglesNumber; i++)
		{
			GLfloat n[3];
			Normal(n, i);

			if (rescale_normal == true)
			{
				Normalize(n);
			}

			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}
	}

	glEnd();

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_NORMALIZE);
		glDisable(GL_COLOR_MATERIAL);

	}

	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height)
{

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (aspect == ASPECT_1_1)
	{
		if (width < height && width > 0)
		{
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		}
		else
		{
			if (width >= height && height > 0)
			{
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
			}
		}
	}
	else
	{
		glFrustum(left, right, bottom, top, near, far);
	}

	Display();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == '+')
	{
		scale += 0.05;
	}
	
	else if (key == '-' && scale > 0.05)
	{
		scale -= 0.05;
	}

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		if (key == 'R' && ambient_light[0] < 1.0)
		{
			ambient_light[0] += 0.05;
		}
		else
		{
			if (key == 'r' && ambient_light[0] > -1.0)
			{
				ambient_light[0] -= 0.05;
			}
		}

		if (key == 'G' && ambient_light[1] < 1.0)
		{
			ambient_light[1] += 0.05;
		}
		else
		{
			if (key == 'g' && ambient_light[1] > -1.0)
			{
				ambient_light[1] -= 0.05;
			}
		}

		if (key == 'B' && ambient_light[2] < 1.0)
		{
			ambient_light[2] += 0.05;
		}
		else
		{
			if (key == 'b' && ambient_light[2] > -1.0)
			{
				ambient_light[2] -= 0.05;
			}
		}
	}
	else if (LightMode == REFLECTOR_OPTION)
	{
		switch (key)
		{
			case 'S':
			{
				if (spot_cutoff == 90)
				{
					spot_cutoff = 180;
				}
				else
				{
					if (spot_cutoff < 90)
					{
						spot_cutoff++;
					}
				}
			}
			break;

			case 's':
			{
				if (spot_cutoff == 180)
				{
					spot_cutoff = 90;
				}
				else
				{
					if (spot_cutoff > 0)
					{
						spot_cutoff--;
					}
				}
			}
			break;

			case 'E':
			{
				if (spot_exponent < 128)
				{
					spot_exponent++;
				}
			}
			break;

			case 'e':
			{
				if (spot_exponent > 0)
				{
					spot_exponent--;
				}
			}
			break;

			case 'C':
			{
				constant_attenuation += 0.1;
			}
			break;

			case 'c':
			{
				if (constant_attenuation > 0)
				{
					constant_attenuation -= 0.1;
				}
			}
			break;

			case 'L':
			{
				linear_attenuation += 0.1;
			}
			break;

			case 'l':
			{
				if (linear_attenuation > 0)
				{
					linear_attenuation -= 0.1;
				}
			}
			break;

			case 'Q':
			{
				quadratic_attenuation += 0.1;
			}
			break;

			case 'q':
			{
				if (quadratic_attenuation > 0)
				{
					quadratic_attenuation -= 0.1;
				}
			}
			break;
		}
	}

	Display();
}

void SpecialKeys(int key, int x, int y)
{
	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		switch (key)
		{
			case GLUT_KEY_LEFT:
			{
				rotatey -= 1;
			}
			break;

			case GLUT_KEY_UP:
			{
				rotatex -= 1;
			}
			break;

			case GLUT_KEY_RIGHT:
			{
				rotatey += 1;
			}
			break;

			case GLUT_KEY_DOWN:
			{
				rotatex += 1;
			}
			break;
		}
	}
	else
	{
		switch (key)
		{
			case GLUT_KEY_LEFT:
			{
				light_rotatey -= 5;
			}
			break;

			case GLUT_KEY_RIGHT:
			{
				light_rotatey += 5;
			}
			break;

			case GLUT_KEY_DOWN:
			{
				light_rotatex += 5;
			}
			break;

			case GLUT_KEY_UP:
			{
				light_rotatex -= 5;
			}
			break;
		}
	}

	Display();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state;

		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Menu(int value)
{
	switch (value)
	{
		case BRASS:
		{
			ambient = BrassAmbient;
			diffuse = BrassDiffuse;
			specular = BrassSpecular;
			shininess = BrassShininess;
			Display();
		}
		break;

		case BRONZE:
		{
			ambient = BronzeAmbient;
			diffuse = BronzeDiffuse;
			specular = BronzeSpecular;
			shininess = BronzeShininess;
			Display();
		}
		break;

		case POLISHED_BRONZE:
		{
			ambient = PolishedBronzeAmbient;
			diffuse = PolishedBronzeDiffuse;
			specular = PolishedBronzeSpecular;
			shininess = PolishedBronzeShininess;
			Display();
		}
		break;

		case CHROME:
		{
			ambient = ChromeAmbient;
			diffuse = ChromeDiffuse;
			specular = ChromeSpecular;
			shininess = ChromeShininess;
			Display();
		}
		break;

		case COPPER:
		{
			ambient = CopperAmbient;
			diffuse = CopperDiffuse;
			specular = CopperSpecular;
			shininess = CopperShininess;
			Display();
		}
		break;

		case POLISHED_COPPER:
		{
			ambient = PolishedCopperAmbient;
			diffuse = PolishedCopperDiffuse;
			specular = PolishedCopperSpecular;
			shininess = PolishedCopperShininess;
			Display();
		}
		break;

		case GOLD:
		{
			ambient = GoldAmbient;
			diffuse = GoldDiffuse;
			specular = GoldSpecular;
			shininess = GoldShininess;
			Display();
		}
		break;

		case POLISHED_GOLD:
		{
			ambient = PolishedGoldAmbient;
			diffuse = PolishedGoldDiffuse;
			specular = PolishedGoldSpecular;
			shininess = PolishedGoldShininess;
			Display();
		}
		break;

		case PEWTER:
		{
			ambient = PewterAmbient;
			diffuse = PewterDiffuse;
			specular = PewterSpecular;
			shininess = PewterShininess;
			Display();
		}
		break;

		case SILVER:
		{
			ambient = SilverAmbient;
			diffuse = SilverDiffuse;
			specular = SilverSpecular;
			shininess = SilverShininess;
			Display();
		}
		break;

		case POLISHED_SILVER:
		{
			ambient = PolishedSilverAmbient;
			diffuse = PolishedSilverDiffuse;
			specular = PolishedSilverSpecular;
			shininess = PolishedSilverShininess;
			Display();
		}
		break;

		case EMERALD:
		{
			ambient = EmeraldAmbient;
			diffuse = EmeraldDiffuse;
			specular = EmeraldSpecular;
			shininess = EmeraldShininess;
			Display();
		}
		break;

		case JADE:
		{
			ambient = JadeAmbient;
			diffuse = JadeDiffuse;
			specular = JadeSpecular;
			shininess = JadeShininess;
			Display();
		}
		break;

		case OBSIDIAN:
		{
			ambient = ObsidianAmbient;
			diffuse = ObsidianDiffuse;
			specular = ObsidianSpecular;
			shininess = ObsidianShininess;
			Display();
		}
		break;

		case PEARL:
		{
			ambient = PearlAmbient;
			diffuse = PearlDiffuse;
			specular = PearlSpecular;
			shininess = PearlShininess;
			Display();
		}
		break;

		case RUBY:
		{
			ambient = RubyAmbient;
			diffuse = RubyDiffuse;
			specular = RubySpecular;
			shininess = RubyShininess;
			Display();
		}
		break;

		case TURQUOISE:
		{
			ambient = TurquoiseAmbient;
			diffuse = TurquoiseDiffuse;
			specular = TurquoiseSpecular;
			shininess = TurquoiseShininess;
			Display();
		}
		break;

		case BLACK_PLASTIC:
		{
			ambient = BlackPlasticAmbient;
			diffuse = BlackPlasticDiffuse;
			specular = BlackPlasticSpecular;
			shininess = BlackPlasticShininess;
			Display();
		}
		break;

		case BLACK_RUBBER:
		{
			ambient = BlackRubberAmbient;
			diffuse = BlackRubberDiffuse;
			specular = BlackRubberSpecular;
			shininess = BlackRubberShininess;
			Display();
		}
		break;

		case NORMALS_SMOOTH:
		{
			normals = NORMALS_SMOOTH;
			Display();
		}
		break;

		case NORMALS_FLAT:
		{
			normals = NORMALS_FLAT;
			Display();
		}
		break;

		case FULL_WINDOW:
		{
			aspect = FULL_WINDOW;
			Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		break;

		case ASPECT_1_1:
		{
			aspect = ASPECT_1_1;
			Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		break;

		case GLOBAL_LIGHT_OPTION:
		{
			LightMode = GLOBAL_LIGHT_OPTION;
			Display();
		}
		break;

		case REFLECTOR_OPTION:
		{
			LightMode = REFLECTOR_OPTION;
			Display();
		}
		break;

		case POINT_LIGHT_OPTION:
		{
			LightMode = POINT_LIGHT_OPTION;
			Display();
		}
		break;

		case EXIT:
		{
			exit(0);
		}
	}
}


void ExtensionSetup()
{
	const char *version = (char*)glGetString(GL_VERSION);
	int major = 0, minor = 0;

	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
		printf("Bledny format wersji OpenGL\n");
		exit(0);
	}

	if (major > 1 || minor >= 2)
	{
		rescale_normal = true;
	}
	else
	{
		if (glutExtensionSupported("GL_EXT_rescale_normal"))
		{
			rescale_normal = true;
		}
	}
}

int main(int argc, char *argv[])
{
	GenerateVerticles(vertex, N);
	GenerateTriangles(triangles, N);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Stozek");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutCreateMenu(Menu);

	int MenuMaterial = glutCreateMenu(Menu);
	glutAddMenuEntry("Mosiądz", BRASS);
	glutAddMenuEntry("Brąz", BRONZE);
	glutAddMenuEntry("Polerowany brąz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedź", COPPER);
	glutAddMenuEntry("Polerowana miedź", POLISHED_COPPER);
	glutAddMenuEntry("Złoto", GOLD);
	glutAddMenuEntry("Polerowane złoto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perła", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

	int MenuLight = glutCreateMenu(Menu);
	glutAddMenuEntry("Swiatlo Globalne", GLOBAL_LIGHT_OPTION);
	glutAddMenuEntry("Reflektor", REFLECTOR_OPTION);
	glutAddMenuEntry("Punktowe", POINT_LIGHT_OPTION);

	int MenuNormals = glutCreateMenu(Menu);
	glutAddMenuEntry("Jeden wektor normalny na wierzchołek", NORMALS_SMOOTH);
	glutAddMenuEntry("Jeden wektor normalny na ścianę", NORMALS_FLAT);

	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	glutCreateMenu(Menu);
	glutAddSubMenu("Materiał", MenuMaterial);
	glutAddSubMenu("Oswietlenie", MenuLight);
	glutAddSubMenu("Wektory normalne", MenuNormals);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
	ExtensionSetup();
	glutMainLoop();
	return 0;
}
