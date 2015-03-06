#ifndef _FFTW_WRAPPER_HPP_
#define _FFTW_WRAPPER_HPP_

#include "..\fftw-3.3.3-dll32\fftw3.h"


class fftw_wrapper
{

private:

public:

	int fft_size;
	unsigned int fft_size_output;
	double sampling_frequency;

	fftw_plan fft_plan;
	double *fft_input;
	fftw_complex *fft_output;

	double *magnitude;
	double *power_db;

	double max_magnitude;
	unsigned int max_magnitude_bin;
	double signal_frequency;
	double samples_per_period;
	double signal_phase;
	double degrees_per_sample;


	int initialize(double sampling_frequency, unsigned int samples);
	int fft(short *real_input);
	int cleanup();
};

#endif // _FFTW_WRAPPER_HPP_
