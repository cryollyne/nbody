#ifndef CANVAS_H
#define CANVAS_H

#include <qt/QtQuick/QQuickItem>
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>


class Renderer : public QObject, protected QOpenGLFunctions_4_3_Core {
    Q_OBJECT
public:
    static Renderer *getRenderer();
    ~Renderer();

    void setT(qreal t) {m_t = t;}
    void setViewportSize(const QSize &size) {m_viewportSize = size;}
    void setWindow(QQuickWindow *win) { m_window = win;}

public slots:
    void init();
    void paint();
    
private:
    Renderer()
        : m_t(0)
        , m_program(nullptr)
    {}
    static Renderer *singleton;

    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window = nullptr;
};



class Canvas : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    QML_ELEMENT

public:
    Canvas()
        : m_t(0)
        , m_renderer(nullptr)
    {
        connect(
            this,
            &QQuickItem::windowChanged,
            this,
            &Canvas::handleWindowChanged
        );
    }

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;
    qreal m_t;
    Renderer *m_renderer;
};


#endif // !CANVAS_H
