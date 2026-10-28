// SPDX-License-Identifier: GPL-3.0-only
#include "../src/common/i18n.h"
#include "../src/common/pow.h"

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QRegularExpression>
#include <QXmlStreamReader>

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace {
void check(bool ok, const char *description)
{
    std::fprintf(stderr, "%s: %s\n", ok ? "ok" : "FAIL", description);
    if (!ok) std::exit(1);
}

QStringList placeholders(const QString &text)
{
    static const QRegularExpression pattern(QStringLiteral("%L?[1-9][0-9]*|%L?n"));
    QStringList result;
    auto matches = pattern.globalMatch(text);
    while (matches.hasNext()) result.append(matches.next().captured());
    std::sort(result.begin(), result.end());
    return result;
}
} // namespace

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    check(i18n::initialize(app, QStringLiteral("en")), "embedded English catalog loads");
    check(QCoreApplication::translate("ChallengeWindow", "Argon2 PoW 出题端")
              == QStringLiteral("Argon2 PoW Challenge Generator"), "window title translates");
    check(argonpow::parseChallenge(QString()).error == QStringLiteral("Enter a challenge."),
          "core validation errors translate");
    check(argonpow::formatDuration(5).contains(QStringLiteral("seconds")), "duration units translate");
    check(QCoreApplication::translate("ChallengeWindow", " 轮", nullptr, 1) == QStringLiteral(" round")
              && QCoreApplication::translate("ChallengeWindow", " 轮", nullptr, 2) == QStringLiteral(" rounds"),
          "English singular and plural forms translate");
    check(QLocale().language() == QLocale::English, "number locale follows selected language");
    check(i18n::initialize(app, QStringLiteral("zh_CN")), "Chinese source language initializes");
    check(argonpow::parseChallenge(QString()).error == QStringLiteral("请输入挑战码。"),
          "switching language removes previous translator");
    check(QLocale().language() == QLocale::Chinese, "Chinese number locale restored");
    check(!i18n::initialize(app, QStringLiteral("invalid")), "unsupported explicit language rejected");

    QFile catalog(app.arguments().value(1));
    check(catalog.open(QIODevice::ReadOnly), "translation catalog opens");
    QXmlStreamReader xml(&catalog);
    QString context;
    int count = 0;
    while (!xml.atEnd()) {
        xml.readNext();
        if (!xml.isStartElement()) continue;
        if (xml.name() == QLatin1String("name")) context = xml.readElementText();
        if (xml.name() != QLatin1String("message")) continue;
        const bool numerus = xml.attributes().value(QLatin1String("numerus")) == QLatin1String("yes");
        QString source;
        QStringList translations;
        bool finished = true;
        while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("source")) source = xml.readElementText();
            else if (xml.name() == QLatin1String("translation")) {
                finished = xml.attributes().value(QLatin1String("type")).isEmpty();
                if (numerus) {
                    while (xml.readNextStartElement()) {
                        if (xml.name() == QLatin1String("numerusform")) {
                            translations.append(xml.readElementText());
                        } else xml.skipCurrentElement();
                    }
                } else translations.append(xml.readElementText());
            } else xml.skipCurrentElement();
        }
        const bool valid = translations.size() == (numerus ? 2 : 1)
            && std::all_of(translations.cbegin(), translations.cend(), [&](const QString &text) {
                   return !text.trimmed().isEmpty() && placeholders(source) == placeholders(text);
               });
        if (!finished || !valid) {
            std::fprintf(stderr, "Invalid translation: %s / %s\n", qPrintable(context), qPrintable(source));
            return 1;
        }
        ++count;
    }
    check(!xml.hasError() && count > 0, "all translations are complete and preserve placeholders");
    return 0;
}
