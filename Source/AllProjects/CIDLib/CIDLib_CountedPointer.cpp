//
// FILE NAME: CIDLib_CountedPointer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/29/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the base (non-templatized) counted pointer class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCntPtrBase,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TCntPtrBase::TCntDataBase
//  PREFIX: cntd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCntPtrBase::TCntDataBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TCntPtrBase::TCntDataBase::TCntDataBase() :

    m_c4Count(0)
{
}

TCntPtrBase::TCntDataBase::~TCntDataBase()
{
}

// ---------------------------------------------------------------------------
//  TCntPtrBase::TCntDataBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCntPtrBase::TCntDataBase::c4RefCount() const
{
    return m_c4Count;
}


tCIDLib::TCard4 TCntPtrBase::TCntDataBase::c4DecCount()
{
    // If an underflow, then log a warning
    if (m_c4Count)
    {
        m_c4Count--;
    }
     else
    {
        // <TBD>
    }

    // And return the new value
    return m_c4Count;
}


tCIDLib::TCard4 TCntPtrBase::TCntDataBase::c4IncCount()
{
    // Bump the count and return the new count
    return ++m_c4Count;
}


TCriticalSection* TCntPtrBase::TCntDataBase::pcrsSync()
{
    return &m_crsSync;
}



// ---------------------------------------------------------------------------
//   CLASS: TCntPtrBase
//  PREFIX: cptr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCntPtrBase: Destructor
// ---------------------------------------------------------------------------
TCntPtrBase::~TCntPtrBase()
{
}


// ---------------------------------------------------------------------------
//  TCntPtrBase: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Provide access to the ref count. Note that we don't need to lock here.
//  We hold a reference to the object so it cannot go away, and we are reading
//  it, not changing the value, and it's a fundamental value that doesn't
//  require multiple CPU ops.
//
tCIDLib::TCard4 TCntPtrBase::c4RefCount() const
{
    return m_pcntdManaged->c4RefCount();
}


// ---------------------------------------------------------------------------
//  TCntPtrBase: Hidden constructors and operators
// ---------------------------------------------------------------------------

//
//  The templatized derived classes must give us a counted data object, even
//  if the pointer is zero.
//
TCntPtrBase::TCntPtrBase(TCntDataBase* const pcntdToAdopt) :

    m_pcntdManaged(pcntdToAdopt)
{
    // Bump the counter
    TCritSecLocker crslSync(m_pcntdManaged->pcrsSync());
    m_pcntdManaged->c4IncCount();
}

TCntPtrBase::TCntPtrBase(const TCntPtrBase& cptrToCopy) :

    m_pcntdManaged(cptrToCopy.m_pcntdManaged)
{
    // Bump the counter on the counted data
    TCritSecLocker crslSync(m_pcntdManaged->pcrsSync());
    m_pcntdManaged->c4IncCount();
}


//
//  We insure the new guy gets accepted first, then we clean up the old data,
//  throwing if there is an error deleting the user data.
//
//  WE MUST bump the incoming first, since it could be the same counted pointer
//  object that we have. We can't check that without locking both. But, to avoid
//  deadlock we CANNOT lock both at the same time.
//
tCIDLib::TVoid TCntPtrBase::operator=(const TCntPtrBase& cptrToAssign)
{
    //
    //  Lock the new guy and increment him and make him our current object. Save a
    //  pointer to our current one before we overwrite it.
    //
    TCntDataBase* pcntdDel = nullptr;
    {
        TCritSecLocker crslThem(cptrToAssign.m_pcntdManaged->pcrsSync());
        cptrToAssign.m_pcntdManaged->c4IncCount();
        pcntdDel = m_pcntdManaged;
        m_pcntdManaged = cptrToAssign.m_pcntdManaged;
    }

    //
    //  We now lock our previous one, and decrement it. If it doesn't go zero, we
    //  zero out the delete pointer so it doesn't get cleaned up. If it does, we
    //  don't need to keep it locked while we clean up since no on can be accessing
    //  it.
    //
    {
        TCritSecLocker crslUs(pcntdDel->pcrsSync());
        if (pcntdDel->c4DecCount())
            pcntdDel = nullptr;
    }

    // Decrement our old pointer and clean up if needed.
    if (pcntdDel)
    {
        if (!pcntdDel->c4DecCount())
        {
            //
            //  Ask the object to clean up the user data first. If this throws
            //  the data pointer will still be zeroed regardless.
            //
            try
            {
                pcntdDel->CleanupData();
            }

            catch(TError& errToCatch)
            {
                delete pcntdDel;
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            delete pcntdDel;
        }
    }
}


// ---------------------------------------------------------------------------
//  TCntPtrBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called when the derived templatized classes destruct. We clean up our
//  reference to the managed data. If we are the last holder, then it's
//  deleted.
//
tCIDLib::TVoid TCntPtrBase::Destructed()
{
    //
    //  Lock and decrement. If it goes zero, save a pointer to the object for
    //  cleanup below. If it does, we don't have to keep it locked since no one
    //  can get to it now. We were the last one to know about it.
    //
    TCntDataBase* pcntdDel = nullptr;
    {
        TCritSecLocker crslUs(m_pcntdManaged->pcrsSync());
        if (!m_pcntdManaged->c4DecCount())
            pcntdDel = m_pcntdManaged;
    }

    if (pcntdDel)
    {
        //
        //  Ask the object to clean up the user data first. If this throws
        //  the data pointer will still be zeroed.
        //
        try
        {
            pcntdDel->CleanupData();
        }

        catch(TError& errToCatch)
        {
            // Don't throw, this is called from the dtor, Just fall through
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            facCIDLib().LogEventObj(errToCatch);
        }

        delete pcntdDel;
    }
}


//
//  WE MUST bump the incoming first, since it could be the same counted pointer
//  object that we have. We can't check that without locking both. But, to avoid
//  deadlock we CANNOT lock both at the same time.
//
tCIDLib::TVoid TCntPtrBase::SetNewPointer(TCntDataBase* const pcntdToAdopt)
{
    //
    //  Lock the new guy and increment him and make him our current object. Save a
    //  pointer to our current one before we overwrite it.
    //
    TCntDataBase* pcntdDel = nullptr;
    {
        TCritSecLocker crslThem(pcntdToAdopt->pcrsSync());
        pcntdToAdopt->c4IncCount();
        pcntdDel = m_pcntdManaged;
        m_pcntdManaged = pcntdToAdopt;
    }

    //
    //  We now lock our previous one, and decrement it. If it doesn't go zero, we
    //  zero out the delete pointer so it doesn't get cleaned up. If it does, we
    //  don't need to keep it locked while we clean up since no on can be accessing
    //  it.
    //
    {
        TCritSecLocker crslUs(pcntdDel->pcrsSync());
        if (pcntdDel->c4DecCount())
            pcntdDel = nullptr;
    }

    // If our old one went zero, clean it up.
    if (pcntdDel)
    {
        //
        //  Ask the old guy to clean up his data. If throws, it'll stil zero
        //  the data pointer so no further attempt will be made to clean it
        //  up.
        //
        try
        {
            pcntdDel->CleanupData();
        }

        catch(TError& errToCatch)
        {
            delete pcntdDel;
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        delete pcntdDel;
    }
}


const TCntPtrBase::TCntDataBase* TCntPtrBase::pcntdThis() const
{
    return m_pcntdManaged;
}

TCntPtrBase::TCntDataBase* TCntPtrBase::pcntdThis()
{
    return m_pcntdManaged;
}


