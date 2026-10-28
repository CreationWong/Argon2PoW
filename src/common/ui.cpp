// SPDX-License-Identifier: GPL-3.0-only
#include "ui.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QLocale>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

namespace ui {

ApplicationOptions parseArguments(QApplication &app)
{
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({QStringLiteral("lang"), QCoreApplication::translate("Ui", "界面语言：auto、zh_CN 或 en。"), QStringLiteral("language")});
    parser.addOption({QStringLiteral("example"), QCoreApplication::translate("Ui", "载入示例参数。")});
    parser.addOption({QStringLiteral("auto-start"), QCoreApplication::translate("Ui", "自动开始解答。")});
    parser.addOption({QStringLiteral("screenshot"), QCoreApplication::translate("Ui", "保存窗口截图并退出。"), QStringLiteral("file")});
    parser.addOption({QStringLiteral("screenshot-delay"), QCoreApplication::translate("Ui", "截图前等待的毫秒数。"), QStringLiteral("ms"), QStringLiteral("700")});
    parser.addPositionalArgument(QStringLiteral("challenge"), QCoreApplication::translate("Ui", "要解答的挑战码。"), QStringLiteral("[challenge]"));
    parser.process(app);
    ApplicationOptions options;
    options.challenge = parser.positionalArguments().value(0);
    options.screenshot = parser.value(QStringLiteral("screenshot"));
    options.screenshotDelay = qMax(0, parser.value(QStringLiteral("screenshot-delay")).toInt());
    options.example = parser.isSet(QStringLiteral("example"));
    options.autoStart = parser.isSet(QStringLiteral("auto-start"));
    return options;
}

void scheduleScreenshot(QWidget *window, const ApplicationOptions &options)
{
    if (options.screenshot.isEmpty()) return;
    QTimer::singleShot(options.screenshotDelay, window, [window, path = options.screenshot]() {
        window->grab().save(path);
        QApplication::quit();
    });
}

QString groupedNumber(quint64 value)
{
    return QLocale().toString(qulonglong(value));
}

QLabel *valueLabel(QWidget *parent)
{
    auto *label = hintLabel(QStringLiteral("—"), parent);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return label;
}

QFont monoFont(int pointSize, bool bold)
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(pointSize);
    font.setBold(bold);
    return font;
}

QWidget *scrollablePage(QMainWindow *window)
{
    auto *scroll = new QScrollArea(window);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *page = new QWidget(scroll);
    // An explicit vertical minimum lets QScrollArea use heightForWidth instead
    // of the layout's conservative minimum hint for wrapped labels. Otherwise
    // the trailing stretch becomes blank, scrollable space at normal widths.
    page->setMinimumHeight(1);
    scroll->setWidget(page);
    window->setCentralWidget(scroll);
    return page;
}

QLabel *hintLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    return label;
}

QPushButton *detailsButton(const QString &showText, const QString &hideText,
                          QWidget *details, QWidget *parent)
{
    auto *button = new QPushButton(showText, parent);
    button->setCheckable(true);
    button->setAutoDefault(false);
    details->hide();
    QObject::connect(button, &QPushButton::toggled, details,
                     [button, details, showText, hideText](bool expanded) {
        details->setVisible(expanded);
        button->setText(expanded ? hideText : showText);
    });
    return button;
}

void showAbout(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox::about(parent, title, text);
}

} // namespace ui
