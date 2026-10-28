// SPDX-License-Identifier: GPL-3.0-only
#include "../src/common/pow.h"

#include <QRegularExpression>

#include <atomic>
#include <cstdio>

namespace {

int failures = 0;

void check(bool condition, const char *what)
{
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    } else {
        std::fprintf(stderr, "ok:   %s\n", what);
    }
}

} // namespace

int main()
{
    const QString sample =
        QStringLiteral("CHALLENGE:262144:1:SALT40519cd9046c:800");
    const argonpow::ParseResult parsed = argonpow::parseChallenge(sample);
    check(parsed.ok, "sample challenge parses");
    check(parsed.challenge.memoryKiB == 262144, "sample memory is 262144 KiB");
    check(parsed.challenge.timeCost == 1, "sample time cost is 1");
    check(parsed.challenge.salt == QStringLiteral("SALT40519cd9046c"), "sample salt");
    check(parsed.challenge.difficulty == 800, "sample difficulty is 800");
    check(parsed.challenge.encoded() == sample, "challenge round-trips");
    check(parsed.challenge.threshold() == 5368709, "threshold is printf(2^32/800)");
    check(argonpow::thresholdFor(1) == 4294967296ULL, "difficulty 1 accepts every prefix");

    const argonpow::HashResult sampleHash =
        argonpow::argon2Prefix(QStringLiteral("UNBLOCK-G5G75SJH-832"), parsed.challenge);
    check(sampleHash.ok, "sample answer hashes");
    check(sampleHash.prefix == 828271U, "sample answer prefix is 000ca36f");
    check(sampleHash.hash.toHex() == QByteArray("000ca36f2cee96b2851dbc20d5c5a46f"
                                                "b6451c35aa87df0e9c0cf13147edd2de"),
          "sample hash matches the reference implementation");

    const argonpow::VerifyResult verified =
        argonpow::verifyUnlockCode(parsed.challenge, QStringLiteral("UNBLOCK-G5G75SJH-832"));
    check(verified.status == argonpow::VerifyStatus::Valid, "sample answer verifies");
    check(verified.prefix == 828271U, "verified prefix");

    const argonpow::VerifyResult wrong =
        argonpow::verifyUnlockCode(parsed.challenge, QStringLiteral("UNBLOCK-G5G75SJH-831"));
    check(wrong.status != argonpow::VerifyStatus::Valid, "wrong answer is rejected");

    const argonpow::VerifyResult malformed =
        argonpow::verifyUnlockCode(parsed.challenge, QStringLiteral("UNBLOCK-bad-832"));
    check(malformed.status == argonpow::VerifyStatus::InvalidFormat, "malformed answer is rejected");

    check(!argonpow::parseChallenge(QStringLiteral("CHALLENGE:7:1:SALT12345:800")).ok,
          "memory below the Argon2 minimum is rejected");
    check(!argonpow::parseChallenge(QStringLiteral("CHALLENGE:262144:1:SALT:800")).ok,
          "short salt is rejected");
    check(!argonpow::parseChallenge(QStringLiteral("NOPE:262144:1:SALT40519cd9046c:800")).ok,
          "wrong prefix is rejected");
    check(!argonpow::parseChallenge(QStringLiteral("CHALLENGE:262144:1:SALT40519cd9046c:0")).ok,
          "zero difficulty is rejected");

    const QString randomA = argonpow::randomPrefix();
    const QString randomB = argonpow::randomPrefix();
    const QRegularExpression prefixPattern(QStringLiteral("^UNBLOCK-[A-Z0-9]{8}-$"));
    check(prefixPattern.match(randomA).hasMatch(), "random prefix format");
    check(prefixPattern.match(randomB).hasMatch(), "random prefix format 2");
    check(randomA != randomB, "random prefixes differ");

    const QString salt = argonpow::randomSalt();
    check(QRegularExpression(QStringLiteral("^SALT[0-9a-f]{12}$")).match(salt).hasMatch(),
          "random salt format");

    // Exercise the threaded search engine on a tiny challenge. Ordered batches
    // must return the same first nonce as a sequential search.
    argonpow::Challenge small;
    small.memoryKiB = 1024;
    small.timeCost = 1;
    small.difficulty = 4;
    small.salt = QStringLiteral("SALTtest1234");
    const QString prefix = QStringLiteral("UNBLOCK-TEST1234-");

    std::atomic<int> progressCalls{0};
    const argonpow::SolveResult sequential =
        argonpow::solve(small, prefix, 1, [&progressCalls](const argonpow::SolveProgress &) {
            progressCalls.fetch_add(1);
        });
    check(sequential.status == argonpow::SolveStatus::Found, "sequential search finds a solution");
    check(progressCalls.load() > 0, "progress callback fires");
    check(argonpow::verifyUnlockCode(small, sequential.code).status == argonpow::VerifyStatus::Valid,
          "sequential solution verifies");

    const argonpow::SolveResult parallel = argonpow::solve(small, prefix, 4);
    check(parallel.status == argonpow::SolveStatus::Found, "parallel search finds a solution");
    check(parallel.nonce == sequential.nonce,
          "parallel ordered search returns the same first nonce");
    check(argonpow::verifyUnlockCode(small, parallel.code).status == argonpow::VerifyStatus::Valid,
          "parallel solution verifies");

    std::atomic<bool> stop{false};
    argonpow::Challenge hard = small;
    hard.difficulty = 1000000000;
    const argonpow::SolveResult cancelled = argonpow::solve(
        hard,
        prefix,
        1,
        [&stop](const argonpow::SolveProgress &p) {
            if (p.attempts >= 2) {
                stop.store(true);
            }
        },
        [&stop]() { return stop.load(); });
    check(cancelled.status == argonpow::SolveStatus::Cancelled, "search can be cancelled");

    if (failures == 0) {
        std::fprintf(stderr, "\nAll PoW tests passed.\n");
        return 0;
    }
    std::fprintf(stderr, "\n%d PoW test(s) failed.\n", failures);
    return 1;
}
