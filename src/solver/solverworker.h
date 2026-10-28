// SPDX-License-Identifier: GPL-3.0-only
#ifndef SOLVERWORKER_H
#define SOLVERWORKER_H

#include "../common/pow.h"

#include <QThread>

#include <atomic>

class SolverWorker : public QThread
{
    Q_OBJECT

public:
    SolverWorker(const argonpow::Challenge &challenge,
                 const QString &prefix,
                 int threads,
                 QObject *parent = nullptr);

    void requestStop();

signals:
    void progress(quint64 attempts,
                  quint32 nextNonce,
                  double elapsedSeconds,
                  double rate,
                  double probability);
    void solved(const QString &code, quint32 nonce, quint32 hashPrefix, quint64 attempts);
    void cancelled(quint64 attempts);
    void failed(const QString &message);

protected:
    void run() override;

private:
    argonpow::Challenge m_challenge;
    QString m_prefix;
    int m_threads = 1;
    std::atomic_bool m_stop{false};
};

#endif // SOLVERWORKER_H
