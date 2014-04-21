#pragma once

#include <windows.h>
#include "pch.h"
#include <include/fftw3.h>
#include <synchapi.h>
#include <audioclient.h>
#include <phoneaudioclient.h>

using namespace Windows::Foundation;

namespace WASAPI_Cpp
{
	public ref class WASAPI sealed
	{
	public:
		WASAPI();
		virtual ~WASAPI();

		// Threading functions
		void startThread();
		void stopThread();
		// This is the function that gets run in a new thread
		void thread(IAsyncAction^ operation);


	private:
		HRESULT Init_Capture();
		HRESULT Init_Render();

		// Device
		IAudioClient2 *inputDevice;
		IAudioClient2 *outputDevice;

		// Client
		IAudioCaptureClient *inputClient;
		IAudioRenderClient *outputClient;


		WAVEFORMATEX *inputFormat;

		HANDLE audioInEvent;
		//HANDLE audioOutEvent;


		// The handle that will refer to the thread we will create
		IAsyncAction^ threadHandle;

	};


	// Note, this is a PURE C++ class!  It cannot be called from C#, all that has to happen through the wrapper class below!
	class FFTW
	{
	private:
		// Internally managed buffers
		float * inputBuffer;
		fftwf_complex * outputBuffer;

		// Length of buffers
		unsigned int N;

		// The fftw plan we generate in the constructor
		fftwf_plan plan;
	public:
		// Creates an FFTW object tuned for a certain length buffer
		FFTW(unsigned int N);

		// Cleanup the internal buffers allocated
		~FFTW();

		// Returns Fourier transform of input.  Because input is real, we
		// exploit the Hermitian Symmetry property of the FT of a real input,
		// omitting the negative frequencies. Practically,this means that if
		// the input is of length N, the output will be of length (N/2) + 1
		fftwf_complex * fft(float * input);
	};


	class CSharpBridge
	{
	public:
		// Constructor
		CSharpBridge();

		// Take the fourier transform of random data of length N, return average magnitude
		// Note: This is a pretty useless operation, this is just to demonstrate how to use FFTW
		float loudestFrequency(unsigned int N);
	};

}