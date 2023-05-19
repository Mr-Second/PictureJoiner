#include "picturejoiner.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setKeyboardInputInterval(50);
  PictureJoiner w;
  w.show();
  return a.exec();
}
