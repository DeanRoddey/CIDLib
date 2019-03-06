//
// FILE NAME: CIDWebBrowser_EmbedBrowser.c
//
// AUTHOR: Dean Roddey (derived from an example, see below)
//
// CREATED: 10/10/2005
//
// COPYRIGHT:
//
//  As best I can tell, this code is based on an example provided by a guy named
//  Chris Becke (http://www.mvps.org/user32/), but of course people are so
//  bad about not giving attribution that I'm not sure. It wasn't indicated in
//  the example I found. I have modified it to provide a wrapper within a CIDLib
//  based wrapper. Most if it is fairly incomprehensible if you aren't a fairly
//  heavy COM/OLE weenie.
//
// DESCRIPTION:
//
//  This file implements the OLE stuff required to support embedding an
//  IWebBrowser2 object in a window (the window is managed outside of here,
//  we just get calls in to move, size, create, destroy, etc... the embedded
//  browser.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $Log$
//

// Use default packing in here
#pragma pack()
#undef WIN32_LEAN_AND_MEAN
#pragma comment(lib, "oleaut32.lib")
#include <windows.h>
#include <exdisp.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <crtdbg.h>
#include <tchar.h>

// #define INEMBEDFILE
#include "CIDWebBrowser_EmbedBrowser.h"
// #undef  INEMBEDFILE



//
//  An el cheapo local way to pump msgs during some operations here. This guy cannot call
//  our c++ stuff, so we have to do a C one.
//
static void PumpMsgs()
{
    SYSTEMTIME  SysTime;
    FILETIME FlTime;

    GetLocalTime(&SysTime);
    if (!SystemTimeToFileTime(&SysTime, &FlTime))
        return;

    // Convert it to a 64 bit int
    unsigned __int64 lCur = FlTime.dwLowDateTime;
    lCur <<= 32;
    lCur |= FlTime.dwHighDateTime;

    // We'll wait for up to 10 millis
    unsigned __int64 lEnd = lCur + 100000;

    MSG msgCur;
    while (lCur < lEnd)
    {
        if (!PeekMessage(&msgCur, 0, 0, 0, PM_REMOVE))
            break;

        TranslateMessage(&msgCur);
        DispatchMessage(&msgCur);

        GetLocalTime(&SysTime);
        if (!SystemTimeToFileTime(&SysTime, &FlTime))
            return;

        lCur = FlTime.dwLowDateTime;
        lCur <<= 32;
        lCur |= FlTime.dwHighDateTime;
    }
}

//
//  A helper to try to wait for an existing operation to end. We check if busy. If so, we
//  issue a stop, then we'll wait for a little while, checking periodically, for it to end.
//
static void WaitNotBusy(IWebBrowser2* pBrowser)
{
    SYSTEMTIME  SysTime;
    FILETIME FlTime;

    GetLocalTime(&SysTime);
    if (!SystemTimeToFileTime(&SysTime, &FlTime))
        return;

    // Convert it to a 64 bit int
    unsigned __int64 lCur = FlTime.dwLowDateTime;
    lCur <<= 32;
    lCur |= FlTime.dwHighDateTime;

    // We'll wait for up to up to a second
    unsigned __int64 lEnd = lCur + 10000000;

    VARIANT_BOOL vBusy;

    // Issue a stop
    pBrowser->lpVtbl->Stop(pBrowser);

    // Now loop, checking for it to be done and pumping messages
    while (lCur < lEnd)
    {
        pBrowser->lpVtbl->get_Busy(pBrowser, &vBusy);
        if (vBusy != VARIANT_TRUE)
            break;

        PumpMsgs();

        GetLocalTime(&SysTime);
        if (!SystemTimeToFileTime(&SysTime, &FlTime))
            return;

        lCur = FlTime.dwLowDateTime;
        lCur <<= 32;
        lCur |= FlTime.dwHighDateTime;
    }
}



//
//  Our data structures we use to track the COM objects.
//
typedef struct
{
    IOleClientSite* client_site;
    IWebBrowser2*   web_browser;
    IOleObject*     wb_object;
}   TBrwsInfo;


typedef struct IOleClientSiteImpl
{
    IOleClientSite IOleClientSite_iface;
    IOleInPlaceSite IOleInPlaceSite_iface;
    IOleInPlaceFrame IOleInPlaceFrame_iface;
    IDocHostUIHandler IDocHostUIHandler_iface;

    /* IOleClientSiteImpl data */
    IOleObject *pBrowserObject;
    LONG ref;

    /* IOleInPlaceFrame data */
    HWND hwndWindow;
} IOleClientSiteImpl;

static IOleClientSiteImpl *impl_from_IOleClientSite(IOleClientSite *iface)
{
    return CONTAINING_RECORD(iface, IOleClientSiteImpl, IOleClientSite_iface);
}

static HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite *iface, REFIID riid, void **ppvObj)
{
    IOleClientSiteImpl *This = impl_from_IOleClientSite(iface);

    *ppvObj = NULL;

    if (IsEqualIID(riid, &IID_IUnknown)) {
        *ppvObj = &This->IOleClientSite_iface;
    }else if(IsEqualIID(riid, &IID_IOleClientSite)) {
        *ppvObj = &This->IOleClientSite_iface;
    }else if (IsEqualIID(riid, &IID_IOleInPlaceSite)) {
        *ppvObj = &This->IOleInPlaceSite_iface;
    }else if (IsEqualIID(riid, &IID_IOleInPlaceFrame)) {
        *ppvObj = &This->IOleInPlaceSite_iface;
    }else if (IsEqualIID(riid, &IID_IDocHostUIHandler)) {
        *ppvObj = &This->IDocHostUIHandler_iface;
    }else {
        return E_NOINTERFACE;
    }

    ((IUnknown*)*ppvObj)->lpVtbl->AddRef((IUnknown*)*ppvObj);
    return S_OK;
}

static ULONG STDMETHODCALLTYPE Site_AddRef(IOleClientSite *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleClientSite(iface);
    LONG ref = InterlockedIncrement(&This->ref);

    return ref;
}

static ULONG STDMETHODCALLTYPE Site_Release(IOleClientSite *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleClientSite(iface);
    LONG ref = InterlockedDecrement(&This->ref);

    if(!ref)
        GlobalFree(This);

    return ref;
}

static HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite *iface)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite *iface, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite *iface, LPOLECONTAINER *ppContainer)
{
    *ppContainer = NULL;

    return E_NOINTERFACE;
}

static HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite *iface)
{
    return NOERROR;
}

static HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite *iface, BOOL fShow)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite *iface)
{
    return E_NOTIMPL;
}

static IOleClientSiteVtbl MyIOleClientSiteTable =
{
    Site_QueryInterface,
    Site_AddRef,
    Site_Release,
    Site_SaveObject,
    Site_GetMoniker,
    Site_GetContainer,
    Site_ShowObject,
    Site_OnShowWindow,
    Site_RequestNewObjectLayout
};

static IOleClientSiteImpl *impl_from_IDocHostUIHandler(IDocHostUIHandler *iface)
{
    return CONTAINING_RECORD(iface, IOleClientSiteImpl, IDocHostUIHandler_iface);
}

static HRESULT STDMETHODCALLTYPE UI_QueryInterface(IDocHostUIHandler *iface, REFIID riid, LPVOID *ppvObj)
{
    IOleClientSiteImpl *This = impl_from_IDocHostUIHandler(iface);

    return This->IOleClientSite_iface.lpVtbl->QueryInterface
    (
        &This->IOleClientSite_iface, riid, ppvObj
    );
}

static ULONG STDMETHODCALLTYPE UI_AddRef(IDocHostUIHandler *iface)
{
    IOleClientSiteImpl *This = impl_from_IDocHostUIHandler(iface);

    return This->IOleClientSite_iface.lpVtbl->AddRef
    (
        &This->IOleClientSite_iface
    );
}

static ULONG STDMETHODCALLTYPE UI_Release(IDocHostUIHandler * iface)
{
    IOleClientSiteImpl *This = impl_from_IDocHostUIHandler(iface);

    return This->IOleClientSite_iface.lpVtbl->Release(&This->IOleClientSite_iface);
}

static HRESULT STDMETHODCALLTYPE UI_ShowContextMenu(IDocHostUIHandler *iface, DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_GetHostInfo(IDocHostUIHandler *iface, DOCHOSTUIINFO *pInfo)
{
    pInfo->cbSize = sizeof(DOCHOSTUIINFO);
    pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_ShowUI(IDocHostUIHandler *iface, DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_HideUI(IDocHostUIHandler *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_UpdateUI(IDocHostUIHandler *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_EnableModeless(IDocHostUIHandler *iface, BOOL fEnable)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_OnDocWindowActivate(IDocHostUIHandler *iface, BOOL fActivate)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_OnFrameWindowActivate(IDocHostUIHandler *iface, BOOL fActivate)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_ResizeBorder(IDocHostUIHandler *iface, LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE UI_TranslateAccelerator(IDocHostUIHandler *iface, LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE UI_GetOptionKeyPath(IDocHostUIHandler *iface, LPOLESTR *pchKey, DWORD dw)
{
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE UI_GetDropTarget(IDocHostUIHandler *iface, IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE UI_GetExternal(IDocHostUIHandler *iface, IDispatch **ppDispatch)
{
    *ppDispatch = NULL;
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE UI_TranslateUrl(IDocHostUIHandler *iface, DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
    *ppchURLOut = NULL;
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE UI_FilterDataObject(IDocHostUIHandler *iface, IDataObject *pDO, IDataObject **ppDORet)
{
    *ppDORet = NULL;
    return S_FALSE;
}

static IDocHostUIHandlerVtbl MyIDocHostUIHandlerTable =
{
    UI_QueryInterface,
    UI_AddRef,
    UI_Release,
    UI_ShowContextMenu,
    UI_GetHostInfo,
    UI_ShowUI,
    UI_HideUI,
    UI_UpdateUI,
    UI_EnableModeless,
    UI_OnDocWindowActivate,
    UI_OnFrameWindowActivate,
    UI_ResizeBorder,
    UI_TranslateAccelerator,
    UI_GetOptionKeyPath,
    UI_GetDropTarget,
    UI_GetExternal,
    UI_TranslateUrl,
    UI_FilterDataObject
};

static IOleClientSiteImpl *impl_from_IOleInPlaceSite(IOleInPlaceSite *iface)
{
    return CONTAINING_RECORD(iface, IOleClientSiteImpl, IOleInPlaceSite_iface);
}

static HRESULT STDMETHODCALLTYPE InPlace_QueryInterface(IOleInPlaceSite *iface, REFIID riid, LPVOID *ppvObj)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);

    return This->IOleClientSite_iface.lpVtbl->QueryInterface
    (
        &This->IOleClientSite_iface, riid, ppvObj
    );
}

static ULONG STDMETHODCALLTYPE InPlace_AddRef(IOleInPlaceSite *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);

    return This->IOleClientSite_iface.lpVtbl->AddRef(&This->IOleClientSite_iface);
}

static ULONG STDMETHODCALLTYPE InPlace_Release(IOleInPlaceSite *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);

    return This->IOleClientSite_iface.lpVtbl->Release(&This->IOleClientSite_iface);
}

static HRESULT STDMETHODCALLTYPE InPlace_GetWindow(IOleInPlaceSite *iface, HWND *lphwnd)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);
    *lphwnd = This->hwndWindow;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_ContextSensitiveHelp(IOleInPlaceSite *iface, BOOL fEnterMode)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE InPlace_CanInPlaceActivate(IOleInPlaceSite *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceActivate(IOleInPlaceSite *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_OnUIActivate(IOleInPlaceSite *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_GetWindowContext(IOleInPlaceSite *iface, LPOLEINPLACEFRAME *lplpFrame, LPOLEINPLACEUIWINDOW *lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);

    *lplpFrame = &This->IOleInPlaceFrame_iface;

    This->IOleInPlaceFrame_iface.lpVtbl->AddRef(&This->IOleInPlaceFrame_iface);

    *lplpDoc = NULL;

    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = This->hwndWindow;
    lpFrameInfo->haccel = NULL;
    lpFrameInfo->cAccelEntries = 0;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_Scroll(IOleInPlaceSite *iface, SIZE scrollExtent)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE InPlace_OnUIDeactivate(IOleInPlaceSite *iface, BOOL fUndoable)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceDeactivate(IOleInPlaceSite *iface)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE InPlace_DiscardUndoState(IOleInPlaceSite *iface)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE InPlace_DeactivateAndUndo(IOleInPlaceSite *iface)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE InPlace_OnPosRectChange(IOleInPlaceSite *iface, LPCRECT lprcPosRect)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceSite(iface);
    IOleInPlaceObject *inplace;

    HRESULT hRes = This->pBrowserObject->lpVtbl->QueryInterface
    (
        This->pBrowserObject, &IID_IOleInPlaceObject, (void **)&inplace
    );

    if (hRes == S_OK)
    {
        inplace->lpVtbl->SetObjectRects(inplace, lprcPosRect, lprcPosRect);
        inplace->lpVtbl->Release(inplace);
    }

    return S_OK;
}

static IOleInPlaceSiteVtbl MyIOleInPlaceSiteTable =
{
    InPlace_QueryInterface,
    InPlace_AddRef,
    InPlace_Release,
    InPlace_GetWindow,
    InPlace_ContextSensitiveHelp,
    InPlace_CanInPlaceActivate,
    InPlace_OnInPlaceActivate,
    InPlace_OnUIActivate,
    InPlace_GetWindowContext,
    InPlace_Scroll,
    InPlace_OnUIDeactivate,
    InPlace_OnInPlaceDeactivate,
    InPlace_DiscardUndoState,
    InPlace_DeactivateAndUndo,
    InPlace_OnPosRectChange
};

static  IOleClientSiteImpl *impl_from_IOleInPlaceFrame(IOleInPlaceFrame *iface)
{
    return CONTAINING_RECORD(iface, IOleClientSiteImpl, IOleInPlaceFrame_iface);
}

static HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame *iface, REFIID riid, LPVOID *ppvObj)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceFrame(iface);

    return This->IOleClientSite_iface.lpVtbl->QueryInterface
    (
        &This->IOleClientSite_iface, riid, ppvObj
    );
}

static ULONG STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceFrame(iface);

    return This->IOleClientSite_iface.lpVtbl->AddRef(&This->IOleClientSite_iface);
}

static ULONG STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame *iface)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceFrame(iface);

    return This->IOleClientSite_iface.lpVtbl->Release(&This->IOleClientSite_iface);
}

static HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame *iface, HWND *lphwnd)
{
    IOleClientSiteImpl *This = impl_from_IOleInPlaceFrame(iface);
    *lphwnd = This->hwndWindow;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame *iface, BOOL fEnterMode)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame *iface, LPRECT lprectBorder)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame *iface, LPCBORDERWIDTHS pborderwidths)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame *iface, LPCBORDERWIDTHS pborderwidths)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame *iface, IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame *iface, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame *iface, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame *iface, HMENU hmenuShared)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame *iface, LPCOLESTR pszStatusText)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame *iface, BOOL fEnable)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame *iface, LPMSG lpmsg, WORD wID)
{
    return E_NOTIMPL;
}

static IOleInPlaceFrameVtbl MyIOleInPlaceFrameTable =
{
    Frame_QueryInterface,
    Frame_AddRef,
    Frame_Release,
    Frame_GetWindow,
    Frame_ContextSensitiveHelp,
    Frame_GetBorder,
    Frame_RequestBorderSpace,
    Frame_SetBorderSpace,
    Frame_SetActiveObject,
    Frame_InsertMenus,
    Frame_SetMenu,
    Frame_RemoveMenus,
    Frame_SetStatusText,
    Frame_EnableModeless,
    Frame_TranslateAccelerator
};

static HRESULT STDMETHODCALLTYPE Storage_QueryInterface(IStorage *This, REFIID riid, LPVOID *ppvObj)
{
    return E_NOTIMPL;
}

static ULONG STDMETHODCALLTYPE Storage_AddRef(IStorage *This)
{
    return 1;
}

static ULONG STDMETHODCALLTYPE Storage_Release(IStorage *This)
{
    return 2;
}

static HRESULT STDMETHODCALLTYPE Storage_CreateStream(IStorage *This, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_OpenStream(IStorage *This, const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_CreateStorage(IStorage *This, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_OpenStorage(IStorage *This, const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_CopyTo(IStorage *This, DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_MoveElementTo(IStorage *This, const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_Commit(IStorage *This, DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_Revert(IStorage *This)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_EnumElements(IStorage *This, DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_DestroyElement(IStorage *This, const OLECHAR *pwcsName)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_RenameElement(IStorage *This, const WCHAR *pwcsOldName, const WCHAR *pwcsNewName)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_SetElementTimes(IStorage *This, const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_SetClass(IStorage *This, REFCLSID clsid)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE Storage_SetStateBits(IStorage *This, DWORD grfStateBits, DWORD grfMask)
{
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE Storage_Stat(IStorage *This, STATSTG *pstatstg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

static IStorageVtbl MyIStorageTable =
{
    Storage_QueryInterface,
    Storage_AddRef,
    Storage_Release,
    Storage_CreateStream,
    Storage_OpenStream,
    Storage_CreateStorage,
    Storage_OpenStorage,
    Storage_CopyTo,
    Storage_MoveElementTo,
    Storage_Commit,
    Storage_Revert,
    Storage_EnumElements,
    Storage_DestroyElement,
    Storage_RenameElement,
    Storage_SetElementTimes,
    Storage_SetClass,
    Storage_SetStateBits,
    Storage_Stat
};

static IStorage MyIStorage = { &MyIStorageTable };



//
//  Load up the indicated URL.
//
long DisplayHTMLPage(void* pData, const wchar_t* webPageName)
{
    VARIANT vEmpty;
    VARIANT vFlags;
    VARIANT vTarget;
    VARIANT vURL;

    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return -1;

    // Make sure we aren't interrupting anything
    WaitNotBusy(pInfo->web_browser);

    VariantInit(&vEmpty);
    VariantInit(&vFlags);
    VariantInit(&vTarget);
    VariantInit(&vURL);

    vURL.vt = VT_BSTR;
    vURL.bstrVal = SysAllocString(webPageName);
    if (!vURL.bstrVal)
        return(-6);

    // We don't do tabs or popups, it's always into the same window
    vTarget.vt = VT_BSTR;
    vTarget.bstrVal = SysAllocString(L"_self");
    if (!vTarget.bstrVal)
    {
        VariantClear(&vURL);
        return(-6);
    }

    // Set up the flags we want
    vFlags.vt = VT_I4;
    vFlags.lVal =  navNoHistory
                   | navTrustedForActiveX;

    // Call the Navigate2() function to actually display the page
    HRESULT hRes = pInfo->web_browser->lpVtbl->Navigate2
    (
        pInfo->web_browser, &vURL, &vFlags, &vTarget, &vEmpty, &vEmpty
    );

    // Clean up stuff we don't need any more
    VariantClear(&vEmpty);
    VariantClear(&vFlags);
    VariantClear(&vTarget);
    VariantClear(&vURL);

    if (hRes == S_OK)
        return(0);

    return -5;
}



//
//  The window calls us here to embed a browser within himself. He shoudl only call
//  this if he doesn't already have embedding data. We will create an embedding data
//  structure and set it up and give it back to him via ppData. He has to pass this
//  back in to any subsequent calls.
//
int EmbedBrowserObject(void** ppData, struct HWND__* hwndParent)
{
    IOleClientSiteImpl *iOleClientSiteImpl;
    IOleInPlaceObject *inplace;
    IOleObject *browserObject;
    IWebBrowser2 *webBrowser2;
    HRESULT hRes;
    RECT rc;

    // Create our browser info and give a pointer back to the caller
    TBrwsInfo* pInfo = (TBrwsInfo*)GlobalAlloc(GMEM_FIXED, sizeof(TBrwsInfo));
    memset(pInfo, 0, sizeof(TBrwsInfo));
    *ppData = pInfo;

    iOleClientSiteImpl = (IOleClientSiteImpl*)GlobalAlloc(GMEM_FIXED, sizeof(IOleClientSiteImpl));
    if (!iOleClientSiteImpl)
        return FALSE;

    iOleClientSiteImpl->ref = 1;
    iOleClientSiteImpl->IOleClientSite_iface.lpVtbl = &MyIOleClientSiteTable;
    iOleClientSiteImpl->IOleInPlaceSite_iface.lpVtbl = &MyIOleInPlaceSiteTable;
    iOleClientSiteImpl->IOleInPlaceFrame_iface.lpVtbl = &MyIOleInPlaceFrameTable;
    iOleClientSiteImpl->hwndWindow = hwndParent;
    iOleClientSiteImpl->IDocHostUIHandler_iface.lpVtbl = &MyIDocHostUIHandlerTable;

    hRes = OleCreate
    (
        &CLSID_WebBrowser
        , &IID_IOleObject
        , OLERENDER_DRAW
        , 0
        , &iOleClientSiteImpl->IOleClientSite_iface
        , &MyIStorage
        , (void **)&browserObject
    );

    if (FAILED(hRes)) goto error;

    pInfo->client_site = &iOleClientSiteImpl->IOleClientSite_iface;
    pInfo->wb_object = browserObject;

    if (FAILED(hRes)) goto error;

    /* make the browser object accessible to the IOleClientSite implementation */
    iOleClientSiteImpl->pBrowserObject = browserObject;

    GetClientRect(hwndParent, &rc);

    hRes = OleSetContainedObject((struct IUnknown *)browserObject, TRUE);
    if (FAILED(hRes)) goto error;

    hRes = browserObject->lpVtbl->DoVerb
    (
        browserObject
        , OLEIVERB_SHOW
        , NULL
        , &iOleClientSiteImpl->IOleClientSite_iface
        , -1
        , hwndParent
        , &rc
    );

    if (FAILED(hRes)) goto error;

    hRes = browserObject->lpVtbl->QueryInterface
    (
        browserObject, &IID_IOleInPlaceObject, (void**)&inplace
    );
    if (FAILED(hRes)) goto error;

    inplace->lpVtbl->SetObjectRects(inplace, &rc, &rc);
    inplace->lpVtbl->Release(inplace);

    hRes = browserObject->lpVtbl->QueryInterface
    (
        browserObject, &IID_IWebBrowser2, (void **)&webBrowser2
    );

    if (SUCCEEDED(hRes))
    {
        pInfo->web_browser = webBrowser2;
        return TRUE;
    }

error:
    UnEmbedBrowserObject(ppData, hwndParent);
    GlobalFree(iOleClientSiteImpl);

    return FALSE;
}



//
// Get the show address bar attribute
//
int GetShowAddrBar(void* pData)
{
    VARIANT_BOOL vRes;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return 0;

    pInfo->web_browser->lpVtbl->get_AddressBar(pInfo->web_browser, &vRes);
    return (vRes == VARIANT_TRUE);
}



//
// Get the show tool bar attribute
//
int GetShowToolBar(void* pData)
{
    int Res = 0;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return 0;

    pInfo->web_browser->lpVtbl->get_ToolBar(pInfo->web_browser, &Res);
    return (Res != 0);
}



//
// Get the silent mode attribute
//
int GetSilentMode(void* pData)
{
    VARIANT_BOOL vRes;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return 0;

    pInfo->web_browser->lpVtbl->get_Silent(pInfo->web_browser, &vRes);
    return (vRes == VARIANT_TRUE);
}



// Size our contained browser to the indicated width and height
void ResizeBrowser(void* pData, const unsigned dwWidth, const unsigned dwHeight)
{
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return;

    pInfo->web_browser->lpVtbl->put_Width(pInfo->web_browser, dwWidth);
    pInfo->web_browser->lpVtbl->put_Height(pInfo->web_browser, dwHeight);
}



//
//  Do some standard browser actions. We just get an action code, which are defined
//  in our header file.
//
void DoPageAction(void* pData, const unsigned dwAction)
{
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return;

    switch (dwAction)
    {
        case WEBPAGE_GOBACK:
            pInfo->web_browser->lpVtbl->GoBack(pInfo->web_browser);
            break;
        case WEBPAGE_GOFORWARD:
            pInfo->web_browser->lpVtbl->GoForward(pInfo->web_browser);
            break;
        case WEBPAGE_GOHOME:
            pInfo->web_browser->lpVtbl->GoHome(pInfo->web_browser);
            break;
        case WEBPAGE_SEARCH:
            pInfo->web_browser->lpVtbl->GoSearch(pInfo->web_browser);
            break;
        case WEBPAGE_REFRESH:
            pInfo->web_browser->lpVtbl->Refresh(pInfo->web_browser);
            break;
        case WEBPAGE_STOP:
            pInfo->web_browser->lpVtbl->Stop(pInfo->web_browser);
            break;

        default :
            break;
    }
}


// Set the show address bar attribute
void SetShowAddrBar(void* pData, const unsigned toset)
{
    VARIANT_BOOL Res = toset ? VARIANT_TRUE : VARIANT_FALSE;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return;

    pInfo->web_browser->lpVtbl->put_AddressBar(pInfo->web_browser, Res);
}


// Set the show tool bar attribute
void SetShowToolBar(void* pData, const unsigned toset)
{
    int Res = toset ? TRUE : FALSE;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return;

    pInfo->web_browser->lpVtbl->put_ToolBar(pInfo->web_browser, Res);
}


// Set the silent mode attribute on our browser object
void SetSilentMode(void* pData, const unsigned toset)
{
    VARIANT_BOOL Res;
    TBrwsInfo *pInfo = (TBrwsInfo*)pData;
    if (!pInfo || !pInfo->web_browser)
        return;

    Res = toset ? VARIANT_TRUE : VARIANT_FALSE;
    pInfo->web_browser->lpVtbl->put_Silent(pInfo->web_browser, Res);
}




//
//  The window calls us here when he is cleaning up, if he current has embedding
//  data, to have us unembed. We delete his passed data and set his pointer to
//  null to let him know he is now unembedded.
//
void UnEmbedBrowserObject(void** ppData, struct HWND__* hwndParent)
{
    HRESULT hres;

    //
    //  Get the pointer our and then clear the caller's pointer, which tells him
    //  he no longer has one.
    //
    TBrwsInfo* pInfo = (TBrwsInfo*)*ppData;
    *ppData = 0;

    IOleClientSiteImpl* pSiteImpl = impl_from_IOleClientSite(pInfo->client_site);

    if (pInfo->web_browser)
    {
        // Try to stop any outstanding operation and wait till its done
        WaitNotBusy(pInfo->web_browser);

        // Do a close on it
        pInfo->web_browser->lpVtbl->ExecWB
        (
            pInfo->web_browser, OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0
        );

        PumpMsgs();

        // Hide the browser
        pInfo->web_browser->lpVtbl->put_Visible(pInfo->web_browser, VARIANT_FALSE);

        // Now we can release it
        pInfo->web_browser->lpVtbl->Release(pInfo->web_browser);
    }

    if (pInfo->client_site)
    {
        IOleInPlaceSite* inplace = 0;
        hres = pInfo->client_site->lpVtbl->QueryInterface
        (
            pInfo->client_site, &IID_IOleInPlaceSite, (void**)&inplace
        );

        if(SUCCEEDED(hres))
        {
            inplace->lpVtbl->OnInPlaceDeactivate(inplace);
            inplace->lpVtbl->Release(inplace);

            PumpMsgs();
        }
    }

    if(pInfo->wb_object)
    {
        pInfo->wb_object->lpVtbl->DoVerb
        (
            pInfo->wb_object, OLEIVERB_HIDE, NULL, pInfo->client_site, 0, hwndParent, NULL
        );

        pInfo->wb_object->lpVtbl->Close(pInfo->wb_object, OLECLOSE_NOSAVE);

        PumpMsgs();

        OleSetContainedObject((struct IUnknown *)pInfo->wb_object, FALSE);

        PumpMsgs();

        pInfo->wb_object->lpVtbl->SetClientSite(pInfo->wb_object, NULL);
        CoDisconnectObject((struct IUnknown *)pInfo->wb_object, 0);

        PumpMsgs();

        pInfo->wb_object->lpVtbl->Release(pInfo->wb_object);
        pInfo->wb_object = NULL;

        PumpMsgs();
    }

    pInfo->client_site->lpVtbl->Release(pInfo->client_site);

    PumpMsgs();

    // Free the memory now
    GlobalFree(pInfo);
}


