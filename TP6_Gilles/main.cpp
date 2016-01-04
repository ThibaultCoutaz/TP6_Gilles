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

float tx=0;
float ty=0;

// Tableau des points de contrôles en global ...
point3 TabPC[4][3];
// Ordre de la courbre  : Ordre
// Degré de la courbe = Ordre - 1
int Orderi = 4,Orderj = 3;




// Point de controle selectionné
int numPointi = 0;
int numPointj = 0;

// Fonction Factorielle
float fact(int n)
{
	if (n > 1)
		return n*fact(n - 1);
	else
		return 1;
}

float Bernstein(int i, int n, float t)
{
	return fact(n) / (fact(i)*fact(n - i))*std::pow(t,i)*std::pow(1 - t, n - i);
}

/* Courbe d'Hermite */
void Hermite(point3 p0, point3 p1, point3 v0, point3 v1){


	float i = 0;
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	while (i < 1){

		float i2 = i*i;
		float i3 = i*i*i;

		float x = (2 * i3 - 3 * i2 + 1)*p0.x + (-2 * i3 + 3 * i2)*p1.x + (i3 - 2 * i2 + i)*v0.x + (i3 - i2)*v1.x;
		float y = (2 * i3 - 3 * i2 + 1)*p0.y + (-2 * i3 + 3 * i2)*p1.y + (i3 - 2 * i2 + i)*v0.y + (i3 - i2)*v1.y;
		float z = (2 * i3 - 3 * i2 + 1)*p0.z + (-2 * i3 + 3 * i2)*p1.z + (i3 - 2 * i2 + i)*v0.z + (i3 - i2)*v1.z;

		glVertex3f(x, y, 0);
		i += 0.01;
	}
	glEnd();
}

/* Courbe de Bezier */
void Bezier(){
	for (int j = 0; j < Orderj; j++){
		float t = 0;
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		while (t < 1){

			float x = 0, y = 0, z = 0;
			for (int i = 0; i < Orderi; i++){
				x += TabPC[i][j].x* Bernstein(i, Orderi - 1, t);
				y += TabPC[i][j].y* Bernstein(i, Orderi - 1, t);
				z += TabPC[i][j].z* Bernstein(i, Orderi - 1, t);
			}

			glVertex3f(x, y, 0);
			t += 0.01;
		}
		glEnd();
	}

	for (int i = 0; i < Orderi; i++){
		float t = 0;
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		while (t < 1){

			float x = 0, y = 0, z = 0;
			for (int j = 0; j < Orderj; j++){
				x += TabPC[i][j].x* Bernstein(j, Orderj - 1, t);
				y += TabPC[i][j].y* Bernstein(j, Orderj - 1, t);
				z += TabPC[i][j].z* Bernstein(j, Orderj - 1, t);
			}

			glVertex3f(x, y, 0);
			t += 0.01;
		}
		glEnd();
	}
}



/* initialisation d'OpenGL*/
static void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	// Initialisation des points de contrôles
	TabPC[0][0] = point3(0, 0, 0);  TabPC[0][1] = point3(0, 1, 0);  TabPC[0][2] = point3(0, 2, 0);
	TabPC[1][0] = point3(1, 0, 0/*1*/);  TabPC[1][1] = point3(1, 1, 1);  TabPC[1][2] = point3(1, 2, 1);
	TabPC[2][0] = point3(2, 0, 0/*1*/);  TabPC[2][1] = point3(2, 1, 1);  TabPC[2][2] = point3(2, 2, 1);
	TabPC[3][0] = point3(3, 0, 0);  TabPC[3][1] = point3(3, 1, 0);  TabPC[3][2] = point3(3, 2, 0);
}


/* Dessin */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(tx,ty,0.0);

	//Hermite(point3(0, 0, 0), point3(2, 0, 0), point3(8, 8, 0), point3(8, -8, 0));
	
	TabPC[numPointi][numPointj] = TabPC[numPointi][numPointj] + point3(tx, ty, 0);

	// Enveloppe des points de controles
	for (int j = 0; j < Orderj; j++){
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < Orderi; i++)
		{
			glVertex3f(TabPC[i][j].x, TabPC[i][j].y, TabPC[i][j].z);
		}
		glEnd();
	}

	for (int i = 0; i < Orderi; i++){
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < Orderj; j++)
		{
			glVertex3f(TabPC[i][j].x, TabPC[i][j].y, TabPC[i][j].z);
		}
		glEnd();
	}


	// Affichage du point de controle courant
	// On se déplace ensuite avec + et - 
    // ° d'un point de contrôle au suivant (+)
    // ° d'un point de contrôle au précédent (-)
	glColor3f (0.0, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
		glVertex3f(TabPC[numPointi][numPointj].x + 0.1, TabPC[numPointi][numPointj].y + 0.1, TabPC[numPointi][numPointj].z);
		glVertex3f(TabPC[numPointi][numPointj].x + 0.1, TabPC[numPointi][numPointj].y - 0.1, TabPC[numPointi][numPointj].z);
		glVertex3f(TabPC[numPointi][numPointj].x - 0.1, TabPC[numPointi][numPointj].y - 0.1, TabPC[numPointi][numPointj].z);
		glVertex3f(TabPC[numPointi][numPointj].x - 0.1, TabPC[numPointi][numPointj].y + 0.1, TabPC[numPointi][numPointj].z);
	glEnd(); 

	// Dessiner ici la courbe de Bézier.
	// Vous devez avoir implémenté Bernstein précédemment.
	void Bezier();
	
	
	glEnd(); 
	glFlush();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-20, 20, -20, 20, -10,10);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
   case '+':
	   if (numPointi < Orderi - 1)
		   numPointi = numPointi + 1;
        else
            numPointi = 0;
		tx=0;
		ty=0;
		break;

   case '-':
		if (numPointi > 0) 
		   numPointi = numPointi - 1;
        else
			numPointi = Orderi - 1;
		tx=0;
		ty=0;
		break;
		
	case '*':
		if (numPointj < Orderj - 1)
			numPointj = numPointj + 1;
		else
			numPointj = 0;
		tx = 0;
		ty = 0;
		break;
	case '/':
		if (numPointj > 0)
			numPointj = numPointj - 1;
		else
			numPointj = Orderj - 1;
		tx = 0;
		ty = 0;
		break;

   case 'd':
         tx=0.1;
		 ty=0;
      break;
   case 'q':
         tx=-0.1;
		 ty=0;
      break;
   case 'z':
         ty=0.1;
		 tx=0;
      break;
   case 's':
         ty=-0.1;
		 tx=0;
      break;
   case ESC:
      exit(0);
      break;
   default :
	   tx=0;
	   ty=0;
   }
   glutPostRedisplay();
}

int main(int argc, char **argv)
{
   glutInitWindowSize(400, 400);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutCreateWindow("Courbe de Bézier");
   init();
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(display);
   glutMainLoop();
   return 0;
}
