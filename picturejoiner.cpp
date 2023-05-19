#include "picturejoiner.h"
#include "ui_picturejoiner.h"

#include "graphicsGridItem.h"

#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsGridLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QIntValidator>
#include <QPainter>
#include <QSignalBlocker>
#include <QSlider>
#include <QTimer>
#include <QVariant>

PictureJoiner::PictureJoiner(QWidget *parent)
    : QWidget(parent), ui(new Ui::PictureJoiner) {
  ui->setupUi(this);
  auto shadow = new QGraphicsDropShadowEffect();
  shadow->setBlurRadius(10);
  shadow->setXOffset(0);
  shadow->setYOffset(0);
  shadow->setColor(QColor(0, 0, 0, 128));
  ui->graphicsView->setGraphicsEffect(shadow);

  ui->marginSlider->setProperty("type", 1);
  ui->paddingSlider->setProperty("type", 2);
  ui->radiusSlider->setProperty("type", 3);

  auto *validator = new QIntValidator(1, 50, this);
  ui->colLine->setValidator(validator);
  ui->rowLine->setValidator(validator);

  rowNum = ui->rowLine->text().toInt();
  colNum = ui->colLine->text().toInt();

  scene = new QGraphicsScene(this);
  ui->graphicsView->setScene(scene);
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);

  images.resize(rowNum * colNum);

  for (int row = 0; row < rowNum; ++row)
    for (int col = 0; col < colNum; ++col) {
      auto idx = row * rowNum + col;
      images[idx].load(":/images/add.svg");
    }

  ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  QFont font1("Times New Roman", 10);
  QFont font2("Cambria", 10);
  QFont font3("Helvetica", 10);
  QFont font4("黑体", 10);
  QFont font5("宋体", 10);
  QFont font6("楷体", 10);

  ui->comboBox->setItemData(0, font1, Qt::FontRole); // 为第一项设置字体
  ui->comboBox->setItemData(1, font2, Qt::FontRole); // 为第二项设置字体
  ui->comboBox->setItemData(2, font3, Qt::FontRole); // 为第三项设置字体
  ui->comboBox->setItemData(3, font4, Qt::FontRole); // 为第一项设置字体
  ui->comboBox->setItemData(4, font5, Qt::FontRole); // 为第二项设置字体
  ui->comboBox->setItemData(5, font6, Qt::FontRole); // 为第三项设置字体

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PictureJoiner::updateGraphicsView);
  timer->setSingleShot(true);
  timer->start(60);
}

PictureJoiner::~PictureJoiner() {
  timer->deleteLater();
  scene->deleteLater();
  delete ui;
}

void PictureJoiner::setPercentValue(int val) {
  auto sender_obj = sender();
  auto slider_obj = qobject_cast<QSlider *>(sender_obj);
  if (slider_obj != nullptr) {
    auto property = slider_obj->property("type");
    bool ok;
    auto type = property.toInt(&ok);
    if (!ok)
      throw;
    switch (type) {
    case 1:
      ui->marginLabel->setText(QString::number(val).append('%'));
      break;
    case 2:
      ui->paddingLabel->setText(QString::number(val).append('%'));
      break;
    case 3:
      ui->radiusLabel->setText(QString::number(val).append('%'));
      break;
    default:
      throw;
    }
  }
}

void PictureJoiner::updateGraphicsItemMargin(int val) {
  int marginDelta = val - marginValue;
  marginValue = val;

  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->changeMargin(marginDelta);
    }
  }
  ui->graphicsView->update();
}

void PictureJoiner::updateGraphicsItemRadius(int val) {
  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->changeRadius(val);
    }
  }
  ui->graphicsView->update();
}

void PictureJoiner::updateGraphicsItemBorderThickness(int val) {
  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->changeBorderWidth(val);
    }
  }
  ui->graphicsView->update();
}

void PictureJoiner::updateRowNum(QString num) {
  rowNum = num.toInt();
  updateGraphicsView();
}

void PictureJoiner::updateColNum(QString num) {
  colNum = num.toInt();
  updateGraphicsView();
}

void PictureJoiner::updateHorizontalText(QString text) {
  horizontalText = text;
  updateText();
}

void PictureJoiner::updateVerticalText(QString text) {
  verticalText = text;
  updateText();
}

void PictureJoiner::updateText() {
  if (!horizontalText.isEmpty()) {
    QRectF gridRect = scene->itemsBoundingRect();
    QPointF centerBottom(gridRect.x() + gridRect.width() / 2,
                         gridRect.y() + gridRect.height());
    auto textItem = new GraphicsTextItem(horizontalText);

    QFont font(ui->comboBox->currentText());
    textItem->setFont(font);

    auto textRect = textItem->boundingRect();
    textItem->setPos(centerBottom.x() - textRect.width() / 2,
                     centerBottom.y() + 10);
    scene->addItem(textItem);
  }

  if (!verticalText.isEmpty()) {
    QRectF gridRect = scene->itemsBoundingRect();
    QPointF centerLeft(gridRect.x(), gridRect.y() + gridRect.height() / 2);
    auto textItem = new GraphicsTextItem(verticalText);

    QFont font(ui->comboBox->currentText());
    textItem->setFont(font);
    textItem->setRotation(90);

    auto textRect = textItem->boundingRect();
    textItem->setPos(centerLeft.x() - textRect.height() + 15,
                     centerLeft.y() - textRect.width() / 2);
    scene->addItem(textItem);
  }
}

void PictureJoiner::resetAll() noexcept {
  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->reset();
    }
  }
  ui->graphicsView->update();

  QSignalBlocker mblocker(ui->marginSlider);
  QSignalBlocker rblocker(ui->radiusSlider);
  QSignalBlocker pblocker(ui->paddingSlider);

  ui->marginSlider->setValue(0);
  ui->radiusSlider->setValue(0);
  ui->paddingSlider->setValue(0);

  mblocker.unblock();
  rblocker.unblock();
  pblocker.unblock();

  emit ui->marginSlider->valueChanged(0);
  emit ui->radiusSlider->valueChanged(0);
  emit ui->paddingSlider->valueChanged(0);

  QSignalBlocker rowblocker(ui->rowLine);
  QSignalBlocker colblocker(ui->colLine);
  ui->rowLine->setText("2");
  ui->colLine->setText("2");
  rowblocker.unblock();
  colblocker.unblock();

  emit ui->rowLine->textChanged("2");
  emit ui->colLine->textChanged("2");

  this->updateGraphicsView();
}

void PictureJoiner::save() noexcept {
  QRectF sceneRect = scene->itemsBoundingRect();
  QPixmap pixmap(sceneRect.size().toSize());
  pixmap.fill(QColor(0, 0, 0, 0));

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);

  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->beginRender();
    }
  }

  scene->render(&painter, QRectF(), sceneRect);

  for (QGraphicsItem *item : scene->items()) {
    auto subItem = qgraphicsitem_cast<GraphicsGridItem *>(item);
    if (subItem) {
      subItem->endRender();
    }
  }

  QString saveFileName = QFileDialog::getSaveFileName(
      this, "Save Image to", "C:/", "Images (*.png *.jpeg *.jpg *.bmp)");
  pixmap.save(saveFileName);
}

void PictureJoiner::updateGraphicsView() {
  int gridSize =
      qMin(ui->graphicsView->height() - 10, ui->graphicsView->width() - 10) /
      qMax(rowNum, colNum);
  int startX = (ui->graphicsView->width() - gridSize * colNum) / 2;
  int startY = (ui->graphicsView->height() - gridSize * rowNum) / 2;

  scene->clear();

  marginValue = ui->marginSlider->value();
  auto radiusValue = ui->radiusSlider->value();
  auto borderThickNess = ui->paddingSlider->value();

  for (int row = 0; row < rowNum; ++row)
    for (int col = 0; col < colNum; ++col) {
      int x = startX + col * gridSize;
      int y = startY + row * gridSize;

      auto graphiGridItem = new GraphicsGridItem(QSize(gridSize, gridSize));
      graphiGridItem->setPos(x, y);
      graphiGridItem->changeMargin(marginValue);
      graphiGridItem->changeRadius(radiusValue);
      graphiGridItem->changeBorderWidth(borderThickNess);
      graphiGridItem->setNumber(row, col);
      scene->addItem(graphiGridItem);
    }
  updateText();

  scene->setSceneRect(scene->itemsBoundingRect());
  ui->graphicsView->setSceneRect(scene->sceneRect());

  ui->graphicsView->update();
  ui->graphicsView->show();
}
