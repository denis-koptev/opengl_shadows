#define _CRT_SECURE_NO_WARNINGS

#include "CMatrix.hpp"

#include <GL/glut.h>
#include <stdexcept>
#include <cstdio>

/* Main glut functions */

void display();
void usualKey(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void idle();

/* Init functions */

void setLight();
void turnLightOff();
GLuint loadTexture(const char * filename);

/* Figure drawing */

void drawRoom();
void drawSphere();
void drawCube();
void drawCylinder();

/* Data */

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

GLfloat angleX = 0.0f, angleY = 0.0f;
GLfloat sizeVal = 0.15f;
GLfloat offset = 0.1f;
GLfloat rotStep = 1.0f;
GLfloat lightStep = 0.1f;

GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

CMatrix4x4 shadowMatrix;
CVector4 planeNormal(0.0f, 1.0f, 0.0f, 0.0f);
CVector4 lightPos(0.0, 2.0, -1.0, 0.0);

/* Materials */

float sphere_dif[] = {1.0f, 1.0f, 1.0f };
float sphere_amb[] = { 0.4f, 0.4f, 0.4f };
float sphere_shininess = 128;
GLuint sphereTex = 0;

float cube_dif[] = { 1.0f, 0.3f, 0.3f };
float cube_amb[] = { 0.4f, 0.4f, 0.4f };

float cyl_dif[] = { 0.2f, 0.8f, 0.2f, 0.2f }; // with opacity
float cyl_amb[] = { 0.4f, 0.4f, 0.4f };

float room_dif[] = { 0.5f, 0.5f, 0.5f };
float room_amb[] = { 0.4f, 0.4f, 0.4f };

/* Implementation */

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(
        (glutGet(GLUT_SCREEN_WIDTH) - SCREEN_WIDTH) / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - SCREEN_HEIGHT) / 2);
    glutCreateWindow("Lab 3. Shadows.");

    glutDisplayFunc(display);
    glutKeyboardFunc(usualKey);
    glutSpecialFunc(specialKey);
    glutIdleFunc(idle);

    sphereTex = loadTexture("img/texture.bmp");

    glutMainLoop();

    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Enable rotation using arrows */

    glRotatef(-angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(-angleY, 0.0f, 1.0f, 0.0f);

    /* Start creating the scene */

    setLight();

    drawRoom();

    /* Creating shadows */

    turnLightOff();

    shadowMatrix.CreateShadowMatrix(planeNormal, lightPos);
    glPushMatrix();
    glMultMatrixf(shadowMatrix.matrix);
    glColor3f(0.0f, 0.0f, 0.0f);

    drawSphere();
    drawCube();
    drawCylinder();

    glPopMatrix();

    setLight();

    /* Drawing original primitives */

    drawSphere();
    drawCube();
    drawCylinder();

    turnLightOff();

    glutSwapBuffers();
}

void usualKey(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '\033':
        {
            exit(0);
            break;
        }
        // Light rotations
        case 'w':
        {
            lightPos.z += lightStep;
            break;
        }
        case 's':
        {
            lightPos.z -= lightStep;
            break;
        }
        case 'a':
        {
            lightPos.x -= lightStep;
            break;
        }
        case 'd':
        {
            lightPos.x += lightStep;
            break;
        }
        // Light color
        case '1':
        {
            light_diffuse[0] = !light_diffuse[0];
            break;
        }
        case '2':
        {
            light_diffuse[1] = !light_diffuse[1];
            break;
        }
        case '3':
        {
            light_diffuse[2] = !light_diffuse[2];
            break;
        }
        default: break;
    }

    glutPostRedisplay();
}

void specialKey(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
        {
            angleY < 360 ? angleY += rotStep : angleY = 0;
            break;
        }
        case GLUT_KEY_RIGHT:
        {
            angleY > 0 ? angleY -= rotStep : angleY = 360;
            break;
        }
        case GLUT_KEY_UP:
        {
            angleX < 360 ? angleX += rotStep : angleX = 0;
            break;
        }
        case GLUT_KEY_DOWN:
        {
            angleX > 0 ? angleX -= rotStep : angleX = 360;
            break;
        }
        default:
        {
            break;
        }
    }

    glutPostRedisplay();
}

void idle()
{
    // TODO: automatic changes are placed here
}

void setLight()
{
    glMatrixMode(GL_PROJECTION);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    GLfloat lightCoords[] = { lightPos.x, lightPos.y, lightPos.z, lightPos.w };
    glLightfv(GL_LIGHT0, GL_POSITION, lightCoords);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void turnLightOff()
{
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

void drawRoom()
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, room_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, room_dif);

    GLfloat dim = 3 * sizeVal + 3 * offset;

    glPushMatrix();

    /* Draw only floor */
    glBegin(GL_QUADS);
    glVertex3f(dim, 0, dim);
    glVertex3f(-dim, 0, dim);
    glVertex3f(-dim, 0, -dim);
    glVertex3f(dim, 0, -dim);
    glEnd();

    glPopMatrix();
}

void drawSphere()
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, sphere_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sphere_dif);

    glPushMatrix();

    GLUquadricObj * sphere = gluNewQuadric();

    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluQuadricTexture(sphere, GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sphereTex);

    glTranslatef(0.0f, sizeVal, 0.0f);
    gluSphere(sphere, sizeVal, 40, 40);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawCube()
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, cube_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cube_dif);

    glPushMatrix();

    glTranslatef(-sizeVal * 2 - offset, sizeVal, 0.0f);

    glutSolidCube(sizeVal * 2);

    glPopMatrix();
}

void drawCylinder()
{
    // Opacity
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMaterialfv(GL_FRONT, GL_AMBIENT, cyl_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cyl_dif);

    glColor4f(0.0f, 0.0f, 0.0f, 0.2f); // for shadows - if DEPTH_TEST disabled

    glPushMatrix();

    GLUquadricObj * cylinder = gluNewQuadric();

    glTranslatef(sizeVal * 2 + offset, sizeVal, -sizeVal);

    gluCylinder(cylinder,
                sizeVal, sizeVal,
                2 * sizeVal,
                40, 40);

    glPopMatrix();

    glDisable(GL_BLEND);
}

GLuint loadTexture(const char * filename)
{
    if (filename == NULL)
    {
        throw std::invalid_argument("Empty filename");
    }

    FILE * image = fopen(filename, "rb");
    if (!image)
    {
        throw std::invalid_argument("Wrong texture filename");
    }

    unsigned char header[54]; // header of the BMP file

    if (fread(header, 1, 54, image) != 54)
    {
        throw std::runtime_error("Wrong header of texture file");
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        throw std::runtime_error("Incorrect BMP texture file");
    }

    // Reading header

    int dataPos = *(int *) & (header[0x0A]); // offset for data
    int imageSize = *(int *) & (header[0x22]); // byte size of image
    int width = *(int *) & (header[0x12]);
    int height = *(int *) & (header[0x16]);

    if (imageSize == 0)
    {
        imageSize = width * height * 3; // 3 - for RGB
    }
    if (dataPos == 0)
    {
        dataPos = 54; // straight after header
    }

    unsigned char * data = new unsigned char[imageSize];
    fread(data, 1, imageSize, image);
    fclose(image);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return texId;
}