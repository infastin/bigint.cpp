/*
 * bigint.cpp - Arbitrary-Precision Integer Arithmetic Library
 * Copyright © 2021 infastin
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <algorithm>
#include <iostream>
#include <climits>

#include "bigint.hpp"

#ifdef _WIN32
#define CLZ(x) __lzcnt(x)
#elif __unix__
#define CLZ(x) __builtin_clz(x)
#endif

/* Private {{{ */

void bigint::clamp()
{
	for (auto it = words.end() - 1; it > words.begin(); --it)
	{
		if (*it != 0)
			return;
		else
			words.erase(it);
	}
}

void bigint::from_string(const std::string &s)
{
	int base = 10;
	int sign = 0;
	size_t i = 0;
	int digit = 0;

	if (s[i] == '-')
	{
		sign = 1;
		i++;
	}

	std::string base_str = s.substr(i, 2);

	if (base_str == "0x")
	{
		base = 16;
		i += 2;
	}
	else if (base_str == "0o")
	{
		base = 8;
		i += 2;
	}
	else if (base_str == "0b")
	{
		base = 2;
		i += 2;
	}

	for (; i < s.size(); ++i)
	{
		char c = s[i];
		if (c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'A' && c <= 'Z')
			digit = c - 'A' + 10;
		else if (c >= 'a' && c <= 'z')
			digit = c - 'a' + 10;
		else
			throw bigint_exception("string is not a number!");

		if (digit >= base)
			throw bigint_exception("string is not a number!");

		*this *= base;
		*this += digit;
	}

	this->sign = sign;
}

int bigint::cmp(const bigint &rhs, bool abs = false) const
{
	if (!abs)
	{
		if (sign < rhs.sign)
			return 1;
		if (sign > rhs.sign)
			return -1;
	}

	if (words.size() > rhs.words.size())
		return 1;
	if (words.size() < rhs.words.size())
		return -1;

	if (words.size() == 0)
		return 0;

	for (size_t i = words.size() - 1; i >= 0; --i)
	{
		if (words[i] > rhs.words[i])
			return 1;

		if (words[i] < rhs.words[i])
			return -1;

		if (i == 0)
			break;
	}

	return 0;
}

bigint bigint::add(const bigint &rhs) const
{
	bigint result = 0;

	const bigint &hi = *this;
	const bigint &lo = rhs;

	result.words.resize(hi.words.size(), 0);

	word_t carry = 0;
	size_t i = 0;

	for (; i < lo.words.size(); ++i)
	{
		word_t hi_word = hi.words[i];
		word_t lo_word = lo.words[i];

		word_t new_word = hi_word + lo_word + carry;

		if ((lo_word == WORD_MAX && carry != 0)
			|| (hi_word > WORD_MAX - lo_word - carry))
			carry = 1;
		else
			carry = 0;

		result.words[i] = new_word;
	}

	for (; i < hi.words.size(); ++i)
	{
		word_t hi_word = hi.words[i];
		word_t new_word = hi_word + carry;

		if (hi_word > WORD_MAX - carry)
			carry = 1;
		else
			carry = 0;

		result.words[i] = new_word;
	}

	if (carry != 0)
	{
		result.words.resize(hi.words.size() + 1, 0);
		result.words[i] = carry;
	}

	return result;
}

bigint bigint::sub(const bigint &rhs) const
{
	bigint result = 0;

	const bigint &hi = *this;
	const bigint &lo = rhs;

	result.words.resize(hi.words.size(), 0);

	word_t carry = 0;
	size_t i = 0;

	for (; i < lo.words.size(); ++i)
	{
		word_t hi_word = hi.words[i];
		word_t lo_word = lo.words[i];

		word_t new_word = hi_word - lo_word - carry;

		if ((hi_word == lo_word && carry != 0)
			|| (lo_word > hi_word) 
			|| (lo_word + carry > hi_word))
			carry = 1;
		else
			carry = 0;

		if (new_word != 0)
			result.words[i] = new_word;
	}

	for (; i < hi.words.size(); ++i)
	{
		word_t hi_word = hi.words[i];
		word_t new_word = hi_word - carry;

		if (hi_word < carry)
			carry = 1;
		else
			carry = 0;

		if (new_word != 0)
			result.words[i] = new_word;
	}

	result.clamp();
	return result;
}

bigint bigint::invert(size_t size = 0) const
{
	bigint result = *this;

	if (size != 0 && words.size() != size)
		result.words.resize(size, 0);

	for (size_t i = 0; i < result.words.size(); ++i)
		result.words[i] = ~result.words[i];

	result -= 1;

	return result;
}

/* }}} Private */

/* Constructors {{{ */

bigint::bigint() : sign(0) 
{
	words.push_back(0);
}

bigint::bigint(const char *c)
{
	from_string(c);
}

bigint::bigint(const std::string &s)
{
	from_string(s);
}

bigint::bigint(int l) : sign(l < 0)
{
	bool add_one = false;
	
	if (l == INT_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned int ul = l;
	if (add_one) 
		ul++;

	words.push_back(ul);
}

bigint::bigint(long l) : sign(l < 0)
{
	bool add_one = false;

	if (l == LONG_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned long ul = l;
	if (add_one) 
		ul++;

	do
	{
		words.push_back(ul & WORD_MASK);
		ul >>= WORD_BIT;
	} while (ul > 0);
}

bigint::bigint(long long l) : sign(l < 0)
{
	bool add_one = false;

	if (l == LLONG_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned long long ul = l;
	if (add_one)
		ul++;

	do
	{
		words.push_back(ul & WORD_MASK);
		ul >>= WORD_BIT;
	} while (ul > 0);
}

bigint::bigint(unsigned int l) : sign(0)
{
	words.push_back(l);
}

bigint::bigint(unsigned long l) : sign(0)
{
	do
	{
		words.push_back(l & WORD_MASK);
		l >>= WORD_BIT;
	} while(l > 0);
}

bigint::bigint(unsigned long long l) : sign(0)
{
	do
	{
		words.push_back(l & WORD_MASK);
		l >>= WORD_BIT;
	} while(l > 0);
}

bigint::bigint(const bigint &l) : sign(l.sign), words(l.words) {}

/* }}} Constructors */

/* Assignment Operators {{{ */

bigint& bigint::operator=(const char *c)
{
	from_string(c);
	return *this;
}

bigint& bigint::operator=(const std::string &s)
{
	from_string(s);
	return *this;
}

bigint& bigint::operator=(int l)
{
	sign = l < 0;
	words.clear();

	bool add_one = false;

	if (l == INT_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned int ul = l;
	if (add_one)
		ul++;

	words.push_back(ul);

	return *this;
}

bigint& bigint::operator=(long l)
{
	sign = l < 0;
	words.clear();

	bool add_one = false;

	if (l == LONG_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned long ul = l;
	if (add_one)
		ul++;
	
	do
	{
		words.push_back(ul & WORD_MASK);
		ul >>= WORD_BIT;
	} while (ul > 0);

	return *this;
}

bigint& bigint::operator=(long long l)
{
	sign = l < 0;
	words.clear();

	bool add_one = false;

	if (l == LLONG_MIN)
	{
		l++;
		add_one = true;
	}

	if (l < 0)
		l = -l;

	unsigned long long ul = l;
	if (add_one)
		ul++;

	do
	{
		words.push_back(ul & WORD_MASK);
		ul >>= WORD_BIT;
	} while (ul > 0);

	return *this;
}

bigint& bigint::operator=(unsigned int l)
{
	sign = 0;
	words.clear();
	words.push_back(l);

	return *this;
}

bigint& bigint::operator=(unsigned long l)
{
	sign = 0;
	words.clear();

	do
	{
		words.push_back(l & WORD_MASK);
		l >>= WORD_BIT;
	} while (l > 0);

	return *this;
}

bigint& bigint::operator=(unsigned long long l)
{
	sign = 0;
	words.clear();

	do
	{
		words.push_back(l & WORD_MASK);
		l >>= WORD_BIT;
	} while (l > 0);

	return *this;
}

bigint& bigint::operator=(const bigint &l)
{
	sign = l.sign;
	words = l.words;

	return *this;
}

bigint& bigint::operator+=(const bigint &rhs)
{
	*this = *this + rhs;
	return *this;
}

bigint& bigint::operator-=(const bigint &rhs)
{
	*this = *this - rhs;
	return *this;
}

bigint& bigint::operator*=(const bigint &rhs)
{
	*this = *this * rhs;
	return *this;
}

bigint& bigint::operator%=(const bigint &rhs)
{
	*this = *this % rhs;
	return *this;
}

bigint& bigint::operator/=(const bigint &rhs)
{
	*this = *this / rhs;
	return *this;
}

bigint& bigint::operator>>=(int rhs)
{
	*this = *this >> rhs;
	return *this;
}

bigint& bigint::operator<<=(int rhs)
{
	*this = *this << rhs;
	return *this;
}

bigint& bigint::operator&=(const bigint &rhs)
{
	*this = *this & rhs;
	return *this;
}

bigint& bigint::operator|=(const bigint &rhs)
{
	*this = *this | rhs;
	return *this;
}

bigint& bigint::operator^=(const bigint &rhs)
{
	*this = *this ^ rhs;
	return *this;
}

/* }}} Assignment Operators */

/* Increment/Decrement Operators {{{ */

bigint bigint::operator-() const
{
	bigint result = *this;
	result.sign = !sign;
	return result;
}

bigint& bigint::operator--()
{
	*this = *this - 1;
	return *this;
}

bigint& bigint::operator++()
{
	*this = *this + 1;
	return *this;
}

bigint bigint::operator--(int)
{
	bigint result = *this;
	*this = *this - 1;
	return result;
}

bigint bigint::operator++(int)
{
	bigint result = *this;
	*this = *this + 1;
	return result;
}

/* }}} Increment/Decrement Operators */

/* Arithmetic Operators {{{ */

bigint bigint::operator+(const bigint &rhs) const
{
	if (*this == 0)
		return rhs;

	if (rhs == 0)
		return *this;

	bigint result;
	int cmp_res;

	cmp_res = cmp(rhs, true);

	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	if (sign == rhs.sign)
	{
		result = hi.add(lo);
		result.sign = sign;
	}
	else
	{
		if (cmp_res == 0)
			return 0;

		result = hi.sub(lo);

		if (cmp_res > 0)
			result.sign = sign;
		else
			result.sign = rhs.sign;
	}

	return result;
}

bigint bigint::operator-(const bigint &rhs) const
{
	if (*this == 0)
		return -rhs;

	if (rhs == 0)
		return -*this;

	bigint result;
	int cmp_res;

	cmp_res = cmp(rhs, true);

	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	if (sign != rhs.sign)
	{
		result = hi.add(lo);
		result.sign = sign;
	}
	else
	{
		if (cmp_res == 0)
			return 0;

		result = hi.sub(lo);

		if (cmp_res > 0)
			result.sign = sign;
		else
			result.sign = !rhs.sign;
	}

	return result;
}

bigint bigint::operator*(const bigint &rhs) const 
{
	if (*this == 0 || rhs == 0)
		return 0;

	bigint result = 0;
	int cmp_res;

	cmp_res = cmp(rhs, true);
	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	result.words.resize(hi.words.size() + lo.words.size(), 0);

	for (size_t i = 0; i < hi.words.size(); ++i)
	{
		lword_t carry = 0;

		for (size_t j = 0; j < lo.words.size(); ++j)
		{
			lword_t res_word = result.words[i + j];
			lword_t hi_word = hi.words[i];
			lword_t lo_word = lo.words[j];

			lword_t new_word = res_word + (hi_word * lo_word) + carry;

			carry = new_word >> WORD_BIT;
			result.words[i + j] = new_word;
		}

		result.words[i + lo.words.size()] = carry;
	}

	result.clamp();
	result.sign = (sign + rhs.sign) % 2;

	return result;
}

bigint bigint::operator/(const bigint &rhs) const
{
	auto divrem = div(rhs);
	return divrem.first;
}

bigint bigint::operator%(const bigint &rhs) const
{
	auto divrem = div(rhs);
	return divrem.second;
}

bigint bigint::operator>>(int rhs) const
{
	if (*this == 0)
		return 0;

	if (rhs == 0)
		return *this;

	if (rhs < 0)
		return *this << -rhs;

	int wrshift = rhs / WORD_BIT;
	int rshift = rhs % WORD_BIT;

	if ((size_t) wrshift >= words.size())
		return 0;

	bigint result = *this;

	if (wrshift != 0)
		result.words.erase(result.words.begin(), result.words.begin() + wrshift);

	if (rshift == 0)
		return result;

	int shift = WORD_BIT - rshift;

	lword_t mask = (1ULL << rshift) - 1;
	lword_t r = 0;

	for (size_t i = result.words.size() - 1; i >= 0; --i) 
	{
		lword_t res_word = result.words[i];
		lword_t rr = res_word & mask;
		lword_t new_word = (res_word >> rshift) | (r << shift);

		result.words[i] = new_word;
		r = rr;

		if (i == 0)
			break;
	}

	result.clamp();
	return result;
}

bigint bigint::operator<<(int rhs) const
{
	if (*this == 0)
		return 0;

	if (rhs == 0)
		return *this;

	if (rhs < 0)
		return *this >> -rhs;

	int wlshift = rhs / WORD_BIT;
	int lshift = rhs % WORD_BIT;

	bigint result = *this;

	if (wlshift != 0)
	{
		result.words.resize(result.words.size() + wlshift, 0);
		std::move_backward(result.words.begin(), result.words.end(), result.words.end() + wlshift);
		std::fill(result.words.begin(), result.words.begin() + wlshift, 0);
	}

	if (lshift == 0)
		return result;

	int shift = WORD_BIT - lshift;

	lword_t mask = (1ULL << lshift) - 1;
	lword_t r = 0;

	for (size_t i = 0; i < result.words.size(); ++i)
	{
		lword_t res_word = result.words[i];
		lword_t rr = (res_word >> shift) & mask;
		lword_t new_word = (res_word << lshift) | r;

		result.words[i] = new_word;
		r = rr;
	}

	if (r != 0)
		result.words.push_back(r);

	return result;
}

bigint bigint::operator~() const
{
	bigint result = *this;
	result += 1;
	result.sign = !sign;

	return result;
}

bigint bigint::operator&(const bigint &rhs) const
{
	int cmp_res = cmp(rhs, true);

	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	bigint result = lo.sign ? lo.invert(hi.words.size()) : lo;
	bigint nrhs = hi.sign ? hi.invert() : hi;

	for (size_t i = 0; i < result.words.size(); ++i)
		result.words[i] &= nrhs.words[i];

	if (hi.sign && lo.sign)
	{
		result.sign = 1;
		result = result.invert();
	}
	else if (hi.sign || lo.sign)
		result.sign = 0;

	result.clamp();
	return result;
}

bigint bigint::operator|(const bigint &rhs) const
{
	int cmp_res = cmp(rhs, true);

	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	bigint result = hi.sign ? hi.invert() : hi;
	bigint nrhs = lo.sign ? lo.invert(hi.words.size()) : lo;

	for (size_t i = 0; i < result.words.size(); ++i)
		result.words[i] |= nrhs.words[i];

	if (hi.sign || lo.sign)
	{
		result.sign = 1;
		result = result.invert();
	}

	result.clamp();
	return result;
}

bigint bigint::operator^(const bigint &rhs) const
{
	int cmp_res = cmp(rhs, true);

	const bigint &lo = (cmp_res > 0) ? rhs : *this;
	const bigint &hi = (cmp_res > 0) ? *this : rhs;

	bigint result = hi.sign ? hi.invert() : hi;
	bigint nrhs = lo.sign ? lo.invert(hi.words.size()) : lo;

	for (size_t i = 0; i < result.words.size(); ++i)
		result.words[i] ^= nrhs.words[i];

	if (hi.sign && lo.sign)
		result.sign = 0;
	else if (hi.sign || lo.sign)
	{
		result.sign = 1;
		result = result.invert();
	}

	result.clamp();
	return result;
}

/* }}} Arithmetic Operators */

/* Logical Operators {{{ */

bool bigint::operator!() const {
	return *this == 0;
}

bool bigint::operator&&(const bigint &rhs) const
{
	return (*this != 0 && rhs != 0);
}

bool bigint::operator||(const bigint &rhs) const
{
	return (*this != 0 || rhs != 0);
}

/* }}} Logical Operators */

/* Relational Operators {{{ */

bool bigint::operator==(const bigint &rhs) const
{
	return (cmp(rhs) == 0) ? true : false;
}

bool bigint::operator!=(const bigint &rhs) const
{
	return (cmp(rhs) != 0) ? true : false;
}

bool bigint::operator<(const bigint &rhs) const
{
	return (cmp(rhs) < 0) ? true : false;
}

bool bigint::operator<=(const bigint &rhs) const
{
	return (cmp(rhs) <= 0) ? true : false;
}

bool bigint::operator>(const bigint &rhs) const
{
	return (cmp(rhs) > 0) ? true : false;
}

bool bigint::operator>=(const bigint &rhs) const
{
	return (cmp(rhs) >= 0) ? true : false;
}

/* }}} Relational Operators */

/* Stream operators {{{ */

std::ostream& operator<<(std::ostream &s, const bigint &bi)
{
	s << bi.to_string();
	return s;
}

std::istream& operator>>(std::istream &s, bigint &bi)
{
	std::string str;
	s >> str;
	bi.from_string(str);
	return s;
}

/* }}} Stream operators */

/* Conversion {{{ */

std::string bigint::to_string(int base, const std::string &prefix) const
{
	if (base < 2 || base > 16)
		throw bigint_exception("base of integer can only be in the range [2, 16]");

	if (*this == 0)
		return std::string("0");

	bigint tmp = *this;
	std::string result;

	while (tmp != 0)
	{
		auto qr = tmp.div(base);
		bigint quot = qr.first;
		bigint rem = qr.second;
		char c;

		if (rem.words[0] < 10)
			c = rem.words[0] + '0';
		else
			c = rem.words[0] + 'A' - 10;

		result.push_back(c);
		tmp = quot;
	}

	for (auto iter = prefix.end() - 1; iter >= prefix.begin(); --iter) 
		result.push_back(*iter);

	if (sign == 1)
		result.push_back('-');

	std::reverse(result.begin(), result.end());

	return result;
}

int bigint::to_int() const
{
	if (*this > INT_MAX || *this < INT_MIN)
		throw bigint_exception("out of bounds");

	int result = words[0];
	
	return sign ? -result : result;
}

long bigint::to_long() const
{
	if (*this > LONG_MAX || *this < LONG_MIN)
		throw bigint_exception("out of bounds");

	long result = 0;

	for (size_t i = words.size() - 1; i >= 0; --i)
	{
		result <<= WORD_BIT;
		result |= words[i];

		if (i == 0)
			break;
	}

	return sign ? -result : result;
}

long long bigint::to_llong() const
{
	if (*this > LLONG_MAX || *this < LLONG_MIN)
		throw bigint_exception("out of bounds");

	long long result = 0;

	for (size_t i = words.size() - 1; i >= 0; --i)
	{
		result <<= WORD_BIT;
		result |= words[i];

		if (i == 0)
			break;
	}

	return sign ? -result : result;
}

unsigned int bigint::to_uint() const
{
	if (*this > UINT_MAX || *this < 0)
		throw bigint_exception("out of bounds");

	return words[0];
}

unsigned long bigint::to_ulong() const
{
	if (*this > ULONG_MAX || *this < 0)
		throw bigint_exception("out of bounds");

	unsigned long result = 0;

	for (size_t i = words.size() - 1; i >= 0; --i)
	{
		result <<= WORD_BIT;
		result |= words[i];

		if (i == 0)
			break;
	}

	return result;
}

unsigned long long bigint::to_ullong() const
{
	if (*this > ULLONG_MAX || *this < 0)
		throw bigint_exception("out of bounds");

	unsigned long long result = 0;

	for (size_t i = words.size() - 1; i >= 0; --i)
	{
		result <<= WORD_BIT;
		result |= words[i];

		if (i == 0)
			break;
	}

	return result;
}

/* }}} Conversion */

/* Other Stuff {{{ */

bigint bigint::abs() const
{
	bigint result = *this;
	result.sign = 0;
	return result;
}

bigint bigint::sqrt() const
{
	if (*this == 0 || *this == 1)
		return *this;

	if (sign)
		throw bigint_exception("sqrt called for non-positive integer");

	bigint lo = 1;
	bigint hi = *this / 2 + 1;
	bigint mid, mid2;

	while (lo < hi - 1)
	{
		mid = (lo + hi) / 2;
		mid2 = mid * mid;

		if (mid2 == *this)
		{
			lo = mid;
			break;
		}
		
		if (mid2 < *this)
			lo = mid;
		else
			hi = mid;
	}

	return lo;
}

size_t bigint::size() const
{
	return words.size() * sizeof(word_t);
}

std::pair<bigint, bigint> bigint::div(const bigint &rhs) const
{
	if (rhs == 0)
		throw bigint_exception("division by zero");

	if (*this == 0)
		return std::pair(0, 0);

	if (rhs == 1)
		return std::pair(*this, 0);

	if (cmp(rhs, true) < 0)
		return std::pair(0, *this);

	lword_t b = WORD_BASE; // Number base
	lword_t mask = WORD_MASK; // Number mask b - 1

	std::vector<word_t> tn, rn; // Normalized form of dividend(*this) and divisor(&rhs)
	lword_t qhat; // Estimated quotient digit
	lword_t rhat; // A remainder
	lword_t p; // Product of two digits

	slword_t t, k;

	size_t m = words.size();
	size_t n = rhs.words.size();

	bigint quot = 0;
	bigint rem = 0;

	quot.sign = (sign + rhs.sign) % 2;
	rem.sign = sign;

	quot.words.resize(m, 0);
	rem.words.resize(m, 0);

	if (n == 1)
	{
		k = 0;

		for (size_t j = m - 1; j >= 0; --j)
		{
			quot.words[j] = (k * b + words[j]) / rhs.words[0];
			k = (k * b + words[j]) - (quot.words[j] * rhs.words[0]);

			if (j == 0)
				break;
		}

		rem.words[0] = k;

		quot.clamp();
		rem.clamp();

		return std::pair(quot, rem);
	}

	// Normalize by shifting v left just enough so that
	// its high-order bit is on, and shift u left the
	// same amount.  We may have to append a high-order
	// digit on the dividend; we do that unconditionally.

	int s = CLZ(rhs.words[n - 1]) + 1;
	rn.resize(n, 0);

	for (size_t i = n - 1; i > 0; --i)
		rn[i] = ((rhs.words[i] << s) & mask) | ((static_cast<lword_t>(rhs.words[i - 1]) >> (WORD_BIT - s)) & mask);

	rn[0] = (rhs.words[0] << s) & mask;

	tn.resize(m + 1, 0);
	tn[m] = (words[m - 1] >> (WORD_BIT - s)) & mask;

	for (size_t i = m - 1; i > 0; --i)
		tn[i] = ((words[i] << s) & mask) | ((static_cast<lword_t>(words[i - 1]) >> (WORD_BIT - s)) & mask);

	tn[0] = (words[0] << s) & mask;

	for (size_t j = m - n; j >= 0; --j) // Main loop
	{
		// Compute estimate qhat of q[j]
		qhat = ((tn[j + n] * b) + tn[j + n - 1]) / (rn[n - 1]);
		rhat = ((tn[j + n] * b) + tn[j + n - 1]) % rn[n - 1];

		while (1) 
		{
			if (qhat >= b || (static_cast<word_t>(qhat) * static_cast<lword_t>(rn[n - 2])) > (b * rhat + tn[j + n - 2]))
			{
				qhat -= 1;
				rhat += rn[n-1];

				if (rhat < b)
					continue;
			}

			break;
		}

		// Multiply and subtract
		k = 0;
		for (size_t i = 0; i < n; ++i)
		{
			p = static_cast<word_t>(qhat) * static_cast<lword_t>(rn[i]);
			t = tn[i + j] - k - (p & mask);
			tn[i + j] = t & mask;
			k = (p / b) - (t / b);
		}

		t = tn[j + n] - k;
		tn[j + n] = t;

		quot.words[j] = qhat; // Store quotient digit

		if (t < 0) // If we subtracted too
		{
			quot.words[j] -= 1; // ...much, add back
			k = 0;

			for (size_t i = 0; i < n; ++i)
			{
				t = static_cast<lword_t>(tn[i + j]) + tn[i] + k;
				tn[i + j] = t & mask;
				k = t / b;
			}

			tn[j + n] = tn[j + n] + k;
		}

		if (j == 0)
			break;
	}

	// If the caller wants the remainder, unnormalize
	// it and pass it back.

	for (size_t i = 0; i < n; i++)
		rem.words[i] = ((tn[i] >> s) & mask) | ((static_cast<lword_t>(tn[i + 1]) << (WORD_BIT - s)) & mask);

	rem.words[n - 1] = (tn[n - 1] >> s) & mask;

	quot.clamp();
	rem.clamp();

	return std::pair(quot, rem);
}

/* }}} Other Stuff */
