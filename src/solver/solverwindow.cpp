// SPDX-License-Identifier: GPL-3.0-only
#include "solverwindow.h"

#include "solverworker.h"

#include "../common/ui.h"
#include "../common/i18n.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

#ifdef Q_OS_WIN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

namespace {

quint64 availablePhysicalMemory()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        return static_cast<quint64>(status.ullAvailPhys);
    }
#endif
    return 8ULL * 1024ULL * 1024ULL * 1024ULL;
}

} // namespace

SolverWindow::SolverWindow(const QString &initialChallenge, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Argon2 PoW 解答端"));
    setMinimumSize(600, 460);
    resize(minimumSize());

    buildMenus();
    buildUi();

    if (!initialChallenge.trimmed().isEmpty()) {
        m_challengeEdit->setText(initialChallenge.trimmed());
        parseInput(true);
    }
}

SolverWindow::~SolverWindow()
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->requestStop();
        m_worker->wait();
    }
}

void SolverWindow::buildMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));

    m_exampleAction = fileMenu->addAction(tr("载入示例挑战(&E)"));
    connect(m_exampleAction, &QAction::triggered, this, &SolverWindow::loadExample);

    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(tr("退出(&X)"));
    quitAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Q")));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    QMenu *runMenu = menuBar()->addMenu(tr("计算(&R)"));
    QAction *startAction = runMenu->addAction(tr("开始 / 停止(&S)"));
    startAction->setShortcut(QKeySequence(QStringLiteral("F5")));
    connect(startAction, &QAction::triggered, this, &SolverWindow::startOrStop);

    i18n::addLanguageMenu(this);

    QMenu *helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    QAction *aboutAction = helpMenu->addAction(tr("关于(&A)"));
    connect(aboutAction, &QAction::triggered, this, &SolverWindow::showAbout);
}

void SolverWindow::buildUi()
{
    auto *central = ui::scrollablePage(this);
    auto *page = new QVBoxLayout(central);
    page->setContentsMargins(12, 12, 12, 12);
    page->setSpacing(12);
    page->addWidget(ui::hintLabel(
        tr("粘贴挑战码并开始计算，找到解锁码后复制回原来的页面。计算在本机完成。"), central));

    auto *inputGroup = new QGroupBox(tr("1. 粘贴挑战码"), central);
    auto *inputLayout = new QVBoxLayout(inputGroup);
    auto *inputRow = new QHBoxLayout;
    m_challengeEdit = new QLineEdit(inputGroup);
    m_challengeEdit->setObjectName(QStringLiteral("challengeEdit"));
    m_challengeEdit->setAccessibleName(tr("挑战码"));
    m_challengeEdit->setPlaceholderText(tr("在此粘贴 CHALLENGE:… 挑战码"));
    m_challengeEdit->setFont(ui::monoFont());
    m_pasteButton = new QPushButton(tr("粘贴(&P)"), inputGroup);
    m_pasteButton->setObjectName(QStringLiteral("pasteButton"));
    m_loadExampleButton = new QPushButton(tr("载入示例"), inputGroup);
    inputRow->addWidget(m_challengeEdit, 1);
    inputRow->addWidget(m_pasteButton);
    inputRow->addWidget(m_loadExampleButton);
    inputLayout->addLayout(inputRow);
    m_inputStatus = ui::hintLabel(
        tr("粘贴后会自动检查格式。还没有挑战码？可先载入示例了解操作。"), inputGroup);
    inputLayout->addWidget(m_inputStatus);

    auto *details = new QWidget(inputGroup);
    auto *paramsForm = new QFormLayout(details);
    paramsForm->setContentsMargins(0, 0, 0, 0);
    paramsForm->setRowWrapPolicy(QFormLayout::WrapLongRows);
    m_memoryValue = ui::valueLabel(details);
    m_timeValue = ui::valueLabel(details);
    m_saltValue = ui::valueLabel(details);
    m_saltValue->setFont(ui::monoFont(9));
    m_difficultyValue = ui::valueLabel(details);
    m_thresholdValue = ui::valueLabel(details);
    paramsForm->addRow(tr("每线程内存："), m_memoryValue);
    paramsForm->addRow(tr("计算轮数："), m_timeValue);
    paramsForm->addRow(tr("盐值："), m_saltValue);
    paramsForm->addRow(tr("平均尝试次数："), m_difficultyValue);
    paramsForm->addRow(tr("通过阈值："), m_thresholdValue);
    inputLayout->addWidget(ui::detailsButton(tr("显示挑战的技术参数"), tr("收起挑战的技术参数"), details, inputGroup),
                           0, Qt::AlignLeft);
    inputLayout->addWidget(details);
    page->addWidget(inputGroup);

    auto *settingsGroup = new QGroupBox(tr("2. 确认设置并开始"), central);
    auto *settingsLayout = new QVBoxLayout(settingsGroup);
    auto *settingsRow = new QHBoxLayout;
    auto *threadsLabel = new QLabel(tr("计算线程(&T)："), settingsGroup);
    m_threadsSpin = new QSpinBox(settingsGroup);
    m_threadsSpin->setObjectName(QStringLiteral("threadsSpin"));
    m_threadsSpin->setRange(1, argonpow::MaxSearchThreads);
    m_threadsSpin->setValue(4);
    m_threadsSpin->setSuffix(tr(" 个", nullptr, m_threadsSpin->value()));
    m_threadsSpin->setMinimumWidth(90);
    threadsLabel->setBuddy(m_threadsSpin);
    m_recommendButton = new QPushButton(tr("使用推荐值"), settingsGroup);
    m_recommendButton->setObjectName(QStringLiteral("recommendButton"));
    m_recommendButton->setEnabled(false);
    m_startButton = new QPushButton(tr("开始解答(&S)"), settingsGroup);
    m_startButton->setObjectName(QStringLiteral("startButton"));
    m_startButton->setMinimumWidth(120);
    m_startButton->setEnabled(false);
    m_startButton->setToolTip(tr("开始或停止计算（F5）。停止需要等待当前计算结束。"));
    settingsRow->addWidget(threadsLabel);
    settingsRow->addWidget(m_threadsSpin);
    settingsRow->addWidget(m_recommendButton);
    settingsRow->addStretch();
    settingsLayout->addLayout(settingsRow);
    m_memoryEstimate = ui::hintLabel(tr("内存占用：请先粘贴有效的挑战码。"), settingsGroup);
    settingsLayout->addWidget(m_memoryEstimate);
    m_threadHint = ui::hintLabel(
        tr("将自动选择推荐线程数；需要降低电脑负载时，可减少线程。"), settingsGroup);
    auto *startRow = new QHBoxLayout;
    startRow->addWidget(m_threadHint, 1);
    startRow->addWidget(m_startButton);
    settingsLayout->addLayout(startRow);
    page->addWidget(settingsGroup);

    auto *resultGroup = new QGroupBox(tr("3. 复制解锁码"), central);
    auto *resultLayout = new QVBoxLayout(resultGroup);
    auto *resultRow = new QHBoxLayout;
    m_solutionEdit = new QLineEdit(resultGroup);
    m_solutionEdit->setObjectName(QStringLiteral("solutionEdit"));
    m_solutionEdit->setAccessibleName(tr("计算得到的解锁码"));
    m_solutionEdit->setReadOnly(true);
    m_solutionEdit->setFont(ui::monoFont(10, true));
    m_copyButton = new QPushButton(tr("复制解锁码(&C)"), resultGroup);
    m_copyButton->setObjectName(QStringLiteral("copySolutionButton"));
    resultRow->addWidget(m_solutionEdit, 1);
    resultRow->addWidget(m_copyButton);
    resultLayout->addLayout(resultRow);
    m_solutionMeta = ui::hintLabel(QString(), resultGroup);
    m_solutionMeta->setTextInteractionFlags(Qt::TextSelectableByMouse);
    resultLayout->addWidget(m_solutionMeta);
    page->addWidget(resultGroup);

    auto *progressGroup = new QGroupBox(tr("计算状态"), central);
    auto *progressLayout = new QVBoxLayout(progressGroup);
    m_progress = new QProgressBar(progressGroup);
    m_progress->setObjectName(QStringLiteral("searchActivity"));
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_progress->setTextVisible(false);
    m_progress->setAccessibleName(tr("计算活动状态"));
    progressLayout->addWidget(m_progress);
    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(6);
    auto metric = [&](int row, int col, const QString &caption, QLabel *value) {
        auto *cell = new QWidget(progressGroup);
        auto *layout = new QVBoxLayout(cell);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(2);
        layout->addWidget(ui::hintLabel(caption, cell));
        layout->addWidget(value);
        grid->addWidget(cell, row, col / 2);
    };
    m_attemptsValue = ui::valueLabel(progressGroup);
    m_rateValue = ui::valueLabel(progressGroup);
    m_elapsedValue = ui::valueLabel(progressGroup);
    m_etaValue = ui::valueLabel(progressGroup);
    m_etaValue->setToolTip(tr("难度 ÷ 当前速率，是从现在起的平均等待估算；实际结束时间不确定。"));
    metric(0, 0, tr("已尝试："), m_attemptsValue);
    metric(0, 2, tr("计算速度："), m_rateValue);
    metric(1, 0, tr("已用时间："), m_elapsedValue);
    metric(1, 2, tr("平均等待估算："), m_etaValue);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    progressLayout->addLayout(grid);
    m_probabilityLabel = ui::hintLabel(QString(), progressGroup);
    progressLayout->addWidget(m_probabilityLabel);
    progressLayout->addWidget(ui::hintLabel(
        tr("找到答案的时间有随机性。尝试次数达到难度后仍可能继续计算；可随时停止。"), progressGroup));

    auto *diagnostics = new QWidget(progressGroup);
    auto *diagnosticsForm = new QFormLayout(diagnostics);
    diagnosticsForm->setContentsMargins(0, 0, 0, 0);
    m_currentCandidate = ui::valueLabel(diagnostics);
    m_currentCandidate->setFont(ui::monoFont(9));
    diagnosticsForm->addRow(tr("下一个候选："), m_currentCandidate);
    progressLayout->addWidget(ui::detailsButton(tr("显示搜索细节"), tr("收起搜索细节"), diagnostics, progressGroup),
                              0, Qt::AlignLeft);
    progressLayout->addWidget(diagnostics);
    page->addWidget(progressGroup);
    page->addStretch();

    m_parseTimer = new QTimer(this);
    m_parseTimer->setSingleShot(true);
    m_parseTimer->setInterval(260);
    connect(m_parseTimer, &QTimer::timeout, this, [this]() { parseInput(false); });
    connect(m_challengeEdit, &QLineEdit::textChanged, this, [this]() {
        if (m_running) return;
        m_challengeValid = false;
        clearParsedValues();
        resetResult();
        updateStartButton();
        m_inputStatus->setText(m_challengeEdit->text().trimmed().isEmpty()
            ? tr("粘贴后会自动检查格式。还没有挑战码？可先载入示例了解操作。")
            : tr("正在检查挑战码…"));
        m_parseTimer->start();
    });
    connect(m_challengeEdit, &QLineEdit::editingFinished, this, [this]() { parseInput(false); });
    connect(m_challengeEdit, &QLineEdit::returnPressed, this, [this]() {
        if (!m_running) beginSolving();
    });
    connect(m_pasteButton, &QPushButton::clicked, this, [this]() {
        m_challengeEdit->setText(QApplication::clipboard()->text().trimmed());
        m_challengeEdit->setFocus();
        parseInput(false);
    });
    connect(m_loadExampleButton, &QPushButton::clicked, this, &SolverWindow::loadExample);
    connect(m_recommendButton, &QPushButton::clicked, this, [this]() {
        m_threadsUserSet = false;
        setThreadsSilently(recommendedThreads());
        updateThreadHint();
    });
    connect(m_startButton, &QPushButton::clicked, this, &SolverWindow::startOrStop);
    connect(m_threadsSpin, &QSpinBox::valueChanged, this, [this](int) {
        m_threadsSpin->setSuffix(tr(" 个", nullptr, m_threadsSpin->value()));
        if (!m_adjustingThreads) m_threadsUserSet = true;
        updateThreadHint();
    });
    connect(m_copyButton, &QPushButton::clicked, this, &SolverWindow::copySolution);
    auto *focusInput = new QAction(this);
    focusInput->setShortcut(QKeySequence(QStringLiteral("Ctrl+L")));
    addAction(focusInput);
    connect(focusInput, &QAction::triggered, this, [this]() {
        if (!m_running) {
            m_challengeEdit->setFocus();
            m_challengeEdit->selectAll();
        }
    });
    QWidget::setTabOrder(m_challengeEdit, m_pasteButton);
    QWidget::setTabOrder(m_pasteButton, m_loadExampleButton);
    QWidget::setTabOrder(m_loadExampleButton, m_threadsSpin);
    QWidget::setTabOrder(m_threadsSpin, m_recommendButton);
    QWidget::setTabOrder(m_recommendButton, m_startButton);
    QWidget::setTabOrder(m_startButton, m_solutionEdit);
    QWidget::setTabOrder(m_solutionEdit, m_copyButton);
    resetResult();
    updateThreadHint();
    m_challengeEdit->setFocus();
    statusBar()->showMessage(tr("等待挑战码"));
}

void SolverWindow::parseInput(bool showErrors)
{
    m_parseTimer->stop();
    if (m_running) {
        return;
    }

    const QString text = m_challengeEdit->text().trimmed();
    if (text.isEmpty()) {
        m_challengeValid = false;
        clearParsedValues();
        m_inputStatus->setText(
            tr("粘贴后会自动检查格式。还没有挑战码？可先载入示例了解操作。"));
        updateStartButton();
        resetResult();
        return;
    }

    const argonpow::ParseResult parsed = argonpow::parseChallenge(text);
    if (!parsed.ok) {
        m_challengeValid = false;
        clearParsedValues();
        m_inputStatus->setText(tr("格式错误：%1").arg(parsed.error));
        updateStartButton();
        resetResult();
        statusBar()->showMessage(tr("挑战码格式错误"), 4000);
        if (showErrors) {
            QMessageBox::warning(this, tr("挑战码格式错误"), parsed.error);
        }
        return;
    }

    const bool sameChallenge =
        m_challengeValid && m_challenge.encoded() == parsed.challenge.encoded();
    m_challenge = parsed.challenge;
    m_challengeValid = true;
    updateChallengeValues();

    if (!m_threadsUserSet) {
        setThreadsSilently(recommendedThreads());
    }
    updateThreadHint();

    m_inputStatus->setText(
        tr("格式正确。平均约需 %1 次尝试，每线程内存 %2。可点击“开始解答”。")
            .arg(ui::groupedNumber(m_challenge.difficulty), m_challenge.memoryLabel()));
    updateStartButton();
    if (!sameChallenge) {
        resetResult();
    }
    statusBar()->showMessage(tr("挑战码解析成功"), 3000);
}

void SolverWindow::clearParsedValues()
{
    m_memoryValue->setText(QStringLiteral("—"));
    m_timeValue->setText(QStringLiteral("—"));
    m_saltValue->setText(QStringLiteral("—"));
    m_difficultyValue->setText(QStringLiteral("—"));
    m_thresholdValue->setText(QStringLiteral("—"));
    m_memoryEstimate->setText(tr("内存占用：请先粘贴有效的挑战码。"));
    m_recommendButton->setEnabled(false);
    m_threadHint->setText(tr("将自动选择推荐线程数；需要降低电脑负载时，可减少线程。"));
}

void SolverWindow::updateChallengeValues()
{
    if (!m_challengeValid) {
        return;
    }
    const double mib = static_cast<double>(m_challenge.memoryKiB) / 1024.0;
    m_memoryValue->setText(
        QStringLiteral("%1 MiB（%2 KiB）")
            .arg(mib, 0, 'f', mib < 10.0 ? 1 : 0)
            .arg(ui::groupedNumber(m_challenge.memoryKiB)));
    m_timeValue->setText(tr("%n 轮", nullptr, static_cast<int>(m_challenge.timeCost)));
    m_saltValue->setText(m_challenge.salt);
    m_difficultyValue->setText(tr("%1 次").arg(ui::groupedNumber(m_challenge.difficulty)));
    m_thresholdValue->setText(ui::groupedNumber(m_challenge.threshold()));
}

void SolverWindow::updateThreadHint()
{
    if (!m_challengeValid) {
        m_memoryEstimate->setText(tr("内存占用：请先粘贴有效的挑战码。"));
        m_recommendButton->setEnabled(false);
        return;
    }

    const int threads = m_threadsSpin->value();
    const quint64 bytes = static_cast<quint64>(threads)
                          * static_cast<quint64>(m_challenge.memoryKiB) * 1024ULL;
    const quint64 available = availablePhysicalMemory();
    const int safe = safeThreadCount(m_challenge.memoryKiB);

    m_memoryEstimate->setText(
        tr("预计内存占用：%1；本机可用约 %2（未含程序额外开销）。")
            .arg(argonpow::formatBytes(bytes), argonpow::formatBytes(available)));
    if (threads > recommendedThreads()) {
        m_threadHint->setText(tr("当前 %1 个线程高于推荐值 %2。更多线程不一定更快；若电脑卡顿，可停止后减少线程。")
                                 .arg(threads).arg(recommendedThreads()));
    } else if (safe <= 1) {
        m_threadHint->setText(
            tr("内存较紧，建议使用 1 个线程，避免系统换页。"));
    } else {
        m_threadHint->setText(
            tr("本机推荐 %1 个线程。每增加一个线程，都需要额外一份挑战内存。")
                .arg(recommendedThreads()));
    }
}

void SolverWindow::setThreadsSilently(int value)
{
    m_adjustingThreads = true;
    m_threadsSpin->setValue(std::clamp(value, 1, argonpow::MaxSearchThreads));
    m_adjustingThreads = false;
}

int SolverWindow::safeThreadCount(quint32 memoryKiB) const
{
    if (memoryKiB == 0) {
        return 1;
    }
    const quint64 available = availablePhysicalMemory();
    const double perThread = static_cast<double>(memoryKiB) * 1024.0 * 1.15;
    const double budget = static_cast<double>(available) * 0.55;
    const int count = static_cast<int>(budget / std::max(1.0, perThread));
    return std::clamp(count, 1, argonpow::MaxSearchThreads);
}

int SolverWindow::recommendedThreads() const
{
    if (!m_challengeValid) {
        return 4;
    }
    const int ideal = std::max(1, QThread::idealThreadCount());
    return std::clamp(std::min({8, ideal, safeThreadCount(m_challenge.memoryKiB)}), 1, argonpow::MaxSearchThreads);
}

void SolverWindow::startOrStop()
{
    if (m_stopping) return;
    if (m_running) {
        requestStop();
        return;
    }
    beginSolving();
}

void SolverWindow::beginSolving()
{
    if (m_running || m_worker) return;
    parseInput(true);
    if (!m_challengeValid) {
        m_challengeEdit->setFocus();
        return;
    }

    const quint64 required = static_cast<quint64>(m_threadsSpin->value())
                             * static_cast<quint64>(m_challenge.memoryKiB) * 1024ULL;
    const quint64 available = availablePhysicalMemory();
    if (required > static_cast<quint64>(static_cast<double>(available) * 0.9)) {
        QMessageBox::warning(
            this,
            tr("内存不足"),
            tr("按当前线程数预计需要 %1 内存，但本机当前可用约 %2。\n"
                           "请减少线程数，或关闭其他占用内存的程序后重试。")
                .arg(argonpow::formatBytes(required), argonpow::formatBytes(available)));
        m_threadsSpin->setFocus();
        return;
    }

    resetResult();
    m_prefix = argonpow::randomPrefix();
    m_solutionEdit->setPlaceholderText(tr("搜索中，找到后立即显示…"));
    m_solutionMeta->setText(
        tr("正在计算，找到后会在这里显示解锁码。你可以随时点击“停止计算”。"));
    m_inputStatus->setText(tr("正在解答当前挑战码。停止计算后可以更换挑战码或设置。"));
    setRunningUi(true);

    m_worker = new SolverWorker(m_challenge, m_prefix, m_threadsSpin->value(), this);
    connect(m_worker, &SolverWorker::progress, this, &SolverWindow::handleProgress);
    connect(m_worker, &SolverWorker::solved, this, &SolverWindow::handleSolved);
    connect(m_worker, &SolverWorker::cancelled, this, &SolverWindow::handleCancelled);
    connect(m_worker, &SolverWorker::failed, this, &SolverWindow::handleFailed);
    connect(m_worker, &QThread::finished, this, &SolverWindow::handleWorkerFinished);
    m_worker->start();
}

void SolverWindow::requestStop()
{
    if (!m_worker || !m_running) {
        return;
    }
    if (m_stopping) return;
    m_stopping = true;
    m_worker->requestStop();
    m_startButton->setEnabled(false);
    m_startButton->setText(tr("正在停止…"));
    statusBar()->showMessage(tr("正在停止，请等待当前一批计算结束。"));
}

void SolverWindow::handleProgress(quint64 attempts,
                                  quint32 nextNonce,
                                  double elapsedSeconds,
                                  double rate,
                                  double probability)
{
    m_attemptsValue->setText(tr("%1 次").arg(ui::groupedNumber(attempts)));
    m_rateValue->setText(
        tr("%1 次/秒").arg(rate, 0, 'f', rate < 10.0 ? 2 : 1));
    m_elapsedValue->setText(argonpow::formatDuration(elapsedSeconds));
    if (rate > 0.0) {
        const double remaining = static_cast<double>(m_challenge.difficulty) / rate;
        m_etaValue->setText(argonpow::formatDuration(remaining));
    } else {
        m_etaValue->setText(QStringLiteral("—"));
    }
    m_probabilityLabel->setText(
        tr("按已尝试次数估算，累计找到答案的机会约 %1%（非完成进度）。")
            .arg(std::min(99.9, probability * 100.0), 0, 'f', 1));
    m_currentCandidate->setText(m_prefix + QString::number(nextNonce));
}

void SolverWindow::handleSolved(const QString &code,
                                quint32 nonce,
                                quint32 hashPrefix,
                                quint64 attempts)
{
    m_found = true;
    m_solutionEdit->setText(code);
    m_solutionEdit->setCursorPosition(0);
    m_solutionEdit->setPlaceholderText(QString());
    m_copyButton->setEnabled(true);
    m_solutionMeta->setText(
        tr("已找到有效解锁码，共计算 %1 次。复制后粘贴回原来的页面即可。")
            .arg(ui::groupedNumber(attempts)));
    m_solutionEdit->setToolTip(tr("命中序号 %1；哈希前缀 %2，小于通过阈值 %3。")
                                 .arg(nonce)
                                 .arg(QStringLiteral("%1").arg(hashPrefix, 8, 16, QLatin1Char('0')))
                                 .arg(ui::groupedNumber(m_challenge.threshold())));
    m_attemptsValue->setText(tr("%1 次").arg(ui::groupedNumber(attempts)));
    m_etaValue->setText(QStringLiteral("—"));
    m_probabilityLabel->setText(tr("已找到有效答案，可以复制解锁码。"));
    m_progress->setRange(0, 1);
    m_progress->setValue(1);
    m_currentCandidate->setText(tr("已命中：%1").arg(code));
    statusBar()->showMessage(tr("已找到解锁码"), 5000);
}

void SolverWindow::handleCancelled(quint64 attempts)
{
    if (m_found) {
        return;
    }
    m_solutionEdit->setPlaceholderText(tr("计算已停止，未找到解锁码。"));
    m_solutionMeta->setText(
        tr("已停止，共尝试 %1 次；再次开始会使用新的随机前缀。")
            .arg(ui::groupedNumber(attempts)));
    m_copyButton->setEnabled(false);
    m_etaValue->setText(QStringLiteral("—"));
    m_probabilityLabel->setText(tr("已停止；重新开始会从新的随机前缀开始计算。"));
    statusBar()->showMessage(tr("搜索已停止"), 4000);
}

void SolverWindow::handleFailed(const QString &message)
{
    m_solutionEdit->setPlaceholderText(tr("搜索失败。"));
    m_solutionMeta->setText(message);
    m_copyButton->setEnabled(false);
    m_etaValue->setText(QStringLiteral("—"));
    m_probabilityLabel->setText(tr("计算失败，请检查提示后重试。"));
    statusBar()->showMessage(tr("搜索失败"), 5000);
    QMessageBox::critical(this, tr("搜索失败"), message);
}

void SolverWindow::handleWorkerFinished()
{
    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
    }
    setRunningUi(false);
}

void SolverWindow::loadExample()
{
    if (m_running) return;
    m_challengeEdit->setText(
        QStringLiteral("CHALLENGE:262144:1:SALT40519cd9046c:800"));
    m_challengeEdit->setCursorPosition(0);
    parseInput(true);
    statusBar()->showMessage(tr("已载入示例挑战"), 3000);
}

void SolverWindow::resetResult()
{
    m_found = false;
    m_solutionEdit->clear();
    m_solutionEdit->setToolTip(QString());
    m_solutionEdit->setPlaceholderText(tr("找到的解会显示在这里"));
    m_copyButton->setEnabled(false);
    m_solutionMeta->setText(
        tr("找到后点击“复制解锁码”，再粘贴回要求解答的页面。"));
    m_attemptsValue->setText(tr("0 次"));
    m_rateValue->setText(tr("0.0 次/秒"));
    m_elapsedValue->setText(QStringLiteral("0:00"));
    m_etaValue->setText(QStringLiteral("—"));
    m_probabilityLabel->setText(tr("尚未开始计算。"));
    m_progress->setRange(0, 1);
    m_progress->setValue(0);
    m_currentCandidate->setText(QStringLiteral("—"));
}

void SolverWindow::setRunningUi(bool running)
{
    m_running = running;
    m_stopping = false;
    if (running) {
        m_progress->setRange(0, 0);
    } else {
        m_progress->setRange(0, 1);
        m_progress->setValue(m_found ? 1 : 0);
    }
    m_challengeEdit->setEnabled(!running);
    m_pasteButton->setEnabled(!running);
    m_exampleAction->setEnabled(!running);
    m_recommendButton->setEnabled(!running && m_challengeValid);
    m_loadExampleButton->setEnabled(!running);
    m_threadsSpin->setEnabled(!running);
    updateStartButton();

    if (running) {
        statusBar()->showMessage(
            tr("正在使用 %1 个线程搜索。停止操作会等待当前批次结束。")
                .arg(m_threadsSpin->value()));
    } else {
        m_inputStatus->setText(m_found
            ? tr("当前挑战已解答。解锁码可在第 3 步复制。")
            : tr("计算已结束。可重新开始，或粘贴其他挑战码。"));
    }
}

void SolverWindow::updateStartButton()
{
    m_startButton->setText(m_running ? tr("停止计算(&S)")
                                     : (m_found ? tr("重新计算(&S)")
                                                : tr("开始解答(&S)")));
    m_startButton->setEnabled(!m_stopping && (m_running || m_challengeValid));
    m_recommendButton->setEnabled(!m_running && m_challengeValid);
}

void SolverWindow::copySolution()
{
    if (m_solutionEdit->text().isEmpty()) {
        return;
    }
    QApplication::clipboard()->setText(m_solutionEdit->text());
    statusBar()->showMessage(tr("解锁码已复制到剪贴板"), 3000);
}

void SolverWindow::closeEvent(QCloseEvent *event)
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->requestStop();
        m_worker->wait();
    }
    QMainWindow::closeEvent(event);
}

void SolverWindow::showAbout()
{
    ui::showAbout(this,
                  tr("关于 Argon2 PoW 解答端"),
                  tr("Argon2 PoW 解答端\n\n"
                                 "按参考脚本的规则搜索 UNBLOCK 解锁码。\n"
                                 "Argon2id，版本 19，并行度 1，32 字节哈希。"));
}
