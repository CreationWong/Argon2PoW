// SPDX-License-Identifier: GPL-3.0-only
#include "solverworker.h"

SolverWorker::SolverWorker(const argonpow::Challenge &challenge,
                           const QString &prefix,
                           int threads,
                           QObject *parent)
    : QThread(parent)
    , m_challenge(challenge)
    , m_prefix(prefix)
    , m_threads(threads)
{
}

void SolverWorker::requestStop()
{
    m_stop.store(true, std::memory_order_relaxed);
}

void SolverWorker::run()
{
    const argonpow::SolveResult result = argonpow::solve(
        m_challenge,
        m_prefix,
        m_threads,
        [this](const argonpow::SolveProgress &p) {
            emit progress(p.attempts,
                          p.nextNonce,
                          p.elapsedSeconds,
                          p.rate,
                          p.probability);
        },
        [this]() { return m_stop.load(std::memory_order_relaxed); });

    switch (result.status) {
    case argonpow::SolveStatus::Found:
        emit solved(result.code, result.nonce, result.hashPrefix, result.attempts);
        break;
    case argonpow::SolveStatus::Cancelled:
        emit cancelled(result.attempts);
        break;
    case argonpow::SolveStatus::Error:
        emit failed(result.error);
        break;
    }
}
