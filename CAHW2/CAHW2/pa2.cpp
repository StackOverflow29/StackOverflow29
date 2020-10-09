#include "pa2.h"
/* Convert 32-bit signed integer to 12-bit floating point */
fp12 int_fp12(int n)
{
	fp12 flt = 0;
	int x = n;
	unsigned int uns_x;
	int exponent = 31;
	bool is_half;
	int flag = false;
	int count=0;
	bool is_negative = false;
	bool escape = true;
	if ((unsigned int)x >> 31) {//0 if positive, 1 negative.
		is_negative = true;
		uns_x = -x; //unsigned int 형태로 바꾸기
	}

	else {
		uns_x = x;
	}

	for (int i = 31; i>=0; i--) {// i는 큰 자릿수 부터의 bit.
		
		
		if (flag == false) {
			if (uns_x>>31 == 1) {//제일 앞자리가 1이면//
				flag = true;
				exponent += i;
				uns_x = uns_x << 1;
			}
		}
		if (flag == true) {//첫번째 1은 안받음.5개까지만 받음. 암묵)
			for (int count = 0; count <= 4; count++) { //31개 넘어가더라도 일단 5가지 받는다.
				flt = flt << 1;
				flt += uns_x >> 31;
				uns_x = uns_x << 1;
			}
			//count = 0이고 uns_x <<1 하기 전에도 첫번째 1 다음 숫자를 기록중이다.
			//count = 4면 frac는 전부 채워져 있다.
			is_half = uns_x >> 31; // 6번째 (5count) 숫자.
			if (!is_half) { // 6번째 숫자가 0이면 그냥 올림 안한다.
				break;
			}
			else {
				if (flt % 2 == 1) { //만약 frac의 마지막 숫자가 1이라면, even으로 만들어줘야 한다.
					flt += 1; // 즉 1이 올림된다.
					break;
				}
				if ((uns_x << 1) != 0) { //만약 7번째 숫자부터 1이 하나라도 있다면 역시 올림한다.
					flt += 1;
					break;
				}
				else
					break; //다른 모든 경우는 올림하지 않는다.
			}
		}
		uns_x= uns_x << 1; //앞에서부터 읽도록 하겠다. 큰 for문은 첫번째 1을 찾을때까지만 의미가 있다.
	} 
	flt += 32 * (exponent);  //exponent자리에 exponent를 넣는다.(5~10)
	if (exponent != 31 && (flt & ((1 << 11) - (1 << 4))) == 0) { //만약 overflow가 일어나면
		flt = flt | ((1 << 11) - (1 << 4)); // flt의 exp 부분을 111111로 만들어준다.
	}
	if (flt != 0) {//0일때 뭐든 넣어지는 것 방지.
		flt += (2 << 11) * is_negative;//(11~) (11~15)
		flt += (2 << 12) * is_negative;
		flt += (2 << 13) * is_negative;
		flt += (2 << 14) * is_negative;
		flt += (2 << 15) * is_negative;
	}
	/* TODO */
	return flt; 
}

/* Convert 12-bit floating point to 32-bit signed integer */
int fp12_int(fp12 x)
{
	/* TODO */
	int ent = 0;
	bool sign = x >> 15;
	unsigned int exponent = (x & (((1 << 11)-1)))>>5;
	exponent = exponent - 31;
	unsigned int frac = x & ((1 << 5) - 1);
	if (exponent >= 0b100000) { //32승은 안된다 sign bit를 건드리기 때문에... 31승은 괜찮나? 괜찮다!
		return 0x80000000; //가장 작은수.
	}
	if (exponent > 0) { //0이면 불가. 무조건 분수이므로 버림.
		ent += 1; //첫자리 1이라고 가정.
	}
	for (int i = 0; i < exponent; i++) {
		ent = ent << 1;
		if (i <= 4) {//5번
			ent += (frac >>4)%2;
			frac = frac << 1;
		}
	}
	if (sign == 1) {
		ent = -ent;
	}
	return ent;
}

/* Convert 32-bit single-precision floating point to 12-bit floating point */
fp12 float_fp12(float f)
{
	unsigned int flo = f;
	bool sign = (unsigned int)flo >> 31;
	int biased_exp = (flo >> 23) & ((1 << 7) - 1);
		/* TODO */
	int exp = biased_exp - ((1 << 7)-1);
	unsigned int f_frac = (flo & ((1 << 23) - 1));
	fp12 flt = 0;
	bool is_half;
	for (int count = 0; count <= 4; count++){ //31개 넘어가더라도 일단 5가지 받는다.
		flt = flt << 1;
		flt += (f_frac&(1<<22))>>22; //23번째 자리만 뽑아오기.
		f_frac = f_frac << 1;
	}
	//count = 0이고 uns_x <<1 하기 전에도 첫번째 1 다음 숫자를 기록중이다.
	//count = 4면 frac는 전부 채워져 있다.
	is_half = ((f_frac >> 22)%2); // 6번째 (5count) 숫자.
	if (is_half) { // 6번째 숫자가 0이면 그냥 올림 안한다.
		if (flt % 2 == 1) { //만약 frac의 마지막 숫자가 1이라면, even으로 만들어줘야 한다.
			flt += 1; // 즉 1이 올림된다.
		}
		if ((f_frac << 10) != 0) { //만약 7번째 숫자부터 1이 하나라도 있다면 역시 올림한다.
			flt += 1;
		}//<<10을 한 이유: 현재 총 5번 밀렸기 때문에 10번 더 밀면 15번
		//민 것이 되고, 그러면 16번째에 있는 녀석이 1번째로 오기 때문.
	}
	if (exp + 31 > 0b111111) {
		exp = 0b111111;
	}
	else {
		exp = exp + 31; //bias
	}
	flt += (1<<5) * (exp);  //exponent자리에 exponent를 넣는다.(5~10)
	if (exp != 31 && (flt & ((1 << 11) - (1 << 4)))==0) { //만약 overflow가 일어나면
		flt = flt | ((1 << 11) - (1 << 4)); // flt의 exp 부분을 111111로 만들어준다.
	}
	if (flt != 0) {//0일때 negative sign 넣어지는 것 방지.
		flt += (2 << 11) * sign;//(11~) (11~15)
		flt += (2 << 12) * sign;
		flt += (2 << 13) * sign;
		flt += (2 << 14) * sign;
		flt += (2 << 15) * sign;
	}
	return flt;
}

/* Convert 12-bit floating point to 32-bit single-precision floating point */
float fp12_float(fp12 x)
{
	unsigned int flo=0;
	unsigned int frac = x % 32;
	unsigned int float_fraction_pointer = (1 << 22);
	unsigned int float_exponent_pointer = (1 << 30);
	unsigned int fp12_fraction_pointer = (1 << 4);
	unsigned int fp12_exponent_pointer = (1 << 14);
	int x_biased_exp = (x & ((1 << 11) - (1 << 4))) >> 5;
	int x_exp = x_biased_exp-31;
	if (x_biased_exp !=0) {/*denormalized가 아닌경우*/
		flo += (x_biased_exp +((1 << 7) - 1)-31)<<23; //frac 부분. float가 용량이더 크므로 overflow걱정 X
		flo += (frac)<<18;
		flo += (x / (1 << 15)) << 31;
	}
	else {/*denormalized인 특수 case*/ //:x_exp =-31, 하지만 -30으로 해석
		for (int i = 0; i < 5; i++) {
			x_biased_exp--;
			if ((frac>>4)%2 == 1) {
				frac = frac << 1;
				break;
			}
			frac = frac << 1;
		}
		flo += (x_biased_exp + ((1 << 7) - 1)-30) << 23;
		flo += ((frac)%32) << 18;
		flo += (x / (1 << 15)) << 31; //sign bit
	}
	/* TODO */
	return flo;
}
void print_data_binary(int x, int size) {
	unsigned int t = x;
	if (size == 0) {
		return;
	}
	print_data_binary(x >> 1,size-1);
}
