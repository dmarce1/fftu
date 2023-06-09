#include "fft.hpp"
#include "util.hpp"

#include <array>
#include <cmath>
#include <cstring>
#include <numeric>
#include "fftu.hpp"
#include <stack>

int bsqrt(int N) {
	auto l = ilogb(N);
	l /= 2;
	return 1 << l;
}

void fft_split_indices(int R, int* I, int N) {
	if (N <= FFT_NMAX) {
		return;
	}
	const int N1 = R;
	const int N1o2 = N1 / 2;
	const int N1o4 = N1 / 4;
	const int N2 = N / N1;
	const int No2 = N / 2;
	std::vector<int> J;
	J.resize(N);
	for (int n = 0; n < No2; n++) {
		J[n] = I[2 * n];
	}
	for (int n1 = 0; n1 < N1o4; n1++) {
		for (int n2 = 0; n2 < N2; n2++) {
			J[No2 + N2 * n1 + n2] = I[N1 * n2 + 2 * n1 + 1];
		}
	}
	for (int n1 = 0; n1 < N1o4; n1++) {
		for (int n2 = 0; n2 < N2; n2++) {
			J[No2 + N2 * (N1o2 - 1 - n1) + n2] = I[mod(N1 * n2 - 2 * n1 - 1, N)];
		}
	}
	fft_indices(J.data(), No2);
	for (int n1 = 0; n1 < N1o2; n1++) {
		const int o = No2 + N2 * n1;
		fft_indices(J.data() + o, N2);
	}
	std::memcpy(I, J.data(), sizeof(int) * N);
}


template<int N1>
void fft_split(complex<fft_simd4>* X, int N) {
	constexpr int N1o2 = N1 / 2;
	constexpr int N1o4 = N1 / 4;
	std::array<complex<fft_simd4>, N1> ze;
	std::array<complex<fft_simd4>, N1> zo;
	const int N2 = N / N1;
	const auto& W = twiddles(N);
	const int No2 = N / 2;
	fft(X, No2);
	for (int n1 = 0; n1 < N1o2; n1++) {
		int o = No2 + N2 * n1;
		fft(X + o, N2);
	}
	for (int k2 = 0; k2 < N2; k2++) {
		for (int n1 = 0; n1 < N1o2; n1++) {
			ze[n1] = X[n1 * N2 + k2];
		}
		const int twok2 = 2 * k2;
		int wi = k2;
		int m1 = N1o2 - 1;
		int p = No2 + k2;
		int q = No2 + N2 * m1 + k2;
		for (int n1 = 0; n1 < N1o4; n1++) {
			const auto& w = W[wi];
			zo[n1] = X[p] * w;
			zo[m1] = X[q] * w.conj();
			wi += twok2;
			m1--;
			p += N2;
			q -= N2;
		}
		if (N1 == 4) {
			fft_complex_odd_exe_4((fft_simd4*) zo.data());
		} else if (N1 == 8) {
			fft_complex_odd_exe_8((fft_simd4*) zo.data());
		} else if (N1 == 16) {
			fft_complex_odd_exe_16((fft_simd4*) zo.data());
		} else if (N1 == 32) {
			fft_complex_odd_exe_32((fft_simd4*) zo.data());
		}
		p = k2;
		for (int k1 = 0; k1 < N1o2; k1++) {
			const auto& e = ze[k1];
			const auto& o = zo[k1];
			X[p] = e + o;
			X[p + No2] = e - o;
			p += N2;
		}
	}
}

void fft_split(int R, complex<fft_simd4>* X, int N) {
	switch (R) {
	case 4:
		return fft_split<4>(X, N);
	case 8:
		return fft_split<8>(X, N);
	case 16:
		return fft_split<16>(X, N);
	case 32:
		return fft_split<32>(X, N);
	case 64:
		return fft_split<64>(X, N);
	}
}
