// SPDX-License-Identifier: GPL-3.0-only
#include "challengewindow.h"

#include "../common/i18n.h"
#include "../common/ui.h"

#include <QApplication>
#include <QPushButton>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Argon2PoW-Challenge"));
    QApplication::setOrganizationName(QStringLiteral("Argon2PoW"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    if (!i18n::initialize(app, i18n::languageArgument(app.arguments()))) return 1;
    QApplication::setApplicationDisplayName(QCoreApplication::translate("Application", "Argon2 PoW 出题端"));
    const auto options = ui::parseArguments(app);

    ChallengeWindow window;
    window.show();
    if (options.example) {
        QTimer::singleShot(0, &window, [&window]() {
            if (auto *button = window.findChild<QPushButton *>(QStringLiteral("exampleButton"))) {
                button->click();
            }
        });
    }
    ui::scheduleScreenshot(&window, options);
    return QApplication::exec();
}
