//
// FILE NAME: CtrlsWnd_DragDropIntf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/16/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the guts of the drag and drop interface. Windows mix this
//  in and it allows them to accept dropped files. That's all we currently support,
//  the dropping of files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"


// ---------------------------------------------------------------------------
//  We have to derive our own drop target COM class.
// ---------------------------------------------------------------------------
class CDropTarget : public IDropTarget
{
    public:
        // -------------------------------------------------------------------
        //  Constructor and destructor
        // -------------------------------------------------------------------
        CDropTarget(MDragAndDrop* pmdadTarget) :

            m_scntRef(1)
            , m_bAllowDrop(FALSE)
            , m_colExts(73, TStringKeyOps(kCIDLib::False))
            , m_pmdadTarget(pmdadTarget)
        {
        }

        ~CDropTarget()
        {
        }


        // -------------------------------------------------------------------
        // IUnknown implementation
        // -------------------------------------------------------------------
        HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject) final
        {
            HRESULT hRes = S_OK;

            if(NULL == ppvObject)
                hRes = E_POINTER;
            else
                *ppvObject = NULL;

            if(SUCCEEDED(hRes))
            {
                if (IsEqualIID(iid, IID_IUnknown)
                ||  IsEqualIID(iid, IID_IDropTarget))
                {
                    *ppvObject = static_cast<IDropTarget*>(this);
                    AddRef();
                }
                 else
                {
                    hRes = E_NOINTERFACE;
                }
            }
            return hRes;
        }

        ULONG __stdcall AddRef(void) final
        {
            return ++m_scntRef;
        }

        ULONG __stdcall Release(void) final
        {
            CIDAssert(m_scntRef.c4Value() > 0, L"DragDrop COM object ref count underflow");

            // Pre-decrement so we get zero if it's currently one
            ULONG uRet = --m_scntRef;
            if (!uRet)
                delete this;
            return uRet;
        }


        // -------------------------------------------------------------------
        // IDropTarget implementation
        // -------------------------------------------------------------------
        HRESULT __stdcall DragEnter
        (
            IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect
        )   final;

        HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)  final;

        HRESULT __stdcall DragLeave(void)  final;

        HRESULT __stdcall Drop
        (
            IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect
        )   final;


    private:
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        bool bQueryFileList
        (
                    IDataObject*            pDataObject
            ,       tCIDLib::TStrCollect&   colToFill
            ,       tCIDLib::TStrHashSet&   colExts
        );

        DWORD dwQueryEffect(DWORD grfKeyState, DWORD dwAllowed);


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bAllowDrop;
        tCIDLib::TStrList       m_colFiles;
        tCIDLib::TStrHashSet    m_colExts;
        MDragAndDrop*           m_pmdadTarget;
        TSafeCard4Counter       m_scntRef;
};


//
//  We first see if there's any acceptable format availab.e If not, we set the
//  allow flag to false, which will cause us to ignore everything else for this
//  particular pass. Else, we set it true and then see if the target window will
//  accept at this point or not and set the effect accordingly.
//
HRESULT __stdcall
CDropTarget::DragEnter( IDataObject*    pDataObject
                        , DWORD         grfKeyState
                        , POINTL        pt
                        , DWORD*        pdwEffect)
{
    // See if it provides a file list format
    FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    m_bAllowDrop = (pDataObject->QueryGetData(&fmtetc) == S_OK);
    if (m_bAllowDrop)
    {
        // If we have a file list, query the files available
        m_bAllowDrop = bQueryFileList(pDataObject, m_colFiles, m_colExts);
        if (m_bAllowDrop)
        {
            //
            //  We got some files, so see if the target can accept any of them. If
            //  not, no need to do any more this time around.
            //
            m_bAllowDrop = m_pmdadTarget->bCanAcceptFiles(m_colFiles, m_colExts);

            //
            //  If so, then let's provisionally allow the drop. So we need to do the
            //  initial drop feedback test and let the target window know we are now
            //  inside his window.
            //
            if (m_bAllowDrop)
            {
                TPoint pntAt(pt.x, pt.y);
                if (m_pmdadTarget->bDragEnter(pntAt, m_colFiles, m_colExts))
                    *pdwEffect = dwQueryEffect(grfKeyState, *pdwEffect);
                else
                    *pdwEffect = DROPEFFECT_NONE;
            }
        }
    }

    // If not allowing the drop, set the effect to none
    if (!m_bAllowDrop)
        *pdwEffect = DROPEFFECT_NONE;

    return S_OK;
}


//
//  Let the windwo know it's exited and then just say all is fine
//
HRESULT __stdcall CDropTarget::DragLeave(void)
{
    try
    {
        // Clear the file/extension lists just in case
        m_colFiles.RemoveAll();
        m_colExts.RemoveAll();

        m_pmdadTarget->DragExit();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }

    return S_OK;
}


//
//  If we are allowing the drop at all, then see if we can accept it at the new
//  point being reported and set the effect accordingly.
//
HRESULT __stdcall CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (m_bAllowDrop)
    {
        TPoint pntAt(pt.x, pt.y);
        try
        {
            if (m_pmdadTarget->bDragOver(pntAt, m_colFiles, m_colExts))
                *pdwEffect = dwQueryEffect(grfKeyState, *pdwEffect);
            else
                *pdwEffect = DROPEFFECT_NONE;
        }

        catch(...)
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
     else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }
    return S_OK;
}


//
//  If we provisionally allowed the drop, then let's let the target know that the
//  drop occurred, where and passing along the files to process.
//
HRESULT __stdcall
CDropTarget::Drop(  IDataObject*    pDataObject
                    , DWORD         grfKeyState
                    , POINTL        pt
                    , DWORD*        pdwEffect)
{
    if(m_bAllowDrop)
    {
        TPoint pntAt(pt.x, pt.y);
        try
        {
            if (m_pmdadTarget->bDragDropped(pntAt, m_colFiles, m_colExts))
                *pdwEffect = dwQueryEffect(grfKeyState, *pdwEffect);
            else
                *pdwEffect = DROPEFFECT_NONE;
        }

        catch(...)
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
     else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }
    return S_OK;
}


//
//  Combine the incoming allowed effect with any that we think should be set based
//  on key state and/or defaults from the source.
//
DWORD CDropTarget::dwQueryEffect(DWORD grfKeyState, DWORD dwAllowed)
{
    DWORD dwEffect = 0;

    // See if there's a requested mode based on shift state
    if (grfKeyState & MK_CONTROL)
        dwEffect = dwAllowed & DROPEFFECT_COPY;
    else if (grfKeyState & MK_SHIFT)
        dwEffect = dwAllowed & DROPEFFECT_MOVE;

    // If not, take a default from the source
    if(dwEffect == 0)
    {
        if (dwAllowed & DROPEFFECT_COPY)
            dwEffect = DROPEFFECT_COPY;
        else if (dwAllowed & DROPEFFECT_MOVE)
            dwEffect = DROPEFFECT_MOVE;
    }
    return dwEffect;
}


//
//  We have already checked that the data object contains a file list, so we query
//  the list of files and put them into the caller's string collection. We also
//  pull out each one's extension and fill in a list of unique extensions, which is
//  often of more use to the target handler than the file names, in terms of deciding
//  if any of the files are acceptable.
//
bool
CDropTarget::bQueryFileList(IDataObject*            pDataObject
                            , tCIDLib::TStrCollect& colFiles
                            , tCIDLib::TStrHashSet& colExts)
{
    // Clear the lists out to start so they are empty if we load nothing
    colFiles.RemoveAll();
    colExts.RemoveAll();

    // See if it has an HDROP type format available. If not, return false now
    FORMATETC Fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM Stgm;
    if (FAILED(pDataObject->GetData(&Fmte, &Stgm)))
    {
        return kCIDLib::False;
    }

    // Cast to the drop handle (really just a structure pointer)
    HDROP hDrop = reinterpret_cast<HDROP>(Stgm.hGlobal);

    // And iterate it to get all of the names and extensions out
    const tCIDLib::TCard4 c4Count = ::DragQueryFile(hDrop, kCIDLib::c4MaxCard, 0, 0);
    if (c4Count)
    {
        tCIDLib::TBoolean   bAdded;
        TPathStr            pathCur;
        TString             strExt;

        // Create a worst case path buffer
        const tCIDLib::TCard4 c4BufSz = kCIDLib::c4MaxPathLen * 2;
        tCIDLib::TCh achBuf[c4BufSz + 1];

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (::DragQueryFileW(hDrop, c4Index, achBuf, c4BufSz))
            {
                pathCur = achBuf;
                colFiles.objAdd(achBuf);

                if (pathCur.bQueryExt(strExt))
                    colExts.objAddIfNew(strExt, bAdded);
            }
        }
    }

    // If we got any return true
    return !colFiles.bIsEmpty();
}




// ---------------------------------------------------------------------------
//  The internal format of our opaque data pointer
// ---------------------------------------------------------------------------
struct TDropData
{
    CDropTarget*    pDropTarget;
};



// ---------------------------------------------------------------------------
//  CLASS: MDragAndDrop
// PREFIX: dad
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MDragAndDrop: Constructors and Destructor
// ---------------------------------------------------------------------------
MDragAndDrop::~MDragAndDrop()
{
    // They have to call TermDragAndDrop() themselves
    CIDAssert(m_pData == nullptr, L"The drag and drop interface was not terminated");
}


// ---------------------------------------------------------------------------
//  MDragAndDrop: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The window that mixes in this interface should call this in his bCreated() method
//  to get ready for accepting drops.
//
tCIDLib::TVoid
MDragAndDrop::InitDragAndDrop(TWindow& wndCont)
{
    // Clean up any existing stuff
    if (m_pData)
        TermDragAndDrop(wndCont);

    // Create a new data structure and a new drop target
    TDropData* pDropData = new TDropData{0};
    m_pData = pDropData;
    pDropData->pDropTarget = new CDropTarget(this);


    // And register our target
    HRESULT hRes = ::RegisterDragDrop(wndCont.hwndThis(), pDropData->pDropTarget);
    if (hRes != S_OK)
    {
        // What can we do here?
    }
}


tCIDLib::TVoid MDragAndDrop::TermDragAndDrop(TWindow& wndCont)
{
    if (m_pData)
    {
        //
        //  Not that this will release the system's ref to our pDropTarget object
        //  but we still have a ref ourself and will release it below if the object
        //  got created.
        //
        ::RevokeDragDrop(wndCont.hwndThis());

        // Get a cast pointer to our data and then clear the member
        TDropData* pData = static_cast<TDropData*>(m_pData);
        TJanitor<TDropData> janData(pData);
        m_pData = nullptr;

        // If we created the drop target COM object, release it
        if (pData->pDropTarget)
        {
            pData->pDropTarget->Release();
            pData->pDropTarget = nullptr;
        }
    }
}


// ---------------------------------------------------------------------------
//  MDragAndDrop: Hidden Constructors
// ---------------------------------------------------------------------------
MDragAndDrop::MDragAndDrop() :

    m_pData(nullptr)
{
}


