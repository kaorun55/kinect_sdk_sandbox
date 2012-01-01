#pragma once 

#include <Windows.h>
#include <dmo.h> // IMediaObject
#include <vector>

// Kinect SDK‚ÌƒTƒ“ƒvƒ‹‚©‚ç
class CStaticMediaBuffer : public IMediaBuffer {
public:
   CStaticMediaBuffer() {}
   STDMETHODIMP_(ULONG) AddRef() { return 2; }
   STDMETHODIMP_(ULONG) Release() { return 1; }
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
      if (riid == IID_IUnknown) {
         AddRef();
         *ppv = (IUnknown*)this;
         return NOERROR;
      }
      else if (riid == IID_IMediaBuffer) {
         AddRef();
         *ppv = (IMediaBuffer*)this;
         return NOERROR;
      }
      else
         return E_NOINTERFACE;
   }

   STDMETHODIMP SetLength( DWORD ulLength ) {
       m_ulData = ulLength;
       return NOERROR;
   }

   STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) {
       *pcbMaxLength = buffer_.size();
       return NOERROR;
   }

   STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
        if ( ppBuffer )
            *ppBuffer = &buffer_[0];

        if ( pcbLength )
            *pcbLength = m_ulData;

        return NOERROR;
   }

   void Clear()
   {
        m_ulData = 0;
   }

   ULONG GetDataLength() const
   {
       return m_ulData;
   }

   void SetBufferLength( ULONG length )
   {
       buffer_.resize( length );
   }

   std::vector< BYTE > Clone() const
   {
       return std::vector< BYTE >( buffer_.begin(), buffer_.begin() + GetDataLength() );
   }

protected:
    std::vector< BYTE > buffer_;
    ULONG m_ulData;
};

