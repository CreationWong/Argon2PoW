// SPDX-License-Identifier: GPL-3.0-only
#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include "../common/pow.h"

#include <QMainWindow>

class QCloseEvent;
class QAction;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;
class QTimer;

class SolverWorker;

class SolverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SolverWindow(const QString &initialChallenge = QString(),
                          QWidget *parent = nullptr);
    ~SolverWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void buildUi();
    void buildMenus();
    void parseInput(bool showErrors);
    void clearParsedValues();
    void updateChallengeValues();
    void updateThreadHint();
    void setThreadsSilently(int value);
    int recommendedThreads() const;
    int safeThreadCount(quint32 memoryKiB) const;

    void startOrStop();
    void beginSolving();
    void requestStop();
    void handleProgress(quint64 attempts,
                        quint32 nextNonce,
                        double elapsedSeconds,
                        double rate,
                        double probability);
    void handleSolved(const QString &code, quint32 nonce, quint32 hashPrefix, quint64 attempts);
    void handleCancelled(quint64 attempts);
    void handleFailed(const QString &message);
    void handleWorkerFinished();
    void loadExample();
    void resetResult();
    void setRunningUi(bool running);
    void updateStartButton();
    void copySolution();
    void showAbout();

    QTimer *m_parseTimer = nullptr;
    QLineEdit *m_challengeEdit = nullptr;
    QPushButton *m_pasteButton = nullptr;
    QPushButton *m_loadExampleButton = nullptr;
    QAction *m_exampleAction = nullptr;
    QLabel *m_inputStatus = nullptr;

    QLabel *m_memoryValue = nullptr;
    QLabel *m_timeValue = nullptr;
    QLabel *m_saltValue = nullptr;
    QLabel *m_difficultyValue = nullptr;
    QLabel *m_thresholdValue = nullptr;

    QSpinBox *m_threadsSpin = nullptr;
    QLabel *m_threadHint = nullptr;
    QLabel *m_memoryEstimate = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_recommendButton = nullptr;

    QLabel *m_attemptsValue = nullptr;
    QLabel *m_rateValue = nullptr;
    QLabel *m_elapsedValue = nullptr;
    QLabel *m_etaValue = nullptr;
    QLabel *m_probabilityLabel = nullptr;
    QLabel *m_currentCandidate = nullptr;
    QProgressBar *m_progress = nullptr;

    QLineEdit *m_solutionEdit = nullptr;
    QPushButton *m_copyButton = nullptr;
    QLabel *m_solutionMeta = nullptr;

    SolverWorker *m_worker = nullptr;
    argonpow::Challenge m_challenge;
    QString m_prefix;
    bool m_running = false;
    bool m_stopping = false;
    bool m_challengeValid = false;
    bool m_found = false;
    bool m_threadsUserSet = false;
    bool m_adjustingThreads = false;
};

#endif // SOLVERWINDOW_H
