/* A simple program to show how to set up an X window for OpenGL rendering.
 * X86 compilation: gcc -o -L/usr/X11/lib   main main.c -lGL -lX11
 * X64 compilation: gcc -o -L/usr/X11/lib64 main main.c -lGL -lX11
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <GL/glx.h>    /* this includes the necessary X headers */
#include <GL/gl.h>

#include <GL/glu.h>

#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>

#include "../commons/geometry.h"
#include "DisplayClient.h"

#define NETWORK 1
#define LOCAL 2

static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

Display   *dpy;
Window     win;

Vect3d position = {-2,0,0}, orientation = {1,0,0}, azimut = {0,0,1};
double step = 0.1;
double turn_factor = 0.1;

GLboolean  doubleBuffer = GL_TRUE;

void addPointsChar(char* file_name, unsigned short r_color, unsigned short g_color, unsigned short b_color);

void fatalError(char *message)
{
  fprintf(stderr, "main: %s\n", message);
  exit(1);
}

void redraw(DisplayClient* dc)
{
  static GLboolean   displayListInited = GL_FALSE;
  
  if  (dc -> mainLoop())  // en cas d'echec reception
  {
  	error("connection lost, need to restart display\n");
  }
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50,(long double)2, 0.001, 1000);
  gluLookAt(position.x, position.y, position.z, position.x+orientation.x, position.y+orientation.y, position.z+orientation.z, azimut.x, azimut.y, azimut.z);
  printf("Position : (%lf, %lf, %lf), (%lf, %lf, %lf), (%lf, %lf, %lf)\n", position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, azimut.x, azimut.y, azimut.z);
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i = 0 ; i < dc->index_display_lists ; i++)
  {
  	glCallList(i);
  }
  
  if (doubleBuffer)
    glXSwapBuffers(dpy, win);/* buffer swap does implicit glFlush */
  else
    glFlush();  /* explicit flush for single buffered case */
}

void addPointsChar(char* file_name, unsigned short r_color, unsigned short g_color, unsigned short b_color);

int main(int argc, char **argv)
{
  XVisualInfo         *vi;
  Colormap             cmap;
  XSetWindowAttributes swa;
  GLXContext           cx;
  XEvent               event;
  GLboolean            needRedraw = GL_FALSE, recalcModelView = GL_TRUE;
  int                  dummy;
  
  srand(time(NULL));
  
  // parses command-line arguments
  long client_id = -1;
  char* path = NULL;
  char* config_file = NULL;
  int mode = LOCAL;
  
  int opt;
	while ((opt = getopt (argc, argv, "n:f:c:")) != -1)
	{
		switch (opt)
		{
			case 'n' :
				if  (sscanf(optarg, "%ld", &client_id)) mode = NETWORK;
				else { printf("Error : invalid argument\n"); exit(0); }
				break;
			case 'f' :
				path = optarg;
				break;
			case 'c' :
				config_file = optarg;
				break;

			default :
				printf("Error : invalid argument\n");
				exit(0);
		}
	}
	
	if  (config_file == NULL)
	{
		config_file = new char[1000];
		strcpy(config_file, "display.conf");
	}
	if  (path == NULL)
	{
		path = new char[1000];
		strcpy(path, ".");
	}

  /*** (1) open a connection to the X server ***/

  dpy = XOpenDisplay(NULL);
  if (dpy == NULL)
    fatalError("could not open display");

  /*** (2) make sure OpenGL's GLX extension supported ***/

  if(!glXQueryExtension(dpy, &dummy, &dummy))
    fatalError("X server has no OpenGL GLX extension");

  /*** (3) find an appropriate visual ***/

  /* find an OpenGL-capable RGB visual with depth buffer */
  vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
  if (vi == NULL)
  {
    vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
    if (vi == NULL) fatalError("no RGB visual with depth buffer");
    doubleBuffer = GL_FALSE;
  }
 /* if(vi->class != TrueColor)
    fatalError("TrueColor visual required for this program");*/

  /*** (4) create an OpenGL rendering context  ***/

  /* create an OpenGL rendering context */
  cx = glXCreateContext(dpy, vi, /* no shared dlists */ None,
                        /* direct rendering if possible */ GL_TRUE);
  if (cx == NULL)
    fatalError("could not create rendering context");

  /*** (5) create an X window with the selected visual ***/

  /* create an X colormap since probably not using default visual */
  cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = KeyPressMask    | ExposureMask
                 | ButtonPressMask | StructureNotifyMask;
  win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0,
                      600, 600, 0, vi->depth, InputOutput, vi->visual,
                      CWBorderPixel | CWColormap | CWEventMask, &swa);
  XSetStandardProperties(dpy, win, "main", "main", None,
                         argv, argc, NULL);

  /*** (6) bind the rendering context to the window ***/

  glXMakeCurrent(dpy, win, cx);

  /*** (7) request the X window to be displayed on the screen ***/

  XMapWindow(dpy, win);

  /*** (8) configure the OpenGL context for rendering ***/

  glEnable(GL_DEPTH_TEST); /* enable depth buffering */
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glDepthFunc(GL_LESS);    /* pedantic, GL_LESS is the default */
  glClearDepth(1.0);       /* pedantic, 1.0 is the default */

  /* frame buffer clears should be to black */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* set up projection transform */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
  /* establish initial viewport */
  /* pedantic, full window size is default viewport */
  glViewport(0, 0, 300, 300);

  /*** (9) dispatch X events ***/
  
  DisplayClient dc(config_file, mode, path, client_id);
  dc.connect();

  while (1)
  {
    do
    {
      XNextEvent(dpy, &event);
      switch (event.type)
      {
        case KeyPress:
        {
          KeySym     keysym;
          XKeyEvent *kevent;
          char       buffer[1];
          /* It is necessary to convert the keycode to a
           * keysym before checking if it is an escape */
          kevent = (XKeyEvent *) &event;
          if (   (XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL) == 1)
              && (keysym == (KeySym)XK_Escape) )
            exit(0);
            
            orientation.normalize();
            azimut.normalize();

            // se deplace vers l'avant
            if  (keysym == (KeySym)XK_Left)
            {
                    orientation = add(orientation, mult(vectorProduct(azimut, orientation).normalizeCopy(), turn_factor));
            }
            if  (keysym == (KeySym)XK_Right)
            {
                    orientation = soust(orientation, mult(vectorProduct(azimut, orientation).normalizeCopy(), turn_factor));
            }
            if  (keysym == (KeySym)XK_Up)
            {
                    position = add(position, mult(orientation, step));
            }
            if  (keysym == (KeySym)XK_Down)
            {
                    position = soust(position, mult(orientation, step));
            }
            if  (keysym == (KeySym)XK_Page_Up)
            {
                    position = add(position, mult(azimut, step));
            }
            if  (keysym == (KeySym)XK_Page_Down)
            {
                    position = soust(position, mult(azimut, step));
            }

            recalcModelView = GL_TRUE;
            
            orientation.normalize();
            azimut.normalize();


          break;
        }
        case ButtonPress:
          recalcModelView = GL_TRUE;
          switch (event.xbutton.button)
          {
            /*case 1: xAngle += 10.0;
              break;
            case 2: yAngle += 10.0;
              break;
            case 3: zAngle += 10.0;
              break;*/
          }
          break;
        case ConfigureNotify:
          glViewport(0, 0, event.xconfigure.width,
                     event.xconfigure.height);
          /* fall through... */
        case Expose:
          needRedraw = GL_TRUE;
          break;
      }
    } while(XPending(dpy)); /* loop to compress events */

    if (recalcModelView)
    {
      //glMatrixMode(GL_MODELVIEW);

      /* reset modelview matrix to the identity matrix */
      
      
      

      /* move the camera back three units */
      //glTranslatef(x, y, z);

      /* rotate by X, Y, and Z angles */
      //glRotatef(xAngle, 0.1, 0.0, 0.0);
      //glRotatef(yAngle, 0.0, 0.1, 0.0);
      //glRotatef(zAngle, 0.0, 0.0, 1.0);

      recalcModelView = GL_FALSE;
      needRedraw = GL_TRUE;
    }
    if (needRedraw)
    {
      redraw(&dc);
      needRedraw = GL_FALSE;
    }
  }

  return 0;
}

// ajoute tous les points d'un fichier a la liste d'affichage en cours d'édition
void addPointsChar(char* file_name, unsigned short r_color, unsigned short g_color, unsigned short b_color)
{
    double x, y, z, w;
    unsigned short r, g, b;
    long id, i;

    FILE *f = fopen(file_name, "rt");
	if  (f == NULL) return;

	long tmp_nb_points;
	double size, epsilon_pruning, X, Y, Z;
	fscanf(f, "%ld\n%lf\n%lf\n%lf %lf %lf\n", &tmp_nb_points, &size, &epsilon_pruning, &X, &Y, &Z);
	glColor3f(((double)r_color)/256,((double)g_color)/256,((double)b_color)/256);
	glBegin(GL_POINTS);
	for (i = 0 ; i < tmp_nb_points ; i++)
	{
		fscanf(f, "%ld  %lf %lf %lf  %lf  %d %d %d\n", &id, &x, &y, &z, &w, &r, &g, &b);
		glVertex3f(x, y, z);
	}
	glEnd();
	fclose(f);

	return;
}

int createDisplayList(Point* points, int nb_points, int list, unsigned short r_color, unsigned short g_color, unsigned short b_color)
{
    glNewList(list, GL_COMPILE_AND_EXECUTE);

    glPointSize(1.0);
    glColor3f(((double)r_color)/256,((double)g_color)/256,((double)b_color)/256);
    glBegin(GL_POINTS);
    for (int i = 0 ; i < nb_points ; i++)
    {
    	glVertex3f(points[i].x, points[i].y, points[i].z);
    }
    glEnd();

    glEndList();
}
