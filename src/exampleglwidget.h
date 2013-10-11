#ifndef EXAMPLEGLWIDGET_H
#define EXAMPLEGLWIDGET_H

#include "examplerenderthread.h"
#include <qgl.h>
#include <qmutex.h>
#include <qwaitcondition.h>

/** QGLWidget subclass with a separate rendering thread and GL picking capabilities.
 *
 */
class ExampleGLWidget : public QGLWidget
{
public:
    ExampleGLWidget(QWidget *parent = 0);
    /** Starts the rendering thread.
     * This method is called in the contructor.
     */
    void initRendering();
    /** Stops the rendering thread of the widget. */
    void finishRendering();
    /** Call this method before doing any OpenGL rendering from a thread.
     * This method will aquire the GL rendering context for the calling thread.
     * Rendering will only be possible from this thread until unlockGLContext()
     * is called from the same thread.
     */
    void lockGLContext();
    /** Call this method to release the rendering context again after calling lockGLContext().
     */
    void unlockGLContext();
    /** Returns a reference to the render wait condition.
     * This is only for internal purpose (render thread communication)
     */
    QWaitCondition& renderCondition();
    /** Returns a reference to the render context mutex.
     * This is only for internal purpose (render thread communication)
     */
    QMutex& renderMutex();

public slots:
    /** Cause the rendering thread to render one frame of the OpenGL scene.
     * This method is thread save.
     * \warning If the rendering thread is currently rendering (not idle) when this method is called
     * NO additional new frame will be rendered afterwards!
     */
    void render();

protected:
    /** Handles mouse press events on the QGLWidget. */
    void mousePressEvent(QMouseEvent *event);
    /** Handles mouse move events on the QGLWidget. */
    void mouseMoveEvent(QMouseEvent *event);
    /** Handles mouse double click events on the QGLWidget. */
    void mouseDoubleClickEvent(QMouseEvent *event);
    /** Performs a save shutdown if the widget recieves a close event. */
    void closeEvent(QCloseEvent* _e);
    /** Calls render() if the widget recieves a paint event. */
    void paintEvent(QPaintEvent*);
    /** Requests a GL viewport resize from the rendering thread. */
    void resizeEvent(QResizeEvent* _e);

private:
    /** The rendering thread. */
    ExampleRenderThread glt;
    /** Mutex for protecting the GL rendering context for multithreading. */
    QMutex render_mutex;
    /** The rendering thread uses this wait condition to save CPU ressources. */
    QWaitCondition render_condition;

    // for the example implementation
    QPoint lastPos;
};

#endif // EXAMPLEGLWIDGET_H
