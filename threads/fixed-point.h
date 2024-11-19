#define F (1 << 14)

typedef int fixed_point;

inline fixed_point fp_itof(int i);
inline fixed_point fp_ftoi(fixed_point f);
inline fixed_point fp_ftoi_round(fixed_point f);

inline fixed_point fp_add_f(fixed_point a, fixed_point b);
inline fixed_point fp_sub_f(fixed_point a, fixed_point b);
inline fixed_point fp_mul_f(fixed_point a, fixed_point b);
inline fixed_point fp_div_f(fixed_point a, fixed_point b);

inline fixed_point fp_add_i(fixed_point a, int b);
inline fixed_point fp_sub_i(fixed_point a, int b);
inline fixed_point fp_mul_i(fixed_point a, int b);
inline fixed_point fp_div_i(fixed_point a, int b);
 
inline fixed_point fp_itof(int i)
{
	return i * F;
}

inline fixed_point fp_ftoi(fixed_point f)
{
	return f / F;
}

inline fixed_point fp_ftoi_round(fixed_point f)
{
	if (f >= 0)
		return (f + F/2)/F;
	else 
		return (f - F/2)/F;
}

inline fixed_point fp_add_f(fixed_point a, fixed_point b)
{
	return a + b;
}

inline fixed_point fp_sub_f(fixed_point a, fixed_point b)
{
	return a - b;
}

inline fixed_point fp_mul_f(fixed_point a, fixed_point b)
{
	return (int64_t) a * b / F;
}

inline fixed_point fp_div_f(fixed_point a, fixed_point b)
{
	return (int64_t) a * F / b;
}

inline fixed_point fp_add_i(fixed_point a, int b)
{
	return a + b * F;
}

inline fixed_point fp_sub_i(fixed_point a, int b)
{
	return a - b * F;
}

inline fixed_point fp_mul_i(fixed_point a, int b)
{
	return a * b;
}

inline fixed_point fp_div_i(fixed_point a, int b)
{
	return a / b;
}
