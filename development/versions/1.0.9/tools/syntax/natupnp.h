// A stand-in for the Windows SDK's natupnp.h, used only by tools/syntax/check.sh.
//
// The real header is not in mingw's set and not in the Linux sandbox, and the module is compiled by
// MSVC on CI against the SDK.  This file declares just enough of the UPnP NAT interfaces for a
// cross-compiler to type-check session.cpp (which is where the module's UPnP thread lives).  It is
// never on the include path of build.bat and never compiled into a module.
#pragma once
#include <windows.h>
#include <oleauto.h>

// mingw answers __uuidof with a template specialisation; these are the interface ids from the SDK's
// header, written the way mingw wants them. MSVC never sees this file.

// {BD1DE1B0-0F93-434E-A73A-6446CFEAA067}
struct __declspec(uuid("BD1DE1B0-0F93-434E-A73A-6446CFEAA067")) UPnPNAT;

struct IStaticPortMapping;
struct IStaticPortMappingCollection;

struct __declspec(uuid("B171C812-CC76-485A-94D8-B6B3A2794E99")) IUPnPNAT : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_StaticPortMappingCollection(IStaticPortMappingCollection **pp) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NAComType() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SupportsDynamicLoading() = 0;
};

struct __declspec(uuid("CD1ACA29-F3BF-4CE8-B32A-48398DD9B4B9")) IStaticPortMappingCollection : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **pp) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(long lExternalPort, BSTR bstrProtocol, IStaticPortMapping **pp) = 0;
    virtual HRESULT STDMETHODCALLTYPE Add(long lExternalPort, BSTR bstrProtocol, long lInternalPort,
                                          BSTR bstrInternalClient, VARIANT_BOOL bEnabled, BSTR bstrDescription,
                                          IStaticPortMapping **pp) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(long lExternalPort, BSTR bstrProtocol) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Count(long *plCount) = 0;
};

struct __declspec(uuid("6F10711F-729B-41E5-93B8-F21D0F818DF1")) IStaticPortMapping : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_ExternalIPAddress(BSTR *pbstr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ExternalPort(long *pl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InternalPort(long *pl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Protocol(BSTR *pbstr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InternalClient(BSTR *pbstr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(VARIANT_BOOL *pb) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR *pbstr) = 0;
    virtual HRESULT STDMETHODCALLTYPE EditInternalClient(BSTR bstrInternalClient) = 0;
    virtual HRESULT STDMETHODCALLTYPE Enable(VARIANT_BOOL b) = 0;
    virtual HRESULT STDMETHODCALLTYPE EditDescription(BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE EditInternalPort(long lInternalPort) = 0;
};

#ifdef __MINGW32__
__CRT_UUID_DECL(UPnPNAT, 0xBD1DE1B0, 0x0F93, 0x434E, 0xA7, 0x3A, 0x64, 0x46, 0xCF, 0xEA, 0xA0, 0x67)
__CRT_UUID_DECL(IUPnPNAT, 0xB171C812, 0xCC76, 0x485A, 0x94, 0xD8, 0xB6, 0xB3, 0xA2, 0x79, 0x4E, 0x99)
__CRT_UUID_DECL(IStaticPortMappingCollection, 0xCD1ACA29, 0xF3BF, 0x4CE8, 0xB3, 0x2A, 0x48, 0x39, 0x8D, 0xD9, 0xB4, 0xB9)
__CRT_UUID_DECL(IStaticPortMapping, 0x6F10711F, 0x729B, 0x41E5, 0x93, 0xB8, 0xF2, 0x1D, 0x0F, 0x81, 0x8D, 0xF1)
#endif
