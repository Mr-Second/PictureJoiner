#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include <QGraphicsItem>
#include <QPainter>
#include <QSize>

class ImageItem : public QGraphicsItem {
  Q_OBJECT
public:
  ImageItem();
  void setPixelMap(QString &path) noexcept {
      m_pixelmap = QPixmap(path);
  };

  void updateGeometry(QSize sz) noexcept;

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;
  virtual QRectF boundingRect() const override;

private:
  QPointF m_pos;
  QPixmap m_pixelmap;
  QSize m_size;
};

#endif // IMAGEITEM_H
