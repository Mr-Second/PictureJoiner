#include "imageitem.h"

ImageItem::ImageItem() {
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

void ImageItem::updateGeometry(QSize sz) noexcept
{
    this->m_size = sz;
}

void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget) {
    painter->drawPixmap(m_pos, m_pixelmap);
}

QRectF ImageItem::boundingRect() const { return QRectF(m_pos, m_size); }
