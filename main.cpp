#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "../shader_lib/image.h"
#include "../shader_lib/snake.h"
#include "glm/glm.h"
extern "C"
{
#include "glm_helper.h"
}

#define numtriangles 12

//GLMmodel *head;
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

class particles {
public:
	double x;
	double y;

	float red;
	float green;
	float blue;

	float life;

	float speedy;
	float speedx;
};

double vertex[108] = {
	//top
	-0.025, 0.025, 0.025,
	-0.025, 0.025, -0.025,
	0.025, 0.025, -0.025,

	-0.025, 0.025, 0.025,
	0.025, 0.025, -0.025,
	0.025, 0.025, 0.025,
	//right
	0.025, -0.025, 0.025,
	0.025, 0.025, 0.025,
	0.025, 0.025, -0.025,

	0.025, -0.025, 0.025,
	0.025, 0.025, -0.025,
	0.025, -0.025, -0.025,
	//left
	-0.025, -0.025, -0.025,
	-0.025, 0.025, -0.025,
	-0.025, 0.025, 0.025,

	-0.025, -0.025, -0.025,
	-0.025, 0.025, 0.025,
	-0.025, -0.025, 0.025,

	//front
	-0.025, -0.025, 0.025,
	-0.025, 0.025, 0.025,
	0.025, 0.025, 0.025,

	-0.025, -0.025, 0.025,
	0.025, 0.025, 0.025,
	0.025, -0.025, 0.025,

	//back
	0.025, -0.025, -0.025,
	0.025, 0.025, -0.025,
	-0.025, 0.025, -0.025,

	0.025, -0.025, -0.025,
	-0.025, 0.025, -0.025,
	-0.025, -0.025, -0.025,

	//bottom
	0.025, -0.025, -0.025,
	0.025, -0.025, 0.025,
	-0.025, -0.025, 0.025,

	0.025, -0.025, -0.025,
	-0.025, -0.025, 0.025,
	-0.025, -0.025, -0.025
};

double normal[18]
{
	0.0,1.0,0.0,
	1.0,0.0,0.0,
	-1.0,0.0,0.0,
	0.0,0.0,1.0,
	0.0,0.0,-1.0,
	0.0,-1.0,0.0,
};

double texcoord[12]
{
	0.0, 1.0,
	1.0, 1.0,
	1.0, 0.0,
	0.0, 1.0,
	1.0, 0.0,
	0.0, 0.0,
};

Snake *player;
particles particle[1000];

float seed = 0.0;
float r = 0;
float g = 0;
float b = 0;

int irand = 0;
int life = 2;
int type;

GLfloat light_pos[] = { 0.0, 0.0, 1.0 };
// parameters for phong shading
GLfloat light_specular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

GLfloat Ka[] = { 1.0 , 1.0 , 1.0 };
GLfloat Kd[] = { 1.0 , 1.0 , 1.0 };
GLfloat Ks[] = { 1.0 , 1.0 , 1.0 };
GLfloat Ns = 100;

// viewport
double minX = -1.0, maxX = 1.0;
double minY = -1.0, maxY = 1.0;

// window size
double width = 640;
double height = 480;
double sub_width = 200;
double sub_height = 200;

double unitSize = 10;

int unitsPerRow = width / unitSize;
int unitsPerCol = height / unitSize;

// variables 2D
int limitX = unitsPerRow;
int limitY = unitsPerCol;

// variables shared 2D/3D
// score
int score = 0;
int scoreMultiplier = 1;

// timer parameters
double timerTick = 65, timerMultiplier = 0.8, speed = 1.0;

// snake direction
// X=1 right, X=-1 left
// Y=1 up,    Y=-1 down
int dirX = 1, dirY = 0;

// flag of display map
bool showMap = false;
// flag of view mode
bool mode = false;

// texture name
static GLuint texName[36];

int appleX, appleY;
int appleAngle = 0;

int growth = 0;

int main_w, sub_w;
GLuint vert, frag, program;
double hitx, hity;
int flag_s = 0, flag_c = 0;

// Reads a bitmap image from a file
Image* loadBMP(const char* filename);

void Light()
{
	glShadeModel(GL_SMOOTH);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
	glMaterialf(GL_FRONT, GL_SHININESS, Ns);
}

void loadTexture(Image* image, int k) {
	glBindTexture(GL_TEXTURE_2D, texName[k]); // Tell OpenGL which texture to edit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,      // Always GL_TEXTURE_2D
		0,                           // 0 for now
		GL_RGB,                      // Format OpenGL uses for image
		image->width, image->height, // Width and height
		0,                           // The border of the image
		GL_RGB,                      // GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE,            // GL_UNSIGNED_BYTE, because pixels are stored
									 //   as unsigned numbers
		image->pixels);              // The actual pixel data
}

static void spark(int &flag) {
	int i = 0;
	float c1, c2, c3;

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	if (flag > 0)
		flag--;
	do {
		seed += 0.1;
		r += 0.1;
		b += 0.002;
		g += 0.009;
		if (r > 1.0) {
			r = float(rand() % 2) - 0.7;
		}
		if (g > 1.0) {
			g = float(rand() % 1) - 0.3;
		}
		if (b > 0.5) {
			b = 0.0;
		}
		if (seed > 1.0) {
			seed = 0.0;
		}
		if (irand > 100) {
			if (type == 2) {
				irand = -100;
			}
			else {
				irand = -50;
			}
		}

		if (type == 2) {
			c1 = rand() % 255 / 255.0;
			c2 = rand() % 255 / 255.0;
			c3 = rand() % 255 / 255.0;
			glColor4f(c1, c2, c3, particle[i].life);
			glRotated(rand(), 0.0, 0.0, 1.0);
			glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(particle[i].x + 0.03, particle[i].y - 0.03);
			glVertex2f(particle[i].x - 0.03, particle[i].y - 0.03);
			glVertex2f(particle[i].x, particle[i].y + 0.03);
			glEnd();
		}
		else {
			glColor4f(particle[i].red, particle[i].green, particle[i].blue, particle[i].life);

			glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(particle[i].x + 0.03, particle[i].y + 0.03);
			glVertex2f(particle[i].x - 0.03, particle[i].y + 0.03);
			glVertex2f(particle[i].x + 0.03, particle[i].y - 0.03);
			glVertex2f(particle[i].x - 0.03, particle[i].y - 0.03);
			glEnd();
		}

		if (particle[i].life <= 0.0) {
			if (type == 1) {
				life = 2.5;
				srand((seed + r + g + b) * 500);
			}
			else if (type == 2) {
				life = 4.0;
				srand(50 * r - irand);
				particle[i].x = 0;
				particle[i].y = 0;
			}
			particle[i].life = float(rand() % life);
			particle[i].speedy = float((rand() % 50) - 26.0f);
			particle[i].speedx = float((rand() % 50) - 26.0f);
			particle[i].red = r;
			particle[i].green = g;
			particle[i].blue = b;
			irand += rand() % 50;
		}
		particle[i].life -= 0.05;
		particle[i].y += particle[i].speedx / 1000;
		particle[i].x += particle[i].speedy / 1000;
		++i;
	} while (i < 50);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

static void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);

	player = new Snake(unitsPerRow / 2, // initial position in X
		unitsPerCol / 2, // initial position in y
		100);            // maximum length of tail

	// Random seed
	srand((int)time(NULL));

	// generate first apple
	appleX = rand() % unitsPerRow + 1;
	appleY = rand() % unitsPerCol + 1;

	// Textures
	glGenTextures(5, texName);
	Image* image;

	image = loadBMP("../Resources/snake.bmp");
	loadTexture(image, 0);

	image = loadBMP("../Resources/apple.bmp");
	loadTexture(image, 1);

	image = loadBMP("../Resources/stone.bmp");
	loadTexture(image, 2);

	image = loadBMP("../Resources/DirtyBrick_S.bmp");
	loadTexture(image, 3);

	image = loadBMP("../Resources/DirtyBrick_N.bmp");
	loadTexture(image, 4);

	delete image;

	GLuint vbo_id;
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	Vertex *vertices = (Vertex *)malloc(sizeof(Vertex) * numtriangles * 3);

	for (GLuint i = 0; i < numtriangles; i++) {
		for (GLuint j = 0; j < 3; j++) {
			// vertex position
			vertices[i * 3 + j].position[0] = vertex[3 * j + 9 * i + 0];
			vertices[i * 3 + j].position[1] = vertex[3 * j + 9 * i + 1];
			vertices[i * 3 + j].position[2] = vertex[3 * j + 9 * i + 2];
			// vertex normal
			vertices[i * 3 + j].normal[0] = normal[0 + 3 * (i / 2)];
			vertices[i * 3 + j].normal[1] = normal[1 + 3 * (i / 2)];
			vertices[i * 3 + j].normal[2] = normal[2 + 3 * (i / 2)];
			// vertex texcoord
			vertices[i * 3 + j].texcoord[0] = texcoord[(0 + 2 * j + 6 * i) % 12];
			vertices[i * 3 + j].texcoord[1] = texcoord[(1 + 2 * j + 6 * i) % 12];
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numtriangles * 3, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindVertexArray(0);

	vert = createShader("wall.vert", "vertex");
	frag = createShader("wall.frag", "fragment");
	program = createProgram(vert, frag);
}

double xPos2d(int x) {
	double wide = maxX - minX;
	double mappedX = x * (wide / unitsPerRow);

	return minX + mappedX;
}

double yPos2d(int y) {
	double tall = maxY - minY;
	double mappedY = y * (tall / unitsPerCol);

	return minY + mappedY;
}

// drawString
void drawString(void *font, const char *s, float x, float y) {
	unsigned int i;

	glRasterPos2f(x, y);
	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

static void drawMap(void) {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	// render grid
	glColor3f(0.2, 0.0, 0.2);
	glLineWidth(1);

	glBegin(GL_LINES);

	for (int i = 0; i <= unitsPerRow; i += 2) {
		glVertex2d(minX, xPos2d(i));
		glVertex2f(maxX, xPos2d(i));
		glVertex2d(xPos2d(i), minY);
		glVertex2f(xPos2d(i), maxY);
	}

	glEnd();

	// render frame
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(3);

	glBegin(GL_LINE_LOOP);
	glVertex2f(minX, minY);
	glVertex2f(minX, maxY);
	glVertex2f(maxX, maxY);
	glVertex2f(maxX, minY);
	glEnd();

	glLineWidth(1);

	// render apple
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(6);

	glBegin(GL_POINTS);
	glVertex2f(xPos2d(appleX), yPos2d(appleY));
	glEnd();

	glPointSize(1);

	// render snake
	glColor3f(1.0, 1.0, 0.0);
	glLineWidth(unitSize * sub_width / width);

	glBegin(GL_LINE_STRIP);
	for (int i = player->length - 1; i >= 0; i--) {
		glVertex2f(xPos2d(player->xAt(i)), yPos2d(player->yAt(i)));
	}
	glEnd();

	glLineWidth(1);

	// render score
	glColor3f(1.0, 1.0, 1.0);

	std::stringstream ss; // Score
	ss << "Score: " << std::to_string(score);
	drawString(GLUT_BITMAP_9_BY_15, ss.str().c_str(), -0.85, -0.85);
}

static void drawWall() {
	glUseProgram(program);
	GLfloat mtx[16];
	GLint loc;
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
	GLfloat mtx2[16];
	GLint loc2;
	glGetFloatv(GL_PROJECTION_MATRIX, mtx2);
	loc = glGetUniformLocation(program, "MV");
	loc2 = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(loc, 1, GL_FALSE, mtx);
	// parse projection matrix
	glUniformMatrix4fv(loc2, 1, GL_FALSE, mtx2);
	// parse light position
	GLint loc3;
	loc3 = glGetUniformLocation(program, "Light_position");
	glUniform3fv(loc3, 1, light_pos);
	// parse light specular
	GLint loc4;
	loc4 = glGetUniformLocation(program, "Light_specular");
	glUniform4fv(loc4, 1, light_specular);
	// parse light diffuse
	GLint loc5;
	loc5 = glGetUniformLocation(program, "Light_diffuse");
	glUniform4fv(loc5, 1, light_diffuse);
	// parse light ambient
	GLint loc6;
	loc6 = glGetUniformLocation(program, "Light_ambient");
	glUniform4fv(loc6, 1, light_ambient);
	// parse Ka
	GLint loc7;
	loc7 = glGetUniformLocation(program, "Ka");
	glUniform3fv(loc7, 1, Ka);
	// parse Kd
	GLint loc8;
	loc8 = glGetUniformLocation(program, "Kd");
	glUniform3fv(loc8, 1, Kd);
	// parse Ks
	GLint loc9;
	loc9 = glGetUniformLocation(program, "Ks");
	glUniform3fv(loc9, 1, Ks);
	// parse Ns
	GLint loc10;
	loc10 = glGetUniformLocation(program, "Ns");
	glUniform1f(loc10, Ns);

	GLint loc11 = glGetUniformLocation(program, "mainTexture");
	glActiveTexture(GL_TEXTURE0 + 3); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, texName[3]);
	glUniform1i(loc11, 3);

	GLint loc12 = glGetUniformLocation(program, "normalTexture");
	glActiveTexture(GL_TEXTURE0 + 4); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, texName[4]);
	glUniform1i(loc12, 4);

	glDrawArrays(GL_TRIANGLES, 0, numtriangles * 3);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
}
//drawing in Perspective
static void drawPerspective(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Light();

	// render grid
	glColor3f(0.2, 0.0, 0.2);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (double i = 0; i <= unitsPerRow; i += 2) {
		glVertex2d(minX, xPos2d(i));
		glVertex2f(maxX, xPos2d(i));
		glVertex2d(xPos2d(i), minY);
		glVertex2f(xPos2d(i), maxY);
	}
	glEnd();

	//render frame
	//right wall
	glPushMatrix();
	glTranslated(maxX + 0.05, 0.0, 0.0);
	glScaled(1.0, 1.0 * 41, 1.0 * 41);//ªø¼e°ª
	drawWall();
	glPopMatrix();
	//left wall
	glPushMatrix();
	glTranslated(minX - 0.05, 0.0, 0.0);
	glScaled(1.0, 1.0 * 41, 1.0 * 41);
	//glutSolidCube(0.05);//(0.05, 0.05, 0.05),(0.05, 0.05, -0.05),(-,+,-),(-,+,+),(+,-,+),(+,-,-),(-,-,-),(-,-,+)
	drawWall();
	glPopMatrix();
	//down wall
	glPushMatrix();
	glTranslated(0.0, minY - 0.05, 0.0);
	glScaled(1.0 * 43, 1.0, 1.0 * 43);
	drawWall();
	glPopMatrix();
	//up wall
	glPushMatrix();
	glTranslated(0.0, maxY + 0.05, 0.0);
	glScaled(1.0 * 43, 1.0, 1.0 * 43);
	drawWall();
	glPopMatrix();

	//floor
	glEnable(GL_TEXTURE_2D);
	glColor3f(255.0, 255.0, 255.0);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// Linear Filtering
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1.1, -1.1, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(-1.1, 1.1, 0.0);
	glTexCoord2f(1, 1);
	glVertex3f(1.1, 1.1, 0.0);
	glTexCoord2f(1, 0);
	glVertex3f(1.1, -1.1, 0.0);
	glEnd();

	//render apple
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glPushMatrix();
	glTranslated(xPos2d(appleX), yPos2d(appleY), 0.025);
	glRotated(appleAngle, 0.3, 1.0, 0.0);
	glutSolidSphere(0.05, 5, 5);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, NULL);

	if (flag_s) {
		type = 2;
		glPushMatrix();
		glTranslated(xPos2d(appleX), yPos2d(appleY), 0.01);
		spark(flag_s);
		glPopMatrix();
	}

	if (flag_c) {
		type = 1;
		glPushMatrix();
		glTranslated(xPos2d(hitx), yPos2d(hity), 0.05);
		if (hitx >= unitsPerRow)
			glRotated(90, 0.0, 1.0, 0.0);
		else if (hitx <= 0)
			glRotated(-90, 0.0, 1.0, 0.0);
		else if (hity >= unitsPerCol)
			glRotated(90, 1.0, 0.0, 0.0);
		else
			glRotated(-90, 1.0, 0.0, 0.0);
		spark(flag_c);
		glPopMatrix();
	}

	// render snake
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	for (int i = player->length - 1; i >= 0; i--) {
		glPushMatrix();
		glTranslated(xPos2d(player->xAt(i)), yPos2d(player->yAt(i)), 0.025);
		glutSolidSphere(0.05, 10, 10);
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 20.0);
}

static void sub_display(void) {
	glutSetWindow(sub_w);
	glClear(GL_COLOR_BUFFER_BIT);
	reshape(200, 200);
	gluLookAt(0.0, 0.0, 2.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	drawMap();
	glutSwapBuffers();
	glutSetWindow(main_w);
}

static void display(void) {
	double snakeX, snakeY;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	snakeX = xPos2d(player->x());
	snakeY = yPos2d(player->y());
	snakeY = snakeY <= -0.5 ? -0.5 : snakeY;
	snakeY = snakeY >= 1.5 ? 1.5 : snakeY;
	if (mode) {
		gluLookAt(0.0, 0.0, 2.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
	}
	else {
		if (dirX == 1) {
			gluLookAt(xPos2d(player->x()), yPos2d(player->y()), 0.1,
				2.0, snakeY, 0.05,
				0.0, 0.0, 1.0);
		}
		else if (dirX == -1) {
			gluLookAt(xPos2d(player->x()), yPos2d(player->y()), 0.1,
				-2.0, snakeY, 0.05,
				0.0, 0.0, 1.0);
		}
		else if (dirY == 1) {
			gluLookAt(xPos2d(player->x()), yPos2d(player->y()), 0.1,
				snakeX, 2.0, 0.05,
				0.0, 0.0, 1.0);
		}
		else if (dirY == -1) {
			gluLookAt(xPos2d(player->x()), yPos2d(player->y()), 0.1,
				snakeX, -2.0, 0.05,
				0.0, 0.0, 1.0);
		}
	}
	drawPerspective();
	if (showMap)
		sub_display();
	glutSwapBuffers();
}

// when hits return true
bool snakeHits(float x, float y) {
	double nextX, nextY;

	nextX = player->x();
	nextY = player->y();

	return nextX == x && nextY == y;
}

void resetGame() {
	player->reset();
	dirX = 1;
	dirY = 0;
}

void Timer(int valor) {
	int nextX, nextY;

	// check if snake is crash into the wall
	// if true then restart
	if ((dirX == 1 && player->x() >= unitsPerRow) || (dirX == -1 && player->x() <= 0) || (dirY == 1 && player->y() >= unitsPerCol) || (dirY == -1 && player->y() <= 0)) {
		for (int i = 0; i < 50; i++) {
			particle[i].x = 0;
			particle[i].y = 0;
		}
		hitx = player->x();
		hity = player->y();
		flag_c = 15;
		resetGame();
		score = 0;
		scoreMultiplier = 1;
		speed = 1.0;
		timerMultiplier = 0.8;
	}
	nextX = player->x() + dirX;
	nextY = player->y() + dirY;

	appleAngle = (appleAngle >= 360) ? 0 : appleAngle + 5;
	if (growth == 1 || snakeHits(appleX, appleY)) {
		for (int i = 0; i < 50; i++) {
			particle[i].x = 0;
			particle[i].y = 0;
		}
		flag_s = 20;
		// increasing the score = speed * 1
		score += (1 * scoreMultiplier * speed);

		//if is not max length(100)
		if (!player->full()) {
			player->eat();
		}
		//if is max length, reset length and speed up
		else {
			// reset
			resetGame();
			// now every apple worth more score
			scoreMultiplier++;
			// increasing speed
			timerTick *= timerMultiplier;
		}
		appleX = rand() % unitsPerRow + 1;
		appleY = rand() % unitsPerCol + 1;
		growth = 0;
	}
	if (!player->moveTo(dirX, dirY)) {
		resetGame();
		score = 0;
		scoreMultiplier = 1;
		speed = 1.0;
		timerMultiplier = 0.8;
	}
	glutPostRedisplay();
	glutTimerFunc(timerTick / speed, Timer, 1);
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
	switch (theKey) {
		// move the snake
	case 'a': case 'A':
		if (dirX == 1) { dirX = 0; dirY = 1; }
		else if (dirX == -1) { dirX = 0; dirY = -1; }
		else if (dirY == 1) { dirX = -1; dirY = 0; }
		else if (dirY == -1) { dirX = 1; dirY = 0; }
		break;
	case 'd': case 'D':
		if (dirX == 1) { dirX = 0; dirY = -1; }
		else if (dirX == -1) { dirX = 0; dirY = 1; }
		else if (dirY == 1) { dirX = 1; dirY = 0; }
		else if (dirY == -1) { dirX = -1; dirY = 0; }
		break;
		// increasing the length
	case 'c': case 'C':
		growth = 1;
		break;
		// hide/show the map
	case 'm': case 'M':
		showMap = !showMap;
		if (showMap) {
			sub_w = glutCreateSubWindow(main_w, 0, 0, sub_width, sub_height);
			glutDisplayFunc(sub_display);
		}
		else {
			glutDestroyWindow(sub_w);
		}
		break;
		// increase the speed
	case 'v':
		speed *= 1.1;
		break;
		// slow down
	case 'V':
		speed *= 0.9;
		break;
		// enter: change view
	case 13:
		if (!mode)
			mode = true;
		else
			mode = false;
		break;
		// esc
	case 27: case 'e': case 'E':
		exit(-1);
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(120, 120);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	main_w = glutCreateWindow("Snake 3D");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboard);
	glutTimerFunc(2000, Timer, 1);
	if (showMap) {
		sub_w = glutCreateSubWindow(main_w, 0, 0, sub_width, sub_height);
		glutDisplayFunc(sub_display);
	}
	glutMainLoop();
	return EXIT_SUCCESS;
}