#ifndef MIS2X264_H
#define MIS2X264_H

#include <QObject>
#include <QProcess>
#include <QMainWindow>
#include "ui_Mis2x264.h"


class Mis2x264 : public QMainWindow
{
  Q_OBJECT
  public:
    Mis2x264(QWidget *parent = 0, int mode = 0, QString inputFile = QString());
    ~Mis2x264();

  private:
    Ui::Mis2x264Class ui;
    QProcess *m_process;
    QString m_mediaInfoOutput;
    QString m_mediaInfo;
    QString m_inputFile;
    void analyseMediaInfoOutput();
    void convertTox264(QString settings);
    void analyzeInput();

  private slots:
    void on_convertPushButton_clicked();
    void on_importPushButton_clicked();
    void on_setMediaInfoPushButton_clicked();
    void collectOutput();
    void processFinished(const int exitCode, const QProcess::ExitStatus exitStatus);
};

#endif // MIS2X264_H
