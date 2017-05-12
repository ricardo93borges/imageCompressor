#ifndef MYOPENGL_H
#define MYOPENGL_H

#ifdef WIN32
#include <windows.h>    // includes only in MSWindows not in UNIX
#include "gl/glut.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

enum { IMG24, IMG8, IMGRLE };

// Representa um pixel na imagem de 24 bits
typedef struct {
    unsigned char r, g, b;
} RGB8;

typedef struct {
   int width, height;
   RGB8* pixels;
} Image24;

typedef struct {
    int width, height;
    unsigned char* pixels;
    RGB8 pal[256];
} Image8;

typedef struct {
    int width, height;
    unsigned char* data; // dados no formato RLE
    RGB8 pal[256];
} ImageRLE;

// Funções da interface gráfica e OpenGL
void init(int argc, char** argv);
void display();
void keyboard( unsigned char key, int x, int y );
void mouse(int button, int state, int x, int y);
void arrow_keys(int key, int x, int y);
void reshape(int w, int h);
void buildTex();

#endif // MYOPENGL_H
