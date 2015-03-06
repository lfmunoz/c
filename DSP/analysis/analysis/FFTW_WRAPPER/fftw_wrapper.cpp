#include <malloc.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "fftw_wrapper.hpp"


int fftw_wrapper::initialize(double sampling_frequency, unsigned int samples)
{
	this->fft_size = samples;
	this->fft_size_output = fft_size /2 + 1;
	this->sampling_frequency = sampling_frequency;

	this->magnitude = (double *) _aligned_malloc(sizeof(double)*fft_size_output, 4096);
	this->power_db = (double *) _aligned_malloc(sizeof(double)*fft_size_output, 4096);

	this->fft_input = (double *)_aligned_malloc(sizeof(double)*samples, 4096);
	this->fft_output = (fftw_complex *)_aligned_malloc(sizeof(fftw_complex)*fft_size_output, 4096);
	this->fft_plan = fftw_plan_dft_r2c_1d(samples, fft_input, fft_output, 0);
	return 0;
}

int fftw_wrapper::fft(short *real_input)
{
	for(unsigned int i=0; i<this->fft_size; i++)
	{
		this->fft_input[i] = real_input[i];
	}

	fftw_execute(this->fft_plan);

	for(unsigned int i=0; i<this->fft_size_output; i++)
	{
		this->magnitude[i] = sqrt((this->fft_output[i][0] * this->fft_output[i][0]) + (this->fft_output[i][1] * this->fft_output[i][1])) / (this->fft_size * 2);
		
		if(i == 1)
		{
			this->max_magnitude = this->magnitude[i];
			this->max_magnitude_bin = 1;
		}
		else if(this->magnitude[i] > this->max_magnitude)
		{
			this->max_magnitude = this->magnitude[i];
			this->max_magnitude_bin = i;
		}
	}

	this->signal_frequency = this->max_magnitude_bin * this->sampling_frequency/fft_size;
	this->samples_per_period = this->sampling_frequency / this->signal_frequency;
	this->degrees_per_sample = 360.0f / this->samples_per_period;
	this->signal_phase = atan2(this->fft_output[this->max_magnitude_bin][1], this->fft_output[this->max_magnitude_bin][0]) * 180 / M_PI;

	return 0;
}

int fftw_wrapper::cleanup()
{

	fftw_destroy_plan(this->fft_plan);
	_aligned_free(this->magnitude);
	_aligned_free(this->power_db);

	_aligned_free(this->fft_input);
	_aligned_free(this->fft_output);

	return 0;
}