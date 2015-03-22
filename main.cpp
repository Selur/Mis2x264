#include "Mis2x264.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int mode = 0;
    if (argc == 2) {
      mode = QString(argv[1]).toInt();
    }
    QString inputFile;
    if (argc == 3) {
      mode = QString(argv[1]).toInt();
      inputFile = QString(argv[2]);
    }
    Mis2x264 w(0, mode, inputFile);
    w.show();
    return a.exec();
}
