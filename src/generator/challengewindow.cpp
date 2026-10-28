// SPDX-License-Identifier: GPL-3.0-only
#include "challengewindow.h"

#include "../common/pow.h"
#include "../common/ui.h"
#include "../common/i18n.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QThread>
#include <QVBoxLayout>

#include <memory>
#include <cmath>

namespace {

// Preserve every KiB when decoding a challenge without showing trailing zeroes.
class MemorySpinBox : public QDoubleSpinBox
{
public:
    using QDoubleSpinBox::QDoubleSpinBox;

protected:
    QString textFromValue(double value) const override
    {
        QString text = QDoubleSpinBox::textFromValue(value);
        const QString decimal = locale().decimalPoint();
        if (text.contains(decimal)) {
            while (text.endsWith(locale().zeroDigit())) text.chop(1);
            if (text.endsWith(decimal)) text.chop(decimal.size());
        }
        return text;
    }
};

} // namespace

ChallengeWindow::ChallengeWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Argon2 PoW 出题端"));
    setMinimumSize(600, 460);
    resize(minimumSize());

    buildMenus();
    buildUi();
}

ChallengeWindow::~ChallengeWindow()
{
    if (m_verificationThread) {
        m_verificationThread->wait();
    }
}

void ChallengeWindow::buildMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));

    QAction *generateAction = fileMenu->addAction(tr("生成挑战码(&G)"));
    generateAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+G")));
    connect(generateAction, &QAction::triggered, this, &ChallengeWindow::generateChallenge);

    QAction *exampleAction = fileMenu->addAction(tr("载入示例参数(&E)"));
    connect(exampleAction, &QAction::triggered, this, &ChallengeWindow::loadExample);

    QAction *importAction = fileMenu->addAction(tr("导入并检查挑战码(&I)"));
    importAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+I")));
    connect(importAction, &QAction::triggered, this, [this]() {
        m_importToggle->setChecked(true);
        m_importEdit->setFocus();
        m_importEdit->selectAll();
    });

    QAction *copyAction = fileMenu->addAction(tr("复制挑战码(&C)"));
    connect(copyAction, &QAction::triggered, this, &ChallengeWindow::copyChallenge);

    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(tr("退出(&X)"));
    quitAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Q")));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    i18n::addLanguageMenu(this);

    QMenu *helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    QAction *aboutAction = helpMenu->addAction(tr("关于(&A)"));
    connect(aboutAction, &QAction::triggered, this, &ChallengeWindow::showAbout);
}

void ChallengeWindow::buildUi()
{
    auto *central = ui::scrollablePage(this);
    auto *page = new QVBoxLayout(central);
    page->setContentsMargins(12, 12, 12, 12);
    page->setSpacing(12);
    page->addWidget(ui::hintLabel(
        tr("设置计算要求，生成挑战码发给对方；收到解锁码后，可在下方校验。"), central));

    auto *importGroup = new QGroupBox(tr("已有挑战码？导入并检查"), central);
    auto *importLayout = new QVBoxLayout(importGroup);
    auto *importRow = new QHBoxLayout;
    m_importEdit = new QLineEdit(importGroup);
    m_importEdit->setObjectName(QStringLiteral("importEdit"));
    m_importEdit->setAccessibleName(tr("要导入的挑战码"));
    m_importEdit->setPlaceholderText(tr("粘贴 CHALLENGE:…，反查参数并校验解锁码"));
    m_importEdit->setFont(ui::monoFont());
    auto *importButton = new QPushButton(tr("导入并检查(&I)"), importGroup);
    importButton->setObjectName(QStringLiteral("importButton"));
    importRow->addWidget(m_importEdit, 1);
    importRow->addWidget(importButton);
    importLayout->addLayout(importRow);
    m_importStatus = ui::hintLabel(
        tr("检查格式与参数范围，回填下方设置；导入后可直接在第 3 步校验解锁码。"), importGroup);
    importLayout->addWidget(m_importStatus);
    m_importToggle = ui::detailsButton(tr("显示已有挑战码的导入与检查"), tr("收起已有挑战码的导入与检查"), importGroup, central);
    m_importToggle->setObjectName(QStringLiteral("importToggle"));
    page->addWidget(m_importToggle, 0, Qt::AlignLeft);
    page->addWidget(importGroup);

    auto *paramsGroup = new QGroupBox(tr("1. 设置挑战参数"), central);
    auto *paramsLayout = new QVBoxLayout(paramsGroup);
    auto *form = new QFormLayout;
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::WrapLongRows);
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(8);

    auto addSetting = [&](const QString &caption, QWidget *field, const QString &hint) {
        auto *row = new QWidget(paramsGroup);
        auto *layout = new QHBoxLayout(row);
        layout->setContentsMargins(0, 0, 0, 0);
        field->setMinimumWidth(140);
        layout->addWidget(field);
        layout->addWidget(ui::hintLabel(hint, row), 1);
        auto *label = new QLabel(caption, paramsGroup);
        label->setBuddy(field);
        form->addRow(label, row);
    };

    m_memorySpin = new MemorySpinBox(paramsGroup);
    m_memorySpin->setObjectName(QStringLiteral("memorySpin"));
    m_memorySpin->setDecimals(10);
    m_memorySpin->setRange(argonpow::MinMemoryKiB / 1024.0, argonpow::MaxMemoryKiB / 1024.0);
    m_memorySpin->setSingleStep(16);
    m_memorySpin->setValue(256);
    m_memorySpin->setSuffix(QStringLiteral(" MiB"));
    addSetting(tr("每线程内存(&M)："), m_memorySpin,
               tr("每个计算线程需要的内存，越大越占资源。"));

    m_timeSpin = new QSpinBox(paramsGroup);
    m_timeSpin->setObjectName(QStringLiteral("timeSpin"));
    m_timeSpin->setRange(1, static_cast<int>(argonpow::MaxTimeCost));
    m_timeSpin->setValue(1);
    m_timeSpin->setSuffix(tr(" 轮", nullptr, m_timeSpin->value()));
    connect(m_timeSpin, &QSpinBox::valueChanged, this, [this](int count) {
        m_timeSpin->setSuffix(tr(" 轮", nullptr, count));
    });
    addSetting(tr("计算轮数(&T)："), m_timeSpin,
               tr("单次尝试的计算轮数，越多耗时越长。"));

    m_difficultySpin = new QDoubleSpinBox(paramsGroup);
    m_difficultySpin->setObjectName(QStringLiteral("difficultySpin"));
    m_difficultySpin->setDecimals(0);
    m_difficultySpin->setRange(1, argonpow::MaxDifficulty);
    m_difficultySpin->setSingleStep(100);
    m_difficultySpin->setValue(800);
    m_difficultySpin->setGroupSeparatorShown(true);
    m_difficultySpin->setSuffix(tr(" 次"));
    addSetting(tr("平均尝试次数(&D)："), m_difficultySpin,
               tr("即难度；实际可能更早或更晚找到答案。"));

    auto *saltWidget = new QWidget(paramsGroup);
    auto *saltLayout = new QVBoxLayout(saltWidget);
    saltLayout->setContentsMargins(0, 0, 0, 0);
    saltLayout->setSpacing(6);
    m_saltMode = new QComboBox(saltWidget);
    m_saltMode->addItem(tr("自动生成（推荐）"));
    m_saltMode->addItem(tr("自定义盐值"));
    m_saltMode->setToolTip(tr("盐值用于区分挑战；自动模式下每次生成都会更新。"));
    m_saltEdit = new QLineEdit(saltWidget);
    m_saltEdit->setAccessibleName(tr("自定义盐值"));
    m_saltEdit->setPlaceholderText(tr("8–128 个 ASCII 字符，不含空格和冒号"));
    m_saltEdit->setMaxLength(128);
    m_saltEdit->hide();
    saltLayout->addWidget(m_saltMode);
    saltLayout->addWidget(m_saltEdit);
    auto *saltLabel = new QLabel(tr("盐值(&S)："), paramsGroup);
    saltLabel->setBuddy(m_saltMode);
    form->addRow(saltLabel, saltWidget);
    paramsLayout->addLayout(form);

    auto *buttonRow = new QHBoxLayout;
    auto *exampleButton = new QPushButton(tr("载入示例"), paramsGroup);
    exampleButton->setObjectName(QStringLiteral("exampleButton"));
    auto *generateButton = new QPushButton(tr("生成挑战码(&G)"), paramsGroup);
    generateButton->setObjectName(QStringLiteral("generateButton"));
    generateButton->setToolTip(tr("根据上方参数生成挑战码（Ctrl+G）；自动盐值每次都会改变。"));
    buttonRow->addWidget(exampleButton);
    buttonRow->addStretch();
    buttonRow->addWidget(generateButton);
    paramsLayout->addLayout(buttonRow);
    page->addWidget(paramsGroup);

    auto *challengeGroup = new QGroupBox(tr("2. 复制并发送挑战码"), central);
    auto *challengeLayout = new QVBoxLayout(challengeGroup);
    m_staleLabel = ui::hintLabel(
        tr("参数已修改。请先点击“生成挑战码”，再复制或校验。"), challengeGroup);
    m_staleLabel->hide();
    challengeLayout->addWidget(m_staleLabel);
    auto *challengeRow = new QHBoxLayout;
    m_challengeEdit = new QLineEdit(challengeGroup);
    m_challengeEdit->setObjectName(QStringLiteral("challengeEdit"));
    m_challengeEdit->setAccessibleName(tr("已生成的挑战码"));
    m_challengeEdit->setReadOnly(true);
    m_challengeEdit->setFont(ui::monoFont());
    m_copyButton = new QPushButton(tr("复制挑战码(&C)"), challengeGroup);
    m_copyButton->setObjectName(QStringLiteral("copyChallengeButton"));
    challengeRow->addWidget(m_challengeEdit, 1);
    challengeRow->addWidget(m_copyButton);
    challengeLayout->addLayout(challengeRow);
    challengeLayout->addWidget(ui::hintLabel(
        tr("将完整挑战码发给对方，让对方在解答端粘贴并开始计算。"), challengeGroup));

    auto *summary = new QWidget(challengeGroup);
    auto *summaryForm = new QFormLayout(summary);
    summaryForm->setContentsMargins(0, 0, 0, 0);
    summaryForm->setRowWrapPolicy(QFormLayout::WrapLongRows);
    m_summaryMemory = ui::valueLabel(summary);
    m_summaryTime = ui::valueLabel(summary);
    m_summarySalt = ui::valueLabel(summary);
    m_summarySalt->setFont(ui::monoFont(9));
    m_summaryDifficulty = ui::valueLabel(summary);
    m_summaryThreshold = ui::valueLabel(summary);
    summaryForm->addRow(tr("每线程内存："), m_summaryMemory);
    summaryForm->addRow(tr("计算轮数："), m_summaryTime);
    summaryForm->addRow(tr("盐值："), m_summarySalt);
    summaryForm->addRow(tr("平均尝试次数："), m_summaryDifficulty);
    summaryForm->addRow(tr("通过阈值："), m_summaryThreshold);
    challengeLayout->addWidget(ui::detailsButton(tr("显示当前挑战的技术参数"), tr("收起当前挑战的技术参数"), summary, challengeGroup),
                               0, Qt::AlignLeft);
    challengeLayout->addWidget(summary);
    page->addWidget(challengeGroup);

    auto *verifyGroup = new QGroupBox(tr("3. 校验对方的解锁码（可选）"), central);
    auto *verifyLayout = new QVBoxLayout(verifyGroup);
    auto *verifyRow = new QHBoxLayout;
    m_verifyEdit = new QLineEdit(verifyGroup);
    m_verifyEdit->setObjectName(QStringLiteral("verifyEdit"));
    m_verifyEdit->setAccessibleName(tr("待校验的解锁码"));
    m_verifyEdit->setPlaceholderText(tr("粘贴对方返回的 UNBLOCK-… 解锁码"));
    m_verifyEdit->setFont(ui::monoFont());
    m_verifyButton = new QPushButton(tr("校验解锁码(&V)"), verifyGroup);
    m_verifyButton->setObjectName(QStringLiteral("verifyButton"));
    m_verifyButton->setEnabled(false);
    verifyRow->addWidget(m_verifyEdit, 1);
    verifyRow->addWidget(m_verifyButton);
    verifyLayout->addLayout(verifyRow);
    m_verifyResult = ui::hintLabel(
        tr("使用第 2 步显示的挑战码校验；粘贴解锁码后按 Enter 也可校验。"), verifyGroup);
    m_verifyResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
    verifyLayout->addWidget(m_verifyResult);
    page->addWidget(verifyGroup);
    page->addStretch();

    connect(importButton, &QPushButton::clicked, this, &ChallengeWindow::importChallenge);
    connect(m_importEdit, &QLineEdit::returnPressed, this, &ChallengeWindow::importChallenge);
    connect(m_importEdit, &QLineEdit::textChanged, this, [this]() {
        m_importStatus->setText(tr("点击“导入并检查”或按 Enter，检查后才会替换当前挑战。"));
    });

    connect(generateButton, &QPushButton::clicked, this, &ChallengeWindow::generateChallenge);
    connect(exampleButton, &QPushButton::clicked, this, &ChallengeWindow::loadExample);
    connect(m_copyButton, &QPushButton::clicked, this, &ChallengeWindow::copyChallenge);
    connect(m_verifyButton, &QPushButton::clicked, this, &ChallengeWindow::verifySubmittedCode);
    connect(m_verifyEdit, &QLineEdit::returnPressed, this, &ChallengeWindow::verifySubmittedCode);
    connect(m_verifyEdit, &QLineEdit::textChanged, this, [this]() {
        m_verifyButton->setEnabled(!m_verificationThread && !m_parametersStale
                                  && !m_verifyEdit->text().trimmed().isEmpty());
        m_verifyResult->setText(tr("使用第 2 步显示的挑战码校验；粘贴解锁码后按 Enter 也可校验。"));
    });
    connect(m_saltMode, &QComboBox::currentIndexChanged, this, &ChallengeWindow::updateSaltMode);
    connect(m_saltEdit, &QLineEdit::textChanged, this, &ChallengeWindow::markParametersChanged);
    connect(m_memorySpin, &QDoubleSpinBox::valueChanged, this, &ChallengeWindow::markParametersChanged);
    connect(m_timeSpin, &QSpinBox::valueChanged, this, &ChallengeWindow::markParametersChanged);
    connect(m_difficultySpin, &QDoubleSpinBox::valueChanged, this, &ChallengeWindow::markParametersChanged);

    QWidget::setTabOrder(m_memorySpin, m_timeSpin);
    QWidget::setTabOrder(m_timeSpin, m_difficultySpin);
    QWidget::setTabOrder(m_difficultySpin, m_saltMode);
    QWidget::setTabOrder(m_saltMode, m_saltEdit);
    QWidget::setTabOrder(m_saltEdit, exampleButton);
    QWidget::setTabOrder(exampleButton, generateButton);
    QWidget::setTabOrder(generateButton, m_challengeEdit);
    QWidget::setTabOrder(m_challengeEdit, m_copyButton);
    updateSaltMode();
    generateChallenge();
    m_memorySpin->setFocus();
}

void ChallengeWindow::importChallenge()
{
    const auto parsed = argonpow::parseChallenge(m_importEdit->text());
    if (!parsed.ok) {
        m_importStatus->setText(tr("无法导入：%1 当前挑战未更改。").arg(parsed.error));
        m_importEdit->setFocus();
        return;
    }
    const auto &challenge = parsed.challenge;
    m_memorySpin->setValue(static_cast<double>(challenge.memoryKiB) / 1024.0);
    m_timeSpin->setValue(static_cast<int>(challenge.timeCost));
    m_difficultySpin->setValue(static_cast<double>(challenge.difficulty));
    m_saltMode->setCurrentIndex(1);
    m_saltEdit->setText(challenge.salt);
    // Keep the imported salt and exact parameters; importing never randomizes them.
    generateChallenge();
    m_importStatus->setText(
        tr("格式与参数检查通过：内存 %1 KiB，%2 轮，难度 %3。已设为当前挑战，可在第 3 步校验解锁码。")
            .arg(ui::groupedNumber(challenge.memoryKiB)).arg(challenge.timeCost).arg(ui::groupedNumber(challenge.difficulty)));
    m_verifyEdit->setFocus();
    statusBar()->showMessage(tr("已导入挑战码并还原参数；这不代表已有有效解锁码。"), 5000);
}

void ChallengeWindow::generateChallenge()
{
    QString salt;
    if (m_saltMode->currentIndex() == 1) {
        salt = m_saltEdit->text().trimmed();
        QString reason;
        if (!argonpow::isValidSalt(salt, &reason)) {
            QMessageBox::warning(this, tr("无法生成挑战码"), reason);
            m_saltEdit->setFocus();
            m_saltEdit->selectAll();
            return;
        }
        m_saltEdit->setText(salt);
    } else {
        salt = argonpow::randomSalt();
    }

    argonpow::Challenge challenge;
    challenge.memoryKiB = static_cast<quint32>(std::llround(m_memorySpin->value() * 1024.0));
    challenge.timeCost = static_cast<quint32>(m_timeSpin->value());
    challenge.difficulty = static_cast<quint32>(m_difficultySpin->value());
    challenge.salt = salt;

    const QString encoded = challenge.encoded();
    m_challengeEdit->setText(encoded);
    m_challengeEdit->setCursorPosition(0);
    m_parametersStale = false;
    m_copyButton->setEnabled(true);
    m_staleLabel->setVisible(false);
    updateSummary();

    m_verifyEdit->clear();
    m_verifyButton->setEnabled(false);
    m_verifyResult->setText(
        tr("使用第 2 步显示的挑战码校验；粘贴解锁码后按 Enter 也可校验。"));

    statusBar()->showMessage(tr("挑战码已生成"), 3000);
}

void ChallengeWindow::updateSaltMode()
{
    const bool custom = m_saltMode->currentIndex() == 1;
    m_saltEdit->setVisible(custom);
    if (custom && m_saltEdit->text().trimmed().isEmpty()) {
        m_saltEdit->setText(argonpow::randomSalt());
    }
    markParametersChanged();
}

void ChallengeWindow::updateSummary()
{
    const argonpow::ParseResult parsed = argonpow::parseChallenge(m_challengeEdit->text());
    if (!parsed.ok) {
        return;
    }
    const argonpow::Challenge &challenge = parsed.challenge;

    const double mib = static_cast<double>(challenge.memoryKiB) / 1024.0;
    m_summaryMemory->setText(
        QStringLiteral("%1 MiB（%2 KiB）")
            .arg(mib, 0, 'f', mib < 10.0 ? 1 : 0)
            .arg(ui::groupedNumber(challenge.memoryKiB)));
    m_summaryTime->setText(tr("%n 轮", nullptr, static_cast<int>(challenge.timeCost)));
    m_summarySalt->setText(challenge.salt);
    m_summaryDifficulty->setText(
        tr("%1 次（约 1/%1 通过率）").arg(ui::groupedNumber(challenge.difficulty)));
    m_summaryThreshold->setText(
        tr("%1（哈希前缀小于该值即通过）")
            .arg(ui::groupedNumber(challenge.threshold())));
}

void ChallengeWindow::markParametersChanged()
{
    if (m_staleLabel && !m_challengeEdit->text().isEmpty()) {
        m_parametersStale = true;
        m_staleLabel->setVisible(true);
        m_copyButton->setEnabled(false);
        m_verifyButton->setEnabled(false);
        m_verifyResult->setText(tr("参数已修改，请先重新生成挑战码。"));
    }
}

void ChallengeWindow::loadExample()
{
    m_memorySpin->setValue(256);
    m_timeSpin->setValue(1);
    m_difficultySpin->setValue(800);
    m_saltMode->setCurrentIndex(1);
    m_saltEdit->setText(QStringLiteral("SALT40519cd9046c"));
    generateChallenge();

    m_verifyEdit->setText(QStringLiteral("UNBLOCK-G5G75SJH-832"));
    verifySubmittedCode();
}

void ChallengeWindow::copyChallenge()
{
    if (m_parametersStale || m_challengeEdit->text().isEmpty()) {
        return;
    }
    QApplication::clipboard()->setText(m_challengeEdit->text());
    statusBar()->showMessage(tr("挑战码已复制到剪贴板"), 3000);
}

void ChallengeWindow::verifySubmittedCode()
{
    if (m_verificationThread) {
        return;
    }
    if (m_parametersStale) {
        m_verifyResult->setText(tr("参数已修改，请先重新生成挑战码。"));
        return;
    }
    const argonpow::ParseResult parsed = argonpow::parseChallenge(m_challengeEdit->text());
    if (!parsed.ok) {
        QMessageBox::warning(this, tr("无法校验"), parsed.error);
        return;
    }
    if (m_verifyEdit->text().trimmed().isEmpty()) {
        m_verifyResult->setText(tr("请输入解锁码。"));
        m_verifyEdit->setFocus();
        return;
    }

    const QString challengeText = m_challengeEdit->text();
    const QString code = m_verifyEdit->text();
    auto result = std::make_shared<argonpow::VerifyResult>();
    auto *worker = QThread::create([challenge = parsed.challenge, code, result]() {
        *result = argonpow::verifyUnlockCode(challenge, code);
    });
    worker->setParent(this);
    m_verificationThread = worker;
    m_verifyButton->setEnabled(false);
    m_verifyButton->setText(tr("正在校验…"));
    m_verifyResult->setText(tr("正在本机校验，请稍候。参数越高，单次校验耗时越长。"));
    connect(worker, &QThread::finished, this, [this, worker, challengeText, code, result]() {
        m_verificationThread = nullptr;
        worker->deleteLater();
        m_verifyButton->setText(tr("校验解锁码(&V)"));
        m_verifyButton->setEnabled(!m_parametersStale && !m_verifyEdit->text().trimmed().isEmpty());
        // Never attach a completed verification to inputs edited while it ran.
        if (m_parametersStale || m_challengeEdit->text() != challengeText
            || m_verifyEdit->text() != code) {
            return;
        }
        m_verifyResult->setText(result->message);
        statusBar()->showMessage(result->status == argonpow::VerifyStatus::Valid
                                     ? tr("校验通过") : tr("校验未通过"), 4000);
    });
    worker->start();
}

void ChallengeWindow::showAbout()
{
    ui::showAbout(this,
                  tr("关于 Argon2 PoW 出题端"),
                  tr("Argon2 PoW 出题端\n\n"
                                 "生成兼容参考脚本的 Argon2id 挑战码。\n"
                                 "挑战格式：CHALLENGE:内存KiB:时间成本:盐:难度\n"
                                 "Argon2id，版本 19，并行度 1，32 字节哈希。"));
}
