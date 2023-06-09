/*
 * fftu.hpp
 *
 *  Created on: Apr 10, 2023
 *      Author: dmarce1
 */

#ifndef FFTU_HPP_
#define FFTU_HPP_

#include "types.hpp"
#include <vector>

void fft_split(int R, complex<fft_simd4>* X, int N);
void fft_cooley_tukey(int N1, complex<fft_simd4>* X, int N);
void fft_split_indices(int R, int* I, int N);
void fft_cooley_tukey_indices(int N1, int* I, int N);
void fft_six_step(complex<fft_simd4>* X, int N);
void fft_stockham(int R, complex<fft_simd4>* X, int N);

void fft_scramble(complex<fft_simd4>* X, int N);
void fft(complex<fft_simd4>* X, int N);
void fft(complex<double>* X, int N);
void fft_indices(int*, int);
const std::vector<int>& fft_indices(int N);

inline double rand1() {
	return (rand() + 0.5) / (RAND_MAX + 1.0);
}

#endif /* FFTU_HPP_ */
