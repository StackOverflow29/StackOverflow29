#ifndef _PA2_H_
#define _PA2_H_

typedef unsigned short int fp12;

fp12 int_fp12(int n);		// convert int -> fp12
int fp12_int(fp12 x);		// convert fp12 -> int
fp12 float_fp12(float f);	// convert float -> fp12
float fp12_float(fp12 x);	// convert fp12 -> float

#endif /* _PA2_H_ */