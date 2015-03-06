
#include <stdio.h>
#include <malloc.h>

#include <math.h> 
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include "fftw_wrapper.hpp"
#define NUM_WAVEFORMS 1
#define ADC_SAMPLE_CLOCK (1000000000.0f)
#define FFT_SIZE (32*1024)
#define PI 3.1415926535

#include <iostream>
#include <vector>


#define SINE_WAVE	0									/*!< GenerateWaveform() generates sine wave */
#define SAW_WAVE	1									/*!< GenerateWaveform() generates saw wave */
#define DC_WAVE		2									/*!< GenerateWaveform() generates dc wave */

#define ASCII			0								/*!< Save16BitArrayToFile() saves the samples as ASCII */
#define BINARY			1								/*!< Save16BitArrayToFile() saves the samples as binary */


/**
 *  Generate a 16 bit waveform into a previously allocated memory buffer. This function can generate several data types and both
 *  signal period as well as amplitude are configurable.
 *
 *  @param buffer	pointer to a buffer about to receive the waveform data. This point to a previously allocated memory as big as
 *					numbersamples*2 ( byte size ) or numbersamples*1 ( sample size ).
 *  @param numbersamples	number of samples to be written on the buffer where one sample is as big as 2 bytes.
 *  @param period	period of the signal to generate.
 *  @param amplitude	amplitude of the signal to generate.
 *  @param datatype	decide what kind of data the function generates :
 *						- SINE_WAVE
 *						- SAW_WAVE
 *						- DC_WAVE
 *  @return 
 *						- -1 ( Unexpected NULL argument )
 *						- 0 ( Success )
 */
int GenerateWaveform16(unsigned short *buffer, unsigned int numbersamples, unsigned int period, unsigned int amplitude, unsigned char datatype)
{
	int ampl				= 0;
	double pi				= 3.1415926535;
	int tmp2				= 0x0;
	double x, y;

	// set our buffer with known value ( 0 ). Note the MUL(2) because memset takes a byte size
	memset(buffer,0, numbersamples*2);

	// make sure we are not going to hit the wall
	if(!buffer) {
		printf("GenerateWaveform() cannot receive a NULL first argument...\n");
		return -1;
	}

	// proceed with the data generation
	switch(datatype)
	{
	case SINE_WAVE:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			ampl=amplitude/2-1;
			x = (pi /period*2)*(i*2+0);
			y = sin( x );
			buffer[2*i+0] =  ((unsigned short)(y*ampl));
			x = (pi /period*2)*(i*2+1);
			y = sin( x );
			buffer[2*i+1] =  ((unsigned short)(y*ampl));
		}
		break;
	case SAW_WAVE:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			tmp2 =(0xffff)& ((2*i)%period*(amplitude-1)/period*2);
			buffer[2*i+0] = (unsigned short)(tmp2);
			tmp2 = (0xffff)& ((2*i)%period*(amplitude-1)/period*2);
			buffer[2*i+1] = (unsigned short)(tmp2);
		}
		break;
	case DC_WAVE:
	default:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			tmp2 =(0xffff) & (0x8000);
			buffer[2*i+0] = (unsigned short)(tmp2);
			buffer[2*i+1] = (unsigned short)(tmp2);
		}
	}
	return 0;
}


void generate_sine(uint32_t num_of_samples, uint32_t sampling_rate, uint32_t frequency)
{
	double param, result;
	param = 30.0;
	result = sin (param*PI/180);
	printf ("The sine of %f degrees is %f.\n", param, result );

}

template<typename T>
std::vector<double> linspace(T start_in, T end_in, int num_in)
{
  double start = static_cast<double>(start_in);
  double end = static_cast<double>(end_in);
  double num = static_cast<double>(num_in);
  double delta = (end - start) / (num - 1);

  std::vector<double> linspaced(num - 1);


  for(int i=0; i < num-1; ++i) {
      linspaced[i] = start + delta * i;
  }

  linspaced.push_back(end);
  return linspaced;
}

void print_vector(std::vector<double> vec)
{
  for (double d : vec)
    std::cout << d << " ";
  std::cout << std::endl;
}

void print_vector(std::vector<unsigned short> vec)
{
  for (short d : vec)
    std::cout << d << " ";
  std::cout << std::endl;
}

std::vector<unsigned short> sine(std::vector<double> t, double f, double phi)
{
	double ampl = pow(2.0,14.0);
	double y = 0.0;
	std::vector<unsigned short> wave(t.size());
	for(uint32_t x = 0; x < t.size(); x++) {
		y =  sin(2*PI*f*t[x] + phi);

		wave[x] = ((unsigned short)(y*ampl));
		
	}
	return wave;
}

template<typename T>
std::vector<T> arange(T start, T stop, T step = 1) {
    std::vector<T> values;
    for (T value = start; value < stop; value += step)
        values.push_back(value);
    return values;
}


int main()
{
	double fs = 100;
	double f = 5;

	printf("Starting application \n");
    std::vector<double> t0 = arange<double>(-1, 1, 1.0/fs);
	std::cout << t0.size() << std::endl;

	std::vector<unsigned short> x0 = sine(t0, f, PI/3.0);
	//print_vector(t0);
	//print_vector(x0);

	unsigned short* a = &x0[0];

/*
  std::vector<double> vec_2 = linspace(6.0, 23.4, 5);
  print_vector(vec_2);
  std::cout << vec_2.size() << std::endl;
 */

	
#if 1	
	//uint16_t *wave0 = (uint16_t*) malloc(FFT_SIZE*sizeof(uint16_t));
	//GenerateWaveform16(wave0, FFT_SIZE, FFT_SIZE/16, pow(2.0,14.0), SINE_WAVE);
	uint16_t *wave0 = (uint16_t*) a;

	fftw_wrapper fftw[NUM_WAVEFORMS];
	double fft_input[NUM_WAVEFORMS][FFT_SIZE];

	fftw[0].initialize(fs, t0.size());

	fftw[0].fft((short *)wave0);

	double advanced_phase = fftw[0].signal_phase;
	double signal_frequency = fftw[0].signal_frequency;

	// off by 90?
	printf("Signal phase: %f \n", advanced_phase );
	printf("Signal frequency: %f\n", signal_frequency);
#endif

	getchar();
	return 0;
}