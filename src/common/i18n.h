// SPDX-License-Identifier: GPL-3.0-only
#ifndef I18N_H
#define I18N_H

#include <QString>
#include <QStringList>

class QApplication;
class QMainWindow;

namespace i18n {
QString languageArgument(const QStringList &arguments);
bool initialize(QApplication &app, const QString &language = QString());
void addLanguageMenu(QMainWindow *window);
} // namespace i18n

#endif
