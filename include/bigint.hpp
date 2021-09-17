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

class bigint_exception: public std::exception
{
public:
	bigint_exception(const std::string& txt) throw() : std::exception(), txt(txt) {};
	~bigint_exception() throw() {};
	const char* what() const throw() { return txt.c_str(); };
private:
	std::string txt;
};

class bigint
{
	/* Stream Operators */
	friend std::ostream& operator<<(std::ostream &s, const bigint &bi);
	friend std::istream& operator>>(std::istream &s, bigint &bi);

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

	std::pair<bigint, bigint> div(const bigint &rhs) const;

	size_t size() const;

	/* Conversion */
	std::string to_string(int base = 10) const;
	
	int to_int() const;
	long to_long() const;
	long long to_llong() const;
	unsigned int to_uint() const;
	unsigned long to_ulong() const;
	unsigned long long to_ullong() const;

private:
	void from_string(const std::string &s);
	void clamp();
	
	int cmp(const bigint &rhs, bool abs = false) const;

	bigint add(const bigint &rhs) const;
	bigint sub(const bigint &rhs) const;
	bigint invert(size_t size = 0) const;

	int sign;
	std::vector<word_t> words;
};
