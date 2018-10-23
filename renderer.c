#include "renderer.h"
#include <unistd.h>

/**
  create window with no decorations.
ref: http://wiki.tcl.tk/13409
 **/
/* MWM decorations values */
#define MWM_DECOR_NONE          0
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

/* KDE decoration values */
enum {
  KDE_noDecoration = 0,
  KDE_normalDecoration = 1,
  KDE_tinyDecoration = 2,
  KDE_noFocus = 256,
  KDE_standaloneMenuBar = 512,
  KDE_desktopIcon = 1024 ,
  KDE_staysOnTop = 2048
};

void SetRendererName(char* deviceName)
{
  free(device_name);
  int nameLen =  strlen(deviceName);
  device_name = (char*)malloc(nameLen+1);
  memcpy(device_name, deviceName, nameLen);
  device_name[nameLen] = '\0';
  XRectangle dummy;
  XmbTextExtents(font_set, device_name, nameLen, &dummy, &boundingbox);

  XStoreName (m_display, win, deviceName);//title
}

#if 1
void Resize(int aPos, int aCount)
{
  calculate_size(aPos, aCount);
  if (!IsRenderWindowVisible())
  {
    m_isPositionSet = true;
    return;
  }

  m_isResizeDone = true;
  setWindowPos(wX, wY);
  setWindowSize(wWidth, wHeight);
  m_isResizeDone = true;
  pollEvents();
}
#endif

void calculate_size (int aPos, int aCount)
{
  Screen*  scrn = DefaultScreenOfDisplay(m_display);
  int w = XWidthOfScreen(scrn) - 1;
  int h  =  XHeightOfScreen(scrn) - 1;
  int x = 1;
  int y = 1;

  switch (aCount)
  {
    case 1:
      wX = x;
      wY = y;
      wWidth = w;
      wHeight = h;
      break;

    case 2:
      if (aPos == 1)
      {
        wX = x;
        wY = y;
        wWidth = w/2;
        wHeight = h;
      }
      else if (aPos == 2)
      {
        wX = x+ (w/2);
        wY = y;
        wWidth = w/2;
        wHeight = h;
      }
      break;
    case 3:
      if (aPos == 1)
      {
        wX = x;
        wY = y;
        wWidth = w/2;
        wHeight = h/2;
      }
      else if (aPos == 2)
      {
        wX = x+(w/2);
        wY = y;
        wWidth = w/2;
        wHeight = h/2;
      }
      else if (aPos == 3)
      {
        wX = x;
        wY = (y+h)/2;
        wWidth = w;
        wHeight = h/2;
      }
      break;
    case 4:
      if (aPos == 1)
      {
        wX = x;
        wY = y;
        wWidth = w/2;
        wHeight = h/2;
      }
      else if (aPos == 2)
      {
        wX = x+(w/2);
        wY = y;
        wWidth = w/2;
        wHeight = h/2;
      }
      else if (aPos == 3)
      {
        wX = x;
        wY = (y+h)/2;
        wWidth = w/2;
        wHeight = h/2;
      }
      else if (aPos == 4)
      {
        wX = x+(w/2);
        wY = (y+h)/2;
        wWidth = w/2;
        wHeight = h/2;
      }
      break;
  }
}

void XSetNoDecorations(Window window, Window root) {
  Atom WM_HINTS;

  WM_HINTS = XInternAtom(m_display, "_MOTIF_WM_HINTS", True);
  //succeeds
  if ( WM_HINTS != None ) {
#define MWM_HINTS_DECORATIONS   (1L << 1)
    struct {
      unsigned long flags;
      unsigned long functions;
      unsigned long decorations;
      long input_mode;
      unsigned long status;
    } MWMHints = { MWM_HINTS_DECORATIONS, 0,
      MWM_DECOR_NONE, 0, 0 };
    XChangeProperty(m_display, window, WM_HINTS, WM_HINTS, 32,
        PropModeReplace, (unsigned char *)&MWMHints,
        sizeof(MWMHints)/4);
  }

  WM_HINTS = XInternAtom(m_display, "KWM_WIN_DECORATION", True);
  if ( WM_HINTS != None ) {

    long KWMHints = KDE_tinyDecoration;
    XChangeProperty(m_display, window, WM_HINTS, WM_HINTS, 32,
        PropModeReplace, (unsigned char *)&KWMHints,
        sizeof(KWMHints)/4);
  }

  WM_HINTS = XInternAtom(m_display, "_WIN_HINTS", True);
  if ( WM_HINTS != None ) {
    long GNOMEHints = 0;
    XChangeProperty(m_display, window, WM_HINTS, WM_HINTS, 32,
        PropModeReplace, (unsigned char *)&GNOMEHints,
        sizeof(GNOMEHints)/4);
  }

  WM_HINTS = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", True);
  //succeeds
  if ( WM_HINTS != None ) {
    Atom NET_WMHints[2];
    NET_WMHints[0] = XInternAtom(m_display,
        "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE", True);
    NET_WMHints[1] = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_NORMAL", True);
    XChangeProperty(m_display, window,
        WM_HINTS, XA_ATOM, 32, PropModeReplace,
        (unsigned char *)&NET_WMHints, 2);
  }
  XSetTransientForHint(m_display, window, root);
  XUnmapWindow(m_display, window);
  XMapWindow(m_display, window);
}

/**
 *
 * @param aWidth: width of the window
 * @param aHeight: height of the window
 */
renderer_init(int aWidth, int aHeight, int thread_num)
{
  m_Width = aWidth;
  m_Height = aHeight;
  //wWidth = 700;//10;
  //wHeight = 500;//10;
  if(thread_num == 1)
  {
    wWidth = 960;//10;
    wHeight = 540;//10;
    wX = 1;//10;//1;
    wY = 1;//10;//1;
  }
  else if(thread_num == 2)
  {
    wWidth = 960;//10;
    wHeight = 540;//10;
    wX = 961;//10;//1;
    wY = 1;//10;//1;
  }
  else if(thread_num == 3)
  {
    wWidth = 960;//10;
    wHeight = 540;//10;
    wX = 1;//10;//1;
    wY = 541;//10;//1;
  }
  else if(thread_num == 4)
  {
    wWidth = 960;//10;
    wHeight = 540;//10;
    wX = 961;//10;//1;
    wY = 541;//10;//1;
  }
  m_it = 0;
  prev_wX = -1;
  prev_wY = -1;
  prev_wWidth = -1;
  prev_wHeight = -1;
  prev_m_Width = -1;
  prev_m_Height = -1;

  m_screenUpdateCount = 0;
  m_backUpPacket = NULL;
  m_rotation = -1;

  m_isResizeDone = false;

  m_display = XOpenDisplay(NULL);
  if ( m_display == NULL )
  {
    printf("XOpenDisplay failed\n");
  }
}

void setInputResolution(int aWidth, int aHeight)
{
  m_Width = aWidth;
  m_Height = aHeight;
  if(m_it)
  {
    setResolution(m_Width, m_Height);
  }
  //printf("%d %d\n", m_Width, m_Height);
}

/**
 *
 * @param :
 */
renderer_deinit()
{
  eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(m_display, m_eglContext);
  eglDestroySurface(m_display, m_eglSurface);
  eglTerminate(m_display);
  XCloseDisplay(m_display);
  if(m_it)
  {
    image_texture_deinit();
    m_it = 0;
  }
}


/**
 * initRenderer: initialize video renderer
 *
 * @param :
 *
 * @return : 0/1 success/failure
 */
int initRenderer(char* deviceName)
{
  int w = 200, h= 200;
  Window root;
  XSetWindowAttributes swa;
  XSetWindowAttributes  xattr;
  Atom wm_state;
  XWMHints hints;
  XEvent xev;

  root = DefaultRootWindow(m_display);
  //swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  swa.event_mask  =  ExposureMask;
  win = XCreateWindow(
      m_display, root,
      w>>1, 0, w, h, 0,
      CopyFromParent, InputOutput,
      CopyFromParent, CWEventMask,
      &swa );

  xattr.override_redirect = false;
  XChangeWindowAttributes ( m_display, win, CWOverrideRedirect, &xattr );

  hints.input = true;
  hints.flags = InputHint;
  XSetWMHints(m_display, win, &hints);

  // make the window visible on the screen
  XMapWindow (m_display, win);
  m_isVisible = true;

  XSetNoDecorations(win, root);

  XStoreName (m_display, win, deviceName);//title

  // get identifiers for the provided atom name strings
  wm_state = XInternAtom (m_display, "_NET_WM_STATE", false);

  memset ( &xev, 0, sizeof(xev) );
  xev.type                 = ClientMessage;
  xev.xclient.window       = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format       = 32;
  xev.xclient.data.l[0]    = 1;
  xev.xclient.data.l[1]    = false;
  XSendEvent (
      m_display,
      DefaultRootWindow ( m_display ),
      false,
      SubstructureNotifyMask,
      &xev );


  int nameLen =  strlen(deviceName);
  device_name = (char*)malloc(nameLen+1);
  memcpy(device_name, deviceName, nameLen);
  device_name[nameLen]='\0';

  xattr.background_pixel = BlackPixel(m_display, DefaultScreen(m_display));
  xattr.event_mask  =  ExposureMask;
  win_c = XCreateWindow(
      m_display, win,
      1, 1, 100, 50, 0,
      CopyFromParent, InputOutput,
      CopyFromParent, CWBackPixel | CWOverrideRedirect | CWEventMask,
      &xattr );

  XGCValues values;			/* initial values for the GC.   */
  int screen_num = DefaultScreen(m_display);
  gc = XCreateGC(m_display, win_c, 0, &values);
  XSetForeground(m_display, gc, WhitePixel(m_display, screen_num));
  XSetBackground(m_display, gc, BlackPixel(m_display, screen_num));

  char** name_list;
  int char_count;
  char* str_ret;
  font_set = XCreateFontSet(m_display, "-*-*-*-r", 
      &name_list, &char_count, &str_ret);

  XFreeStringList(name_list);
  XRectangle dummy;
  XmbTextExtents(font_set, device_name, nameLen, &dummy, &boundingbox);
  XmbDrawImageString(m_display, win_c, font_set, gc, 1-boundingbox.x, 1-boundingbox.y, deviceName, nameLen);

  XMapWindow (m_display, win_c);
  m_hWnd_c = (EGLNativeWindowType) win_c;




  m_hWnd = (EGLNativeWindowType) win;

  //openGL init
  EGLint attribList[] =
  {
    EGL_RED_SIZE,       8,
    EGL_GREEN_SIZE,     8,
    EGL_BLUE_SIZE,      8,
    EGL_ALPHA_SIZE,     EGL_DONT_CARE,
    EGL_DEPTH_SIZE,     EGL_DONT_CARE,
    EGL_STENCIL_SIZE,   EGL_DONT_CARE,
    EGL_SAMPLE_BUFFERS, 0,
    EGL_NONE
  };

  EGLint numConfigs;
  EGLint majorVersion;
  EGLint minorVersion;
  EGLDisplay display;
  EGLContext context;
  EGLSurface surface;
  EGLConfig config;
  EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

  // Get Display
  display = eglGetDisplay((EGLNativeDisplayType)m_display);
  if ( display == EGL_NO_DISPLAY )
  {
    printf("eglGetDisplay failed\n");
    return EGL_FALSE;
  }

  // Initialize EGL
  if ( !eglInitialize(display, &majorVersion, &minorVersion) )
  {
    printf("eglInitialize failed\n");
    return EGL_FALSE;
  }

  // Get configs
  if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
  {
    printf("eglGetConfigs failed\n");
    return EGL_FALSE;
  }

  // Choose config
  if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
  {
    printf("eglChooseConfig failed\n");
    return EGL_FALSE;
  }

  // Create a surface
  surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)m_hWnd, NULL);
  if ( surface == EGL_NO_SURFACE )
  {
    printf("eglCreateWindowSurface failed\n");
    return EGL_FALSE;
  }

  // Create a GL context
  context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
  if ( context == EGL_NO_CONTEXT )
  {
    printf("eglCreateContext failed\n");
    return EGL_FALSE;
  }   

  // Make the context current
  if ( !eglMakeCurrent(display, surface, surface, context) )
  {
    printf("eglMakeCurrent failed\n");
    return EGL_FALSE;
  }

  m_eglDisplay = display;
  m_eglSurface = surface;
  m_eglContext = context;
  image_texture_init(m_Width, m_Height, GL_LUMINANCE);

  m_isPositionSet = true;
  return true;
}

void pollEvents()
{
  XEvent an_event;
  GLboolean userinterrupt = GL_FALSE;

  if (m_display)
  {
    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while ( XPending ( m_display ) )
    {
      XNextEvent( m_display, &an_event );
      switch (an_event.type)
      {
        case Expose:
          {
#if 1
            //if ((m_isResizeDone) && (m_backUpPacket != NULL))
            if (m_isResizeDone)
            {
              RenderPacket_Resize(m_backUpPacket);
              m_isResizeDone = false;
            }
            else
            {
              int nameLen =  strlen(device_name);
              XmbDrawImageString(m_display, win_c, font_set, gc, 1-boundingbox.x, 1-boundingbox.y, device_name, nameLen);
            }
#endif

            //int nameLen =  strlen(device_name);
            //XmbDrawImageString(m_display, win_c, font_set, gc, 1-boundingbox.x, 1-boundingbox.y, device_name, nameLen);

            break;
          }
        case DestroyNotify:
          {
            userinterrupt = GL_TRUE;
            break;
          }
        default: /* unknown event type - ignore it. */
          break;

      }
    }
  }
}

void showWindow(bool isVisible)
{
  if(isVisible) 
  {
    XMapWindow(m_display, m_hWnd);
    m_isVisible = true;
  }
  else
  {
    XUnmapWindow(m_display, m_hWnd);
    m_isVisible = false;
    m_isPositionSet = false;
    m_screenUpdateCount = 0;
    m_isResizeDone = false;
    if (m_backUpPacket != NULL)
    {
      free(m_backUpPacket->m_packet_data);
      free(m_backUpPacket);
      m_backUpPacket = NULL;
    }
  }
}

void setWindowSize(int w, int h)
{
  XResizeWindow(m_display, m_hWnd, w, h);
}

void setWindowPos(int x, int y)
{
  XMoveWindow(m_display, m_hWnd, x, y);
}
bool IsRenderWindowVisible()
{
  return m_isVisible;
}

void GetRotatedWindowResolution(int lroration, int *lwidth, int *lheight)
{
  unsigned int w = *lwidth;
  unsigned int h = *lheight;

  if(lroration==0 || lroration==2)
  {
    return;
  }
  if(lroration==1 || lroration==3)
  {
    *lwidth = h;
    *lheight = w;
  }
}

int RenderPacket(RPacket *lPacket)
{
  unsigned char* aLuma = (unsigned char*) lPacket->m_Y;//lPacket->m_packet_data;
  int aLumaSize = lPacket->m_width;//lPacket->m_stride;
  unsigned char* aChromaB = (unsigned char*)lPacket->m_U; //aLuma + (lPacket->m_stride*lPacket->m_verStride);
  int aChromaBSize = lPacket->m_width >> 1; //lPacket->m_stride >> 1;
  unsigned char* aChromaR = (unsigned char*)lPacket->m_V; //aChromaB + ((lPacket->m_stride*lPacket->m_verStride) >> 2);
  int aChromaRSize = lPacket->m_width >> 1; //lPacket->m_stride >> 1;
  int verStride = lPacket->m_height;//lPacket->m_verStride;
  int rotation = lPacket->m_rotation;//lPacket->m_rotation;

  if(m_it==0)
    return 0;

  int x=0 , y = 0, w = 0, h = 0;
  bool isPositionSet = false;

  /*if (m_backUpPacket != NULL)
    {
    free(m_backUpPacket->m_packet_data);
    free(m_backUpPacket);
    m_backUpPacket = NULL;
    }

    m_backUpPacket = lPacket;*/

  loadImageData(aLuma,aLumaSize,aChromaB,aChromaBSize,aChromaR,aChromaRSize, lPacket->m_format, verStride);
  int visible = IsRenderWindowVisible();
  if(visible == 0)
  {
    if (m_isPositionSet)
    {
      isPositionSet = m_isPositionSet;
      m_isPositionSet = false;
      showWindow(1);
      setWindowPos(wX, wY);
      setWindowSize(wWidth, wHeight);
    }
  }

  x = wX;
  y = wY;
  w = wWidth;
  h = wHeight;

  bool doesViewPortNeedChange = x!=prev_wX || y!=prev_wY || w!=prev_wWidth || h!=prev_wHeight || prev_m_Width!=m_Width || prev_m_Height!=m_Height ||
    isPositionSet || m_rotation!=rotation;
  if (doesViewPortNeedChange || (m_screenUpdateCount == 1))
  {
    int orgViewPortWidth,orgViewPortHeight,orgViewPortX,orgViewPortY;

    setWindowPos(x, y);
    setWindowSize(w, h);

    int tmpWidth = w;
    int tmpHeight = h;
    GetRotatedWindowResolution(rotation, &tmpWidth, &tmpHeight);
    if((float)((float)m_Width/(float)m_Height)>(float)((float)tmpWidth/(float)tmpHeight))
    {
      imageWIdthInWindow = tmpWidth;
      imageHeightInWindow = (((float)m_Height*(float)imageWIdthInWindow)/(float)m_Width);
    }
    else
    {
      imageHeightInWindow = tmpHeight;
      imageWIdthInWindow = (((float)m_Width*(float)imageHeightInWindow)/(float)m_Height);
    }

    int scissorWidth = 0;
    int scissorHeight = 0;
    if(rotation==1 || rotation==3)
    {
      scissorWidth = imageHeightInWindow;
      scissorHeight = imageWIdthInWindow;

      orgViewPortHeight = (float)(((float)aLumaSize*(float)imageWIdthInWindow)/(float)m_Width);
      orgViewPortWidth = (float)(((float)verStride*(float)imageHeightInWindow)/(float)m_Height);
      if(rotation==3)
      {
        orgViewPortX = ((float)w - imageHeightInWindow)/2;            
        orgViewPortY = (h-imageWIdthInWindow)/2 + (imageWIdthInWindow-orgViewPortHeight-1) ;
      }
      else
      {
        orgViewPortX = ((float)w - imageHeightInWindow)/2 + (imageHeightInWindow-orgViewPortWidth-1) ;            
        orgViewPortY = (h-imageWIdthInWindow)/2;
      }
    }
    else
    {
      scissorWidth = imageWIdthInWindow;
      scissorHeight = imageHeightInWindow;

      orgViewPortWidth = (float)(((float)aLumaSize*(float)imageWIdthInWindow)/(float)m_Width);
      orgViewPortHeight = (float)(((float)verStride*(float)imageHeightInWindow)/(float)m_Height);
      if(rotation==2 )
      {
        orgViewPortX = ((float)w - imageWIdthInWindow)/2 + (imageWIdthInWindow-orgViewPortWidth-1);            
        orgViewPortY = (h-imageHeightInWindow)/2;
      }
      else
      {
        orgViewPortX = ((float)w - imageWIdthInWindow)/2;            
        orgViewPortY = (h-imageHeightInWindow)/2 + (imageHeightInWindow-orgViewPortHeight-1);
      }
    }

    int nameLen =  strlen(device_name);
    int width11 = XmbTextEscapement(font_set, device_name, nameLen);
    XMoveWindow(m_display, m_hWnd_c, (orgViewPortX+orgViewPortWidth)-width11-10, orgViewPortY);
    XResizeWindow(m_display, m_hWnd_c, width11+2, boundingbox.height+2);

    glEnable(GL_SCISSOR_TEST);
    glViewport ( orgViewPortX, orgViewPortY, orgViewPortWidth, orgViewPortHeight);
    glScissor(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor((w - scissorWidth)/2, (h-scissorHeight)/2, scissorWidth-1, scissorHeight-1);  

    m_screenUpdateCount++;
    m_screenUpdateCount = m_screenUpdateCount%2;

    prev_wWidth = w;
    prev_wHeight = h;
    prev_wX = x;
    prev_wY = y;
    prev_m_Width = m_Width;
    prev_m_Height = m_Height;
    m_rotation = rotation;
  }

  renderTexture(rotation);

  //printf("Render Texture\n");
  pollEvents();
  eglSwapBuffers(m_eglDisplay, m_eglSurface);
  return 0;
}

int RenderPacket_Resize(RPacket *lPacket)
{
  int aLumaSize = m_Width;
  int verStride = m_Height;
  int rotation = m_rotation;

  if(m_it==0)
    return 0;

  int x=0 , y = 0, w = 0, h = 0;

  x = wX;
  y = wY;
  w = wWidth;
  h = wHeight;

  bool doesViewPortNeedChange = x!=prev_wX || y!=prev_wY || w!=prev_wWidth || h!=prev_wHeight;

  if (doesViewPortNeedChange || (m_screenUpdateCount == 1))
  {
    int orgViewPortWidth,orgViewPortHeight,orgViewPortX,orgViewPortY;

    setWindowPos(x, y);
    setWindowSize(w, h);

    int tmpWidth = w;
    int tmpHeight = h;
    GetRotatedWindowResolution(rotation, &tmpWidth, &tmpHeight);
    if((float)((float)m_Width/(float)m_Height)>(float)((float)tmpWidth/(float)tmpHeight))
    {
      imageWIdthInWindow = tmpWidth;
      imageHeightInWindow = (((float)m_Height*(float)imageWIdthInWindow)/(float)m_Width);
    }
    else
    {
      imageHeightInWindow = tmpHeight;
      imageWIdthInWindow = (((float)m_Width*(float)imageHeightInWindow)/(float)m_Height);
    }

    int scissorWidth = 0;
    int scissorHeight = 0;
    if(rotation==1 || rotation==3)
    {
      scissorWidth = imageHeightInWindow;
      scissorHeight = imageWIdthInWindow;

      orgViewPortHeight = (float)(((float)aLumaSize*(float)imageWIdthInWindow)/(float)m_Width);
      orgViewPortWidth = (float)(((float)verStride*(float)imageHeightInWindow)/(float)m_Height);
      if(rotation==3)
      {
        orgViewPortX = ((float)w - imageHeightInWindow)/2;            
        orgViewPortY = (h-imageWIdthInWindow)/2 + (imageWIdthInWindow-orgViewPortHeight-1) ;
      }
      else
      {
        orgViewPortX = ((float)w - imageHeightInWindow)/2 + (imageHeightInWindow-orgViewPortWidth-1) ;            
        orgViewPortY = (h-imageWIdthInWindow)/2;
      }
    }
    else
    {
      scissorWidth = imageWIdthInWindow;
      scissorHeight = imageHeightInWindow;

      orgViewPortWidth = (float)(((float)aLumaSize*(float)imageWIdthInWindow)/(float)m_Width);
      orgViewPortHeight = (float)(((float)verStride*(float)imageHeightInWindow)/(float)m_Height);
      if(rotation==2 )
      {
        orgViewPortX = ((float)w - imageWIdthInWindow)/2 + (imageWIdthInWindow-orgViewPortWidth-1);            
        orgViewPortY = (h-imageHeightInWindow)/2;
      }
      else
      {
        orgViewPortX = ((float)w - imageWIdthInWindow)/2;            
        orgViewPortY = (h-imageHeightInWindow)/2 + (imageHeightInWindow-orgViewPortHeight-1);
      }
    }

    int nameLen =  strlen(device_name);
    int width11 = XmbTextEscapement(font_set, device_name, nameLen);
    XMoveWindow(m_display, m_hWnd_c, (orgViewPortX+orgViewPortWidth)-width11-10, orgViewPortY);
    XResizeWindow(m_display, m_hWnd_c, width11+2, boundingbox.height+2);

    glEnable(GL_SCISSOR_TEST);
    glViewport ( orgViewPortX, orgViewPortY, orgViewPortWidth, orgViewPortHeight);
    glScissor(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor((w - scissorWidth)/2, (h-scissorHeight)/2, scissorWidth-1, scissorHeight-1);  

    m_screenUpdateCount++;
    m_screenUpdateCount = m_screenUpdateCount%2;

    prev_wWidth = w;
    prev_wHeight = h;
    prev_wX = x;
    prev_wY = y;
    prev_m_Width = m_Width;
    prev_m_Height = m_Height;
    m_rotation = rotation;
  }

  renderTexture(rotation);
  eglSwapBuffers(m_eglDisplay, m_eglSurface);
  return 0;
}
