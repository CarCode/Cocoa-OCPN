#include <qgl.h>
#include <qapplication.h>
#include <qevent.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

/*	Create checkerboard image	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
GLubyte checkImage[checkImageHeight][checkImageWidth][3];

static GLdouble zoomFactor = 1.0;
static GLint myHeight;
static GLint screeny = 0, screenx = 0;

class ImageWidget : public QGLWidget
{
public:
    ImageWidget( QWidget *parent=0, const char *name=0 );

protected:
    void initializeGL( void );
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e );
    void makeCheckImage( void );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );

private:
    bool down;
};

ImageWidget::ImageWidget( QWidget *parent, const char *name )
     : QGLWidget( parent, name )
{
}

void ImageWidget::mousePressEvent( QMouseEvent *e )
{
    down = TRUE;
    if( down == TRUE )
    {
        screeny = myHeight - (GLint)e->y();
        screenx = (GLint)e->x();
        updateGL();
    }}

void ImageWidget::mouseReleaseEvent( QMouseEvent *e )
{
    down = FALSE;
}

void ImageWidget::mouseMoveEvent( QMouseEvent *e )
{
    if( down == TRUE )
    {
        screeny = myHeight - (GLint)e->y();
        screenx = (GLint)e->x();
        updateGL();
    }
}

void ImageWidget::initializeGL(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void ImageWidget::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   //glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
   glRasterPos2i (screenx, screeny);
   glPixelZoom (zoomFactor, zoomFactor);
   glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
   glFlush ();
}

void ImageWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   myHeight = (GLint) h;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void ImageWidget::keyPressEvent( QKeyEvent *e )
{
   switch ( e->key() )
   {
      case Key_R:
         zoomFactor = 1.0;
         updateGL();
         printf ("zoomFactor reset to 1.0\n");
         break;
      case Key_Z:
         zoomFactor += 0.5;
         if (zoomFactor >= 3.0)
            zoomFactor = 3.0;
         updateGL();
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case Key_X:
         zoomFactor -= 0.5;
         if (zoomFactor <= 0.5)
            zoomFactor = 0.5;
         updateGL();
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case Key_Escape:
         exit(0);
         break;
      default:
         break;
   }
}

void ImageWidget::makeCheckImage(void)
{
   int i, j, c;

   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = (((i&0x8)==0)^((j&0x8)==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
      }
   }
}

int main(int argc, char** argv)
{
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() )
    {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    ImageWidget l;
    a.setMainWidget( &l );
    l.show();
    return a.exec();
}
