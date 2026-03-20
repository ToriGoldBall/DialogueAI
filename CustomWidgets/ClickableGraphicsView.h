#pragma once

#include <QGraphicsView>

//a custom class for supporting mouse events on an image view in order to change it
class ClickableGraphicsView : public QGraphicsView
{
  Q_OBJECT

public:
    explicit ClickableGraphicsView(QWidget* parent = nullptr);

signals:
    void clicked();
    void imageDropped(const QString& path);
protected:
    void mousePressEvent(QMouseEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};
