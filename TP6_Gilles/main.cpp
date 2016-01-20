/*
 *	Tableau des points permettant de gérer les points de controles
 * On sélectionne le point en se déplaçant avec + et -, ...
 * On sélectionne ensuite si on veut faire monter, descendre amener vers la gauche ou la droite le point.
 *   d : translation à droite
 *   q : à gauche
 *   z : en haut
 *   s : en bas
 *   
 */



#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "struct.h"
#include <glut.h>

/* au cas ou M_PI ne soit defini */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ESC 27

#define WIDTH  600
#define HEIGHT 600

float angleX = 0.0f; //angle de rotation en Y de la scene
float angleY = 0.0f; //angle de rotation en X de la scene

float tx = 0.0f;
float ty = 0.0f;

// Taille des points des courbes du carreau
const int CarreauLength = 3;
const int CarreauListLength = 4;

// Ordre de la courbre  : Ordre
// Degré de la courbe = Ordre - 1
const int Ordre = 4;

// Tableau des points de contrôles en global
point3 TabPC[Ordre] = { point3(-2., -2., -1.), point3(-1., 1., 0.), point3(1., 1., 1.), point3(2., -2., 2.) };
point3 TabPC2[Ordre] = { point3(-1., -1., 0.), point3(0., 1., 0.5), point3(2., 1., 0.), point3(1., -1., -1.) };

// Points de controles du carreau paramétrique
point3 P0[CarreauLength] = { point3(0., 0., 0.), point3(0., 1., 0.), point3(0., 2., 0.) };
point3 P1[CarreauLength] = { point3(1., 0., 0.), point3(1., 1., 1.), point3(1., 2., 1.) };
point3 P2[CarreauLength] = { point3(2., 0., 0.), point3(2., 1., 1.), point3(2., 2., 1.) };
point3 P3[CarreauLength] = { point3(3., 0., 0.), point3(3., 1., 0.), point3(3., 2., 0.) };
point3 *P[CarreauListLength] = { P0, P1, P2, P3 };

point3 TabPC_Gen[10] = { point3(-1., -1., 0.), point3(0., 1., 0.5), point3(2., 1., 0.), point3(1., -1., -1.), point3(3., 0., 0.), point3(3., 1., 0.), point3(3., 2., 0.), point3(2., 0., 0.), point3(2., 1., 1.), point3(2., 2., 1.) };

// Point de controle selectionné
int numPoint = 0;

// Fonction Factorielle
float fact(int n)
{
	if (n <= 0)
		return 1;
	return n * fact(n - 1);
}

float Bernstein(const int i, const int n, const float t)
{
	return (fact(n) / (fact(i) * fact(n - i))) * powf(t, i) * powf(1 - t, n - i);
}

point3 cubique(const point3 &p0, const point3 &p1, const point3 &v0, const point3 &v1, float t)
{
	const float u3 = t * t * t;
	const float u2 = t * t;
	const float f1 = 2.f * u3 - 3.f * u2 + 1.f;
	const float f2 = -2.f * u3 + 3.f * u2;
	const float f3 = u3 - 2.f * u2 + t;
	const float f4 = u3 - u2;
	return point3((p0 * f1) + (p1 * f2) + (v0 * f3) + (v1 * f4));
}

point3 getCasteljauPoint(point3 * points, int r, int i, float t)
{
	if (r == 0) return points[i];

	point3 p1 = getCasteljauPoint(points, r - 1, i, t);
	point3 p2 = getCasteljauPoint(points, r - 1, i + 1, t);

	return point3((1 - t) * p1.x + t * p2.x, (1 - t) * p1.y + t * p2.y, (1 - t) * p1.z + t * p2.z);
}

point3 tensorielle(point3 **p, const float u, const float v, const int length, const int listLength)
{
	point3 ret(0., 0., 0.);

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < listLength; j++)
			//ret = ret + p[j][i]  * Bernstein(i, length - 1, u)  * Bernstein(j, listLength - 1, v);
			ret = ret + getCasteljauPoint(p[j], length - 1, 0, u) * Bernstein(i, length - 1, u)  * Bernstein(j, listLength - 1, v);
	}

	return ret;
}

void drawCasteljau(point3 * points, const int lg)
{
	glColor3f(0.f, 0.f, 1.f);
	glBegin(GL_LINE_STRIP);
	point3 tmp;
	for (float t = 0; t <= 1; t += 0.001)
	{
		tmp = getCasteljauPoint(points, lg - 1, 0, t);
		glVertex3f(tmp.x, tmp.y, tmp.z);
	}
	glEnd();
}

void drawHermite(const point3 &p0, const point3 &p1, const point3 &v0, const point3 &v1)
{
	glColor3f(0.f, 1.f, 1.f);
	glBegin(GL_LINE_STRIP);
	for (float u = 0.f; u <= 1.f; u += 0.01f)
	{
		point3 p = cubique(p0, p1, v0, v1, u);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();
}

void drawBezier(point3 * tabPC, const unsigned int lg)
{
	glColor3f(0.f, 1.f, 0.f);
	glBegin(GL_LINE_STRIP);
	for (float t = 0.f; t <= 1.f; t += 0.01f)
	{
		point3 s(0.f, 0.f, 0.f);
		for (int i = 0; i < lg; i++)
		{
			s = s + point3(tabPC[i] * Bernstein(i, lg - 1, t));
		}
		glVertex3f(s.x, s.y, s.z);
	}
	glEnd();
}

void drawCarreauParam(point3 **p)
{
	for (int i = 0; i < CarreauListLength; i++)
		drawCasteljau(p[i], CarreauLength);

	float delta = 0.05f;

	for (float u = 0.f; u < 1.f; u += delta)
	{
		for (float v = 0.f; v < 1.f; v += delta)
		{
			point3 p_1 = tensorielle(P, u, v, CarreauLength, CarreauListLength);
			point3 p_2 = tensorielle(P, u + delta, v, CarreauLength, CarreauListLength);
			point3 p_3 = tensorielle(P, u + delta, v + delta, CarreauLength, CarreauListLength);
			point3 p_4 = tensorielle(P, u, v + delta, CarreauLength, CarreauListLength);

			glColor3f(u, v, 1 - v);
			glBegin(GL_QUADS);

			glVertex3f(p_1.x, p_1.y, p_1.z);
			glVertex3f(p_2.x, p_2.y, p_2.z);
			glVertex3f(p_3.x, p_3.y, p_3.z);
			glVertex3f(p_4.x, p_4.y, p_4.z);

			glEnd();
		}
	}
}

void drawSurfaceReglee(point3 * tabPC1, point3 * tabPC2)
{
	drawBezier(tabPC1, Ordre);
	drawCasteljau(tabPC2, Ordre);
	glColor3f(1.f, 0.f, 0.f);
	glBegin(GL_LINE_STRIP);
	for (float t = 0.f; t <= 1.f; t += 0.01f)
	{
		point3 s(0.f, 0.f, 0.f);
		point3 s2(0.f, 0.f, 0.f);
		for (int i = 0; i < Ordre; i++)
		{
			float b = Bernstein(i, CarreauLength - 1, t);
			s = s + point3(tabPC1[i] * b);
			s2 = s2 + point3(tabPC2[i] * b);
		}
		glVertex3f(s.x, s.y, s.z);
		glVertex3f(s2.x, s2.y, s2.z);
	}
	glEnd();
}

void drawSurfaceBalayee(point3 * tabpc, point3 *generatrice, const int directrice_length, const int generatrice_length, const float step, const float delta)
{
	drawBezier(tabpc, directrice_length);
	drawCasteljau(generatrice, generatrice_length);

	for (float s = 0.f; s < 1.f; s += step)
	{
		point3 p(getCasteljauPoint(tabpc, directrice_length - 1, 0, s));
		point3 p2(getCasteljauPoint(tabpc, directrice_length - 1, 0, s + step));

		for (float t = 0.f; t < 1.f; t += delta)
		{
			point3 acc(0.f, 0.f, 0.f);
			point3 acc2(0.f, 0.f, 0.f);
			point3 acc3(0.f, 0.f, 0.f);
			point3 acc4(0.f, 0.f, 0.f);
			for (int i = 0; i < generatrice_length; i++)
			{
				point3 p_gen(generatrice[i] * Bernstein(i, generatrice_length - 1, t));
				acc = acc + point3(p + p_gen);
				acc2 = acc2 + point3(p2 + p_gen);
				acc3 = acc3 + p2 + generatrice[i] * Bernstein(i, generatrice_length - 1, t + delta);
				acc4 = acc4 + p + generatrice[i] * Bernstein(i, generatrice_length - 1, t + delta);
			}

			glBegin(GL_TRIANGLES);
			glColor3f(1.f, 0.f, 0.f);
			glVertex3f(acc.x, acc.y, acc.z);
			glVertex3f(acc2.x, acc2.y, acc2.z);
			glVertex3f(acc3.x, acc3.y, acc3.z);
			glColor3f(0.f, 1.f, 0.f);
			glVertex3f(acc4.x, acc4.y, acc4.z);
			glVertex3f(acc.x, acc.y, acc.z);
			glVertex3f(acc3.x, acc3.y, acc3.z);
			glEnd();
		}
	}
}

void addContrainteBezier2C0(point3 *tabPC1, point3 *tabPC2, const int ordre1, const int ordre2)
{
	tabPC2[0] = tabPC1[ordre1 - 1];
}

void addContrainteBezier2C1(point3 *tabPC1, point3 *tabPC2, const int ordre1, const int ordre2)
{
	tabPC2[0] = tabPC1[ordre1 - 1];
	point3 vecOffset = tabPC1[ordre1 - 1] - tabPC1[ordre1 - 2];
	tabPC2[1] = tabPC1[ordre1 - 1] + vecOffset;
}

void drawEnveloppe(point3 * tabPC, const int ordre)
{
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < ordre; i++)
	{
		glVertex3f(tabPC[i].x, tabPC[i].y, tabPC[i].z);
	}
	glEnd();
}

// Initialisation d'OpenGL
static void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f); // Set background depth to farthest
	glEnable(GL_DEPTH_TEST); // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL); // Set the type of depth-test
	//glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH); // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

/* Dessin */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-angleY, 0.0f, 0.0f, 1.0f);
	glRotatef(angleX, 0.0f, 1.0f, 0.0f);
	glRotatef(-angleY, 0.0f, 0.0f, 1.0f);
	glRotatef(angleX, 0.0f, 1.0f, 0.0f);

	TabPC[numPoint] = TabPC[numPoint] + point3(tx, ty, 0);

	// Contraintes classe C
	//addContrainteBezier2C1(TabPC, TabPC2, Ordre, Ordre);

	// Enveloppe des points de controles
	//glColor3f(1.0, 1.0, 0.0);
	//drawEnveloppe(TabPC, Ordre);
	//drawEnveloppe(TabPC2, Ordre);

	// Affichage du point de controle courant
	// On se déplace ensuite avec + et - 
	// ° d'un point de contrôle au suivant (+)
	// ° d'un point de contrôle au précédent (-)
	/*glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(TabPC[numPoint].x + 0.1f, TabPC[numPoint].y + 0.1f, TabPC[numPoint].z);
	glVertex3f(TabPC[numPoint].x + 0.1f, TabPC[numPoint].y - 0.1f, TabPC[numPoint].z);
	glVertex3f(TabPC[numPoint].x - 0.1f, TabPC[numPoint].y - 0.1f, TabPC[numPoint].z);
	glVertex3f(TabPC[numPoint].x - 0.1f, TabPC[numPoint].y + 0.1f, TabPC[numPoint].z);
	glEnd();*/

	// Courbe de Bézier
	//drawBezier(TabPC, Ordre);
	//drawBezier(TabPC2, Ordre);

	//drawSurfaceReglee(TabPC, TabPC2);

	//drawCarreauParam(P);

	drawSurfaceBalayee(TabPC, TabPC_Gen, Ordre, 10, 0.05f, 0.05f);

	//glEnd();
	glFlush();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-20, 20, -20, 20, -10, 10);
	glOrtho(-5, 5, -5, 5, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '+':
		if (numPoint < Ordre - 1)
			numPoint = numPoint + 1;
		else
			numPoint = 0;
		tx = 0;
		ty = 0;
		break;
	case '-':
		if (numPoint > 0)
			numPoint = numPoint - 1;
		else
			numPoint = Ordre - 1;
		tx = 0;
		ty = 0;
		break;

	case 'd':
		tx = 0.1;
		ty = 0;
		break;
	case 'q':
		tx = -0.1;
		ty = 0;
		break;
	case 'z':
		ty = 0.1;
		tx = 0;
		break;
	case 's':
		ty = -0.1;
		tx = 0;
		break;
	case ESC:
		exit(0);
		break;
	default:
		tx = 0;
		ty = 0;
	}
	glutPostRedisplay();
}

GLvoid gestionSouris(int x, int y)
{
	angleX = x * 720 / WIDTH;
	angleY = -(y * 180 / HEIGHT - 90) * 4;

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Courbe de Bézier");
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(gestionSouris);
	glutMainLoop();
	return 0;
}
