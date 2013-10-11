/*
 *   Example implementation of a QGLWidget with a separat rendering thread
 *   and GL picking capabilites.
 *
 *   This is the QGLWidget subclass implementation.
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
#include <qcolordialog.h>
#include <QMouseEvent>

#include "exampleglwidget.h"
#include "examplerenderthread.h"

ExampleGLWidget::ExampleGLWidget(QWidget *parent)
    : QGLWidget(parent),
    glt(*this)

{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

    // Buffer swap is handled in the rendering thread
    setAutoBufferSwap(false);

    // start the rendering thread
    initRendering();
}


void ExampleGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void ExampleGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // modify scence variables and render the next frame
    GLfloat dx = (GLfloat)(event->x() - lastPos.x()) / width();
    GLfloat dy = (GLfloat)(event->y() - lastPos.y()) / height();

    if (event->buttons() & Qt::LeftButton)
    {
        glt.setRotation(180 * dy, 180 * dx, 0.0);
        render();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        glt.setRotation(180 * dy,0.0,180 * dx);
        render();
    }
    lastPos = event->pos();
}

void ExampleGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // get the name of the clicked surface
    int face = glt.faceAtPosition(event->pos());
    if (face != -1)
    {
        QColor color = QColorDialog::getColor(glt.faceColors[face],
                                              this);
        if (color.isValid())
        {
            glt.faceColors[face] = color;
        }
    }
}

void ExampleGLWidget::initRendering( )
{
    // start the rendering thread
    glt.start();
    // wake the waiting render thread
    renderCondition().wakeAll();
}

void ExampleGLWidget::finishRendering( )
{
    // request stopping
    glt.stop();
    // wake up render thread to actually perform stopping
    renderCondition().wakeAll();
    // wait till the thread has exited
    glt.wait();
}

void ExampleGLWidget::closeEvent( QCloseEvent * _e )
{
    // request stopping
    finishRendering();
    // close the widget (base class)
    QGLWidget::closeEvent(_e);
}

void ExampleGLWidget::paintEvent( QPaintEvent * )
{
    render();
}

void ExampleGLWidget::resizeEvent( QResizeEvent * _e )
{
    // signal the rendering thread that a resize is needed
    glt.resizeViewport(_e->size());

    render();
}

void ExampleGLWidget::lockGLContext( )
{
    // lock the render mutex for the calling thread
    renderMutex().lock();
    // make the render context current for the calling thread
    makeCurrent();
}

void ExampleGLWidget::unlockGLContext( )
{
    // release the render context for the calling thread
    // to make it available for other threads
    doneCurrent();
    // unlock the render mutex for the calling thread
    renderMutex().unlock();
}

void ExampleGLWidget::render( )
{
    // let the wait condition wake up the waiting thread
    renderCondition().wakeAll();
}

QWaitCondition & ExampleGLWidget::renderCondition( )
{
    return(render_condition);
}

QMutex & ExampleGLWidget::renderMutex( )
{
    return(render_mutex);
}



