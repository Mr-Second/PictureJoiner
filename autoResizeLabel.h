#ifndef AUTORESIZELABEL_H
#define AUTORESIZELABEL_H
#include <QLabel>
#include <QObject>

class AutoResizeLabel : public QLabel {
public:
  AutoResizeLabel(QWidget *parent = nullptr) : QLabel(parent) {
    //    this->setFixedSize(this->size());
  }
  //  virtual void setText(const QString &text) {
  //    qDebug() << this->size();
  //    // 获取当前字体
  //    QFont font = this->font();
  //    // 计算文本的宽度
  //    QFontMetrics fm(font);
  //    int text_width = fm.horizontalAdvance(text);
  //    // 如果文本宽度大于标签宽度，则缩小字号
  //    while (text_width > this->width()) {
  //      font.setPointSize(font.pointSize() - 1);
  //      QFontMetrics fm(font);
  //      text_width = fm.horizontalAdvance(text);
  //    }

  //    // 设置字体和文本
  //    this->setFont(font);
  //    QLabel::setText(text);
  //  }

protected:
  virtual void resizeEvent(QResizeEvent *e) override {
    QLabel::resizeEvent(e);
    QFont font = this->font();
    QFontMetrics fm(font);
    int text_width = fm.horizontalAdvance(this->text());
    int label_width = this->width();

    if (text_width > label_width) {
      int font_size = font.pointSize();
      while (text_width > label_width && font_size > 6) {
        font_size -= 1;
        font.setPointSize(font_size);
        QFontMetrics fm(font);
        text_width = fm.horizontalAdvance(this->text());
      }
      this->setFont(font);
    }
  }
};
#endif // AUTORESIZELABEL_H
