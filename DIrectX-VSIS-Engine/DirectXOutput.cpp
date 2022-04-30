//*********************************************************
// Written by Sacha Berwin 
// Copyright (c) StokedOnIt Software. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//*********************************************************


#include "pch.h"
#include "DirectXOutput.h"
#include "DX.h"
#include "windows.h"
#define _USE_MATH_DEFINES
#include <cmath>

using namespace concurrency;
using namespace Windows::Devices::Enumeration;
using namespace DirectX_VSIS_Engine;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;
using namespace Windows::Globalization::Collation;

DXOutput::DXOutput(int pixelWidth, int pixelHeight, float dpi)
{
	m_dpi = dpi;

	m_width = pixelWidth;
	m_height = pixelHeight;
	m_backgroundColor = Windows::UI::Color();
}

DXOutput::~DXOutput() {


	m_sisNative = nullptr;
	m_d3dDevice = nullptr;
	m_d2dDevice = nullptr;
	m_d2dContext = nullptr;
	m_d2dFactory = nullptr;
	m_d2dDXGISurfaceBitmap = nullptr;
	m_wicFactory = nullptr;
	m_surface = nullptr;
	m_virtualSIS = nullptr;
	//m_vsisNative = nullptr;
	m_d2dDevice = nullptr;

}

// SafeRelease inline function.
template <class T> inline void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

void DXOutput::SetVSIS(VirtualSurfaceImageSource^ virtualSIS)
{
	m_virtualSIS = virtualSIS;
}

void DXOutput::InitializeDevices()
{

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	SetDpi(m_dpi);

}

// Initialize resources that are independent of hardware.
void DXOutput::CreateDeviceIndependentResources()
{
	if (m_virtualSIS != nullptr)
	{
		// Query for ISurfaceImageSourceNative interface.
		DX::ThrowIfFailed(
			reinterpret_cast<IUnknown*>(m_virtualSIS)->QueryInterface(IID_PPV_ARGS(&m_sisNative))
		);
	}
	else
	{
	}


	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	//If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	DX::ThrowIfFailed(
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,//	D2D1_FACTORY_TYPE_MULTI_THREADED,
			__uuidof(ID2D1Factory2),
			&options,
			&m_d2dFactory
		)
	);

	DX::ThrowIfFailed(
		CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_wicFactory)
		)
	);

}

// Initialize hardware-dependent resources.
void DXOutput::CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (DX::SdkLayersAvailable())
	{
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		//D3D_FEATURE_LEVEL_12_2,
		//D3D_FEATURE_LEVEL_12_1,
		//D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};



	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,					// Returns the Direct3D device created.
		nullptr,			// Returns feature level of device created.
		nullptr					// Returns the device immediate context.
	);

	if (FAILED(hr))
	{
		// If the initialization fails, fall back to the WARP device.
		// For more information on WARP, see: 
		// http://go.microsoft.com/fwlink/?LinkId=286690
		DX::ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				nullptr,
				nullptr
			)
		);
	}

	DX::ThrowIfFailed(
		device.As(&m_d3dDevice)
	);



	ComPtr<IDXGIDevice> dxgiDevice;

	DX::ThrowIfFailed(
		m_d3dDevice.As(&dxgiDevice)
	);

	// Create the Direct2D device object and a corresponding context.
	DX::ThrowIfFailed(
		m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
	);

	DX::ThrowIfFailed(
		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_d2dContext
		)
	);

	//Associate the DXGI device with the SurfaceImageSource.
	DX::ThrowIfFailed(
		m_sisNative->SetDevice(dxgiDevice.Get())
	);

	D3D_FEATURE_LEVEL featurelevel;

	featurelevel = m_d3dDevice->GetFeatureLevel();

	if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1) m_DXFeatureLevel = 9.1;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_2) m_DXFeatureLevel = 9.2;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_3) m_DXFeatureLevel = 9.3;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0) m_DXFeatureLevel = 10.0;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1) m_DXFeatureLevel = 10.1;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0) m_DXFeatureLevel = 11.0;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1) m_DXFeatureLevel = 11.1;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0) m_DXFeatureLevel = 12.0;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1) m_DXFeatureLevel = 12.1;
	else if (featurelevel == D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_2) m_DXFeatureLevel = 12.2;
	else m_DXFeatureLevel = 9.2;


}

// Sets the current DPI.
void DXOutput::SetDpi(float dpi)
{
	// Update Direct2D's stored DPI.
	m_d2dContext->SetDpi(dpi, dpi);
	m_dpi = dpi;
}

// Begins drawing, allowing updates to content in the specified area.
void DXOutput::BeginDraw(Windows::Foundation::Rect updateRect)
{

	try
	{
		double sizeDPIRatio = (double)m_dpi / (double)96;
		double sizeDPIRatioInvert = (double)96 / (double)m_dpi;

		POINT SurfaceDimentions;
		SurfaceDimentions.x = m_width;
		SurfaceDimentions.y = m_height;
		POINT offset = { 0 };
		ComPtr<IDXGISurface> surface;


		if (updateRect.X < 0)
			updateRect.X = 0;
		if (updateRect.Y < 0)
			updateRect.Y = 0;
		if (updateRect.X + updateRect.Width > SurfaceDimentions.x) {
			updateRect.Width = updateRect.X - SurfaceDimentions.x;//((updateRect.X + updateRect.Width) - SurfaceDimentions.x);
		}
		if (updateRect.Y + updateRect.Height > SurfaceDimentions.y) {
			updateRect.Y = updateRect.Y - ((updateRect.Y + updateRect.Height) - SurfaceDimentions.y);
		}

		if ((int)updateRect.Width < 1 || (int)updateRect.Height < 1)
		{
			updateRect.X = 0;
			updateRect.Y = 0;
			updateRect.Width = 1;
			updateRect.Height = 1;
		}

		RECT updateRectNative;

		updateRectNative.left = static_cast<LONG>(updateRect.X);
		updateRectNative.top = static_cast<LONG>(updateRect.Y);
		updateRectNative.right = static_cast<LONG>(updateRect.X + updateRect.Width);
		updateRectNative.bottom = static_cast<LONG>(updateRect.Y + updateRect.Height);


		// Begin drawing - returns a target surface and an offset to use as the top left origin when drawing.
		HRESULT beginDrawHR = m_sisNative->BeginDraw(updateRectNative, &surface, &offset);


		if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device has been removed or reset, attempt to recreate it and continue drawing.
			CreateDeviceResources();
			BeginDraw(updateRect);
		}
		else
		{
			// Notify the caller by throwing an exception if any other error was encountered.
			DX::ThrowIfFailed(
				beginDrawHR
			);
		}

		m_offset = offset;

		// Create render target.
		DX::ThrowIfFailed(
			m_d2dContext->CreateBitmapFromDxgiSurface(
				surface.Get(), NULL, &m_d2dDXGISurfaceBitmap
			));

		D2D1_SIZE_F size = m_d2dDXGISurfaceBitmap.Get()->GetSize();
		D2D1_SIZE_U pixelSize = m_d2dDXGISurfaceBitmap.Get()->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = m_d2dDXGISurfaceBitmap.Get()->GetPixelFormat();

		// Set context's render target.
		m_d2dContext->SetTarget(m_d2dDXGISurfaceBitmap.Get());

		// Begin drawing using D2D context.
		m_d2dContext->BeginDraw();


		// Apply transform
		double x = (-updateRect.X + offset.x);
		double y = (-updateRect.Y + offset.y);
		double xScaled = x * sizeDPIRatioInvert;
		double yScaled = y * sizeDPIRatioInvert;
		m_d2dContext->SetTransform(
			D2D1::Matrix3x2F::Translation(
				static_cast<float>((xScaled)),
				static_cast<float>((yScaled))
			));


		m_d2dContext->PushAxisAlignedClip(
			D2D1::RectF(
				static_cast<float>(updateRect.X),
				static_cast<float>(updateRect.Y),
				static_cast<float>(updateRect.Width),
				static_cast<float>(updateRect.Height)
			),
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
		);

	}
	catch (Platform::Exception^ e)
	{
		/*CoreWindowDialog^ dialog = ref new CoreWindowDialog(e->Message);
		dialog->ShowAsync();*/
	}

}



// Ends drawing updates started by a previous BeginDraw call.
void DXOutput::EndDraw()
{

	// Remove the transform and clip applied in BeginDraw since
	// the target area can change on every update.
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	m_d2dContext->PopAxisAlignedClip();


	// Remove the render target and end drawing.
	DX::ThrowIfFailed(
		m_d2dContext->EndDraw()
	);

	m_d2dContext->SetTarget(nullptr);

	DX::ThrowIfFailed(
		m_sisNative->EndDraw()
	);
}




void DXOutput::Render(Windows::Foundation::Rect updateSize, bool forceComplete)
{

	double sizeDPIRatio = m_dpi / (double)96;
	double sizeDPIRatioInvert = (double)96 / m_dpi;

	Windows::Foundation::Rect fullSize;
	fullSize.X = 0;
	fullSize.Y = 0;
	fullSize.Width = m_width;
	fullSize.Height = m_height;

	if (updateSize.X + updateSize.Width > m_width)
	{
		updateSize.Width = updateSize.Width - (updateSize.X + updateSize.Width - m_width);
	}
	if (updateSize.Y + updateSize.Height > m_height)
	{
		updateSize.Height = updateSize.Height - (updateSize.Y + updateSize.Height - m_height);
	}

	updateSize.X = floor(updateSize.X);
	updateSize.Y = floor(updateSize.Y);
	updateSize.Width = ceil(updateSize.Width);
	updateSize.Height = ceil(updateSize.Height);

	m_d2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	if (forceComplete) {
		BeginDraw(fullSize);
		DrawGrid();
		EndDraw();
	}
	else {
		BeginDraw(updateSize);
		DrawUpdateSquare();
		EndDraw();
	}


}


void DXOutput::DrawUpdateSquare()
{
	ComPtr<ID2D1SolidColorBrush> brush;
	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(
			D2D1::ColorF(1, 0, 0, 1),
			&brush
		));

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//m_d2dContext->Clear();

	//D2D1_SIZE_F rtSize = m_d2dContext->GetSize();


	//// Draw two rectangles.
	//D2D1_RECT_F rectangle1 = D2D1::RectF(
	//	rtSize.width / 2 - 50.0f,
	//	rtSize.height / 2 - 50.0f,
	//	rtSize.width / 2 + 50.0f,
	//	rtSize.height / 2 + 50.0f
	//);

	//D2D1_RECT_F rectangle2 = D2D1::RectF(
	//	rtSize.width / 2 - 100.0f,
	//	rtSize.height / 2 - 100.0f,
	//	rtSize.width / 2 + 100.0f,
	//	rtSize.height / 2 + 100.0f
	//);


	//// Draw a filled rectangle.
	//m_d2dContext->FillRectangle(&rectangle1, brush.Get());

	//// Draw the outline of a rectangle.
	//m_d2dContext->DrawRectangle(&rectangle2, brush.Get());
}

void DXOutput::DrawGrid()
{
	ComPtr<ID2D1SolidColorBrush> grayBrush;
	DX::ThrowIfFailed(
		m_d2dContext->CreateSolidColorBrush(
			D2D1::ColorF(0.1, 0.1, 0.1, 1),
			&grayBrush
		));

	m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

	m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::HotPink));

	D2D1_SIZE_F rtSize = m_d2dContext->GetSize();

	// Draw a grid background.
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	for (int x = 0; x < width; x += 10)
	{
		m_d2dContext->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
			grayBrush.Get(),
			0.5f
		);
	}

	for (int y = 0; y < height; y += 10)
	{
		m_d2dContext->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
			grayBrush.Get(),
			0.5f
		);
	}


}





