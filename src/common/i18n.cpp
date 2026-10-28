// SPDX-License-Identifier: GPL-3.0-only
#include "i18n.h"

#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QTranslator>

// Referencing the resource explicitly also retains it when linking static libraries.
static void initializeResources()
{
    static const bool initialized = []() {
        Q_INIT_RESOURCE(translations);
        return true;
    }();
    Q_UNUSED(initialized);
}

namespace i18n {
namespace {
QSettings preferences()
{
    return QSettings(QStringLiteral("Argon2PoW"), QStringLiteral("Argon2PoW"));
}

QString savedLanguage()
{
    return preferences().value(QStringLiteral("ui/language"), QStringLiteral("auto")).toString();
}

bool isSupported(const QString &language)
{
    return language == QStringLiteral("auto") || language == QStringLiteral("en")
        || language == QStringLiteral("zh_CN");
}
} // namespace

QString languageArgument(const QStringList &arguments)
{
    for (int i = 1; i < arguments.size(); ++i) {
        if (arguments.at(i) == QStringLiteral("--lang") && i + 1 < arguments.size()) {
            return arguments.at(i + 1);
        }
        if (arguments.at(i).startsWith(QStringLiteral("--lang="))) {
            return arguments.at(i).mid(7);
        }
    }
    return QString();
}

bool initialize(QApplication &app, const QString &language)
{
    initializeResources();
    QString selected = language.isEmpty() ? savedLanguage() : language;
    if (!isSupported(selected)) {
        if (!language.isEmpty()) return false;
        selected = QStringLiteral("auto");
    }
    app.setProperty("languagePreference", selected);
    const auto previous = app.findChildren<QTranslator *>(QStringLiteral("argon2pow.translation"),
                                                          Qt::FindDirectChildrenOnly);
    for (auto *translator : previous) {
        app.removeTranslator(translator);
        delete translator;
    }
    if (selected == QStringLiteral("auto")) {
        selected = QLocale::system().language() == QLocale::Chinese
            ? QStringLiteral("zh_CN") : QStringLiteral("en");
    }
    const QLocale locale(selected);
    QLocale::setDefault(locale);

    // The source language is Simplified Chinese; English is embedded in the executable.
    if (selected == QStringLiteral("en")) {
        auto *translator = new QTranslator(&app);
        translator->setObjectName(QStringLiteral("argon2pow.translation"));
        if (!translator->load(QStringLiteral(":/i18n/argon2pow_en.qm"))) {
            delete translator;
            return false;
        }
        app.installTranslator(translator);
    }

    // Load only Qt's own widget strings, first from the portable package.
    const QStringList directories = {
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("translations")),
        QLibraryInfo::path(QLibraryInfo::TranslationsPath)
    };
    for (const QString &directory : directories) {
        auto *translator = new QTranslator(&app);
        translator->setObjectName(QStringLiteral("argon2pow.translation"));
        if (translator->load(locale, QStringLiteral("qtbase"), QStringLiteral("_"), directory)) {
            app.installTranslator(translator);
            break;
        }
        delete translator;
    }
    return true;
}

void addLanguageMenu(QMainWindow *window)
{
    auto *menu = window->menuBar()->addMenu(QCoreApplication::translate("LanguageMenu", "语言(&L)"));
    auto *group = new QActionGroup(menu);
    const QString selected = qApp->property("languagePreference").toString();
    const QList<QPair<QString, QString>> languages = {
        {QStringLiteral("auto"), QCoreApplication::translate("LanguageMenu", "跟随系统")},
        {QStringLiteral("zh_CN"), QStringLiteral("简体中文")},
        {QStringLiteral("en"), QStringLiteral("English")}
    };
    for (const auto &language : languages) {
        auto *action = menu->addAction(language.second);
        action->setCheckable(true);
        action->setChecked(language.first == selected);
        group->addAction(action);
        QObject::connect(action, &QAction::triggered, window, [window, code = language.first]() {
            preferences().setValue(QStringLiteral("ui/language"), code);
            window->statusBar()->showMessage(
                QCoreApplication::translate("LanguageMenu", "语言设置已保存，重新启动程序后生效。"));
        });
    }
}
} // namespace i18n
