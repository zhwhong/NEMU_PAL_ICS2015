#include "FLOAT.h"
#include "inttypes.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	//nemu_assert(0);
	int64_t temp_a = a;
	int64_t temp_b = b;
	return (temp_a * temp_b) >> 16;
}

/*
FLOAT F_div_F(FLOAT a, FLOAT b) {

	nemu_assert(b);
	int s = 0;
	s = (a < 0) ? !s : s;
	long long _a = (a < 0) ? -a : a;
	s = (b < 0) ? !s : s;
	long long _b = (b < 0) ? -b : b;
	long long _p = ((a << 12) / (b >> 2)) << 2;
	long long _q = 0;
	_a <<= 32;
	_b <<= 32;
	int shl = 0, shr = 0;
	while (_b > _a){
		_b >>= 1;
		++shr;
	}
	while ((_b < _a) && ((_b << 1) <= _a)){
		_b <<= 1;
		++shl;
	}

	int len = 0;
	while (_b){
		if (_a >= _b){
			_a = _a - _b;
			_q = (_q << 1) + 1;
			_b >>= 1;
		} else{
			_q = _q << 1;
			_b >>= 1;
		}
		++len;
	}	
	if (49 >= len)
		shl += 49 - len;
	else
		shr += len - 49;
	while (shr){
		_q >>= 1;
		--shr;
	}
	while (shl){
		_q <<= 1;
		--shl;
	}
	_q >>= 32;

	_q = s ? -_q : _q;

	return _q;
}
*/

FLOAT F_div_F(FLOAT a, FLOAT b) {
	nemu_assert(b);
	int sign = 0;
	sign = (a < 0) ? 1 : 0;
	long long remain = (a < 0) ? -a : a;
	long long divisor = (b < 0) ? -b : b;
	sign = (b < 0) ? !sign : sign;
	
	int count = 16;
	FLOAT result = 0;
	remain <<= 16;
	divisor <<= 16;
	
	while(remain != 0){
		/*
		if(remain >= divisor){
			remain -= divisor;
			result |= (1 << count);
		}
		*/
		while(remain >= divisor){
			remain -= divisor;
			result += (1 << count);
		}
		if(count == 0)
			break;
		divisor >>= 1;
		count--;
	}
	if(sign == 1)
		result = -result;
	return result;
}


FLOAT f2F(float a) {

	int f2int_a = *(int *)(&a); 
	//int float_S = f2int_a & 0x80000000;
	int len_M = 23;
	int float_S = ((unsigned int)f2int_a) >> 31;
	int float_E = (f2int_a >> len_M) & 0xff;
	int float_M = f2int_a & 0x007fffff;
	int temp_off = 0;

	if (float_E == 255)
		return 0x7fffffff;
	else if (float_E == 0){
		if(float_M == 0)
			return 0;
		else{
			while (len_M && !(float_M & 0x1)){
				float_M >>= 1;
				len_M--;
			}

			float_E -= 0x7e;
			temp_off = len_M - float_E - 16;

			if(temp_off >= 0)
				float_M >>= temp_off;
			else
				float_M <<= -temp_off;

			if(float_S == 1)
				float_M = -float_M;

			return float_M;
		}
	}
	else if (float_E >= 1 && float_E <= 254)
	{
		while (len_M && !(float_M & 0x1)){
			float_M >>= 1;
			len_M--;
		}

		float_M |= 1 << len_M;
		float_E -= 0x7f;
		temp_off = 16 - len_M + float_E;

		if (temp_off >= 0)
			float_M <<= temp_off;
		else
			float_M >>= -temp_off;

		if(float_S == 1)
			float_M = -float_M;

		return float_M;
	}

	nemu_assert(0);
	return 0;
}

FLOAT Fabs(FLOAT a) {
	//nemu_assert(0);
	return (a < 0) ? -a : a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

