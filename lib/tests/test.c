#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>


// X stuff
static Display* g_xdisplay;
static Window	g_xroot;
// X window
static Window	g_xwindow;
static char*	g_xwindow_title = "GLES example";
static int	g_xwindow_width = 500;
static int	g_xwindow_height = 500;
// EGL stuff
static EGLDisplay g_egldisplay;
static EGLContext g_eglcontext;
static EGLSurface g_eglsurface;
static EGLConfig  g_eglconfig;
// GLESv2
static GLuint vertex_shader;
static GLuint fragment_shader;
static GLuint program_object;
// functions
static void draw_geometry ();
static int init_pipeline ();

int 
main (int argc, char** argv)
{
    // 1. X stuff
    //1.1 X display
    g_xdisplay = XOpenDisplay (NULL);
    if (g_xdisplay == NULL)
    {
        return EGL_FALSE;
    }
    g_xroot = DefaultRootWindow (g_xdisplay);
    //1.2 X window
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    g_xwindow = XCreateWindow (g_xdisplay, g_xroot, 0, 0, g_xwindow_width, g_xwindow_height, 0,
			      CopyFromParent, InputOutput, CopyFromParent, CWEventMask,
                              &swa);
    XSetWindowAttributes xattr;
    xattr.override_redirect = 0;
    XChangeWindowAttributes (g_xdisplay, g_xwindow, CWOverrideRedirect, &xattr);
    XWMHints hints;
    hints.input = 1;
    hints.flags = InputHint;
    XSetWMHints(g_xdisplay, g_xwindow, &hints);
    XMapWindow (g_xdisplay, g_xwindow);
    XStoreName (g_xdisplay, g_xwindow, g_xwindow_title);
    Atom wm_state;
    wm_state = XInternAtom (g_xdisplay, "_NET_WM_STATE", 0);
    // 2. EGL stuff
    // 2.1 egl display.
    g_egldisplay = eglGetDisplay ((EGLNativeDisplayType)g_egldisplay);
    if (g_egldisplay == EGL_NO_DISPLAY)
    {
	return EGL_FALSE;
    }
    // 2.2 Initialize EGL
    EGLint major_version;
    EGLint minor_version;
    if (!eglInitialize (g_egldisplay, &major_version, &minor_version))
    {
	return EGL_FALSE;
    }
    // 2.3 configuration management
    EGLint num_configs;
    EGLint attrib_list[] = {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_NONE
    };
    if (!eglGetConfigs (g_egldisplay, NULL, 0, &num_configs))
    {
	return EGL_FALSE;
    }
    if (!eglChooseConfig (g_egldisplay, attrib_list, &g_eglconfig, 1, &num_configs))
    {
	return EGL_FALSE;
    }
    // 2.4  surface creation
    g_eglsurface = eglCreateWindowSurface (g_egldisplay, g_eglconfig, (EGLNativeWindowType)g_xwindow, NULL);
    if (g_eglsurface == EGL_NO_SURFACE)
    {
	return EGL_FALSE;
    }
    // 2.5  Create a GL context
    EGLint context_attribs[] = { 
	EGL_CONTEXT_CLIENT_VERSION, 2, 
	EGL_NONE, EGL_NONE 
    };
    g_eglcontext = eglCreateContext (g_egldisplay, g_eglconfig, EGL_NO_CONTEXT, context_attribs);
    if (g_eglcontext == EGL_NO_CONTEXT)
    {
	return EGL_FALSE;
    }   
    // 2.6 Make the context current
    if (!eglMakeCurrent(g_egldisplay, g_eglsurface, g_eglsurface, g_eglcontext))
    {
	return EGL_FALSE;
    }

    //3. init graphics pipeline.
    if (!init_pipeline ())
	return GL_FALSE;

    //4. Main Loop
    int state = 1;
    XEvent xev;
    while (state)
    {
	XNextEvent (g_xdisplay, &xev);
	switch (xev.type)
	{
	    case Expose:
		draw_geometry ();
		break;
	    case DestroyNotify:
		state = 0;
		break;
	    default:
		draw_geometry ();
		break;
	}
	// we must call eglSwapBuffers to show the back buffer.
	eglSwapBuffers (g_egldisplay, g_eglsurface);
   }
}

static GLuint 
load_shader (GLenum type, const char* shader_src)
{
    GLuint shader;

    shader = glCreateShader (type);
    if (shader == 0)
	return 0;
    glShaderSource (shader, 1, &shader_src, NULL);
    glCompileShader (shader);

    GLint compiled;
    glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) 
    {
	GLint info_len = 0;
	glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 1)
	{
	    char* info_log = malloc (sizeof(char) * info_len);
	    glGetShaderInfoLog (shader, info_len, NULL, info_log);
	    fprintf (stderr, "Error compiling shader:\n%s\n", info_log);            
	    free (info_log);
	}
	glDeleteShader (shader);
	return 0;
    }
    return shader;
}

static int 
init_pipeline ()
{
    GLbyte vertex_shader_src[] =  
      "attribute vec4 vPosition;    \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vPosition;  \n"
      "}                            \n";
    GLbyte fragment_shader_src[] =  
      "precision mediump float;\n"\
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
      "}                                            \n";

    vertex_shader = load_shader (GL_VERTEX_SHADER, vertex_shader_src);
    fragment_shader = load_shader (GL_FRAGMENT_SHADER, fragment_shader_src);
    program_object = glCreateProgram ();
    if (program_object == 0)
	return 0;
    glAttachShader (program_object, vertex_shader);
    glAttachShader (program_object, fragment_shader);

    glBindAttribLocation (program_object, 0, "vPosition");
    glLinkProgram (program_object);

    GLint linked;
    glGetProgramiv (program_object, GL_LINK_STATUS, &linked);
    if (!linked) 
    {
	GLint info_len = 0;
	glGetProgramiv (program_object, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 1)
	{
	    char* info_log = malloc (sizeof(char) * info_len);
	    glGetProgramInfoLog (program_object, info_len, NULL, info_log);
	    fprintf (stderr, "Error linking program:\n%s\n", info_log);            
            free (info_log);
	}
	glDeleteProgram (program_object);
	return GL_FALSE;
    }
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

static void 
draw_geometry ()
{
    GLfloat vertices[] = {
	0.0f,  0.5f, 0.0f, 
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f 
    };
    glViewport (0, 0, g_xwindow_width, g_xwindow_height);
    glClear (GL_COLOR_BUFFER_BIT);

    glUseProgram (program_object);
    
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray (0);
    
    glDrawArrays (GL_TRIANGLES, 0, 3);
}
