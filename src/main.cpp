#include <qapplication.h>
#include "exampleglwidget.h"
#ifdef Q_WS_X11
#include <X11/Xlib.h>  // for XInitThreads() call
#endif

int main(int _argc, char* _argv[])
{
//    qDebug("start");
#ifdef Q_WS_X11
    // this needs to be the first in the app to make Xlib calls thread save
    // needed for OpenGl rendering threads
    XInitThreads();
#endif
    // init the Qt application
    QApplication app(_argc, _argv);

    // fail if there is no OpenGL support
    if (!QGLFormat::hasOpenGL())
        qFatal("This system has no OpenGL support");

    // the example QGLWidget
    ExampleGLWidget cube;
    cube.setWindowTitle(QObject::tr("Cube"));
    // resize the widget
    cube.resize(300, 300);
    // and finally show it
    cube.show();

    // enter the main event loop
    return app.exec();
}
