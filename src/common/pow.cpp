// SPDX-License-Identifier: GPL-3.0-only
#include "pow.h"

#include <argon2.h>

#include <QLocale>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QRegularExpression>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <limits>
#include <thread>
#include <vector>

namespace argonpow {

namespace {

constexpr double kTwoPow32 = 4294967296.0;

QString numberWithGrouping(quint64 value)
{
    return QLocale().toString(qulonglong(value));
}

bool isValidPrefix(const QString &prefix, QString *reason)
{
    if (prefix.size() != 17 || !prefix.startsWith(QStringLiteral("UNBLOCK-"))
        || !prefix.endsWith(QLatin1Char('-'))) {
        if (reason) {
            *reason = QCoreApplication::translate("PoW", "随机前缀格式不正确。");
        }
        return false;
    }
    for (int i = 8; i < 16; ++i) {
        const QChar c = prefix.at(i);
        if (!((c >= QLatin1Char('A') && c <= QLatin1Char('Z'))
              || (c >= QLatin1Char('0') && c <= QLatin1Char('9')))) {
            if (reason) {
                *reason = QCoreApplication::translate("PoW", "随机前缀只能包含大写字母和数字。");
            }
            return false;
        }
    }
    return true;
}

} // namespace

quint64 Challenge::threshold() const
{
    return thresholdFor(difficulty);
}

QString Challenge::encoded() const
{
    return QStringLiteral("CHALLENGE:%1:%2:%3:%4")
        .arg(memoryKiB)
        .arg(timeCost)
        .arg(salt)
        .arg(difficulty);
}

QString Challenge::memoryLabel() const
{
    const double mib = static_cast<double>(memoryKiB) / 1024.0;
    if (mib >= 1024.0) {
        return QStringLiteral("%1 GiB (%2 KiB)")
            .arg(mib / 1024.0, 0, 'f', 2)
            .arg(numberWithGrouping(memoryKiB));
    }
    return QStringLiteral("%1 MiB (%2 KiB)")
        .arg(mib, 0, 'f', mib < 10.0 ? 1 : 0)
        .arg(numberWithGrouping(memoryKiB));
}

quint64 thresholdFor(quint32 difficulty)
{
    if (difficulty == 0) {
        return 0;
    }
    // bc computes 256^(4 - log_256(difficulty)) = 2^32 / difficulty and
    // printf("%.0f") rounds to nearest. nearbyint() uses the default
    // round-to-nearest-even mode, matching printf on normal platforms.
    const double raw = kTwoPow32 / static_cast<double>(difficulty);
    if (raw >= kTwoPow32) {
        return static_cast<quint64>(kTwoPow32);
    }
    return static_cast<quint64>(std::nearbyint(raw));
}

ParseResult parseChallenge(const QString &text)
{
    ParseResult result;
    const QString input = text.trimmed();
    if (input.isEmpty()) {
        result.error = QCoreApplication::translate("PoW", "请输入挑战码。");
        return result;
    }

    const QStringList fields = input.split(QLatin1Char(':'), Qt::KeepEmptyParts);
    if (fields.size() != 5) {
        result.error = QCoreApplication::translate("PoW", "挑战码必须包含 5 段：CHALLENGE:内存:时间:盐:难度。");
        return result;
    }

    QStringList trimmed;
    trimmed.reserve(fields.size());
    for (const QString &field : fields) {
        trimmed.append(field.trimmed());
    }

    if (trimmed.at(0).compare(QStringLiteral("CHALLENGE"), Qt::CaseInsensitive) != 0) {
        result.error = QCoreApplication::translate("PoW", "挑战码应以 CHALLENGE 开头。");
        return result;
    }

    bool ok = false;
    const quint32 memory = trimmed.at(1).toUInt(&ok);
    if (!ok || memory < MinMemoryKiB || memory > MaxMemoryKiB) {
        result.error = QCoreApplication::translate("PoW", "内存参数必须在 %1 到 %2 KiB 之间。")
                           .arg(MinMemoryKiB)
                           .arg(MaxMemoryKiB);
        return result;
    }

    const quint32 time = trimmed.at(2).toUInt(&ok);
    if (!ok || time < 1 || time > MaxTimeCost) {
        result.error = QCoreApplication::translate("PoW", "时间成本必须在 1 到 %1 之间。").arg(MaxTimeCost);
        return result;
    }

    const QString salt = trimmed.at(3);
    QString saltReason;
    if (!isValidSalt(salt, &saltReason)) {
        result.error = saltReason;
        return result;
    }

    const quint32 difficulty = trimmed.at(4).toUInt(&ok);
    if (!ok || difficulty < 1 || difficulty > MaxDifficulty) {
        result.error = QCoreApplication::translate("PoW", "难度必须是 1 到 %1 之间的整数。").arg(MaxDifficulty);
        return result;
    }

    result.ok = true;
    result.challenge.memoryKiB = memory;
    result.challenge.timeCost = time;
    result.challenge.difficulty = difficulty;
    result.challenge.salt = salt;
    return result;
}

bool isValidSalt(const QString &salt, QString *reason)
{
    if (salt.size() < 8 || salt.size() > 128) {
        if (reason) {
            *reason = QCoreApplication::translate("PoW", "盐长度必须在 8 到 128 个字符之间。");
        }
        return false;
    }
    for (const QChar ch : salt) {
        const ushort u = ch.unicode();
        if (u < 0x21 || u > 0x7E || ch == QLatin1Char(':')) {
            if (reason) {
                *reason = QCoreApplication::translate("PoW", "盐只能使用可打印 ASCII 字符，且不能包含冒号或空格。");
            }
            return false;
        }
    }
    return true;
}

HashResult argon2Prefix(const QString &password, const Challenge &challenge)
{
    HashResult result;
    const QByteArray passwordBytes = password.toUtf8();
    const QByteArray saltBytes = challenge.salt.toUtf8();

    unsigned char raw[32] = {};
    const int rc = argon2id_hash_raw(challenge.timeCost,
                                     challenge.memoryKiB,
                                     1,
                                     passwordBytes.constData(),
                                     static_cast<size_t>(passwordBytes.size()),
                                     saltBytes.constData(),
                                     static_cast<size_t>(saltBytes.size()),
                                     raw,
                                     sizeof(raw));
    if (rc != ARGON2_OK) {
        result.error = QCoreApplication::translate("PoW", "Argon2 计算失败：%1（错误码 %2）。")
                           .arg(QString::fromUtf8(argon2_error_message(rc)))
                           .arg(rc);
        return result;
    }

    result.ok = true;
    result.hash = QByteArray(reinterpret_cast<const char *>(raw), sizeof(raw));
    result.prefix = (quint32(raw[0]) << 24) | (quint32(raw[1]) << 16)
                    | (quint32(raw[2]) << 8) | quint32(raw[3]);
    return result;
}

VerifyResult verifyUnlockCode(const Challenge &challenge, const QString &code)
{
    VerifyResult result;
    const QString value = code.trimmed();

    static const QRegularExpression pattern(
        QStringLiteral("^UNBLOCK-([A-Z0-9]{8})-([1-9][0-9]{0,9})$"));
    const QRegularExpressionMatch match = pattern.match(value);
    if (!match.hasMatch()) {
        result.message = QCoreApplication::translate("PoW", "格式无效。解锁码应形如 UNBLOCK-XXXXXXXX-123（8 位大写字母/数字，加非零序号）。");
        return result;
    }

    bool ok = false;
    const qulonglong nonce = match.capturedView(2).toULongLong(&ok);
    if (!ok || nonce > std::numeric_limits<quint32>::max()) {
        result.message = QCoreApplication::translate("PoW", "解锁码序号超出支持范围。");
        return result;
    }

    const HashResult hash = argon2Prefix(value, challenge);
    if (!hash.ok) {
        result.message = hash.error;
        return result;
    }

    result.prefix = hash.prefix;
    const quint64 threshold = challenge.threshold();
    if (quint64(hash.prefix) < threshold) {
        result.status = VerifyStatus::Valid;
        result.message = QCoreApplication::translate("PoW", "校验通过：哈希前缀 %1 < 阈值 %2。")
                             .arg(QStringLiteral("%1").arg(hash.prefix, 8, 16, QLatin1Char('0')))
                             .arg(numberWithGrouping(threshold));
    } else {
        result.status = VerifyStatus::NoMatch;
        result.message = QCoreApplication::translate("PoW", "未达到难度要求：哈希前缀 %1 ≥ 阈值 %2。")
                             .arg(QStringLiteral("%1").arg(hash.prefix, 8, 16, QLatin1Char('0')))
                             .arg(numberWithGrouping(threshold));
    }
    return result;
}

QString randomToken(int length)
{
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString token;
    token.reserve(length);
    QRandomGenerator *rng = QRandomGenerator::system();
    for (int i = 0; i < length; ++i) {
        token.append(QLatin1Char(alphabet[rng->bounded(36)]));
    }
    return token;
}

QString randomPrefix()
{
    return QStringLiteral("UNBLOCK-%1-").arg(randomToken(8));
}

QString randomSalt()
{
    static constexpr char hex[] = "0123456789abcdef";
    QString suffix;
    suffix.reserve(12);
    QRandomGenerator *rng = QRandomGenerator::system();
    for (int i = 0; i < 12; ++i) {
        suffix.append(QLatin1Char(hex[rng->bounded(16)]));
    }
    return QStringLiteral("SALT%1").arg(suffix);
}

QString formatDuration(double seconds)
{
    if (!std::isfinite(seconds) || seconds < 0.0) {
        return QStringLiteral("--");
    }
    if (seconds < 60.0) {
        return QCoreApplication::translate("PoW", "%1 秒").arg(seconds, 0, 'f', seconds < 10.0 ? 1 : 0);
    }
    const qint64 total = static_cast<qint64>(std::llround(seconds));
    const qint64 h = total / 3600;
    const qint64 m = (total % 3600) / 60;
    const qint64 s = total % 60;
    if (h > 0) {
        return QStringLiteral("%1:%2:%3")
            .arg(h)
            .arg(m, 2, 10, QLatin1Char('0'))
            .arg(s, 2, 10, QLatin1Char('0'));
    }
    return QStringLiteral("%1:%2").arg(m).arg(s, 2, 10, QLatin1Char('0'));
}

QString formatBytes(quint64 bytes)
{
    constexpr double kMiB = 1024.0 * 1024.0;
    constexpr double kGiB = kMiB * 1024.0;
    if (bytes >= static_cast<quint64>(kGiB)) {
        const double value = static_cast<double>(bytes) / kGiB;
        return QStringLiteral("%1 GiB").arg(value, 0, 'f', value < 10.0 ? 2 : 1);
    }
    const double value = static_cast<double>(bytes) / kMiB;
    return QStringLiteral("%1 MiB").arg(value, 0, 'f', value < 10.0 ? 1 : 0);
}

SolveResult solve(const Challenge &challenge,
                  const QString &prefix,
                  int threads,
                  const ProgressCallback &onProgress,
                  const StopCallback &shouldStop)
{
    SolveResult result;
    QString validationError;
    if (!isValidPrefix(prefix, &validationError)) {
        result.error = validationError;
        return result;
    }
    if (challenge.timeCost == 0 || challenge.memoryKiB < MinMemoryKiB) {
        result.error = QCoreApplication::translate("PoW", "挑战参数无效。");
        return result;
    }
    const quint64 threshold = challenge.threshold();
    if (threshold == 0) {
        result.error = QCoreApplication::translate("PoW", "难度阈值无效。");
        return result;
    }

    const int width = std::clamp(threads, 1, MaxSearchThreads);
    const auto started = std::chrono::steady_clock::now();
    quint64 attempts = 0;
    quint64 batchStart = 1;
    auto lastReport = started;

    auto stopRequested = [&shouldStop]() {
        return shouldStop && shouldStop();
    };

    auto emitProgress = [&](bool force, quint32 nextNonce) {
        const auto now = std::chrono::steady_clock::now();
        if (!force && now - lastReport < std::chrono::milliseconds(80)) {
            return;
        }
        lastReport = now;
        if (!onProgress) {
            return;
        }
        const double elapsed = std::chrono::duration<double>(now - started).count();
        SolveProgress progress;
        progress.attempts = attempts;
        progress.nextNonce = nextNonce;
        progress.elapsedSeconds = elapsed;
        progress.rate = elapsed > 0.0 ? static_cast<double>(attempts) / elapsed : 0.0;
        const double mean = static_cast<double>(challenge.difficulty);
        progress.probability =
            attempts > 0 ? 1.0 - std::exp(-static_cast<double>(attempts) / mean) : 0.0;
        onProgress(progress);
    };

    emitProgress(true, 1);

    for (;;) {
        if (stopRequested()) {
            result.status = SolveStatus::Cancelled;
            result.attempts = attempts;
            emitProgress(true, static_cast<quint32>(std::min<quint64>(batchStart, 0xFFFFFFFFULL)));
            return result;
        }

        struct Slot
        {
            bool hashed = false;
            bool hit = false;
            quint32 prefix = 0;
            QString error;
        };

        std::vector<Slot> batchSlots(static_cast<size_t>(width));
        std::vector<std::thread> workers;
        workers.reserve(static_cast<size_t>(width));
        QString spawnError;

        try {
            for (int i = 0; i < width; ++i) {
                const quint64 nonce = batchStart + static_cast<quint64>(i);
                if (nonce > std::numeric_limits<quint32>::max()) {
                    break;
                }
                workers.emplace_back([&, i, nonce]() {
                    if (stopRequested()) {
                        return;
                    }
                    Slot &slot = batchSlots[static_cast<size_t>(i)];
                    const QString code = prefix + QString::number(nonce);
                    const HashResult hash = argon2Prefix(code, challenge);
                    slot.hashed = true;
                    if (!hash.ok) {
                        slot.error = hash.error;
                        return;
                    }
                    slot.prefix = hash.prefix;
                    slot.hit = quint64(hash.prefix) < threshold;
                });
            }
        } catch (const std::exception &ex) {
            spawnError = QCoreApplication::translate("PoW", "无法创建计算线程：%1").arg(QString::fromUtf8(ex.what()));
        }

        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        if (!spawnError.isEmpty()) {
            result.status = SolveStatus::Error;
            result.attempts = attempts;
            result.error = spawnError;
            return result;
        }

        int completed = 0;
        int firstHit = -1;
        QString hashError;
        for (int i = 0; i < width; ++i) {
            const Slot &slot = batchSlots[static_cast<size_t>(i)];
            if (!slot.hashed) {
                continue;
            }
            ++completed;
            if (!slot.error.isEmpty()) {
                hashError = slot.error;
                continue;
            }
            if (slot.hit && firstHit < 0) {
                firstHit = i;
            }
        }

        attempts += static_cast<quint64>(completed);

        if (!hashError.isEmpty()) {
            result.status = SolveStatus::Error;
            result.attempts = attempts;
            result.error = hashError;
            return result;
        }

        if (firstHit >= 0) {
            const quint64 nonce = batchStart + static_cast<quint64>(firstHit);
            result.status = SolveStatus::Found;
            result.code = prefix + QString::number(nonce);
            result.nonce = static_cast<quint32>(nonce);
            result.hashPrefix = batchSlots[static_cast<size_t>(firstHit)].prefix;
            result.attempts = attempts;
            emitProgress(true, result.nonce);
            return result;
        }

        const quint64 next = batchStart + static_cast<quint64>(width);
        if (stopRequested()) {
            result.status = SolveStatus::Cancelled;
            result.attempts = attempts;
            emitProgress(true, static_cast<quint32>(std::min<quint64>(next, 0xFFFFFFFFULL)));
            return result;
        }
        if (next > std::numeric_limits<quint32>::max()) {
            result.status = SolveStatus::Error;
            result.attempts = attempts;
            result.error = QCoreApplication::translate("PoW", "已搜索完 32 位序号空间，仍未找到符合条件的解。");
            return result;
        }
        batchStart = next;
        emitProgress(false, static_cast<quint32>(batchStart));
    }
}

} // namespace argonpow
