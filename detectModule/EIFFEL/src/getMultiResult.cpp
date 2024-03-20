#include "HSED/include/common.h"

double getULPE(double x, double origin);
double getULPE2(double x1, double x2, double origin);
double getULPE3(double x1, double x2, double x3, double origin);
double getULPE4(double x1, double x2, double x3, double x4, double origin);
double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);
double getComputeValueE(const double &inputx);
double getComputeValueE2(const double &inputx1, const double &inputx2);
double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);
double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);
double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);

double getULPE(double x, double origin) {
	return 1.0;
}
double getULPE2(double x1, double x2, double origin) {
	return 1.0;
}
double getULPE3(double x1, double x2, double x3, double origin) {
	return 1.0;
}
double getULPE4(double x1, double x2, double x3, double x4, double origin) {
	return 1.0;
}
double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin) {
	mpfr_t mpfr_origin, mpfr_oracle, mp1, mp2, mp3, mp4, mp5, mp6, mp7, mp8, mp9, mp10, mp11;
	mpfr_inits2(128, mpfr_origin, mpfr_oracle, mp1, mp2, mp3, mp4, mp5, mp6, mp7, mp8, mp9, mp10, mp11, (mpfr_ptr) 0);
	mpfr_set_d(mp1, x1, MPFR_RNDN);
	mpfr_set_d(mp2, x2, MPFR_RNDN);
	mpfr_mul(mp3, mp1, mp2, MPFR_RNDN);
	mpfr_set_d(mp4, x3, MPFR_RNDN);
	mpfr_mul(mp5, mp3, mp4, MPFR_RNDN);
	mpfr_set_d(mp6, x4, MPFR_RNDN);
	mpfr_mul(mp7, mp5, mp6, MPFR_RNDN);
	mpfr_set_d(mp8, x5, MPFR_RNDN);
	mpfr_mul(mp9, mp7, mp8, MPFR_RNDN);
	mpfr_set_d(mp10, 6, MPFR_RNDN);
	mpfr_div(mp11, mp9, mp10, MPFR_RNDN);

	mpfr_set(mpfr_oracle, mp11, MPFR_RNDN);
	mpfr_set_d(mpfr_origin, origin, MPFR_RNDN);
	double ulp = computeULPDiff(mpfr_origin, mpfr_oracle);
	mpfr_clears(mpfr_origin, mpfr_oracle, mp1, mp2, mp3, mp4, mp5, mp6, mp7, mp8, mp9, mp10, mp11, (mpfr_ptr) 0);
	mpfr_free_cache();
	return ulp;
}
double getComputeValueE(const double &inputx) {
	return 1.0;
}
double getComputeValueE2(const double &inputx1, const double &inputx2) {
	return 1.0;
}
double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3) {
	return 1.0;
}
double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4) {
	return 1.0;
}
double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5){
	double x1 = inputx1;
	double x2 = inputx2;
	double x3 = inputx3;
	double x4 = inputx4;
	double x5 = inputx5;
	return (x1 * x2 * x3 * x4 * x5) / 6.0;
}
