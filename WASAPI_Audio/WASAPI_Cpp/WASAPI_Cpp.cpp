// WASAPI_Cpp.cpp
#include "pch.h"
#include "WASAPI_Cpp.h"
#include <audioclient.h>
#include <phoneaudioclient.h>

using namespace WASAPI_Cpp;
using namespace Platform;

using namespace Windows::System::Threading;


// Consturctor
WASAPI::WASAPI()
{
	// Initialize variables to null values
	/*this->threadHandle = nullptr;
	this->inputDevice = nullptr;
	this->outputDevice = nullptr;
	this->inputClient = nullptr;
	this->outputClient = nullptr;
	this->inputFormat = nullptr;
	this->audioInEvent = nullptr;
	*/

	Init_Capture();
	Init_Render();

}

// Destructor
WASAPI::~WASAPI()
{
	this->stopThread();
}


///////////////////////////
//  Threading Functions  //
///////////////////////////

void WASAPI::startThread()
{
	
	// Only start a new thread if we don't already have one running
	if (this->threadHandle == nullptr)
	{
		this->threadHandle = ThreadPool::RunAsync(ref new WorkItemHandler(this, &WASAPI::thread));
		inputDevice->Start();
		outputDevice->Start();
	
	}
}

void WASAPI::stopThread()
{
	// Only stop a thread if we DO have one running
	if (this->threadHandle != nullptr)
	{
		this->threadHandle->Cancel();
		inputDevice->Stop();
		outputDevice->Stop();
	}
		
}


void WASAPI::thread(IAsyncAction^ operation) {
	unsigned char *rawData;
	unsigned int numSamples;
	unsigned long flags;

	unsigned int padding = 0;
	unsigned int allocatedBufferSize;

	unsigned char *rawDataOut;

	// Keep looping as long as the threadHandle doesn't think we've been canceled
	while (this->threadHandle->Status != AsyncStatus::Canceled) {
		// Sleep until audioInEvent is triggered
		if (WaitForSingleObjectEx(audioInEvent, INFINITE, FALSE) == WAIT_OBJECT_0)
		{
			inputClient->GetBuffer(&rawData, &numSamples, &flags, NULL, NULL);

			outputDevice->GetCurrentPadding(&padding);
			outputDevice->GetBufferSize(&allocatedBufferSize);


			if (padding != allocatedBufferSize)
			{
				outputClient->GetBuffer(numSamples, (unsigned char**)&rawDataOut);
				for (unsigned int i = 0; i < numSamples; ++i)
				{
					rawDataOut[i] = rawData[i];

				}
			}

			outputClient->ReleaseBuffer(numSamples, 0);
			inputClient->ReleaseBuffer(numSamples);
		}
	}
	// Once we've been canceled, clean up the threadHandle resources
	this->threadHandle->Close();
	this->threadHandle = nullptr;
}


///////////////////////////
///   WASAPI Functions  ///
///////////////////////////



HRESULT WASAPI::Init_Capture()
{
	HRESULT hr = E_FAIL;

	// returns the interface id for the specified role
	auto deviceID = GetDefaultAudioCaptureId(AudioDeviceRole::Default);
	auto classID = __uuidof(IAudioClient2);

	if (NULL == deviceID)
	{
		hr = E_FAIL;
	}
	else
	{
		// Finds audio endpoint based on deviceID
		hr = ActivateAudioInterface(deviceID, classID, (void **)&inputDevice);
	}

	if (SUCCEEDED(hr))
	{
		// Writes the address of the mix format
		hr = inputDevice->GetMixFormat(&inputFormat);
	}


	if (SUCCEEDED(hr))
	{
		// enable event driven handling of audio stream
		auto flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

		// enable sharing of audio endpoint device with clients in other processes
		auto mode = AUDCLNT_SHAREMODE_SHARED;
		
		// Initialize input device
		hr = inputDevice->Initialize(mode, flags, 0, 0, inputFormat, NULL);
	}

	if (SUCCEEDED(hr))
	{
		// writes address of requested interface
		classID = __uuidof(IAudioCaptureClient);
		hr = inputDevice->GetService(classID, (void**)&inputClient);
	}

	if (SUCCEEDED(hr))
	{
		// create and set the event handler which is called when the input buffer is ready for processing
		audioInEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
		hr = inputDevice->SetEventHandle(audioInEvent);
	}

	return hr;
}

HRESULT WASAPI::Init_Render()
{
	HRESULT hr = E_FAIL;

	auto renderId = GetDefaultAudioRenderId(AudioDeviceRole::Default);
	auto classID = __uuidof(IAudioClient2);

	if (NULL == renderId)
	{
		hr = E_FAIL;
	}
	else
	{
		hr = ActivateAudioInterface(renderId, __uuidof(IAudioClient2), (void**)&outputDevice);
	}

	if (SUCCEEDED(hr))
	{
		hr = outputDevice->GetMixFormat(&inputFormat);
	}

	if (SUCCEEDED(hr))
	{
		unsigned int flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
		outputDevice->Initialize(AUDCLNT_SHAREMODE_SHARED, flags, 0, 0, inputFormat, NULL);
	}

	if (SUCCEEDED(hr))
	{
		hr = outputDevice->GetService(__uuidof(IAudioRenderClient), (void**)&outputClient);
	}

	
	return hr;
}


