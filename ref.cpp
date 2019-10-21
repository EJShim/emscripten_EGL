#include <iostream>
#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>


/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1 
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2 
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8




//context variables
GLint viewWidth, viewHeight;
EGLNativeWindowType hWnd;
EGLDisplay eglDisplay;
EGLContext eglContext;
EGLSurface eglSurface;
GLuint programObject;

static Display *x_display = NULL;


EGLBoolean CreateXWindow(const char *title, GLint width, GLint height)
{
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    Window win;

    viewWidth = width;
    viewHeight = height;

    /*
     * X11 native display initialization
     */
    x_display = XOpenDisplay(NULL);
    if ( x_display == NULL )
    {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(
               x_display, root,
               0, 0, viewWidth, viewHeight, 0,
               CopyFromParent, InputOutput,
               CopyFromParent, CWEventMask,
               &swa );

    xattr.override_redirect = false;
    XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, title);

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", false);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = false;
    XSendEvent (
       x_display,
       DefaultRootWindow ( x_display ),
       false,
       SubstructureNotifyMask,
       &xev );

    hWnd = (EGLNativeWindowType) win;

    std::cout << root << "<" << hWnd << std::endl;
    return EGL_TRUE;
}


EGLBoolean InitializeEGLContext(){
    
    
    //ESCreateWindow
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)x_display);
    if(eglDisplay == EGL_NO_DISPLAY){
        std::cout << "no display" << std::endl;
        return EGL_FALSE;
    }

    //Initialize EGL
    EGLint majorVersion;
    EGLint minorVersion;
    if ( !eglInitialize(eglDisplay, &majorVersion, &minorVersion) ){
        return EGL_FALSE;
    }
    std::cout << majorVersion << "." << minorVersion << std::endl;


    ///Get Configs
    EGLint numConfigs;
    // Get configs
    if ( !eglGetConfigs(eglDisplay, NULL, 0, &numConfigs) )
    {
        return EGL_FALSE;
    }
    std::cout << numConfigs << std::endl;


    EGLConfig config;
    EGLint attribList[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,     8,
        EGL_ALPHA_SIZE,     8,
        EGL_DEPTH_SIZE,     8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };
    // Choose config
    if ( !eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs) )
    {
        
        std::cout << "something wrong in chooseconfig" << std::endl;
        return EGL_FALSE;
    }


    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)hWnd, NULL);
    if ( eglSurface == EGL_NO_SURFACE )
    {
        
    std::cout << "no surface " << std::endl;
    return EGL_FALSE;
    }

    // Create a GL context
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE };
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs );
    if ( eglContext == EGL_NO_CONTEXT )
    {
    std::cout << "no context " << std::endl;
        return EGL_FALSE;
    
    }   

    // Make the context current
    if ( !eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) )
    {
        std::cout << "failed to make current" << std::endl;
        return EGL_FALSE;

    }

    return EGL_TRUE;
}

GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char*)malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );         
         
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

int Init(){
    GLbyte vShaderStr[] =  
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";
    
    GLbyte fShaderStr[] =  
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";
        
    
   GLuint vertexShader;
   GLuint fragmentShader;
   GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader ( GL_VERTEX_SHADER, (const char*)vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, (const char*)fShaderStr );

    // Create the program object
    programObject = glCreateProgram ( );


    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );
 
    // Bind vPosition to attribute 0   
    glBindAttribLocation ( programObject, 0, "vPosition" );

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    
   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char*)malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );      
         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return GL_FALSE;
   }

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return GL_TRUE;
}


void Draw ()
{
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f, 
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };

   // No clientside arrays, so do this in a webgl-friendly manner
   GLuint vertexPosObject;
   glGenBuffers(1, &vertexPosObject);
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
   
   // Set the viewport
   glViewport ( 0, 0, viewWidth, viewHeight );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( programObject );

   // Load the vertex data
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
   glEnableVertexAttribArray(0);

   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

void MainLoop( void drawFunc() ){
    drawFunc();
}



int main(){
    
    //Create Window
    CreateXWindow("test title", 1000, 1000);
    InitializeEGLContext();


    //Initialize GL
    Init();


    MainLoop(Draw);


    return 0;
}