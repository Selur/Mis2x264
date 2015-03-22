#include "Mis2x264.h"
#include "Converter.h"
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

Mis2x264::Mis2x264(QWidget *parent, int mode, QString inputFile)
    : QMainWindow(parent), m_process(0), m_mediaInfoOutput(QString()), m_mediaInfo(QString())
{
  ui.setupUi(this);
  this->setWindowTitle("Mis2x264 " + QString::fromLocal8Bit(BUILDVERSION) + " - by Selur");
  if (mode == 1) {
    delete ui.encodingSettingsLabel;
    delete ui.convertPushButton;
    delete ui.lineEdit;
    QString path = qApp->applicationDirPath();
    path += QDir::separator();
#ifdef Q_OS_WIN
    path += "mediainfo.exe";
#else
    path += "mediainfo";
#endif
    path = QDir::toNativeSeparators(path);
    if (QFile::exists(path)) {
      m_mediaInfo = path;
      ui.textBrowser->append("m_mediaInfo: " + m_mediaInfo);
      delete ui.mediaInfoLabel;
      delete ui.mediaInfoLineEdit;
      delete ui.setMediaInfoPushButton;
    }
  } else {
    delete ui.mediaInfoLabel;
    delete ui.mediaInfoLineEdit;
    delete ui.importPushButton;
    delete ui.setMediaInfoPushButton;
    ui.textBrowser->append(
        tr("if you call Mis2x264 with a 1 as parameter it will open in 'File Input Mode'"));
  }
  ui.textBrowser->append(tr("see you over at http://forum.selur.de"));
  ui.textBrowser->append("<br/>");
  if (mode == 1 && !m_mediaInfo.isEmpty() && !inputFile.isEmpty()) {
    m_inputFile = inputFile;
    this->analyzeInput();
  }
}

Mis2x264::~Mis2x264()
{

}

void Mis2x264::convertTox264(QString settings)
{
  if (settings.isEmpty()) {
    return;
  }
  QString converted = Converter::encodingSettingsToX264(settings);
  if (!ui.textBrowser->toPlainText().isEmpty()) {
    ui.textBrowser->append("<br/><br/>");
  }
  QString text = "<b>" + tr("MediaInfo - Encoding settings:") + "</b><br/>  " + settings;
  ui.textBrowser->append(text);
  text = "<br/><b>" + tr("Converted to x264 cli:") + "</b><br/>  " + converted;
  ui.textBrowser->append(text);
}

void Mis2x264::on_convertPushButton_clicked()
{
  this->convertTox264(ui.lineEdit->text().trimmed());
}

void removeStartOfLine(QString &line)
{
  line = line.remove(0, line.indexOf(":") + 1);
  line = line.trimmed();
}
void Mis2x264::analyseMediaInfoOutput()
{
  QStringList lines = m_mediaInfoOutput.split("\n", QString::SkipEmptyParts);
  QString settings, tmp;
  bool video = false;
  QString colorSpace;
  foreach(QString line, lines)
  {
    if (!video) {
      if (line.startsWith("Video #") || (line.startsWith("Video") && line.trimmed().size() == 5)) {
        video = true;
      }
      continue;
    }
    if (line.startsWith("Format profile")) {
      removeStartOfLine(line);
      QStringList elems = line.split("@");
      if (elems.count() == 2) {
        tmp = elems.at(1);
        tmp = tmp.remove("L");
        settings = "level=" + tmp + " / " + settings;
        tmp = elems.at(0).simplified();
        tmp = tmp.remove(":");
        tmp = tmp.toLower();
        settings = "profile=" + tmp + " / " + settings;
      }
      continue;
    }
    if (line.startsWith("Frame rate") && line.contains("fps")) {
      removeStartOfLine(line);
      line = line.remove("fps");
      line = line.trimmed();
      settings += "fps=" + line + " / ";
      continue;
    }
    if (line.startsWith("Matrix coefficients")) {
      removeStartOfLine(line);
      if (line != "RGB") {
        line = "YUV";
      }
      colorSpace = line;
      continue;
    }
    if (line.startsWith("Color") && !colorSpace.isEmpty()) {
      if (colorSpace != "YUV") {
        line = "rgb";
      } else {
        line = line.remove(":");
        line = "i" + line;
      }
      settings += "color=" + line + " / " + tmp + settings;
      continue;
    }
    if (line.startsWith("Bit Depth")) {
      removeStartOfLine(line);
      if (!line.endsWith("bits")) {
        line += " bits";
      }
      ui.textBrowser->append(tr("Encoding needs to be done with a %1 x264 version.").arg(line));
      continue;
    }
    if (line.startsWith("Encoding settings")) {
      removeStartOfLine(line);
      settings += line;
      convertTox264(settings);
      return;
    }
  }
  QString message = tr("MediaInfo output didn't contain any 'Encoding setting'(2)!");
  QMessageBox::critical(this, tr("Error"), message);
  ui.textBrowser->append("<b>" + message + "</b>");
  ui.textBrowser->append(lines.join("<b/>"));
}

void Mis2x264::collectOutput()
{
  m_mediaInfoOutput += QString::fromUtf8(m_process->readAllStandardOutput().data());
  m_mediaInfoOutput += QString::fromUtf8(m_process->readAllStandardError().data());
}
void Mis2x264::processFinished(const int exitCode, const QProcess::ExitStatus exitStatus)
{
  ui.textBrowser->append(tr("MediaInfo finished,.."));
  if (exitCode < 0) {
    QMessageBox::critical(this, tr("MediaInfo crash"),
        tr("MediaInfo crashed with exitCode: %1 and status: %2 while analysing the input!").arg(
            exitCode).arg(exitStatus));
    return;
  }
  m_process->disconnect();
  m_process->kill();
  delete m_process;
  m_process = 0;
  this->analyseMediaInfoOutput();
}

void Mis2x264::on_setMediaInfoPushButton_clicked()
{
  QString input = QFileDialog::getOpenFileName(this, tr("Select the MediaInfo binary"), QString());
  if (input.isEmpty()) {
    return;
  }
  ui.mediaInfoLineEdit->setText(input);
}
void Mis2x264::analyzeInput()
{
  if (m_inputFile.isEmpty()) {
    ui.textBrowser->append(tr("Nothing to do, input is empty,.."));
    return;
  }
  if (!QFile::exists(m_inputFile)) {
    QMessageBox::information(this, tr("Info"), tr("File %1 doesn't exist!").arg(m_inputFile));
    return;
  }
  m_mediaInfoOutput.clear();
  delete m_process;
  m_process = new QProcess(this);
  QObject::connect(m_process, SIGNAL(finished( int, QProcess::ExitStatus)), this,
      SLOT(processFinished(int, QProcess::ExitStatus)));
  QObject::connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(collectOutput()));
  QObject::connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(collectOutput()));
  QString call = "\"" + m_mediaInfo + "\" --full \"" + m_inputFile + "\"";
  ui.textBrowser->append(call);
  m_process->start(call);
}

void Mis2x264::on_importPushButton_clicked()
{
  QString mediaInfo;
  if (m_mediaInfo.isEmpty()) {
    mediaInfo = ui.mediaInfoLineEdit->text();
    if (mediaInfo.isEmpty()) {
      QMessageBox::information(this, tr("Info"),
          tr("You need to set the MediaInfo location before you can import anything!"));
      return;
    }
    m_mediaInfo = mediaInfo;
  }
  m_inputFile = QFileDialog::getOpenFileName(this, tr("Select input file"), QString());
  this->analyzeInput();
}
