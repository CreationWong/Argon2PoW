// SPDX-License-Identifier: GPL-3.0-only
#include "../src/generator/challengewindow.h"
#include "../src/solver/solverwindow.h"
#include "../src/common/pow.h"
#include "../src/common/i18n.h"

#include <QApplication>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QThread>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <functional>

namespace {
void check(bool ok, const char *description)
{
    std::fprintf(stderr, "%s: %s\n", ok ? "ok" : "FAIL", description);
    if (!ok) std::exit(1);
}

template <class T> T *child(QWidget &window, const char *name)
{
    T *result = window.findChild<T *>(QString::fromLatin1(name));
    check(result != nullptr, name);
    return result;
}

bool waitFor(const std::function<bool()> &done)
{
    QElapsedTimer timer;
    timer.start();
    while (!done() && timer.elapsed() < 10000) {
        QApplication::processEvents();
        QThread::msleep(5);
    }
    return done();
}

bool hasText(QWidget &window, const QString &text)
{
    for (QLabel *label : window.findChildren<QLabel *>()) {
        if (label->text().contains(text)) return true;
    }
    return false;
}
} // namespace

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    const QString language = i18n::languageArgument(app.arguments());
    check(i18n::initialize(app, language), "translation resources load");
    const bool english = language == QStringLiteral("en");
    ChallengeWindow generator;
    check(generator.size() == generator.minimumSize(), "generator starts at its minimum usable size");
    auto *importEdit = child<QLineEdit>(generator, "importEdit");
    auto *importButton = child<QPushButton>(generator, "importButton");
    auto *challengeEdit = child<QLineEdit>(generator, "challengeEdit");
    auto *copy = child<QPushButton>(generator, "copyChallengeButton");
    auto *generate = child<QPushButton>(generator, "generateButton");
    auto *verify = child<QPushButton>(generator, "verifyButton");
    auto *answer = child<QLineEdit>(generator, "verifyEdit");
    auto *memory = child<QDoubleSpinBox>(generator, "memorySpin");
    check(copy->isEnabled() && !verify->isEnabled(), "initial generator actions");

    // Import must not clamp values to the narrower original GUI ranges or round KiB.
    const QString fullRange = QStringLiteral("CHALLENGE:9:1000:SALT12345:4294967295");
    importEdit->setText(fullRange);
    importButton->click();
    check(challengeEdit->text() == fullRange, "import preserves exact parameters and salt");
    generate->click();
    check(challengeEdit->text() == fullRange, "regeneration round-trips imported KiB and uint32 difficulty");
    importEdit->setText(QStringLiteral("CHALLENGE:7:1:SALT12345:1"));
    importButton->click();
    check(challengeEdit->text() == fullRange, "invalid import preserves active challenge");
    check(hasText(generator, english ? QStringLiteral("Cannot import:") : QStringLiteral("无法导入")),
          "import shows inline error");

    memory->setValue(8);
    check(!copy->isEnabled() && !verify->isEnabled(), "changed parameters disable stale actions");
    generate->click();
    check(copy->isEnabled(), "generation restores copy action");

    const QString easy = QStringLiteral("CHALLENGE:8192:1:SALT12345:1");
    importEdit->setText(easy);
    importButton->click();
    answer->setText(QStringLiteral("UNBLOCK-ABCDEFGH-1"));
    verify->click();
    check(!verify->isEnabled(), "verification runs asynchronously with duplicate action disabled");
    check(waitFor([&]() { return verify->isEnabled(); }), "verification completes");
    check(hasText(generator, english ? QStringLiteral("Verification passed") : QStringLiteral("校验通过")),
          "imported challenge verifies an answer");

    answer->setText(QStringLiteral("UNBLOCK-ABCDEFGH-2"));
    verify->click();
    answer->setText(QStringLiteral("UNBLOCK-ABCDEFGH-3"));
    check(waitFor([&]() { return verify->isEnabled(); }), "edited-input verification finishes");
    check(!hasText(generator, english ? QStringLiteral("Verification passed") : QStringLiteral("校验通过")),
          "old verification does not overwrite edited input");

    SolverWindow solver(easy);
    check(solver.size() == solver.minimumSize(), "solver starts at its minimum usable size");
    auto *input = child<QLineEdit>(solver, "challengeEdit");
    auto *start = child<QPushButton>(solver, "startButton");
    auto *threads = child<QSpinBox>(solver, "threadsSpin");
    auto *recommend = child<QPushButton>(solver, "recommendButton");
    auto *solution = child<QLineEdit>(solver, "solutionEdit");
    auto *copySolution = child<QPushButton>(solver, "copySolutionButton");
    auto *activity = child<QProgressBar>(solver, "searchActivity");
    check(start->isEnabled() && !copySolution->isEnabled(), "valid challenge is ready to start");
    threads->setValue(32);
    recommend->click();
    check(threads->value() <= 8, "recommended threads can be restored");
    start->click();
    check(!input->isEnabled() && activity->maximum() == 0, "running locks inputs and shows busy activity");
    check(waitFor([&]() { return input->isEnabled(); }), "solver completes");
    check(copySolution->isEnabled(), "solved result enables copy");
    check(argonpow::verifyUnlockCode(argonpow::parseChallenge(easy).challenge,
                                    solution->text()).status == argonpow::VerifyStatus::Valid,
          "displayed solution verifies against challenge");
    input->setText(QStringLiteral("invalid"));
    check(!start->isEnabled() && !copySolution->isEnabled() && solution->text().isEmpty(),
          "editing immediately invalidates old answer before debounce");

    input->setText(QStringLiteral("CHALLENGE:8192:10:SALT12345:4294967295"));
    check(waitFor([&]() { return start->isEnabled(); }), "automatic parsing enables start");
    threads->setValue(1);
    start->click();
    start->click();
    check(!start->isEnabled(), "stop waits for the active hash");
    check(waitFor([&]() { return input->isEnabled(); }), "stop restores editable inputs");
    check(!copySolution->isEnabled(), "cancelled search has no copyable answer");

    // Native scroll container must keep all actions reachable at the minimum size.
    for (QWidget *window : {static_cast<QWidget *>(&generator), static_cast<QWidget *>(&solver)}) {
        window->show();
        QApplication::processEvents();
        check(window->size() == window->minimumSize(), "first visible window keeps its minimum startup size");
        auto *initialScroll = window->findChild<QScrollArea *>();
        auto *content = initialScroll->widget();
        const int naturalHeight = content->layout()->totalHeightForWidth(content->width());
        const int requiredHeight = std::max(initialScroll->viewport()->height(), naturalHeight);
        if (content->height() > requiredHeight + 2) {
            std::fprintf(stderr, "Excess page height: viewport=%d, content=%d, natural=%d\n",
                         initialScroll->viewport()->height(), content->height(), naturalHeight);
        }
        check(content->height() <= requiredHeight + 2,
              "wrapped page has no phantom bottom space or unnecessary scrolling");
        window->resize(600, 460);
        window->show();
        for (QPushButton *button : window->findChildren<QPushButton *>()) {
            if (button->isCheckable()) button->setChecked(true);
        }
        QApplication::processEvents();
        auto *scroll = window->findChild<QScrollArea *>();
        check(scroll && scroll->verticalScrollBar()->maximum() > 0,
              "expanded details scroll at minimum window size");
        if (scroll->horizontalScrollBar()->maximum() > 0) {
            std::fprintf(stderr, "Overflow in %s: viewport=%d, content=%d\n",
                         qPrintable(window->windowTitle()), scroll->viewport()->width(), scroll->widget()->width());
            for (QWidget *item : window->findChildren<QWidget *>()) {
                if (item->minimumSizeHint().width() > 300) {
                    std::fprintf(stderr, "  %s %s minimum=%d\n", item->metaObject()->className(),
                                 qPrintable(item->objectName()), item->minimumSizeHint().width());
                }
            }
        }
        check(scroll->horizontalScrollBar()->maximum() == 0, "minimum size has no horizontal overflow");
        window->hide();
    }
    return 0;
}
