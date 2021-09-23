#include "bigint.hpp"

#include <cassert>
#include <climits>
#include <random>

class bigint_test
{
public:
	void limits()
	{
		assert(bigint(INT_MIN) == INT_MIN);
		assert(bigint(INT_MAX) == INT_MAX);
		assert(bigint(INT_MAX).to_int() == INT_MAX);
		assert(bigint(INT_MIN).to_int() == INT_MIN);

		assert(bigint(LONG_MIN) == LONG_MIN);
		assert(bigint(LONG_MAX) == LONG_MAX);
		assert(bigint(LONG_MAX).to_long() == LONG_MAX);
		assert(bigint(LONG_MIN).to_long() == LONG_MIN);

		assert(bigint(LLONG_MIN) == LLONG_MIN);
		assert(bigint(LLONG_MAX) == LLONG_MAX);
		assert(bigint(LLONG_MAX).to_llong() == LLONG_MAX);
		assert(bigint(LLONG_MIN).to_llong() == LLONG_MIN);

		assert(bigint(UINT_MAX) == UINT_MAX);
		assert(bigint(0U) == 0U);
		assert(bigint(UINT_MAX).to_uint() == UINT_MAX);
		assert(bigint(0U).to_uint() == 0U);

		assert(bigint(ULONG_MAX) == ULONG_MAX);
		assert(bigint(0UL) == 0UL);
		assert(bigint(ULONG_MAX).to_ulong() == ULONG_MAX);
		assert(bigint(0UL).to_ulong() == 0UL);

		assert(bigint(ULLONG_MAX) == ULLONG_MAX);
		assert(bigint(0ULL) == 0ULL);
		assert(bigint(ULLONG_MAX).to_ullong() == ULLONG_MAX);
		assert(bigint(0ULL).to_ullong() == 0ULL);
	}

	void sqrt()
	{
		assert(bigint(1).sqrt() == 1);
		assert(bigint(2).sqrt() == 1);
		assert(bigint(3).sqrt() == 1);
		assert(bigint(4).sqrt() == 2);
		assert(bigint(5).sqrt() == 2);
		assert(bigint(6).sqrt() == 2);
		assert(bigint(7).sqrt() == 2);
		assert(bigint(8).sqrt() == 2);
		assert(bigint(9).sqrt() == 3);

		std::mt19937 g(time(0));

		for (int i = 0; i < 1000; ++i)
		{
			bigint i1 = g();
			bigint root = i1.sqrt();
			bigint root1 = root + 1;

			assert((root * root) <= i1 && i1 <= (root1 * root1));
			assert((i1 * i1).sqrt() == i1);
		}
	}

	void abs()
	{
		assert(bigint(-10).abs() == 10);
		assert(bigint(-33).abs() == 33);
		assert(bigint(-512).abs() == 512);
		assert(bigint(-22).abs() == 22);
		assert(bigint(-76).abs() == 76);

		std::mt19937 g(time(0));

		for (int i = 0; i < 1000; ++i)
		{
			bigint r1 = g();
			bigint i1 = -r1;

			assert(i1.abs() == (-r1).abs());
		}
	}

	void arithmetic()
	{
		std::mt19937 g(time(0));
		std::random_device rd;
		std::uniform_int_distribution<int> dist(-10, 10);

		bigint a = "0x1FFFFFFFFFFFFFFFF";
		bigint b = "0xF0000000000";

		assert((a + b) == bigint("36893504640093519871"));

		for (int i = 0; i < 1000; ++i)
		{
			bigint i1, i2;

			int64_t r1 = g() & INT32_MAX;
			int64_t r2 = g() & INT32_MAX;

			int32_t r3 = dist(rd);

			{
				i1 = r1++;
				assert(i1-- == (r1 - 1));
				assert(bigint(r1 - 3) == --i1);
				assert(i1 == (r1 - 3));
			}

			{
				i2 = r2--;
				assert(i2++ == (r2 + 1));
				assert(bigint(r2 + 3) == ++i2);
				assert(i2 == (r2 + 3));
			}

			{
				assert(bigint(r1) / bigint(r2) == r1 / r2);
				assert(bigint(r1) / bigint(-r2) == r1 / -r2);
				assert(bigint(-r1) / bigint(r2) == -r1 / r2);
				assert(bigint(-r1) / bigint(-r2) == -r1 / -r2);

				assert(bigint(r1) % bigint(r2) == r1 % r2);
				assert(bigint(r1) % bigint(-r2) == r1 % -r2);
				assert(bigint(-r1) % bigint(r2) == -r1 % r2);
				assert(bigint(-r1) % bigint(-r2) == -r1 % -r2);
			}

			{
				i1 = r1;
				i1 += r2;
				assert(i1 == (r1 + r2));
			}

			{
				i1 = r1;
				i1 -= r2;
				assert(i1 == (r1 - r2));
			}

			{
				i1 = r1;
				i1 *= r2;
				assert(i1 == (r1 * r2));
			}

			{
				i1 = r1;
				i1 /= r2;
				assert(i1 == (r1 / r2));
			}

			{
				i1 = r1;
				i1 %= r2;
				assert(i1 == (r1 % r2));
			}

			{
				i1 = r1;
				i1 <<= r3;

				if (r3 > 0)
					assert(i1 == (r1 << r3));
				else
					assert(i1 == (r1 >> -r3));
			}

			{
				i1 = r1;
				i1 >>= r3;

				if (r3 > 0)
					assert(i1 == (r1 >> r3));
				else
					assert(i1 == (r1 << -r3));
			}

			{
				i1 = r1;
				assert(~i1 == ~r1);
			}

			{
				i1 = r1;
				i1 |= r2;
				assert(i1 == (r1 | r2));
			}

			{
				i1 = r1;
				i1 &= r2;
				assert(i1 == (r1 & r2));
			}

			{
				i1 = r1;
				i1 ^= r2;
				assert(i1 == (r1 ^ r2));
			}
		}
	}

	void logic()
	{
		assert(!bigint(0) == !0);
		assert(!bigint(33) == !33);
		assert(!bigint(235) == !235);
		assert(!bigint(-22) == !-22);
		assert(!bigint(-76) == !-76);

		std::mt19937 mt(time(0));

		for (int i = 0; i < 1000; ++i)
		{
			long r1 = mt();
			long r2 = mt();
			bigint i1 = r1;
			bigint i2 = r2;

			assert(!i1 == !r1);
			assert((i1 && i2) == (r1 && r2));
			assert((i1 || i2) == (r1 || r2));
		}
	}

	void byte_array()
	{
		std::mt19937 mt(time(0));

		for (int i = 0; i < 1000; ++i)
		{
			long r1 = mt();

			if (r1 % 2 == 0)
				r1 = -r1;

			bigint i1 = r1;

			std::vector<char> v1 = i1.to_byte_array();
			bigint i2 = v1;

			assert(i1 == i2);
			assert(i2 == r1);
		}
	}
};

int main()
{
	bigint_test test;

	test.limits();
	test.sqrt();
	test.abs();
	test.arithmetic();
	test.logic();
	test.byte_array();

	return 0;
}
