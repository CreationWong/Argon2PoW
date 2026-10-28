// SPDX-License-Identifier: GPL-3.0-only
#include "solverwindow.h"

#include "../common/i18n.h"
#include "../common/ui.h"

#include <QApplication>
#include <QPushButton>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Argon2PoW-Solver"));
    QApplication::setOrganizationName(QStringLiteral("Argon2PoW"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    if (!i18n::initialize(app, i18n::languageArgument(app.arguments()))) return 1;
    QApplication::setApplicationDisplayName(QCoreApplication::translate("Application", "Argon2 PoW 解答端"));
    const auto options = ui::parseArguments(app);

    SolverWindow window(options.challenge);
    window.show();
    if (options.autoStart) {
        QTimer::singleShot(0, &window, [&window]() {
            if (auto *button = window.findChild<QPushButton *>(QStringLiteral("startButton"))) {
                button->click();
            }
        });
    }
    ui::scheduleScreenshot(&window, options);
    return QApplication::exec();
}
