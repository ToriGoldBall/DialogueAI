#include "ClickableGraphicsView.h"
#include <QMouseEvent>
#include <QMimeData>
//#include <QUrl>

ClickableGraphicsView::ClickableGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
}

void ClickableGraphicsView::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    QGraphicsView::mousePressEvent(event);
}

void ClickableGraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
        setStyleSheet("border: 2px dashed #3daee0;");
    }
}
void ClickableGraphicsView::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}
void ClickableGraphicsView::dropEvent(QDropEvent* event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }
    const QString path = urls.first().toLocalFile();
    emit imageDropped(path);
    setStyleSheet("");
}

