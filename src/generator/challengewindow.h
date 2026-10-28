// SPDX-License-Identifier: GPL-3.0-only
#ifndef CHALLENGEWINDOW_H
#define CHALLENGEWINDOW_H

#include <QMainWindow>

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QThread;

class ChallengeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChallengeWindow(QWidget *parent = nullptr);
    ~ChallengeWindow() override;

private:
    void buildUi();
    void buildMenus();
    void generateChallenge();
    void importChallenge();
    void updateSaltMode();
    void updateSummary();
    void markParametersChanged();
    void loadExample();
    void copyChallenge();
    void verifySubmittedCode();
    void showAbout();

    QDoubleSpinBox *m_memorySpin = nullptr;
    QSpinBox *m_timeSpin = nullptr;
    QDoubleSpinBox *m_difficultySpin = nullptr;
    QComboBox *m_saltMode = nullptr;
    QLineEdit *m_saltEdit = nullptr;
    QLabel *m_staleLabel = nullptr;
    QLineEdit *m_importEdit = nullptr;
    QLabel *m_importStatus = nullptr;
    QPushButton *m_importToggle = nullptr;

    QLineEdit *m_challengeEdit = nullptr;
    QPushButton *m_copyButton = nullptr;
    QPushButton *m_verifyButton = nullptr;
    bool m_parametersStale = false;
    QThread *m_verificationThread = nullptr;

    QLabel *m_summaryMemory = nullptr;
    QLabel *m_summaryTime = nullptr;
    QLabel *m_summarySalt = nullptr;
    QLabel *m_summaryDifficulty = nullptr;
    QLabel *m_summaryThreshold = nullptr;

    QLineEdit *m_verifyEdit = nullptr;
    QLabel *m_verifyResult = nullptr;
};

#endif // CHALLENGEWINDOW_H
