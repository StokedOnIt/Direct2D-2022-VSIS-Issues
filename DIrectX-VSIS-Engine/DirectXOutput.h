//*********************************************************
// Written by Sacha Berwin 
// Copyright (c) StokedOnIt Software. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//*********************************************************

#pragma once

#include "pch.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media::Imaging;



namespace DirectX_VSIS_Engine
{


	public ref class DXOutput sealed
	{

	public:
		DXOutput(int pixelWidth, int pixelHeight, float dpi);
		virtual ~DXOutput();
		void SetVSIS(VirtualSurfaceImageSource^ virtualSIS);
		void InitializeDevices();
		void SetDpi(float dpi);
		void Render(bool forceComplete) { Render(Windows::Foundation::Rect(0, 0, (float)m_width, (float)m_height), forceComplete); }
		void Render(Windows::Foundation::Rect updateSize) { Render(updateSize, false); };
		void Render(Windows::Foundation::Rect updateSize, bool forceComplete);
		void DrawGrid();
		void DrawUpdateSquare();


	private:
		void BeginDraw(Windows::Foundation::Rect updateRect);
		void BeginDraw() { BeginDraw(Windows::Foundation::Rect(0, 0, (float)m_width, (float)m_height)); }
		void EndDraw();

	private protected:

		void DXOutput::CreateDeviceIndependentResources();
		void DXOutput::CreateDeviceResources();
		ComPtr<ISurfaceImageSourceNative>   m_sisNative;

		// Direct3D device
		ComPtr<ID3D11Device>								m_d3dDevice;

		// Direct2D objects
		ComPtr<ID2D1Device>							        m_d2dDevice;
		ComPtr<ID2D1DeviceContext>					        m_d2dContext;
		ComPtr<ID2D1Factory1>								m_d2dFactory;
		ComPtr<ID2D1Bitmap1>								m_d2dDXGISurfaceBitmap;
		ComPtr<IWICImagingFactory2>							m_wicFactory;
		ComPtr<IDXGISurface>								m_surface;
		Platform::String^									m_imageFileName;

		// Cached renderer properties.
		D3D_FEATURE_LEVEL									m_featureLevel;
		Windows::Foundation::Size							m_renderTargetSize;
		DOUBLE												m_dpi;

		////Virtual Surface Stuff
		VirtualSurfaceImageSource^							m_virtualSIS;
		//ComPtr<IVirtualSurfaceImageSourceNative>			m_vsisNative;
		Windows::UI::Color									m_backgroundColor;
		DOUBLE												m_DXFeatureLevel;
		Windows::Foundation::Rect							m_updateRect;
		Windows::Foundation::Rect							m_clipRect;
		DOUBLE												base96 = 96;
		POINT												m_offset = POINT();
		int                                                 m_width;
		int                                                 m_height;
	};



}
