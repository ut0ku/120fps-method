#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>
#include <QtEndian>
#include <QDesktopServices>
#include <QPalette>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include "hovereventfilter.h"
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QEventLoop>
#include <QTimer>
#include <QCursor>
#include <QTranslator>

#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , settings("Ut0ku", "120FPSPatcher")
    , videoFps(0)
    , isFpsDetected(false)
    , isRussian(false)
    , filePathLabel(nullptr)
    , saveAsButton(nullptr)
    , selectFileButton(nullptr)
{
    mediaPlayer = new QMediaPlayer(this);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);

    // UI
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *topButtonsLayout = new QHBoxLayout();
    topButtonsLayout->setAlignment(Qt::AlignRight);
    topButtonsLayout->setContentsMargins(0, 0, 0, 0);
    topButtonsLayout->setSpacing(12);

    // icons
    QPushButton *tgButton = new QPushButton(this);
    QPushButton *tiktokButton = new QPushButton(this);
    QPushButton *langButton = new QPushButton(this);
    QPushButton *helpButton = new QPushButton(this);

    tgButton->setIcon(QIcon(":/icons/icons/telegram.svg"));
    tiktokButton->setIcon(QIcon(":/icons/icons/tiktok.svg"));
    langButton->setIcon(QIcon(":/icons/icons/language_en.svg"));
    helpButton->setIcon(QIcon(":/icons/icons/guide.svg"));

    tgButton->setIconSize(QSize(32, 32));
    tiktokButton->setIconSize(QSize(32, 32));
    langButton->setIconSize(QSize(32, 32));
    helpButton->setIconSize(QSize(32, 32));

    // lang btn
    langButton->setObjectName("langButton");

    QString buttonStyle = "QPushButton {"
                          "background: #1c1c1c;"
                          "border: 2px solid #333;"
                          "border-radius: 10px;"
                          "width: 40px;"
                          "height: 40px;"
                          "font-weight: bold;"
                          "}"
                          "QPushButton:hover {"
                          "border-color: #e63946;"
                          "background: #2a2a2a;"
                          "}";

    tgButton->setStyleSheet(buttonStyle);
    tiktokButton->setStyleSheet(buttonStyle);
    langButton->setStyleSheet(buttonStyle);
    helpButton->setStyleSheet(buttonStyle);

    qDebug() << "Telegram icon exists:" << QFile::exists(":/icons/icons/telegram.svg");
    qDebug() << "Telegram icon exists:" << QFile::exists(":/icons/icons/tiktok.svg");
    qDebug() << "Telegram icon exists:" << QFile::exists(":/icons/icons/language_en.svg");
    qDebug() << "Telegram icon exists:" << QFile::exists(":/icons/icons/guide.svg");

    // Top btns shadow
    auto addButtonShadow = [](QPushButton* button) {
        QString normalStyle = button->styleSheet();
        QString hoverStyle = normalStyle;
        hoverStyle.replace("border: 2px solid #333;", "border: 2px solid #e63946;");

        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(button);
        shadowEffect->setBlurRadius(30);
        shadowEffect->setColor(QColor(230, 57, 70, 0)); // Opacity
        shadowEffect->setOffset(0, 0);
        button->setGraphicsEffect(shadowEffect);

        QPropertyAnimation* shadowAnimation = new QPropertyAnimation(shadowEffect, "color", button);
        shadowAnimation->setDuration(200);
        shadowAnimation->setEasingCurve(QEasingCurve::OutQuad);

        button->setAttribute(Qt::WA_Hover);
        button->installEventFilter(new HoverEventFilter(button, [=](bool entered) {
            if (entered) {
                shadowAnimation->stop();
                shadowAnimation->setStartValue(QColor(230, 57, 70, 0));
                shadowAnimation->setEndValue(QColor(230, 57, 70, 180));
                shadowAnimation->start();

                button->setStyleSheet(hoverStyle);
            } else {
                shadowAnimation->stop();
                shadowAnimation->setStartValue(shadowEffect->color());
                shadowAnimation->setEndValue(QColor(230, 57, 70, 0));
                shadowAnimation->start();

                button->setStyleSheet(normalStyle);
            }
            return false;
        }));
    };

    addButtonShadow(tgButton);
    addButtonShadow(tiktokButton);
    addButtonShadow(langButton);
    addButtonShadow(helpButton);

    topButtonsLayout->addWidget(tgButton);
    topButtonsLayout->addWidget(tiktokButton);
    topButtonsLayout->addWidget(langButton);
    topButtonsLayout->addWidget(helpButton);

    QWidget *container = new QWidget(this);
    container->setFixedWidth(450); // 450
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 15, 0, 0);
    containerLayout->setSpacing(0);

    QWidget *titleGroup = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleGroup);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);

    QLabel *titleLabel = new QLabel("120 FPS METHOD");
    QFont titleFont("DM Serif Display", 36, QFont::Bold); // 42
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("margin: 0; padding: 0;");

    QLabel *subtitleLabel = new QLabel("by <span style='font-weight:bold;'>ut0ku</span>");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    QFont subtitleFont("DM Serif Display", 19); // 20
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("margin: 0; padding: 0; margin-top: -8px;");
    subtitleLabel->setMinimumHeight(3); // 22

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    containerLayout->addWidget(titleGroup, 0, Qt::AlignHCenter);
    containerLayout->addSpacing(25);

    QWidget *dropContainer = new QWidget();
    dropContainer->setObjectName("dropContainer");
    QVBoxLayout *dropContainerLayout = new QVBoxLayout(dropContainer);
    containerLayout->setContentsMargins(15, 15, 15, 15);

    QFrame *dropArea = new QFrame();
    dropArea->setObjectName("dropArea");
    dropArea->setFrameShape(QFrame::StyledPanel);
    dropArea->setFixedHeight(165); // 170

    QGraphicsDropShadowEffect* dropShadow = new QGraphicsDropShadowEffect(dropArea);
    dropShadow->setBlurRadius(30); // 5
    dropShadow->setColor(QColor(230, 57, 70, 150));
    dropShadow->setOffset(0, 0);
    dropShadow->setEnabled(false);
    dropArea->setGraphicsEffect(dropShadow);

    dropContainer->setStyleSheet("QWidget#dropContainer {"
                                 "background: transparent;"
                                 "border: none;"
                                 "}");

    QVBoxLayout *dropLayout = new QVBoxLayout(dropArea);
    dropLayout->setSpacing(0);
    dropLayout->setContentsMargins(20, 20, 20, 20);

    containerLayout->addWidget(dropArea);
    containerLayout->addWidget(dropContainer);

    QPushButton *selectFileButton = new QPushButton(tr("Select File"));
    selectFileButton->setFixedSize(180, 36);
    QLabel *filePathLabel = new QLabel(tr("or drop MP4 file here"));
    filePathLabel->setAlignment(Qt::AlignCenter);
    filePathLabel->setStyleSheet("color: #ccc;");

    dropArea->setStyleSheet("QFrame#dropArea {"
                            "background: #121212;"
                            "border: 2px dashed #444;"
                            "border-radius: 10px;"
                            "}"
                            "QFrame#dropArea:hover {"
                            "border-color: #e63946;"
                            "}");

    QString mainButtonStyle = "QPushButton {"
                              "background: #1c1c1c;"
                              "color: white;"
                              "border: 2px solid #333;"
                              "border-radius: 10px;"
                              "font-weight: 600;"
                              "}"
                              "QPushButton:hover {"
                              "background: #e63946;"
                              "border-color: #e63946;"
                              "}";

    selectFileButton->setStyleSheet(mainButtonStyle);

    // Select file shadow
    QGraphicsDropShadowEffect* selectFileShadow = new QGraphicsDropShadowEffect(selectFileButton);
    selectFileShadow->setBlurRadius(30);
    selectFileShadow->setColor(QColor(230, 57, 70, 180));
    selectFileShadow->setOffset(0, 0);
    selectFileShadow->setEnabled(false);
    selectFileButton->setGraphicsEffect(selectFileShadow);

    QPropertyAnimation* selectFileShadowAnim = new QPropertyAnimation(selectFileShadow, "color", selectFileButton);
    selectFileShadowAnim->setDuration(200);
    selectFileShadowAnim->setEasingCurve(QEasingCurve::OutQuad);

    selectFileButton->installEventFilter(new HoverEventFilter(selectFileButton,
                                                              [selectFileShadow, selectFileShadowAnim, dropShadow, dropArea, selectFileButton](bool entered) {
                                                                  QString normalStyle = "QPushButton {"
                                                                                        "background: #1c1c1c;"
                                                                                        "color: white;"
                                                                                        "border: 2px solid #333;"
                                                                                        "border-radius: 10px;"
                                                                                        "font-weight: 600;"
                                                                                        "}";

                                                                  QString hoverStyle = "QPushButton {"
                                                                                       "background: #e63946;"
                                                                                       "color: white;"
                                                                                       "border: 2px solid #e63946;"
                                                                                       "border-radius: 10px;"
                                                                                       "font-weight: 600;"
                                                                                       "}";

                                                                  if (entered) {
                                                                      selectFileShadow->setEnabled(true);
                                                                      selectFileShadowAnim->stop();
                                                                      selectFileShadowAnim->setStartValue(QColor(230, 57, 70, 0));
                                                                      selectFileShadowAnim->setEndValue(QColor(230, 57, 70, 180));
                                                                      selectFileShadowAnim->start();
                                                                      selectFileButton->setStyleSheet(hoverStyle);

                                                                      dropShadow->setEnabled(false);
                                                                      dropArea->setStyleSheet(
                                                                          "QFrame#dropArea {"
                                                                          "background: #121212;"
                                                                          "border: 2px dashed #444;"
                                                                          "border-radius: 10px;"
                                                                          "}"
                                                                          );
                                                                  } else {
                                                                      selectFileShadowAnim->stop();
                                                                      selectFileShadowAnim->setStartValue(selectFileShadow->color());
                                                                      selectFileShadowAnim->setEndValue(QColor(230, 57, 70, 0));
                                                                      selectFileShadowAnim->start();
                                                                      selectFileButton->setStyleSheet(normalStyle);

                                                                      QPoint globalPos = QCursor::pos();
                                                                      QPoint localPos = dropArea->mapFromGlobal(globalPos);
                                                                      if (dropArea->rect().contains(localPos)) {

                                                                          dropShadow->setEnabled(true);
                                                                          dropArea->setStyleSheet(
                                                                              "QFrame#dropArea {"
                                                                              "background: #121212;"
                                                                              "border: 2px dashed #e63946;"
                                                                              "border-radius: 10px;"
                                                                              "}"
                                                                              );
                                                                      }
                                                                  }
                                                                  return false;
                                                              }
                                                              ));

    QPropertyAnimation* dropShadowAnim = new QPropertyAnimation(dropShadow, "color", dropArea);
    dropShadowAnim->setDuration(200);
    dropShadowAnim->setEasingCurve(QEasingCurve::OutQuad);

    dropArea->installEventFilter(new HoverEventFilter(dropArea,
                                                      [dropShadow, dropShadowAnim, selectFileButton, dropArea](bool entered) {
                                                          QString normalStyle = "QFrame#dropArea {"
                                                                                "background: #121212;"
                                                                                "border: 2px dashed #444;"
                                                                                "border-radius: 10px;"
                                                                                "}";

                                                          QString hoverStyle = "QFrame#dropArea {"
                                                                               "background: #121212;"
                                                                               "border: 2px dashed #e63946;"
                                                                               "border-radius: 10px;"
                                                                               "}";

                                                          if (entered) {
                                                              dropShadow->setEnabled(true);
                                                              dropShadowAnim->stop();
                                                              dropShadowAnim->setStartValue(QColor(230, 57, 70, 0));
                                                              dropShadowAnim->setEndValue(QColor(230, 57, 70, 150));
                                                              dropShadowAnim->start();
                                                              dropArea->setStyleSheet(hoverStyle);
                                                          } else {
                                                              dropShadowAnim->stop();
                                                              dropShadowAnim->setStartValue(dropShadow->color());
                                                              dropShadowAnim->setEndValue(QColor(230, 57, 70, 0));
                                                              dropShadowAnim->start();

                                                              QPoint globalPos = QCursor::pos();
                                                              QPoint localPos = selectFileButton->mapFromGlobal(globalPos);
                                                              if (!selectFileButton->rect().contains(localPos)) {
                                                                  dropArea->setStyleSheet(normalStyle);
                                                              }
                                                          }
                                                          return false;
                                                      }
                                                      ));
    dropLayout->addStretch(1);
    dropLayout->addWidget(selectFileButton, 0, Qt::AlignHCenter);
    dropLayout->addSpacing(10);
    dropLayout->addWidget(filePathLabel, 0, Qt::AlignHCenter);
    dropLayout->addStretch(1);

    QPushButton *saveAsButton = new QPushButton(tr("Save As"));
    saveAsButton->setStyleSheet(mainButtonStyle);
    saveAsButton->setEnabled(false);
    saveAsButton->setFixedSize(180, 40);

    // Save btn shadow
    QGraphicsDropShadowEffect* saveShadow = new QGraphicsDropShadowEffect(saveAsButton);
    saveShadow->setBlurRadius(30);
    saveShadow->setColor(QColor(230, 57, 70, 180));
    saveShadow->setOffset(0, 0);
    saveShadow->setEnabled(false);
    saveAsButton->setGraphicsEffect(saveShadow);

    saveAsButton->installEventFilter(new HoverEventFilter(saveAsButton,
                                                          [saveShadow, saveAsButton](bool entered) {
                                                              if (saveAsButton->isEnabled()) {
                                                                  saveShadow->setEnabled(entered);
                                                              }
                                                          }));
    QPropertyAnimation* saveShadowAnim = new QPropertyAnimation(saveShadow, "color", saveAsButton);
    saveShadowAnim->setDuration(200);
    saveShadowAnim->setEasingCurve(QEasingCurve::OutQuad);

    saveAsButton->installEventFilter(new HoverEventFilter(saveAsButton,
                                                          [saveShadow, saveShadowAnim, saveAsButton](bool entered) {
                                                              if (saveAsButton->isEnabled()) {
                                                                  QString normalStyle = "QPushButton {"
                                                                                        "background: #1c1c1c;"
                                                                                        "color: white;"
                                                                                        "border: 2px solid #333;"
                                                                                        "border-radius: 10px;"
                                                                                        "font-weight: 600;"
                                                                                        "}";

                                                                  QString hoverStyle = "QPushButton {"
                                                                                       "background: #e63946;"
                                                                                       "color: white;"
                                                                                       "border: 2px solid #e63946;"
                                                                                       "border-radius: 10px;"
                                                                                       "font-weight: 600;"
                                                                                       "}";

                                                                  if (entered) {
                                                                      saveShadow->setEnabled(true);
                                                                      saveShadowAnim->stop();
                                                                      saveShadowAnim->setStartValue(QColor(230, 57, 70, 0));
                                                                      saveShadowAnim->setEndValue(QColor(230, 57, 70, 180));
                                                                      saveShadowAnim->start();
                                                                      saveAsButton->setStyleSheet(hoverStyle);
                                                                  } else {
                                                                      saveShadowAnim->stop();
                                                                      saveShadowAnim->setStartValue(saveShadow->color());
                                                                      saveShadowAnim->setEndValue(QColor(230, 57, 70, 0));
                                                                      saveShadowAnim->start();
                                                                      saveAsButton->setStyleSheet(normalStyle);
                                                                  }
                                                              }
                                                              return false;
                                                          }
                                                          ));

    QLabel *versionLabel = new QLabel("release_version_1.4");
    versionLabel->setStyleSheet("color: #555; font-size: 12pt;");

    containerLayout->addWidget(dropArea);
    containerLayout->addSpacing(40);
    containerLayout->addWidget(saveAsButton, 0, Qt::AlignHCenter);
    containerLayout->addStretch(1);

    mainLayout->addLayout(topButtonsLayout);
    mainLayout->addWidget(container, 0, Qt::AlignHCenter);
    mainLayout->addWidget(versionLabel, 0, Qt::AlignLeft | Qt::AlignBottom);
    mainLayout->setSpacing(0);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Window
    setWindowTitle("120 FPS Method");
    setFixedSize(900, 530); // 850, 520

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(21, 21, 21));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Highlight, QColor(230, 57, 70));
    qApp->setPalette(darkPalette);

    connect(tgButton, &QPushButton::clicked, this, &MainWindow::on_tgButton_clicked);
    connect(tiktokButton, &QPushButton::clicked, this, &MainWindow::on_tiktokButton_clicked);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::on_helpButton_clicked);
    connect(selectFileButton, &QPushButton::clicked, this, &MainWindow::on_selectFileButton_clicked);
    connect(saveAsButton, &QPushButton::clicked, this, &MainWindow::on_saveAsButton_clicked);

    this->filePathLabel = filePathLabel;
    this->saveAsButton = saveAsButton;
    this->selectFileButton = selectFileButton;

    setAcceptDrops(true);
    dropArea->setAcceptDrops(true);

    loadLanguage("en");
    connect(langButton, &QPushButton::clicked, this, &MainWindow::on_langButton_clicked);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_langButton_clicked()
{
    isRussian = !isRussian;

    if (isRussian) {
        loadLanguage("ru");
    } else {
        loadLanguage("en");
    }

    updateLanguageButton();
}

void MainWindow::loadLanguage(const QString &language)
{
    qApp->removeTranslator(&translator);

    QString translationFile = QString(":/translations/translations/language_%1.qm").arg(language);

    qDebug() << "Loading translation file:" << translationFile;

    if (translator.load(translationFile)) {
        qApp->installTranslator(&translator);
        qDebug() << "Translation loaded successfully";
    } else {
        qWarning() << "Failed to load translation file:" << translationFile;

        QFile file(translationFile);
        if (file.exists()) {
            qDebug() << "File exists in resources";
        } else {
            qDebug() << "File does NOT exist in resources";
        }
    }
}

void MainWindow::updateLanguageButton()
{
    QPushButton *langButton = findChild<QPushButton*>("langButton");

    if (langButton) {
        if (isRussian) {
            langButton->setIcon(QIcon(":/icons/icons/language_ru.svg"));
            qDebug() << "Changed to Russian icon";
        } else {
            langButton->setIcon(QIcon(":/icons/icons/language_en.svg"));
            qDebug() << "Changed to English icon";
        }
    } else {
        qWarning() << "Language button not found!";

        QList<QPushButton*> buttons = findChildren<QPushButton*>();
        for (QPushButton *btn : buttons) {
            qDebug() << "Button found:" << btn->objectName() << btn->text();
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        if (currentFilePath.isEmpty()) {
            filePathLabel->setText(tr("or drop MP4 file here"));
        } else {
            filePathLabel->setText(QFileInfo(currentFilePath).fileName());
        }

        selectFileButton->setText(tr("Select File"));

        if (saveFilePath.isEmpty()) {
            saveAsButton->setText(tr("Save As"));
        } else {
            saveAsButton->setText(tr("Process"));
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QString filePath = mimeData->urls().first().toLocalFile();
        if (filePath.endsWith(".mp4", Qt::CaseInsensitive)) {
            filePathLabel->setText(QFileInfo(filePath).fileName());
            saveAsButton->setEnabled(true);
            currentFilePath = filePath;
            saveFilePath.clear();
            detectVideoFps(filePath);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Please drop an MP4 file"));
        }
    }
}

void MainWindow::on_processButton_clicked()
{
    if (!currentFilePath.isEmpty() && !saveFilePath.isEmpty()) {
        processFile(currentFilePath);
    }
}

void MainWindow::on_helpButton_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Help Guide"));
    msgBox.setText(tr("120 FPS Method Patcher Guide\n\n"
                      "Before using the patcher, ensure your source video meets the following requirements:\n"
                      "1. Format: MP4\n"
                      "2. Frame rate: Fixed at 60 / 120 FPS\n"
                      "\n"
                      "Tip: For optimal TikTok uploads, the recommended source file resolution is 1920p (Full HD).\n"
                      "\n"
                      "Step-by-Step Instructions:\n"
                      "1. Drag and drop your MP4 file into the drag-and-drop window, or click the \"Select File\" button to manually choose your source file.\n"
                      "2. Click \"Save As\" and specify where you want the patched file to be saved.\n"
                      "3. Press the \"Process\" button and wait for the \"Successfully Patched\" confirmation message.\n"
                      "4. Don't forget to thank the author by subscribing to his TikTok account or Telegram channel.\n"
                      "\n"
                      "⚠️ Important Note: Always keep backups of your original files before processing."));

    msgBox.setStyleSheet("QLabel{min-width: 610px;}"); // 610
    msgBox.exec();
}

void MainWindow::on_selectFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select MP4 file"), "", tr("MP4 files (*.mp4)"));
    if (!filePath.isEmpty()) {
        if (filePath.endsWith(".mp4", Qt::CaseInsensitive)) {
            filePathLabel->setText(QFileInfo(filePath).fileName());
            saveAsButton->setEnabled(true);
            currentFilePath = filePath;
            saveFilePath.clear();
            detectVideoFps(filePath);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Please select an MP4 file"));
        }
    }
}

void MainWindow::on_saveAsButton_clicked()
{
    if (!currentFilePath.isEmpty()) {
        QFileInfo fileInfo(currentFilePath);
        QString suggestedName = fileInfo.completeBaseName() + "_patched.mp4";
        QString savePath = QFileDialog::getSaveFileName(this, tr("Save processed file"), suggestedName, tr("MP4 files (*.mp4)"));

        if (!savePath.isEmpty()) {
            saveFilePath = savePath;
            saveAsButton->setText(tr("Process"));
            saveAsButton->disconnect();
            connect(saveAsButton, &QPushButton::clicked, this, &MainWindow::on_processButton_clicked);
        }
    }
}

void MainWindow::on_tiktokButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.tiktok.com")); // TT needs updating
}

void MainWindow::on_tgButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://t.me/ut0kyo"));
}

// Fps detector
void MainWindow::detectVideoFps(const QString &filePath)
{
    videoFps = 0;
    isFpsDetected = false;
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
}

void MainWindow::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        QMediaMetaData metaData = mediaPlayer->metaData();
        QVariant fpsVariant = metaData.value(QMediaMetaData::VideoFrameRate);

        if (fpsVariant.isValid()) {
            videoFps = fpsVariant.toDouble();
            qDebug() << "Detected FPS:" << videoFps;
            isFpsDetected = true;
        } else {
            qWarning() << "Couldn't detect video FPS";
            isFpsDetected = false;
        }

        mediaPlayer->stop();

        if (isFpsDetected && videoFps != 60.0 && videoFps != 120.0) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Unsupported video FPS: %1. Only 60 or 120 FPS videos are supported.").arg(videoFps));
            resetUI();
        }
    }
}

size_t MainWindow::patch_mvhd(QByteArray &buf, int divider) {
    size_t patched = 0;
    size_t offset = 0;

    while (true) {
        int idx = buf.indexOf("mvhd", offset);
        if (idx < 0) break;

        size_t pos = static_cast<size_t>(idx) - 4;
        if (pos + 24 >= static_cast<size_t>(buf.size())) break;

        uint8_t version = static_cast<uint8_t>(buf[pos + 8]);
        size_t ts_off = pos + 8 + 1 + 3 + 4 + 4;

        uint32_t old_ts = (static_cast<uint8_t>(buf[ts_off]) << 24) |
                          (static_cast<uint8_t>(buf[ts_off+1]) << 16) |
                          (static_cast<uint8_t>(buf[ts_off+2]) << 8) |
                          static_cast<uint8_t>(buf[ts_off+3]);
        uint32_t new_ts = std::max(1U, old_ts / divider);

        buf[ts_off]     = (new_ts >> 24) & 0xFF;
        buf[ts_off + 1] = (new_ts >> 16) & 0xFF;
        buf[ts_off + 2] = (new_ts >> 8) & 0xFF;
        buf[ts_off + 3] = new_ts & 0xFF;

        size_t dur_off = ts_off + 4;

        if (version == 0) {
            uint32_t old_dur = (static_cast<uint8_t>(buf[dur_off]) << 24) |
                               (static_cast<uint8_t>(buf[dur_off+1]) << 16) |
                               (static_cast<uint8_t>(buf[dur_off+2]) << 8) |
                               static_cast<uint8_t>(buf[dur_off+3]);
            uint32_t new_dur = old_dur / divider;

            buf[dur_off]     = (new_dur >> 24) & 0xFF;
            buf[dur_off + 1] = (new_dur >> 16) & 0xFF;
            buf[dur_off + 2] = (new_dur >> 8) & 0xFF;
            buf[dur_off + 3] = new_dur & 0xFF;
        } else {
            if (dur_off + 8 > static_cast<size_t>(buf.size())) break;

            uint64_t old_dur = 0;
            for (int i = 0; i < 8; ++i) {
                old_dur = (old_dur << 8) | static_cast<uint8_t>(buf[dur_off + i]);
            }
            uint64_t new_dur = old_dur / divider;

            for (int i = 7; i >= 0; --i) {
                buf[dur_off + i] = new_dur & 0xFF;
                new_dur >>= 8;
            }
        }

        patched++;
        offset = idx + 4;
    }

    return patched;
}

size_t MainWindow::patch_mdhd(QByteArray &buf, int divider) {
    size_t patched = 0;
    size_t offset = 0;

    while (true) {
        int idx = buf.indexOf("mdhd", offset);
        if (idx < 0) break;

        size_t pos = static_cast<size_t>(idx) - 4;
        if (pos + 24 >= static_cast<size_t>(buf.size())) break;

        uint8_t version = static_cast<uint8_t>(buf[pos + 8]);
        size_t base = pos + 8 + 1 + 3;

        size_t ts_off, dur_off;

        if (version == 0) {
            ts_off = base + 4 + 4;
            dur_off = ts_off + 4;

            if (dur_off + 4 > static_cast<size_t>(buf.size())) break;

            uint32_t old_ts = (static_cast<uint8_t>(buf[ts_off]) << 24) |
                              (static_cast<uint8_t>(buf[ts_off+1]) << 16) |
                              (static_cast<uint8_t>(buf[ts_off+2]) << 8) |
                              static_cast<uint8_t>(buf[ts_off+3]);
            uint32_t new_ts = std::max(1U, old_ts / divider);

            buf[ts_off]     = (new_ts >> 24) & 0xFF;
            buf[ts_off + 1] = (new_ts >> 16) & 0xFF;
            buf[ts_off + 2] = (new_ts >> 8) & 0xFF;
            buf[ts_off + 3] = new_ts & 0xFF;

            uint32_t old_dur = (static_cast<uint8_t>(buf[dur_off]) << 24) |
                               (static_cast<uint8_t>(buf[dur_off+1]) << 16) |
                               (static_cast<uint8_t>(buf[dur_off+2]) << 8) |
                               static_cast<uint8_t>(buf[dur_off+3]);
            uint32_t new_dur = old_dur / divider;

            buf[dur_off]     = (new_dur >> 24) & 0xFF;
            buf[dur_off + 1] = (new_dur >> 16) & 0xFF;
            buf[dur_off + 2] = (new_dur >> 8) & 0xFF;
            buf[dur_off + 3] = new_dur & 0xFF;

        } else {
            ts_off = base + 8 + 8;
            dur_off = ts_off + 8;

            if (dur_off + 8 > static_cast<size_t>(buf.size())) break;

            uint32_t old_ts = (static_cast<uint8_t>(buf[ts_off]) << 24) |
                              (static_cast<uint8_t>(buf[ts_off+1]) << 16) |
                              (static_cast<uint8_t>(buf[ts_off+2]) << 8) |
                              static_cast<uint8_t>(buf[ts_off+3]);
            uint32_t new_ts = std::max(1U, old_ts / divider);

            buf[ts_off]     = (new_ts >> 24) & 0xFF;
            buf[ts_off + 1] = (new_ts >> 16) & 0xFF;
            buf[ts_off + 2] = (new_ts >> 8) & 0xFF;
            buf[ts_off + 3] = new_ts & 0xFF;

            uint64_t old_dur = 0;
            for (int i = 0; i < 8; ++i) {
                old_dur = (old_dur << 8) | static_cast<uint8_t>(buf[dur_off + i]);
            }
            uint64_t new_dur = old_dur / divider;

            for (int i = 7; i >= 0; --i) {
                buf[dur_off + i] = new_dur & 0xFF;
                new_dur >>= 8;
            }
        }

        patched++;
        offset = idx + 4;
    }

    return patched;
}
// divider 60=2,120=4

void MainWindow::processFile(const QString &filePath)
{
    if (!isFpsDetected) {
        QMessageBox::warning(this, tr("Error"), tr("FPS detection failed. Please try again."));
        return;
    }

    if (videoFps != 60.0 && videoFps != 120.0) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Unsupported video FPS: %1. Only 60 or 120 FPS videos are supported.").arg(videoFps));
        return;
    }

    int divider = (videoFps == 120.0) ? 4 : 2;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open file: ") + filePath);
        return;
    }

    QByteArray buf = file.readAll();
    file.close();

    size_t mvhd_count = patch_mvhd(buf, divider);
    size_t mdhd_count = patch_mdhd(buf, divider);

    if (mvhd_count == 0 && mdhd_count == 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No mvhd or mdhd boxes found"));
        return;
    }

    QFile outFile(saveFilePath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot write output file"));
        return;
    }

    outFile.write(buf);
    outFile.close();

    QMessageBox::information(this, tr("Success"),
                             tr("File processed successfully!\n"
                                "Original FPS: %1\n"
                                "mvhd patched: %2, mdhd patched: %3\n"
                                "Saved as: %4")
                                 .arg(videoFps).arg(mvhd_count).arg(mdhd_count).arg(saveFilePath));

    resetUI();
}

void MainWindow::resetUI() {
    filePathLabel->setText(tr("or drop MP4 file here"));
    saveAsButton->setText(tr("Save as"));
    saveAsButton->setEnabled(false);
    currentFilePath.clear();
    saveFilePath.clear();

    saveAsButton->disconnect();
    connect(saveAsButton, &QPushButton::clicked, this, &MainWindow::on_saveAsButton_clicked);
}
