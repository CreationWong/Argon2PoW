// SPDX-License-Identifier: GPL-3.0-only
#ifndef POW_H
#define POW_H

#include <QByteArray>
#include <QString>

#include <functional>

namespace argonpow {

inline constexpr quint32 MinMemoryKiB = 8;
inline constexpr quint32 MaxMemoryKiB = 4U * 1024U * 1024U;
inline constexpr quint32 MaxTimeCost = 1000;
inline constexpr quint32 MaxDifficulty = 0xFFFFFFFFU;
inline constexpr int MaxSearchThreads = 32;

// A challenge uses the same wire format as the shell reference implementation:
//   CHALLENGE:<memory KiB>:<time cost>:<salt>:<difficulty>
struct Challenge
{
    quint32 memoryKiB = 262144;
    quint32 timeCost = 1;
    quint32 difficulty = 800;
    QString salt;

    quint64 threshold() const;
    QString encoded() const;
    QString memoryLabel() const;
};

struct ParseResult
{
    bool ok = false;
    Challenge challenge;
    QString error;
};

ParseResult parseChallenge(const QString &text);

// The reference script uses bc to compute 256^(4 - log_256(difficulty)) and
// printf "%.0f", which is 2^32 / difficulty rounded to nearest.
quint64 thresholdFor(quint32 difficulty);

bool isValidSalt(const QString &salt, QString *reason = nullptr);

struct HashResult
{
    bool ok = false;
    quint32 prefix = 0;
    QByteArray hash;
    QString error;
};

HashResult argon2Prefix(const QString &password, const Challenge &challenge);

enum class VerifyStatus {
    InvalidFormat,
    NoMatch,
    Valid
};

struct VerifyResult
{
    VerifyStatus status = VerifyStatus::InvalidFormat;
    quint32 prefix = 0;
    QString message;
};

VerifyResult verifyUnlockCode(const Challenge &challenge, const QString &code);

QString randomPrefix();
QString randomToken(int length);
QString randomSalt();

QString formatDuration(double seconds);
QString formatBytes(quint64 bytes);

struct SolveProgress
{
    quint64 attempts = 0;
    quint32 nextNonce = 1;
    double elapsedSeconds = 0.0;
    double rate = 0.0;
    double probability = 0.0;
};

enum class SolveStatus {
    Found,
    Cancelled,
    Error
};

struct SolveResult
{
    SolveStatus status = SolveStatus::Error;
    QString code;
    quint32 nonce = 0;
    quint32 hashPrefix = 0;
    quint64 attempts = 0;
    QString error;
};

using ProgressCallback = std::function<void(const SolveProgress &)>;
using StopCallback = std::function<bool()>;

// Searches candidates in ascending nonce order, W at a time. The returned
// solution is always the first valid nonce, exactly like the sequential
// reference script, while using up to `threads` CPU cores.
SolveResult solve(const Challenge &challenge,
                  const QString &prefix,
                  int threads,
                  const ProgressCallback &onProgress = {},
                  const StopCallback &shouldStop = {});

} // namespace argonpow

#endif // POW_H
