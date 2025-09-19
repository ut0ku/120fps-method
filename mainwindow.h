#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QSettings>
#include <QMimeData>
#include <QDesktopServices>
#include <QUrl>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <functional>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void on_processButton_clicked();
    void on_helpButton_clicked();
    void on_selectFileButton_clicked();
    void on_saveAsButton_clicked();
    void on_tiktokButton_clicked();
    void on_tgButton_clicked();
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_langButton_clicked();

private:
    QSettings settings;
    QString currentFilePath;
    QString saveFilePath;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    double videoFps;
    bool isFpsDetected;
    QTranslator translator;
    bool isRussian;

    size_t patch_mvhd(QByteArray &buf, int divider);
    size_t patch_mdhd(QByteArray &buf, int divider);
    void processFile(const QString &filePath);
    void resetUI();
    void detectVideoFps(const QString &filePath);
    void loadLanguage(const QString &language);
    void updateLanguageButton();

    QLabel *filePathLabel;
    QPushButton *saveAsButton;
    QPushButton *selectFileButton;


};

#endif // MAINWINDOW_H
