/*
 *   Example implementation of a QGLWidget with a separat rendering thread
 *   and GL picking capabilites.
 *
 *   This is the GL rendering thread implementation.
 *
 *   Copyright (C) 2005 by
 *   Michael Hanke        michael.hanke@gmail.com
 *
 *   This example combines two other examples on using Qt for OpenGL
 *   - QGLWidget example (single threaded, GL picking)
 *     from the book 'C++  GUI Programming with Qt 3' by
 *     Jasmin Blanchette and Mark Summerfield
 *     The example code 'cube' available from here:
 *        ftp://ftp.trolltech.com/misc/qtbook-examples.zip
 *   - Qt Quarterly article 'Glimpsing the Third Dimension' on multithreaded
 *     OpenGL with Qt (see here: http://doc.trolltech.com/qq/qq06-glimpsing.html)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "examplerenderthread.h"
#include "exampleglwidget.h"


ExampleRenderThread::ExampleRenderThread( ExampleGLWidget & _glw )
        : QThread(),
        glw(_glw),
        render_flag(true),
        resize_flag(false),
        viewport_size(_glw.size())
{
    // example implemenation init
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
    faceColors[0] = Qt::red;
    faceColors[1] = Qt::green;
    faceColors[2] = Qt::blue;
    faceColors[3] = Qt::cyan;
    faceColors[4] = Qt::yellow;
    faceColors[5] = Qt::magenta;

}

void ExampleRenderThread::resizeViewport( const QSize& _size )
{
    // set size and flag to request resizing
    viewport_size = _size;
    resize_flag = true;
}

void ExampleRenderThread::stop( )
{
    // set flag to request thread to exit
    // REMEMBER: The thread needs to be woken up once
    // after calling this method to actually exit!
    render_flag = false;
}

void ExampleRenderThread::run( )
{
    // lock the render mutex of the Gl widget
    // and makes the rendering context of the glwidget current in this thread
    glw.lockGLContext();

    // general GL init
    initializeGL();

    // do as long as the flag is true
    while( render_flag )
    {
        // resize the GL viewport if requested
        if (resize_flag)
        {
            resizeGL(viewport_size.width(), viewport_size.height());
            resize_flag = false;
        }

        // render code goes here
        paintGL();

        // swap the buffers of the GL widget
        glw.swapBuffers();

        glw.doneCurrent(); // release the GL render context to make picking work!

        // wait until the gl widget says that there is something to render
        // glwidget.lockGlContext() had to be called before (see top of the function)!
        // this will release the render mutex until the wait condition is met
        // and will lock the render mutex again before exiting
        // waiting this way instead of insane looping will not waste any CPU ressources
        glw.renderCondition().wait(&glw.renderMutex());

        glw.makeCurrent(); // get the GL render context back

        // DEACTIVATED -- alternatively render a frame after a certain amount of time
        // prevent to much continous rendering activity
        // msleep(16); //sleep for 16 ms
    }
    // unlock the render mutex before exit
    glw.unlockGLContext();
}

void ExampleRenderThread::initializeGL()
{
    // typical OpenGL init
    // see OpenGL documentation for an explanation
    glw.qglClearColor(Qt::black);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void ExampleRenderThread::resizeGL(int width, int height)
{
    // nothing special
    // see OpenGL documentation for an explanation
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = (GLfloat)width / height;
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void ExampleRenderThread::paintGL()
{
    // clear all and draw the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}

void ExampleRenderThread::draw()
{
    // draws the cube
    static const GLfloat coords[6][4][3] =
        {
            {
                {
                    +1.0, -1.0, +1.0
                }
                , { +1.0, -1.0, -1.0 },
                { +1.0, +1.0, -1.0 }, { +1.0, +1.0, +1.0 }
            },
            { { -1.0, -1.0, -1.0 }, { -1.0, -1.0, +1.0 },
              { -1.0, +1.0, +1.0 }, { -1.0, +1.0, -1.0 } },
            { { +1.0, -1.0, -1.0 }, { -1.0, -1.0, -1.0 },
              { -1.0, +1.0, -1.0 }, { +1.0, +1.0, -1.0 } },
            { { -1.0, -1.0, +1.0 }, { +1.0, -1.0, +1.0 },
              { +1.0, +1.0, +1.0 }, { -1.0, +1.0, +1.0 } },
            { { -1.0, -1.0, -1.0 }, { +1.0, -1.0, -1.0 },
              { +1.0, -1.0, +1.0 }, { -1.0, -1.0, +1.0 } },
            { { -1.0, +1.0, +1.0 }, { +1.0, +1.0, +1.0 },
              { +1.0, +1.0, -1.0 }, { -1.0, +1.0, -1.0 } }
        };

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);

    for (int i = 0; i < 6; ++i)
    {
        // assign names for each surface
        // this make picking work
        glLoadName(i);
        glBegin(GL_QUADS);
        glw.qglColor(faceColors[i]);
        for (int j = 0; j < 4; ++j)
        {
            glVertex3f(coords[i][j][0], coords[i][j][1],
                       coords[i][j][2]);
        }
        glEnd();
    }
}

int ExampleRenderThread::faceAtPosition(const QPoint &pos)
{
    // we need to lock the rendering context
    glw.lockGLContext();

    // this is the same as in every OpenGL picking example
    const int MaxSize = 512; // see below for an explanation on the buffer content
    GLuint buffer[MaxSize];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    // enter select mode
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((GLdouble)pos.x(),
                  (GLdouble)(viewport[3] - pos.y()),
                  5.0, 5.0, viewport);
    GLfloat x = (GLfloat)viewport_size.width() / viewport_size.height();
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    draw();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();


    // finally release the rendering context again
    if (!glRenderMode(GL_RENDER))
    {
        glw.unlockGLContext();
        return -1;
    }
    glw.unlockGLContext();

    // Each hit takes 4 items in the buffer.
    // The first item is the number of names on the name stack when the hit occured.
    // The second item is the minimum z value of all the verticies that intersected
    // the viewing area at the time of the hit. The third item is the maximum z value
    // of all the vertices that intersected the viewing area at the time of the hit
    // and the last item is the content of the name stack at the time of the hit
    // (name of the object). We are only interested in the object name
    // (number of the surface).

    // return the name of the clicked surface
    return buffer[3];
}

void ExampleRenderThread::setRotation( GLfloat _x, GLfloat _y, GLfloat _z )
{
    rotationX += _x;
    rotationY += _y;
    rotationZ += _z;
}
