#ifndef PICTUREJOINER_H
#define PICTUREJOINER_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class PictureJoiner;
}
QT_END_NAMESPACE

class PictureJoiner : public QWidget {
  Q_OBJECT

public:
  PictureJoiner(QWidget *parent = nullptr);
  ~PictureJoiner();

public slots:
  void setPercentValue(int);
  void updateGraphicsItemMargin(int);
  void updateGraphicsItemRadius(int);
  void updateGraphicsItemBorderThickness(int);
  void updateRowNum(QString);
  void updateColNum(QString);
  void updateHorizontalText(QString);
  void updateVerticalText(QString);

  void resetAll() noexcept;
  void save() noexcept;

private:
  void updateText();

  Ui::PictureJoiner *ui;

  void updateGraphicsView();

  QGraphicsScene *scene;
  int rowNum;
  int colNum;
  QVector<QPixmap> images;
  QTimer *timer;

  int marginValue;
  QString horizontalText;
  QString verticalText;
};
#endif // PICTUREJOINER_H
