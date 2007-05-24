
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>

#define GL_GLEXT_PROTOTYPES


#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "ttreader.h"


const int N = 400000;
const int M = 6000;  

GLfloat points[N][4]; 

const int textureWidth = 64; 
const int textureHeight = 64; 

static GLubyte texture1[textureHeight][textureWidth][4]; 
static GLuint texName1; 

static GLubyte texture2[textureHeight][textureWidth][4]; 
static GLuint texName2; 

static GLubyte color_tex[256][256][4];
static GLuint color_tex_id; 
static GLubyte depth_rb[256][256];


struct SP
{
  GLfloat p1; 
  GLfloat p2; 
  GLfloat p3; 
  GLfloat p4; 
  GLfloat ts; 
  GLbyte tchan; 
};


struct CRGBA 
{
  GLubyte R; 
  GLubyte G; 
  GLubyte B; 
  GLubyte A; 
} __attribute__((packed)) ; 

std::vector<SP> SpikePoints; 
std::vector<CRGBA> SpikeColors; 
GLfloat SpikeColors2[1000000]; 

static GLuint fbo_id; 

enum BenchType {SIMPLE, VPOINTER, VPOINTERJUSTDRAW, 
		JUSTTEXTURE, VBO, TEXTUREANDPOINTS, RENDERTOTEXTURE}; 
const BenchType BENCH = RENDERTOTEXTURE; 



//////////////////////////////////////////////////////////////////////////////
//
// OpenGL frame buffer configuration utilities.
//
//////////////////////////////////////////////////////////////////////////////

struct GLConfigUtil
{
  static void print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                              const char* attrib_str,
                              int attrib,
                              bool is_boolean);

  static void examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig);
};

//
// Print a configuration attribute.
//
void GLConfigUtil::print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                                   const char* attrib_str,
                                   int attrib,
                                   bool is_boolean)
{
  int value;

  if (glconfig->get_attrib(attrib, value))
    {
      //std::cout << attrib_str << " = ";
      //if (is_boolean)
        //std::cout << (value == true ? "true" : "false") << std::endl;
      //else
        //std::cout << value << std::endl;
    }
  else
    {
       std::cout << "*** Cannot get "
                 << attrib_str
                 << " attribute value\n";
    }
}

//
// Print configuration attributes.
//
void GLConfigUtil::examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig)
{
//   std::cout << "\nOpenGL visual configurations :\n\n";

//   std::cout << "glconfig->is_rgba() = "
//             << (glconfig->is_rgba() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->is_double_buffered() = "
//             << (glconfig->is_double_buffered() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->is_stereo() = "
//             << (glconfig->is_stereo() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->has_alpha() = "
//             << (glconfig->has_alpha() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->has_depth_buffer() = "
//             << (glconfig->has_depth_buffer() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->has_stencil_buffer() = "
//             << (glconfig->has_stencil_buffer() ? "true" : "false")
//             << std::endl;
//   std::cout << "glconfig->has_accum_buffer() = "
//             << (glconfig->has_accum_buffer() ? "true" : "false")
//             << std::endl;

//   std::cout << std::endl;

//   print_gl_attrib(glconfig, "Gdk::GL::USE_GL",           Gdk::GL::USE_GL,           true);
//   print_gl_attrib(glconfig, "Gdk::GL::BUFFER_SIZE",      Gdk::GL::BUFFER_SIZE,      false);
//   print_gl_attrib(glconfig, "Gdk::GL::LEVEL",            Gdk::GL::LEVEL,            false);
//   print_gl_attrib(glconfig, "Gdk::GL::RGBA",             Gdk::GL::RGBA,             true);
//   print_gl_attrib(glconfig, "Gdk::GL::DOUBLEBUFFER",     Gdk::GL::DOUBLEBUFFER,     true);
//   print_gl_attrib(glconfig, "Gdk::GL::STEREO",           Gdk::GL::STEREO,           true);
//   print_gl_attrib(glconfig, "Gdk::GL::AUX_BUFFERS",      Gdk::GL::AUX_BUFFERS,      false);
//   print_gl_attrib(glconfig, "Gdk::GL::RED_SIZE",         Gdk::GL::RED_SIZE,         false);
//   print_gl_attrib(glconfig, "Gdk::GL::GREEN_SIZE",       Gdk::GL::GREEN_SIZE,       false);
//   print_gl_attrib(glconfig, "Gdk::GL::BLUE_SIZE",        Gdk::GL::BLUE_SIZE,        false);
//   print_gl_attrib(glconfig, "Gdk::GL::ALPHA_SIZE",       Gdk::GL::ALPHA_SIZE,       false);
//   print_gl_attrib(glconfig, "Gdk::GL::DEPTH_SIZE",       Gdk::GL::DEPTH_SIZE,       false);
//   print_gl_attrib(glconfig, "Gdk::GL::STENCIL_SIZE",     Gdk::GL::STENCIL_SIZE,     false);
//   print_gl_attrib(glconfig, "Gdk::GL::ACCUM_RED_SIZE",   Gdk::GL::ACCUM_RED_SIZE,   false);
//   print_gl_attrib(glconfig, "Gdk::GL::ACCUM_GREEN_SIZE", Gdk::GL::ACCUM_GREEN_SIZE, false);
//   print_gl_attrib(glconfig, "Gdk::GL::ACCUM_BLUE_SIZE",  Gdk::GL::ACCUM_BLUE_SIZE,  false);
//   print_gl_attrib(glconfig, "Gdk::GL::ACCUM_ALPHA_SIZE", Gdk::GL::ACCUM_ALPHA_SIZE, false);

//   std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
// Vis scene.
//
///////////////////////////////////////////////////////////////////////////////

void CHECK_FRAMEBUFFER_STATUS()                            
{                                                           
  GLenum status;                                            
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); 
  switch(status) {                                          
  case GL_FRAMEBUFFER_COMPLETE_EXT:                       
    break;                                                
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                    
    /* choose different formats */                        
    break;                                                
  default:                                                
    /* programming error; will fail on all hardware */    
    assert(0);                                            
  }
}

class VisScene : public Gtk::GL::DrawingArea
{
public:
  explicit VisScene(bool is_sync = true);
  virtual ~VisScene();

protected:

  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);
  virtual bool on_idle();

public:
  // Invalidate whole window.
  void invalidate() {
    get_window()->invalidate_rect(get_allocation(), false);
  }

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }

protected:
  // idle signal connection:
  sigc::connection m_ConnectionIdle;

public:
  // get & set view rotation values.

protected:
  // OpenGL scene related variables:

  bool m_IsSync;

protected:
  // frame rate evaluation stuff:
  Glib::Timer m_Timer;
  int m_Frames;
};

VisScene::VisScene(bool is_sync)
  : m_IsSync(is_sync),
    m_Frames(0)
{
  //
  // Configure OpenGL-capable visual.
  //

  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
                                     Gdk::GL::MODE_DEPTH  |
                                     Gdk::GL::MODE_DOUBLE);
  if (!glconfig)
    {
      std::cerr << "*** Cannot find the double-buffered visual.\n"
                << "*** Trying single-buffered visual.\n";

      // Try single-buffered visual
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
                                         Gdk::GL::MODE_DEPTH);
      if (!glconfig)
        {
          std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
          std::exit(1);
        }
    }

  // print frame buffer attributes.
  GLConfigUtil::examine_gl_attrib(glconfig);

  //
  // Set OpenGL-capability to the widget.
  //

  set_gl_capability(glconfig);

  // Add events.
  add_events(Gdk::VISIBILITY_NOTIFY_MASK);
}

VisScene::~VisScene()
{
}

/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 * outer_radius - radius at center of teeth
 * width - width of gear
 * teeth - number of teeth
 * tooth_depth - depth of tooth
 */


void VisScene::on_realize()
{
  std::cout << "VisScene::on_realize()" << std::endl; 

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;


  //glLightfv(GL_LIGHT0, GL_POSITION, pos);
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  //glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE); 

  // Make the vis.
  
  
  
  glEnable(GL_NORMALIZE);

  switch (BENCH) 
    {
    case VPOINTER:
      glEnableClientState(GL_VERTEX_ARRAY); 
      glEnableClientState(GL_COLOR_ARRAY); 
      break; 

    case VPOINTERJUSTDRAW:
      glEnableClientState(GL_VERTEX_ARRAY); 
      glEnableClientState(GL_COLOR_ARRAY); 
      glVertexPointer(2, GL_FLOAT, sizeof(SP),
		      &(SpikePoints[0])); 
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
		     &(SpikeColors[0])); 

      break; 
    case JUSTTEXTURE:

      //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
      //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
      //glEnable(GL_LIGHTING);
      //glEnable(GL_LIGHT0);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);            
      //glEnable(GL_BLEND); 
      //glAlphaFunc(GL_GREATER,0.1); 
      //glEnable(GL_ALPHA_TEST); 
      

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
      glGenTextures(1, &texName1); 
      glBindTexture(GL_TEXTURE_2D, texName1); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, 
		   textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture1); 

      glGenTextures(1, &texName2); 
      glBindTexture(GL_TEXTURE_2D, texName2); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, 
		   textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2); 

      break; 

    case TEXTUREANDPOINTS:
      
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
      glGenTextures(1, &texName1); 
      glBindTexture(GL_TEXTURE_2D, texName1); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, 
		   textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture1); 

      
      glGenTextures(1, &texName2); 
      glBindTexture(GL_TEXTURE_2D, texName2); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, 
		   textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2); 



      break; 

    case RENDERTOTEXTURE:
      glEnableClientState(GL_VERTEX_ARRAY); 
      glEnableClientState(GL_COLOR_ARRAY); 

      glGenFramebuffersEXT(1, &fbo_id); 
      
      
//       glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
      glGenTextures(1, &color_tex_id); 

//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
//       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 
// 		   256, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_tex); 



      

      break; 

    default:
      break; 
    }

  gldrawable->gl_end();
  // *** OpenGL END ***

  // Start timer.
  m_Timer.start();
}

bool VisScene::on_configure_event(GdkEventConfigure* event)
{
  std::cout << "VisScene::on_configure_event()" << std::endl; 

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;


  glViewport(0, 0, get_width(), get_height());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 3, 0, 3, -2, 2); 

  gldrawable->gl_end();
  // *** OpenGL END ***

  return true;
}

bool VisScene::on_expose_event(GdkEventExpose* event)
{
  //std::cout << "VisScene::on_expose_event()" << std::endl; 

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch (BENCH) {
  case SIMPLE:

   glBegin(GL_POINTS); 
   for (int i = 0; i < M; i++){ 
     glColor4ubv((GLubyte*)&SpikeColors[i + ((m_Frames * M) % N) ]);  

     glVertex2fv((GLfloat*) &(SpikePoints[i + ((m_Frames * M) % N) ])); 
   }
   glEnd(); 
   break; 

  case VPOINTER: 

    glVertexPointer(2, GL_FLOAT, sizeof(SP),
		    &(SpikePoints[(m_Frames * M) % N])); 
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
 		   &(SpikeColors[(m_Frames * M) % N])); 
    glDrawArrays(GL_POINTS, 0, M); 
		   
    break;
  case VPOINTERJUSTDRAW:
    glDrawArrays(GL_POINTS, (m_Frames * M) % N, M); 
    
    
    break; 
  case JUSTTEXTURE: 

    glEnable(GL_TEXTURE_2D); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glBindTexture(GL_TEXTURE_2D, texName1); 
    glBegin(GL_QUADS); 
    glTexCoord2f(0.0, 0.0); 
    glVertex3f(0.5, 0.5, -1.0); 
    glTexCoord2f(0.0, 1.0); 
    glVertex3f(2.5, 0.5, -1.0); 
    glTexCoord2f(1.0, 1.0); 
    glVertex3f(2.5, 2.5, -1.0); 
    glTexCoord2f(1.0, 0.0); 
    glVertex3f(0.5, 2.5, -1.0); 
    
    glEnd(); 

    glEnable(GL_TEXTURE_2D); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glBindTexture(GL_TEXTURE_2D, texName2); 
    glBegin(GL_QUADS); 
    glTexCoord2f(0.0, 0.0); 
    glVertex3f(0.7, 0.6, -1.0); 
    glTexCoord2f(0.0, 1.0); 
    glVertex3f(2.7, 0.6, -1.0); 
    glTexCoord2f(1.0, 1.0); 
    glVertex3f(2.7, 2.9, -1.0); 
    glTexCoord2f(1.0, 0.0); 
    glVertex3f(0.7, 2.9, -1.0); 
    
    glEnd(); 


    break; 

  case TEXTUREANDPOINTS: 
    glEnable(GL_TEXTURE_2D); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glBindTexture(GL_TEXTURE_2D, texName1); 
    glColor3f(0.3, 0.3, 0.3); 
    glBegin(GL_QUADS); 
    glTexCoord2f(0.0, 0.0); 
    glVertex3f(0.5, 0.5, -1.0); 
    glTexCoord2f(0.0, 1.0); 
    glVertex3f(2.5, 0.5, -1.0); 
    glTexCoord2f(1.0, 1.0); 
    glVertex3f(2.5, 2.5, -1.0); 
    glTexCoord2f(1.0, 0.0); 
    glVertex3f(0.5, 2.5, -1.0); 
    
    glEnd(); 

//     glEnable(GL_TEXTURE_2D); 
//     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
//     glBindTexture(GL_TEXTURE_2D, texName2); 
//     glBegin(GL_QUADS); 
//     glTexCoord2f(0.0, 0.0); 
//     glVertex3f(0.7, 0.6, -1.0); 
//     glTexCoord2f(0.0, 1.0); 
//     glVertex3f(2.7, 0.6, -1.0); 
//     glTexCoord2f(1.0, 1.0); 
//     glVertex3f(2.7, 2.9, -1.0); 
//     glTexCoord2f(1.0, 0.0); 
//     glVertex3f(0.7, 2.9, -1.0); 
    
//     glEnd(); 
    glDisable(GL_TEXTURE_2D); 
    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_COLOR_ARRAY); 
    glVertexPointer(2, GL_FLOAT, sizeof(SP),
		    &(SpikePoints[0])); 
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
		   &(SpikeColors[0])); 



    glDrawArrays(GL_POINTS, (m_Frames * M) % N, M); 
        
    break; 


  case RENDERTOTEXTURE: 
    glClear(GL_COLOR_BUFFER_BIT); 
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id); 
    
    // initialize texture
    glBindTexture(GL_TEXTURE_2D, color_tex_id); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, 
		 GL_RGBA, GL_INT, NULL); 

    
    // attach texture to framebuffer color buffer
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			      GL_COLOR_ATTACHMENT0_EXT,
			      GL_TEXTURE_2D, color_tex_id, 0);
    
    // Check framebuffer completeness at the end of initialization.
    CHECK_FRAMEBUFFER_STATUS();
    glBindTexture(GL_TEXTURE_2D, 0); 
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id); 
    glViewport(0, 0, 512, 512); 
    glClear(GL_COLOR_BUFFER_BIT); 

    glVertexPointer(2, GL_FLOAT, sizeof(SP),
		    &(SpikePoints[(m_Frames * M) % N])); 
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
 		   &(SpikeColors[(m_Frames *M ) % N])); 
    glDrawArrays(GL_POINTS, 0, M); 

    glColor3f(1.0, 0.0, 0.0); 
     glBegin(GL_QUADS); 
     glVertex3f(1.5, 0.5, -1.0); 
     glVertex3f(2.5, 0.5, -1.0); 
     glVertex3f(2.5, 2.5, -1.0); 
     glVertex3f(1.5, 2.5, -1.0); 
    glEnd(); 
    
    // Re-enable rendering to the window
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, get_width(), get_height());
    
    glEnable(GL_TEXTURE_2D); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glBindTexture(GL_TEXTURE_2D, color_tex_id);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    glColor3f(0.3, 0.3, 0.3); 
    glBegin(GL_QUADS); 
    glTexCoord2f(0.0, 0.0); 
    glVertex3f(0.5, 0.5, -1.0); 
    glTexCoord2f(0.0, 1.0); 
    glVertex3f(2.5, 0.5, -1.0); 
    glTexCoord2f(1.0, 1.0); 
    glVertex3f(2.5, 2.5, -1.0); 
    glTexCoord2f(1.0, 0.0); 
    glVertex3f(0.5, 2.5, -1.0); 
    
    glEnd(); 

        
    break; 


  default:
    break;
  }
  // Swap buffers.
  if (gldrawable->is_double_buffered())
    gldrawable->swap_buffers();
  else
    glFlush();

  gldrawable->gl_end();
  // *** OpenGL END ***

  //
  // Print frame rate.
  //

  ++m_Frames;

  double seconds = m_Timer.elapsed();
  if (seconds >= 5.0)
    {
      // std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(3);
      std::cout << m_Frames << " frames in "
                << seconds << " seconds = "
                << (m_Frames / seconds) << " FPS\n";
      m_Timer.reset();
      m_Frames = 0;
    }

  return true;
}

bool VisScene::on_map_event(GdkEventAny* event)
{
  if (!m_ConnectionIdle.connected())
    m_ConnectionIdle = Glib::signal_idle().connect(
      sigc::mem_fun(*this, &VisScene::on_idle), GDK_PRIORITY_REDRAW);

  return true;
}

bool VisScene::on_unmap_event(GdkEventAny* event)
{
  if (m_ConnectionIdle.connected())
    m_ConnectionIdle.disconnect();

  return true;
}

bool VisScene::on_visibility_notify_event(GdkEventVisibility* event)
{
  if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
    {
      if (m_ConnectionIdle.connected())
        m_ConnectionIdle.disconnect();
    }
  else
    {
      if (!m_ConnectionIdle.connected())
        m_ConnectionIdle = Glib::signal_idle().connect(
          sigc::mem_fun(*this, &VisScene::on_idle), GDK_PRIORITY_REDRAW);
    }

  return true;
}

bool VisScene::on_idle()
{

  // Invalidate the whole window.
  invalidate();

  // Update window synchronously (fast).
  if (m_IsSync)
    update();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// The application class.
//
///////////////////////////////////////////////////////////////////////////////

class Vis : public Gtk::Window
{
public:
  explicit Vis(bool is_sync = true);
  virtual ~Vis();

protected:
  // signal handlers:
  void on_button_quit_clicked();
  virtual bool on_key_press_event(GdkEventKey* event);

protected:
  // member widgets:
  Gtk::VBox m_VBox;
  VisScene m_VisScene;
  Gtk::Button m_ButtonQuit;
};

Vis::Vis(bool is_sync)
  : m_VBox(false, 0), m_VisScene(is_sync), m_ButtonQuit("Quit")
{
  //
  // Top-level window.
  //

  set_title("Vis");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);

  add(m_VBox);

  //
  // Vis scene.
  //

  m_VisScene.set_size_request(400, 400);

  m_VBox.pack_start(m_VisScene);

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &Vis::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  //
  // Show window.
  //

  show_all();
}

Vis::~Vis()
{
}

void Vis::on_button_quit_clicked()
{
  Gtk::Main::quit();
}

bool Vis::on_key_press_event(GdkEventKey* event)
{

  m_VisScene.invalidate();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// Main.
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  //
  // Init gtkglextmm.
  //

  Gtk::GL::init(argc, argv);


  for (int i = 0; i < textureHeight; i++) 
    {
      for (int j = 0; j < textureWidth; j++)
        {
          int c = (((i & 0x4) == 0) ^ ((j & 0x04) != 0)) * 255; 
          texture1[i][j][0] = (GLubyte) c; 
          texture1[i][j][1] = (GLubyte) 0;
          texture1[i][j][2] = (GLubyte) 0; 
          texture1[i][j][3] = (GLubyte) 100; 
        }
    }

  for (int i = 0; i < textureHeight; i++) 
    {
      for (int j = 0; j < textureWidth; j++)
        {
          int c = (((i & 0x4) == 0) ^ ((j & 0x04) ==0)) * 255; 
          texture2[i][j][0] = (GLubyte) 0;
          texture2[i][j][1] = (GLubyte) c;
          texture2[i][j][2] = (GLubyte) 0;  
          texture2[i][j][3] = (GLubyte) 100; 
        }
    }

  
  // populate random points

  bool is_sync = true;
  for (int i = 0; i < N; i++) {
    points[i][0] = (2 * (rand() / (RAND_MAX + 1.0)));
    points[i][1] = (2 * (rand() / (RAND_MAX + 1.0)));
    points[i][2] = (2 * (rand() / (RAND_MAX + 1.0)));
    points[i][3] = ((rand() / (RAND_MAX + 1.0)));
  }

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--async") == 0)
      is_sync = false;
  }

  //
  // Read in spikes
  //
  ttreader ttr("../../../d118.tt"); 
  std::vector<ttspike> spikes; 
  int snum = 0; 
  std::cout << "Reading spike data " << std::endl; 
  
  while (not ttr.eof()) {
    ttspike ts; 
    ttr.getSpike(&ts); 

    SP sp1; 
    sp1.p1 = -10000.0;
    sp1.p2 = -10000.0;
    sp1.p3 = -10000.0;
    sp1.p4 = -10000.0;
      

    sp1.ts = ts.ts; 
    int trigcross = -1; 
    int thold = 250; 
    
    
    for (int i = 0; i < 32; i++) {
      if ((float)ts.w1[i] > sp1.p1) {
	sp1.p1 = (float)ts.w1[i]; 

	if (ts.w1[i] > thold and trigcross < 0 )
	  trigcross = 1; 

      }
      
      if ((float)ts.w3[i] > sp1.p2) {
	if (ts.w3[i] > thold and trigcross < 0 )
	  trigcross = 3; 

	sp1.p2 = (float)ts.w3[i]; 
      }
      
      if ((float)ts.w2[i] > sp1.p3) {
	if (ts.w2[i] > thold and trigcross < 0 )
	  trigcross = 2; 
	sp1.p3 = (float)ts.w2[i]; 
      }

      if ((float)ts.w4[i] > sp1.p4) {

	if (ts.w4[i] > thold and trigcross < 0 )
	  trigcross = 4; 
	sp1.p4 = (float)ts.w4[i]; 
      }
    }
    
    sp1.p1 = sp1.p1 * 0.005; 
    sp1.p2 = sp1.p2 * 0.005; 
    sp1.p3 = sp1.p3 * 0.005; 
    sp1.p4 = sp1.p4 * 0.005; 
    sp1.tchan = trigcross; 
    
    // color setting
    SpikePoints.push_back(sp1); 
    CRGBA c; 
    c.R = 0; 
    c.G = 0; 
    c.B = 0; 
    c.A = 100; 
     switch (trigcross) {
     case 1:
       c.R = 255; 
       break;
       
     case 2:
       c.G = 255; 
       break;
       
     case 3:
       c.B = 255; 
       break;
       
     case 4:
       c.G = 255; 
       c.R = 255; 
       break;
       
     default:
       break; 
      }
    SpikeColors.push_back(c); 

    snum++;
    if ((snum % 100000) == 0) {
      std::cout << "Read " << snum << " spikes" << std::endl;
    }
  }
    
  for(int i  = 0; i < 100000; i++){
    SpikeColors2[i] = 0.0; 
  }
  
  //
  // Instantiate and run the application.
  //

  Vis vis(is_sync);

  kit.run(vis);

  return 0;
}
