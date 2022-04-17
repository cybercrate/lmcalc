#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <cmath>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>
#include <cctype>

class BigInteger {
    int radix_;
    bool signed_;
    std::string value_;

public:
    BigInteger();
    BigInteger(int value);
    BigInteger(std::int64_t value);
    BigInteger(std::string value, int radix = 10);
    ~BigInteger() = default;

public:
    static const BigInteger zero;
    static const BigInteger one;
    static const BigInteger ten;
    static const std::string baseChar;

public:
    // Elementary arithmetic.
    [[nodiscard]] BigInteger add(const BigInteger& bi) const;
    [[nodiscard]] BigInteger subtract(const BigInteger& bi) const;
    [[nodiscard]] BigInteger multiply(const BigInteger& bi) const;
    [[nodiscard]] BigInteger divide(const BigInteger& bi) const;

    // Complex arithmetic.
    [[nodiscard]] BigInteger pow(const BigInteger& bi) const;
    [[nodiscard]] BigInteger modulus(const BigInteger& bi) const;

    // Modification & Checking.
    [[nodiscard]] std::size_t bitLength() const;
    [[nodiscard]] int compare(const BigInteger& bi) const;
    [[nodiscard]] BigInteger negate() const;
    [[nodiscard]] BigInteger absolute() const;
    [[nodiscard]] bool isPositive() const;
    [[nodiscard]] bool isNegative() const;
    void swap(BigInteger& bi);

    // Operators.
    BigInteger operator+(const BigInteger& bi);
    BigInteger operator-(const BigInteger& bi);
    BigInteger operator*(const BigInteger& bi);
    BigInteger operator/(const BigInteger& bi);
    BigInteger operator%(const BigInteger& bi);

    BigInteger operator<<(const BigInteger& bi) const;
    BigInteger operator>>(const BigInteger& bi) const;

    BigInteger& operator+=(const BigInteger& bi);
    BigInteger& operator-=(const BigInteger& bi);
    BigInteger& operator*=(const BigInteger& bi);
    BigInteger& operator/=(const BigInteger& bi);

    BigInteger& operator--();
    const BigInteger operator--(int);
    BigInteger& operator++();
    const BigInteger operator++(int);

    bool operator==(const BigInteger& bi) const;
    bool operator!=(const BigInteger& bi) const;
    bool operator<(const BigInteger& bi) const;
    bool operator>(const BigInteger& bi) const;
    bool operator<=(const BigInteger& bi) const;
    bool operator>=(const BigInteger& bi) const;

    friend std::ostream& operator<<(std::ostream& stream, const BigInteger& bi)
    {
        stream << bi.toString();
        return stream;
    }

    [[nodiscard]] std::string toString(int radix = 10) const;
};

#endif // BIG_INTEGER_H
