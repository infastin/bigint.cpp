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

#pragma once

#include <vector>
#include <iostream>
#include <cinttypes>

typedef uint32_t word_t;
typedef uint64_t lword_t;
typedef int64_t  slword_t;

#define WORD_BIT 32
#define WORD_MAX UINT32_MAX
#define WORD_MASK WORD_MAX
#define WORD_BASE (1ULL << WORD_BIT)

#ifdef _WIN32
	#ifdef BIGINT_EXPORTS
		#define BIGINT_API __declspec(dllexport)
	#else
		#define BIGINT_API __declspec(dllimport)
	#endif
#else
	#define BIGINT_API
#endif

class bigint_exception: public std::exception
{
public:
	bigint_exception(const std::string& txt) throw() : std::exception(), txt(txt) {};
	~bigint_exception() throw() {};
	const char* what() const throw() { return txt.c_str(); };
private:
	std::string txt;
};

class BIGINT_API bigint
{
	/* Stream Operators */
	BIGINT_API friend std::ostream& operator<<(std::ostream &s, const bigint &bi);
	BIGINT_API friend std::istream& operator>>(std::istream &s, bigint &bi);

public:
	/* Constructors */
	bigint();
	bigint(const char *c);
	bigint(const std::string &s);
	bigint(int l);
	bigint(long l);
	bigint(long long l);
	bigint(unsigned int l);
	bigint(unsigned long l);
	bigint(unsigned long long l);
	bigint(const bigint &l);

	/* Assignment Operators */
	bigint& operator=(const char *c);
	bigint& operator=(const std::string &s);
	bigint& operator=(int l);
	bigint& operator=(long l);
	bigint& operator=(long long l);
	bigint& operator=(unsigned int l);
	bigint& operator=(unsigned long l);
	bigint& operator=(unsigned long long l);
	bigint& operator=(const bigint &l);

	bigint& operator+=(const bigint &rhs);
	bigint& operator-=(const bigint &rhs);
	bigint& operator*=(const bigint &rhs);
	bigint& operator/=(const bigint &rhs); // throw
	bigint& operator%=(const bigint &rhs); // throw
	bigint& operator>>=(int rhs);
	bigint& operator<<=(int rhs);
	bigint& operator&=(const bigint &rhs);
	bigint& operator|=(const bigint &rhs);
	bigint& operator^=(const bigint &rhs);

	/* Increment/Decrement Operators */
	bigint operator-() const;
	bigint& operator--();
	bigint& operator++();
	bigint operator--(int);
	bigint operator++(int);

	/* Arithmetic Operators */
	bigint operator+(const bigint &rhs) const;
	bigint operator-(const bigint &rhs) const;
	bigint operator*(const bigint &rhs) const;
	bigint operator/(const bigint &rhs) const; // throw
	bigint operator%(const bigint &rhs) const; // throw
	bigint operator>>(int rhs) const;
	bigint operator<<(int rhs) const;
	bigint operator~() const;
	bigint operator&(const bigint &rhs) const;
	bigint operator|(const bigint &rhs) const;
	bigint operator^(const bigint &rhs) const;

	/* Logical Operators */
	bool operator!() const;
	bool operator&&(const bigint &rhs) const;
	bool operator||(const bigint &rhs) const;

	/* Relational Operators */
	bool operator==(const bigint &rhs) const;
	bool operator!=(const bigint &rhs) const;
	bool operator<(const bigint &rhs) const;
	bool operator<=(const bigint &rhs) const;
	bool operator>(const bigint &rhs) const;
	bool operator>=(const bigint &rhs) const;

	/* Other Stuff */
	bigint abs() const;
	bigint sqrt() const; // throw
	size_t size() const;

	std::pair<bigint, bigint> div(const bigint &rhs) const;

	/* Conversion */
	std::string to_string(int base = 10, const std::string &prefix = "") const;
	
	int to_int() const;
	long to_long() const;
	long long to_llong() const;
	unsigned int to_uint() const;
	unsigned long to_ulong() const;
	unsigned long long to_ullong() const;

private:
	int sign;
	std::vector<word_t> words;

	void clamp();
	void from_string(const std::string&);
	
	int cmp(const bigint&, bool) const;

	bigint add(const bigint&) const;
	bigint sub(const bigint&) const;
	bigint invert(size_t) const;
};
