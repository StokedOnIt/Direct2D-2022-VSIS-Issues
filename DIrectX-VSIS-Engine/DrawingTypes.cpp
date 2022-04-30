#include "pch.h"
#include "DrawingTypes.h"



namespace NoteDrawX_Universal {

	/*unsigned long DrawingObject::Release()
	{
		_references--;
		return _references;
	}
	unsigned long DrawingObject::AddRef()
	{
		_references++;
		return _references;
	}*/
	unsigned long DrawingObject::AddRef()
	{
		//InterlockedIncrement(m_cRef);
		m_cRef++;
		return m_cRef;
	}
	unsigned long DrawingObject::Release()
	{
		// Decrement the object's internal counter.
		unsigned long ulRefCount = m_cRef--;// InterlockedDecrement(m_cRef);
		if (0 == m_cRef)
		{
			this->Bitmap.Bitmap = nullptr;
			this->Bitmap.TransformEffect = nullptr;
			this->Geometry.Geometry = nullptr;
			this->Geometry.BitmapBrush = nullptr;
			this->Geometry.FillBitmapBrush = nullptr;
			this->Geometry.FillImageBrush = nullptr;
			this->Geometry.FillLinearBrush = nullptr;
			this->Geometry.FillRadialBrush = nullptr;
			this->Geometry.FillSolidBrush = nullptr;
			this->Geometry.ImageBrush = nullptr;
			this->Geometry.LinearBrush = nullptr;
			this->Geometry.RadialBrush = nullptr;
			this->Geometry.SolidBrush = nullptr;
			this->Geometry.TransformedGeometry = nullptr;
			this->Geometry.TransformEffect = nullptr;
			delete this;
		}
		return ulRefCount;
	}
	HRESULT DrawingObject::QueryInterface(REFIID   riid,
		LPVOID * ppvObj)
	{
		// Always set out parameter to NULL, validating it first.
		if (!ppvObj)
			return E_INVALIDARG;
		*ppvObj = NULL;
		if (riid == IID_IUnknown )
		{
			// Increment the reference count and return the pointer.
			*ppvObj = (LPVOID)this;
			AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}
	//HRESULT DrawingObject::QueryInterface(
	//	REFIID riid,
	//	void   **ppvObject
	//	)
	//{
	//	//DrawingObject *drawObject = NULL;
	//	HRESULT hRes;

	//	//CoInitialize(NULL);
	//	hRes = CoCreateInstance(riid, NULL, CLSCTX_INPROC_SERVER, riid, (void **)&ppvObject);
	//	if (ppvObject != NULL)
	//	{
	//		//AfxMessageBox("you've got a pointer to IHTMLDocument interface...");
	//		//drawObject - &gt; QueryInterface(IID_IDispatch, (void **)&pHtmlDoc2);
	//		//if (pHtmlDoc2 != NULL)
	//		//{
	//		//	AfxMessageBox("you've got a pointer to IHTMLDocument2 interface...");
	//		//	// now use pHtmlDoc2 methods
	//		//}
	//	}
	//	// when you don't need it any more
	//	//pHtmlDoc2 - &gt; Release();
	//	ppvObject;
	//	Release();
	//	//CoUninitialize();
	//	return hRes;
	//}
	//DrawingObject::Release()
	//{
	//	try {
	//		/*Bitmap.Bitmap.~ComPtr();
	//		Bitmap.TransformEffect.~ComPtr();
	//		Geometry.BitmapBrush.~ComPtr();
	//		Geometry.StrokeStyle.~ComPtr();
	//		Geometry.SolidBrush.~ComPtr();
	//		Geometry.LinearBrush.~ComPtr();
	//		Geometry.RadialBrush.~ComPtr();
	//		Geometry.ImageBrush.~ComPtr();
	//		Geometry.FillSolidBrush.~ComPtr();
	//		Geometry.FillLinearBrush.~ComPtr();
	//		Geometry.FillRadialBrush.~ComPtr();
	//		Geometry.FillBitmapBrush.~ComPtr();
	//		Geometry.FillImageBrush.~ComPtr();*/
	//		//Geometry.TransformEffect.~ComPtr();
	//		//Geometry.TransformedGeometry.~ComPtr();
	//		//Geometry.Geometry.~ComPtr();

	//	}
	//	catch (...) {
	//		OutputDebugString(L"unexpected exception");

	//	}
	//	//delete Bitmap.Bitmap.Get();
	//	//delete Geometry.Geometry.Get();
	//	//delete Geometry.BitmapBrush.Get();
	//	//delete Geometry.StrokeStyle.Get();
	//	//delete Bitmap.TransformEffect.Get();
	//	//delete Geometry.SolidBrush.Get();
	//	//delete Geometry.LinearBrush.Get();
	//	//delete Geometry.RadialBrush.Get();
	//	//delete Geometry.ImageBrush.Get();
	//	//delete Geometry.FillSolidBrush.Get();
	//	//delete Geometry.FillLinearBrush.Get();
	//	//delete Geometry.FillRadialBrush.Get();
	//	//delete Geometry.FillBitmapBrush.Get();
	//	//delete Geometry.FillImageBrush.Get();
	//	//delete Geometry.TransformEffect.Get();
	//	//delete Geometry.TransformedGeometry.Get();

	//	//Bitmap.Bitmap = nullptr;
	//	//Bitmap.TransformEffect = nullptr;
	//	//Geometry.Geometry = nullptr;
	//	//Geometry.BitmapBrush = nullptr;
	//	//Geometry.StrokeStyle = nullptr;
	//	//Geometry.SolidBrush = nullptr;
	//	//Geometry.LinearBrush = nullptr;
	//	//Geometry.RadialBrush = nullptr;
	//	//Geometry.ImageBrush = nullptr;
	//	//Geometry.FillSolidBrush = nullptr;
	//	//Geometry.FillLinearBrush = nullptr;
	//	//Geometry.FillRadialBrush = nullptr;
	//	//Geometry.FillBitmapBrush = nullptr;
	//	//Geometry.FillImageBrush = nullptr;
	//	//Geometry.TransformEffect = nullptr;
	//	//Geometry.TransformedGeometry = nullptr;

	//}
}