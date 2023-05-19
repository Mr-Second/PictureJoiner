#ifndef GRAPHICSGRIDITEM_H
#define GRAPHICSGRIDITEM_H

#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>
#include <QWidget>
#include <qtmetamacros.h>

class GraphicsGridItem : public QGraphicsItem {
public:
  GraphicsGridItem(QSize sz, QGraphicsItem *parent = nullptr)
      : QGraphicsItem(parent), rect_{-sz.width() / 2.0, -sz.height() / 2.0,
                                     qreal(sz.width()), qreal(sz.height())} {
    setFlag(QGraphicsItem::ItemIsFocusable, true);
  }

  QRectF boundingRect() const override { return rect_; }

  void changeMargin(int margin) {
    qreal marginF = -margin * 0.2;
    rect_ = rect_.marginsAdded(QMarginsF(marginF, marginF, marginF, marginF));
    this->update();
  }

  void changeBorderWidth(int width) { borderThickness = 0.02 * width * 5 + 1; }

  void changeRadius(int r) {
    radius = 0.01 * r * (rect_.width() / 2.0);
    this->update();
  }

  void mousePressEvent(QGraphicsSceneMouseEvent *e) override {
    this->setZValue(1);
    if (e->button() == Qt::LeftButton) {
      return;
    }
    QMenu menu;
    auto imageAction = menu.addAction(QIcon(":/images/image.png"), "添加图片");
    auto colorAction = menu.addAction(QIcon(":/images/bucket.png"), "填充颜色");

    QAction *selectedAction = menu.exec(e->screenPos());
    if (selectedAction == imageAction) {
      QString fileName = QFileDialog::getOpenFileName(
          nullptr, "Open Image", ".", "Image Files (*.png *.jpg *.bmp)");
      if (fileName.isEmpty() || !QFileInfo::exists(fileName))
        return;
      resetImageAndColor();
      imgFileName = fileName;
      img.load(fileName);
      this->update();
    } else if (selectedAction == colorAction) {
      auto clr = QColorDialog::getColor(Qt::white, nullptr, "Select Color");
      if (clr.isValid()) {
        resetImageAndColor();
        color = clr;
        this->update();
      }
    }
  }

  void focusOutEvent(QFocusEvent *) override { this->setZValue(0); }

  void keyPressEvent(QKeyEvent *e) override {
    switch (e->key()) {
    case Qt::Key_Up:
      offset.setHeight(offset.height() - 1);
      break;
    case Qt::Key_Left:
      offset.setWidth(offset.width() - 1);
      break;
    case Qt::Key_Right:
      offset.setWidth(offset.width() + 1);
      break;
    case Qt::Key_Down:
      offset.setHeight(offset.height() + 1);
      break;
    default:
      QGraphicsItem::keyPressEvent(e);
    }
    this->update();
  }

  void keyReleaseEvent(QKeyEvent *e) override {
    if (e->isAutoRepeat())
      return;
    else
      QGraphicsItem::keyReleaseEvent(e);
  }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
             QWidget *) override {

    QPen pen;
    pen.setWidth(borderThickness);
    if (hasFocus() || borderThickness == 0) {
      pen.setStyle(Qt::SolidLine);
      pen.setColor(QColor(0, 92, 249));
    } else {
      pen.setStyle(Qt::DashLine);
      pen.setColor(QColor(191, 219, 254));
    }
    painter->setPen(pen);
    QPainterPath path;
    path.addRoundedRect(rect_, radius, radius);

    if (color.isValid())
      painter->fillPath(path, QBrush(color));
    painter->setClipPath(path);

    if (img.isNull())
      painter->drawPath(path);

    if (!color.isValid() && img.isNull()) {
      QPointF center = rect_.center();

      // 绘制十字
      pen.setStyle(Qt::SolidLine);
      painter->setPen(pen);
      painter->drawLine(center.x(), center.y() - 10, center.x(),
                        center.y() + 10); // 竖线
      painter->drawLine(center.x() - 10, center.y(), center.x() + 10,
                        center.y()); // 横线
    } else if (!img.isNull()) {
      auto scaledImage =
          img.scaled(boundingRect().size().toSize(), Qt::KeepAspectRatio,
                     Qt::SmoothTransformation); // 进行等比缩放
      QPointF imgPos = boundingRect().center() - scaledImage.rect().center();

      auto newPos = imgPos + QPointF(offset.width(), offset.height());
      if (rect_.contains(newPos)) {
        imgPos = newPos;
      } else {
        offset.setHeight(0);
        offset.setWidth(0);
      }

      painter->drawPixmap(imgPos, scaledImage);
    }
    if (borderThickness != 0) {
      pen.setStyle(Qt::DashLine);
      painter->setPen(pen);
      painter->drawPath(path);
    }
  }

  void reset() noexcept {
    radius = 0;
    borderThickness = 1.0;
    resetImageAndColor();
  }

  void resetImageAndColor() noexcept {
    color = QColor();
    img = QPixmap();
  }

  void setNumber(int x, int y) noexcept {
    number.setX(x);
    number.setY(y);
  }

  QPoint getNumber() const noexcept { return number; }

  QString getFilePath() const noexcept { return imgFileName; }

  void beginRender() noexcept {
    thickness_backup = borderThickness;
    borderThickness = 0;
    this->update();
  }

  void endRender() noexcept {
    borderThickness = thickness_backup;
    this->update();
  }

private:
  QPoint number;

  QRectF rect_;
  qreal radius{0};
  qreal borderThickness{1.0};
  qreal thickness_backup;

  QString imgFileName;
  QPixmap img;
  QColor color;

  QSizeF offset{0, 0};
};

class GraphicsTextItem : public QGraphicsTextItem {
public:
  explicit GraphicsTextItem(QString &text, QGraphicsItem *parent = nullptr)
      : QGraphicsTextItem(text, parent) {
    setFlag(QGraphicsItem::ItemIsFocusable, true);
  }

  void keyPressEvent(QKeyEvent *e) override {
    switch (e->key()) {
    case Qt::Key_Up:
      this->setPos(this->x(), this->y() - 1);
      break;
    case Qt::Key_Left:
      this->setPos(this->x() - 1, this->y());
      break;
    case Qt::Key_Right:
      this->setPos(this->x() + 1, this->y());
      break;
    case Qt::Key_Down:
      this->setPos(this->x(), this->y() + 1);
      break;
    default:
      GraphicsTextItem::keyPressEvent(e);
    }
    this->update();
  }

  //  void keyReleaseEvent(QKeyEvent *e) override {
  //    //    if (e->isAutoRepeat())
  //    //      return;
  //    //    else
  //    //    GraphicsTextItem::keyReleaseEvent(e);
  //  }
};
#endif // GRAPHICSGRIDITEM_H
