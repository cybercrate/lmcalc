#include "big_integer.h"

const BigInteger BigInteger::zero{"0"};
const BigInteger BigInteger::one{"1"};
const BigInteger BigInteger::ten{"10"};
const std::string BigInteger::baseChar{"0123456789abcdefghijklmnopqrstuv"};

BigInteger::BigInteger() : radix_{10}, signed_{false}, value_{"0"}
{
}

BigInteger::BigInteger(int value) :radix_{10}, signed_{value < 0}
{
    std::string stringValue = std::to_string(value);

    if (signed_)
        stringValue.erase(0, 1);

    value_ = stringValue;
}

BigInteger::BigInteger(std::int64_t value) : radix_{10}, signed_{value < 0}
{
    std::string stringValue = std::to_string(value);

    if (signed_)
        stringValue.erase(0, 1);

    value_ = stringValue;
}

BigInteger::BigInteger(std::string value, int radix)
    : radix_{radix},
      signed_{value.front() == '-'}
{
    if (signed_) value.erase(0, 1);

    while ((value.front() == '0') && (value.length() != 1))
        value.erase(0, 1);

    if (radix == 10) {
        value_ = value;
    }
    else {
        char lastChar = value.back();
        int last = (('0' <= lastChar) && (lastChar <= '9')
            ? (lastChar - '0')
            : (tolower(lastChar) - 'a') + 10);

        int valLength = value.length();

        BigInteger pow{valLength-1};
        BigInteger convertedValue{last};
        BigInteger biRadix{radix};

        for (int i = 0; i < valLength - 1; i++) {
            char c = value.at(i);
            int currentValue = (('0' <= c) && (c <= '9') ? (c - '0') : (tolower(c) - 'a') + 10);
            convertedValue += BigInteger(currentValue).multiply(biRadix.pow(pow--));
        }
        (*this) = (signed_ ? convertedValue.negate() : convertedValue);
    }
}

BigInteger BigInteger::add(const BigInteger& bi) const
{
    BigInteger addition;

    if (signed_ && !bi.signed_) {
        addition = bi.subtract(negate());
    }
    else if (!signed_ && bi.signed_) {
        addition = subtract(bi.negate());
    }
    else {
        std::string sum = value_;
        std::string added = bi.value_;

        int diffLength = std::abs(int(sum.length() - added.length()));

        if (sum.length() > added.length())
            added.insert(0, diffLength, '0');
        else
            sum.insert(0, diffLength, '0');

        std::reverse(sum.begin(), sum.end());
        std::reverse(added.begin(), added.end());

        char carry = '0';
        int index = 0;

        for (char& c : sum) {
            c = (carry - '0') + (c - '0') + (added.at(index) - '0') + '0';

            if (c > '9') {
                c -= 10;
                carry = '1';
            }
            else {
                carry = '0';
            }
            index++;
        }

        if (carry > '0')
            sum.append(1, carry);

        std::reverse(sum.begin(), sum.end());

        addition = signed_
            ? BigInteger{sum}.negate()
            : BigInteger{sum};
    }
    return addition;
}

BigInteger BigInteger::subtract(const BigInteger& bi) const
{
    BigInteger subtraction;

    if (signed_ && !bi.signed_) {
        subtraction = add(bi.negate());
    }
    else if (!signed_ && bi.signed_) {
        subtraction = add(bi.negate());
    }
    else {
        if (signed_) {
            subtraction = add(bi.negate());
        }
        else {
            bool invertSign = (compare(bi) == -1);

            std::string sub = invertSign ? bi.value_ : value_;
            std::string removed = invertSign ? value_ : bi.value_;

            int diffLength = std::abs(int(sub.length() - removed.length()));

            if (sub.size() > removed.size())
                removed.insert(0, diffLength, '0');
            else
                sub.insert(0, diffLength, '0');

            std::reverse(sub.begin(), sub.end());
            std::reverse(removed.begin(), removed.end());

            int index = 0;

            for (char& c : sub) {
                if (c < removed.at(index)) {
                    c += 10;
                    sub[index+1]--;
                }
                c = (c - '0') - (removed.at(index) - '0') + '0';
                index++;
            }
            std::reverse(sub.begin(), sub.end());

            while ((sub.front() == '0') && (sub.length() != 1))
                sub.erase(0, 1);

            subtraction = invertSign
                ? BigInteger{sub}.negate()
                : BigInteger{sub};
        }
    }
    return subtraction;
}

BigInteger BigInteger::multiply(const BigInteger& bi) const
{
    BigInteger multiplication;

    std::string mul = value_;
    std::string multiplied = bi.value_;

    std::reverse(mul.begin(), mul.end());
    std::reverse(multiplied.begin(), multiplied.end());

    int step = 0;
    char carry = '0';

    for (const char& c1 : mul) {
        auto currentOperator = std::string{}.insert(0, step, '0');

        for (const char& c2 : multiplied) {
            unsigned char value = ((c1 - '0') * (c2 - '0')) + (carry - '0') + '0';
            carry = '0';

            if (value > '9') {
                while (value > '9') {
                    value -= 10;
                    carry++;
                }
            }
            currentOperator.insert(0, 1, value);
        }

        if (carry > '0') {
            currentOperator.insert(0, 1, carry);
            carry = '0';
        }
        multiplication += BigInteger(currentOperator);
        step++;
    }

    bool positive = (signed_ && bi.signed_) || (!signed_ && !bi.signed_);

    if (!positive)
        multiplication = multiplication.negate();

    return multiplication;
}

BigInteger BigInteger::divide(const BigInteger& bi) const
{
    BigInteger division;

    if (bi == zero) {
        // Exception.
    }
    else if (bi == one) {
        division = (*this);
    }
    else if (compare(bi) == 0) {
        division = 1;
    }
    else {
        std::string dividend = value_;
        std::string quotient;
        std::string currentQuotient;

        std::reverse(dividend.begin(), dividend.end());
        BigInteger biAbs = bi.absolute();

        do {
            currentQuotient.push_back(dividend.back());
            dividend.pop_back();

            BigInteger biDividend{currentQuotient};

            if (biDividend >= biAbs) {
                auto n = BigInteger{2};

                while (biAbs.multiply(n) <= biDividend)
                    n++;

                n--;
                quotient.append(n.toString());
                currentQuotient = biDividend.subtract(biAbs.multiply(n)).toString();
            }
            else {
                quotient.push_back('0');
            }
        } while (!dividend.empty());

        division = BigInteger(quotient);
    }

    bool positive = (signed_ && bi.signed_) || (!signed_ && !bi.signed_);

    if (!positive)
        division = division.negate();

    return division;
}

BigInteger BigInteger::pow(const BigInteger& bi) const
{
    BigInteger value;

    if (bi == zero) {
        value = one;
    }
    else if (bi == one) {
        value = (*this);
    }
    else {
        BigInteger initialValue = (*this);
        value = (*this);

        for (BigInteger i = one; i < bi; i++)
            value *= initialValue;
    }
    return value;
}

BigInteger BigInteger::modulus(const BigInteger& bi) const
{
    BigInteger mod = subtract(bi.multiply(divide(bi)));
    return mod;
}

std::size_t BigInteger::bitLength() const
{
    return toString(2).length();
}

int BigInteger::compare(const BigInteger& bi) const
{
    int comparison;

    if (signed_ && !bi.signed_) {
        comparison = -1;
    }
    else if (!signed_ && bi.signed_) {
        comparison = 1;
    }
    else {
        if (value_.length() < bi.value_.length()) {
            comparison = -1;
        }
        else if (value_.length() > bi.value_.length()) {
            comparison = 1;
        }
        else {
            bool positive = !signed_;

            if (value_ < bi.value_)
                comparison = (positive ? -1 : 1);
            else if (value_ > bi.value_)
                comparison = (positive ? 1 : -1);
            else
                comparison = 0;
        }
    }
    return comparison;
}

BigInteger BigInteger::negate() const
{
    std::string value = value_;
    return {(signed_ ? value : value.insert(0, 1, '-'))};
}

BigInteger BigInteger::absolute() const
{
    return (isPositive() ? (*this) : negate());
}

bool BigInteger::isPositive() const
{
    return !signed_;
}

bool BigInteger::isNegative() const
{
    return signed_;
}

void BigInteger::swap(BigInteger &bi)
{
    BigInteger tmp = (*this);
    (*this) = bi;
    bi = tmp;
}

BigInteger BigInteger::operator+(const BigInteger& bi)
{
    return add(bi);
}

BigInteger BigInteger::operator-(const BigInteger& bi)
{
    return subtract(bi);
}

BigInteger BigInteger::operator*(const BigInteger& bi)
{
    return multiply(bi);
}

BigInteger BigInteger::operator/(const BigInteger& bi)
{
    return divide(bi);
}

BigInteger BigInteger::operator%(const BigInteger& bi)
{
    return modulus(bi);
}

BigInteger BigInteger::operator<<(const BigInteger& bi) const
{
    std::string bitwiseValue = toString(2);

    for (BigInteger i = zero; i < bi; i++)
        bitwiseValue.push_back('0');

    return {bitwiseValue, 2};
}

BigInteger BigInteger::operator>>(const BigInteger& bi) const
{
    std::string bitwiseValue = toString(2);

    for (BigInteger i = zero; (i < bi) && (bitwiseValue.length() > 0); i++)
        bitwiseValue.pop_back();

    if (bitwiseValue.empty())
        bitwiseValue.push_back('0');

    return {bitwiseValue, 2};
}

BigInteger& BigInteger::operator+=(const BigInteger& bi)
{
    (*this) = add(bi);
    return (*this);
}

BigInteger& BigInteger::operator-=(const BigInteger& bi)
{
    (*this) = subtract(bi);
    return (*this);
}

BigInteger& BigInteger::operator*=(const BigInteger& bi)
{
    (*this) = multiply(bi);
    return (*this);
}

BigInteger& BigInteger::operator/=(const BigInteger& bi)
{
    (*this) = divide(bi);
    return (*this);
}

BigInteger& BigInteger::operator--()
{
    (*this) = subtract(one);
    return (*this);
}

const BigInteger BigInteger::operator--(int)
{
    BigInteger beforeMinus = (*this);
    (*this) = subtract(one);
    return beforeMinus;
}

BigInteger& BigInteger::operator++()
{
    (*this) = add(one);
    return (*this);
}

const BigInteger BigInteger::operator++(int)
{
    BigInteger beforePlus = (*this);
    (*this) = add(one);
    return beforePlus;
}

bool BigInteger::operator==(const BigInteger& bi) const
{
    return (compare(bi) == 0);
}

bool BigInteger::operator!=(const BigInteger& bi) const
{
    return (compare(bi) != 0);
}

bool BigInteger::operator<(const BigInteger& bi) const
{
    return (compare(bi) == -1);
}

bool BigInteger::operator>(const BigInteger& bi) const
{
    return (compare(bi) == 1);
}

bool BigInteger::operator<=(const BigInteger& bi) const
{
    auto cmp = compare(bi);
    return (cmp == -1) || (cmp == 0);
}

bool BigInteger::operator>=(const BigInteger& bi) const
{
    auto cmp = compare(bi);
    return (cmp == 0) || (cmp == 1);
}

std::string BigInteger::toString(int radix) const
{
    std::stringstream ss;
    if (signed_) ss << '-';

    if (radix == 10) {
        ss << value_;
    }
    else {
        BigInteger decimalValue = (*this), modulo(radix);
        std::string value;

        while (decimalValue != zero) {
            BigInteger remain = decimalValue.modulus(modulo);
            decimalValue /= modulo;

            auto c = baseChar.at(std::stoi(remain.toString()));
            value.push_back(c);
        }
        std::reverse(value.begin(), value.end());
        ss << value;
    }

    return ss.str();
}
