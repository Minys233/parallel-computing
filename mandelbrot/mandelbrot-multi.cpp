#define GL_SILENCE_DEPRECATION
#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif __linux__
#include <GL/glut.h>
#endif
#include <omp.h>
#include <cstdio>

/* Defaut data via command line */
/* Can enter other values via command line arguments */

#define CENTERX -0.5
#define CENTERY 0.5
#define HEIGHT 0.5
#define WIDTH 0.5
#define MAX_ITER 2000

/* N x M array to be generated */

#define N 2000
#define M 2000

float height = HEIGHT; /* size of window in complex plane */
float width = WIDTH;
float cx = CENTERX; /* center of window in complex plane */
float cy = CENTERY;
int max = MAX_ITER; /* number of interations per point */

int n=N;
int m=M;

/* Use unsigned bytes for image */

GLubyte image[N][M];

/* Complex data type and complex add, mult, and magnitude functions */
/* Probably not worth overhead */

typedef float complex[2];

void add(const complex a, const complex b, complex p)
{
    p[0]=a[0]+b[0];
    p[1]=a[1]+b[1];
}

void mult(const complex a, const complex b, complex p)
{
    p[0]=a[0]*b[0]-a[1]*b[1];
    p[1]=a[0]*b[1]+a[1]*b[0];
}

float mag2(const complex a)
{
    return(a[0]*a[0]+a[1]*a[1]);
}

void form(float a, float b, complex p)
{
    p[0]=a;
    p[1]=b;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(n,m,GL_COLOR_INDEX, GL_UNSIGNED_BYTE, image);
    glutSwapBuffers();
}


void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluOrtho2D(0.0, 0.0, (GLfloat) n, (GLfloat) m* (GLfloat) h / (GLfloat) w);
    else
        gluOrtho2D(0.0, 0.0, (GLfloat) n * (GLfloat) w / (GLfloat) h,(GLfloat) m);
    glMatrixMode(GL_MODELVIEW);
    display();
}

void myinit()
{
    float redmap[256], greenmap[256],bluemap[256];
    int i;

    glClearColor (1.0, 1.0, 1.0, 1.0);
    gluOrtho2D(0.0, 0.0, (GLfloat) n, (GLfloat) m);

/* Define pseudocolor maps, ramps for red and blue,
   random for green */


    for(i=0;i<256;i++)
    {
        redmap[i]=i/255.;
        greenmap[i]=drand48();
        bluemap[i]=1.0-i/255.;
    }

    glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 256, redmap);
    glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 256, greenmap);
    glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 256, bluemap);
}


int main(int argc, char *argv[])
{

    // original code
    float x=0, y=0, v=0;
    complex c0{0,0}, c{0,0}, d{0,0};
    bool nodisplay=false;
    omp_set_num_threads(128);
    if(argc>1) cx = atof(argv[1]); /* center x */
    if(argc>2) cy = atof(argv[2]);  /* center y */
    if(argc>3) height=width=atof(argv[3]); /* rectangle height and width */
    if(argc>4) max=atoi(argv[4]); /* maximum iterations */
    if(argc>5) nodisplay = atoi(argv[5]);

    double wall_timer=omp_get_wtime();
    for (int i=0; i<n; i++) {
#pragma omp parallel for default(none) private(c0, c, d, x, y, v) shared(i, image, width, height, m, n, cx, cy, max)
        for (int j = 0; j < m; j++) {
/* starting point */
            x = i * (width / (n - 1)) + cx - width / 2;
            y = j * (height / (m - 1)) + cy - height / 2;
            form(0, 0, c);
            form(x, y, c0);
/* complex iteration */
            for (int k = 0; k < max; k++) {
                mult(c, c, d);
                add(d, c0, c);
                v = mag2(c);
                if (v > 4.0) break; /* assume not in set if mag > 4 */
            }
/* assign gray level to point based on its magnitude */
            if (v > 1.0) v = 1.0; /* clamp if > 1 */
            image[i][j] = 255 * v;
        }
    }

    printf("%d\t%d\t%d\t%.3f\n", m, n, max, 1000.0*(omp_get_wtime()-wall_timer));
    if(nodisplay) return 0;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
#ifdef __APPLE__
    glutInitWindowSize(N/2, M/2);
#elif __linux__s
    glutInitWindowSize(N, M);
#endif

    glutCreateWindow("mandlebrot-multi");
    myinit();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
