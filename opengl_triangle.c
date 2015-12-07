#include <GL/gl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <X11/Xlib.h>
#include <stdio.h>
//create the EGL window
EGLSurface window;
EGLDisplay display;
GLuint programObj;
EGLBoolean InitializeWindow(EGLNativeWindowType surface_id, EGLNativeDisplayType display_id)
{
	const EGLint configAttributes[] = 
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE,24,
		EGL_NONE
	};

	const EGLint contextAttributes[]=
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	 display = eglGetDisplay(display_id);
	if (display == EGL_NO_DISPLAY)
	{
		printf("Fail to get the native display! \n");
		return EGL_FALSE;
	}

	EGLint major,minor;
	//EGLBoolean eglInitialize(EGLDisplay display, EGLint* majorVersion, EGLint* minorVersion);  
	if (!eglInitialize(display,&major,&minor))
	{
		printf("Fail to initialize the EGL !\n");
		return EGL_FALSE;
	}
	else
	{
		printf("The version of EGL is: %d . %d\n", major,minor);
	}

	EGLConfig config;
	EGLint ConfigNum;
	if (!eglChooseConfig(display, configAttributes, &config, 1, &ConfigNum))
	{
		return EGL_FALSE;
	}

	//EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
	//			  EGLNativeWindowType win,
	//			  const EGLint *attrib_list);

	
	window = eglCreateWindowSurface(display, config, surface_id, NULL);
	if (window == EGL_NO_SURFACE)
	{
		return EGL_FALSE;
	}

	//eglCreateContext(EGLDisplay dpy, EGLConfig config,
	//		    EGLContext share_context,
	//		    const EGLint *attrib_list);
	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
	if (context == EGL_NO_CONTEXT)
	{
		return EGL_FALSE;
	}

	//eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
	//		  EGLSurface read, EGLContext ctx);
	if (!eglMakeCurrent(display, window, window, context))
	{
		return EGL_FALSE;
	}

	return EGL_TRUE;
}

GLuint LoadShader( GLenum type, const char* shaderSrc )
{
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if (shader == 0)
	{
		return 0;
	}

	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printf("Compile ERROR!\n");
		glDeleteShader(shader);
		return 1;
	}

	return shader;
}

void DrawTriangle(GLuint obj)
{
	GLfloat vertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};
	glViewport(0, 0, 1000,800);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(obj);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	eglSwapBuffers(display, window);
	//while(1);
}

int init()
{
	const char* vertexShaderSrc =
	"attribute vec4 vPosition;	\n"
	"void main()	\n"
	"{	\n"
	"gl_Position = vPosition;		\n"
	"}	\n";

	const char* fragmentShaderSrc = 
	"precision mediump float;	\n"
	"void main()	\n"
	"{	\n"
	"gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
	"}	\n";	

	GLuint vertexShader;
	GLuint fragmentShader;
	
	GLint linked;

	vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSrc);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER,fragmentShaderSrc);
	programObj = glCreateProgram();

	if (programObj == 0)
	{
		return 0;
	}

	glAttachShader(programObj,vertexShader);
	glAttachShader(programObj,fragmentShader);
	glBindAttribLocation(programObj,0,"vPosition");

	glLinkProgram(programObj);
	glGetProgramiv(programObj,GL_LINK_STATUS,&linked);


	if (!linked )
	{
		printf("Link ERROR!\n");
		return 1;
	}

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	return 0;
}

int main()
{
	Display *x_display;
	
	x_display = XOpenDisplay(NULL);
	int screen = DefaultScreen(x_display);

	Window win = XCreateSimpleWindow(x_display, RootWindow(x_display, screen), 0, 0, DisplayWidth(x_display, screen), DisplayHeight(x_display, screen), 0, 0, WhitePixel(x_display,screen) );
	XMapWindow(x_display,win);

	if (!InitializeWindow(win, x_display))
	{
		printf("Intiallize the EGL window Fail!\n");
		return 1;
	}

	init();
	DrawTriangle(programObj);
	return 1;

}