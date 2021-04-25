#include <iostream>
#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "bigmp.h"

using namespace bigmp;

TEST(BigMp, String256) {
    BigInt a(std::string("256"));

    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a[0]);
    ASSERT_EQ(0, a[1]);

}

TEST(BigMp, CopyCtor) {

    BigInt a(4);
    BigInt b(a);

    ASSERT_EQ(BigInt(4), b);
}

TEST(BigMp, MoveCtor) {
    BigInt a(100);
    BigInt b(std::move(a));

    ASSERT_EQ(100, b);
}

TEST(BigMp, EqualsValue) {
    BigInt r(222);

    ASSERT_EQ(222, r);
}

TEST(BigMp, EqualsBig) {
    BigInt r(222);
    BigInt s(222);

    ASSERT_EQ(s, r);
}

TEST(BigMp, AddTwo) {
    BigInt a(2);
    BigInt b(2);

    auto result = a + b;

    ASSERT_EQ(BigInt(4), result);
}

TEST(BigMp, multi256) {
    BigInt a(128);
    BigInt b(128);

    auto twox = a + b;
    ASSERT_EQ(2, twox.size());
    ASSERT_EQ(1, twox[0]);
    ASSERT_EQ(0, twox[1]);
}

TEST(BigMp, multi65636) {
    BigInt a("6");
    ASSERT_EQ(BigInt("6"), a);
    a *= 10;
    a += 5;
    ASSERT_EQ(BigInt("65"), a);
    a *= 10;
    a += 5;
    ASSERT_EQ(BigInt("655"), a);
    a *= 10;
    a += 3;
    ASSERT_EQ(BigInt("6553"), a);
    a *= 10;
    a += 6;
    ASSERT_EQ(BigInt("65536"), a);
}

TEST(BigMp, string250) {
    BigInt a("250");
    ASSERT_EQ(250, a[0]);

    a += 6;
    ASSERT_EQ(1, a[0]);
    ASSERT_EQ(0, a[1]);
}

TEST(BigMp, string65535) {
    auto a = BigInt("65535");
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(255, a[0]);
    ASSERT_EQ(255, a[1]);
}

TEST(BigMp, string65536) {
    auto a = BigInt("65536");

    ASSERT_EQ(3, a.size());
    ASSERT_EQ(1, a[0]);
    ASSERT_EQ(0, a[1]);
    ASSERT_EQ(0, a[2]);
}

TEST(BigMp, multi256x256) {
    BigInt a("256");
    BigInt b("256");

    auto twox = a * b;

    auto expected = BigInt("65536");
    ASSERT_EQ(expected, twox);
}

TEST(BigMp, multi1) {
    BigInt a(255);
    BigInt b(1);

    auto twox = a + b;
    ASSERT_EQ(2, twox.size());
    ASSERT_EQ(1, twox[0]);
    ASSERT_EQ(0, twox[1]);
}

TEST(BigMp, multi) {
    BigInt a(255);
    BigInt b(255);

    auto twox = a + b;
    ASSERT_EQ(2, twox.size());
    ASSERT_EQ(1, twox[0]);
    ASSERT_EQ(254, twox[1]);
}

TEST(BigMp, Mod) {
    BigInt a(2);

    ASSERT_EQ(2, a % 10);
}

TEST(BigMp, by2) {
    BigInt a(255);

    auto twox = a * 2;
    ASSERT_EQ(2, twox.size());
    ASSERT_EQ(1, twox[0]);
    ASSERT_EQ(254, twox[1]);
}

TEST(BigMp, by255) {
    BigInt a(255);

    auto twox = a * 255;
    ASSERT_EQ(2, twox.size());
    ASSERT_EQ(254, twox[0]);
    ASSERT_EQ(1, twox[1]);
}

TEST(BigMp, LongDecomposition) {
    BigInt l(4294967296L);
    ASSERT_EQ(BigInt("4,294,967,296"), l);
}

TEST(BigMp, left_shift) {
    BigInt a(1);

    a.lsh();

    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a[0]);
    ASSERT_EQ(0, a[1]);
}

TEST(BigMp, s27out) {
    BigInt a(27);

    std::stringstream out;

    out << a;

    ASSERT_EQ("27", out.str());
}

TEST(BigMp, s256out) {
    BigInt a(255);

    auto b = a + 1;

    std::stringstream out;

    out << b;

    ASSERT_EQ("256", out.str());
}

TEST(BigMp, s65536out) {
    BigInt a("65536");

    std::stringstream out;

    out << a;

    ASSERT_EQ("65536", out.str());
}

TEST(BigMp, f502out) {
    BigInt f("590606256885570541884749772942551428042092906415213688441386695785274827100237057501589632981816458291377");

    std::stringstream out;

    out << f;

    ASSERT_EQ("590606256885570541884749772942551428042092906415213688441386695785274827100237057501589632981816458291377", out.str());
}

TEST(BigMp, d65536) {
    BigInt a("65536");

    a /= 10;

    ASSERT_EQ(BigInt("6553"), a);
}

TEST(BigMp, m65536) {
    BigInt a("65536");

    auto m = a % 10;

    ASSERT_EQ(6, m);
}

TEST(BigMp, multi65536_MoreThanWords) {
    BigInt a("65536");
    BigInt b("65536");

    auto shortsq = a * b;
    ASSERT_EQ(BigInt("4,294,967,296"), shortsq);
}

TEST(BigMp, Fib102) {
    BigInt fib100("573147844013817084101");
    BigInt fib101("927372692193078999176");

    BigInt fib102 = fib100 + fib101;
    ASSERT_EQ(BigInt("1500520536206896083277"), fib102);
}

TEST(BigMp, Fib502) {
    BigInt fib500("225591516161936330872512695036072072046011324913758190588638866418474627738686883405015987052796968498626");
    BigInt fib501("365014740723634211012237077906479355996081581501455497852747829366800199361550174096573645929019489792751");

    BigInt fib502 = fib500 + fib501;
    ASSERT_EQ(BigInt("590606256885570541884749772942551428042092906415213688441386695785274827100237057501589632981816458291377"), fib502);
}

TEST(BigMp, BigInt) {
    BigInt i1(4294967296UL);
    BigInt i2(4294967296UL);

    auto twox = i1 * i2;

    ASSERT_EQ(BigInt("18446744073709551616"), twox);
}