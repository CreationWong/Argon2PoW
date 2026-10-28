// SPDX-License-Identifier: GPL-3.0-only
#ifndef UI_H
#define UI_H

#include <QFont>
#include <QString>

class QApplication;
class QLabel;
class QMainWindow;
class QPushButton;
class QWidget;

namespace ui {

struct ApplicationOptions {
    QString challenge;
    QString screenshot;
    int screenshotDelay = 700;
    bool example = false;
    bool autoStart = false;
};

ApplicationOptions parseArguments(QApplication &app);
void scheduleScreenshot(QWidget *window, const ApplicationOptions &options);
QString groupedNumber(quint64 value);
QFont monoFont(int pointSize = 10, bool bold = false);
QLabel *valueLabel(QWidget *parent);
QWidget *scrollablePage(QMainWindow *window);
QLabel *hintLabel(const QString &text, QWidget *parent);
QPushButton *detailsButton(const QString &showText, const QString &hideText,
                          QWidget *details, QWidget *parent);
void showAbout(QWidget *parent, const QString &title, const QString &text);

} // namespace ui

#endif // UI_H
