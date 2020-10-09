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
		uns_x = -x; //unsigned int ���·� �ٲٱ�
	}

	else {
		uns_x = x;
	}

	for (int i = 31; i>=0; i--) {// i�� ū �ڸ��� ������ bit.
		
		
		if (flag == false) {
			if (uns_x>>31 == 1) {//���� ���ڸ��� 1�̸�//
				flag = true;
				exponent += i;
				uns_x = uns_x << 1;
			}
		}
		if (flag == true) {//ù��° 1�� �ȹ���.5�������� ����. �Ϲ�)
			for (int count = 0; count <= 4; count++) { //31�� �Ѿ���� �ϴ� 5���� �޴´�.
				flt = flt << 1;
				flt += uns_x >> 31;
				uns_x = uns_x << 1;
			}
			//count = 0�̰� uns_x <<1 �ϱ� ������ ù��° 1 ���� ���ڸ� ������̴�.
			//count = 4�� frac�� ���� ä���� �ִ�.
			is_half = uns_x >> 31; // 6��° (5count) ����.
			if (!is_half) { // 6��° ���ڰ� 0�̸� �׳� �ø� ���Ѵ�.
				break;
			}
			else {
				if (flt % 2 == 1) { //���� frac�� ������ ���ڰ� 1�̶��, even���� �������� �Ѵ�.
					flt += 1; // �� 1�� �ø��ȴ�.
					break;
				}
				if ((uns_x << 1) != 0) { //���� 7��° ���ں��� 1�� �ϳ��� �ִٸ� ���� �ø��Ѵ�.
					flt += 1;
					break;
				}
				else
					break; //�ٸ� ��� ���� �ø����� �ʴ´�.
			}
		}
		uns_x= uns_x << 1; //�տ������� �е��� �ϰڴ�. ū for���� ù��° 1�� ã���������� �ǹ̰� �ִ�.
	} 
	flt += 32 * (exponent);  //exponent�ڸ��� exponent�� �ִ´�.(5~10)
	if (exponent != 31 && (flt & ((1 << 11) - (1 << 4))) == 0) { //���� overflow�� �Ͼ��
		flt = flt | ((1 << 11) - (1 << 4)); // flt�� exp �κ��� 111111�� ������ش�.
	}
	if (flt != 0) {//0�϶� ���� �־����� �� ����.
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
	if (exponent >= 0b100000) { //32���� �ȵȴ� sign bit�� �ǵ帮�� ������... 31���� ������? ������!
		return 0x80000000; //���� ������.
	}
	if (exponent > 0) { //0�̸� �Ұ�. ������ �м��̹Ƿ� ����.
		ent += 1; //ù�ڸ� 1�̶�� ����.
	}
	for (int i = 0; i < exponent; i++) {
		ent = ent << 1;
		if (i <= 4) {//5��
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
	for (int count = 0; count <= 4; count++){ //31�� �Ѿ���� �ϴ� 5���� �޴´�.
		flt = flt << 1;
		flt += (f_frac&(1<<22))>>22; //23��° �ڸ��� �̾ƿ���.
		f_frac = f_frac << 1;
	}
	//count = 0�̰� uns_x <<1 �ϱ� ������ ù��° 1 ���� ���ڸ� ������̴�.
	//count = 4�� frac�� ���� ä���� �ִ�.
	is_half = ((f_frac >> 22)%2); // 6��° (5count) ����.
	if (is_half) { // 6��° ���ڰ� 0�̸� �׳� �ø� ���Ѵ�.
		if (flt % 2 == 1) { //���� frac�� ������ ���ڰ� 1�̶��, even���� �������� �Ѵ�.
			flt += 1; // �� 1�� �ø��ȴ�.
		}
		if ((f_frac << 10) != 0) { //���� 7��° ���ں��� 1�� �ϳ��� �ִٸ� ���� �ø��Ѵ�.
			flt += 1;
		}//<<10�� �� ����: ���� �� 5�� �зȱ� ������ 10�� �� �и� 15��
		//�� ���� �ǰ�, �׷��� 16��°�� �ִ� �༮�� 1��°�� ���� ����.
	}
	if (exp + 31 > 0b111111) {
		exp = 0b111111;
	}
	else {
		exp = exp + 31; //bias
	}
	flt += (1<<5) * (exp);  //exponent�ڸ��� exponent�� �ִ´�.(5~10)
	if (exp != 31 && (flt & ((1 << 11) - (1 << 4)))==0) { //���� overflow�� �Ͼ��
		flt = flt | ((1 << 11) - (1 << 4)); // flt�� exp �κ��� 111111�� ������ش�.
	}
	if (flt != 0) {//0�϶� negative sign �־����� �� ����.
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
	if (x_biased_exp !=0) {/*denormalized�� �ƴѰ��*/
		flo += (x_biased_exp +((1 << 7) - 1)-31)<<23; //frac �κ�. float�� �뷮�̴� ũ�Ƿ� overflow���� X
		flo += (frac)<<18;
		flo += (x / (1 << 15)) << 31;
	}
	else {/*denormalized�� Ư�� case*/ //:x_exp =-31, ������ -30���� �ؼ�
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
