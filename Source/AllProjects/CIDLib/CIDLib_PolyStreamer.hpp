//
// FILE NAME: CIDLib_PolyStreamer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/1997
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
//  This is the header for the CIDLib_PolyStreamer.Cpp file. This class
//  supports smart polymorphic streaming, in a way that fully minimizes the
//  storage requirements of such storage.
//
//  In order to polymorphically store objects, the type must be stored with
//  the object. In many cases this overhead is trivial and not an issue. But,
//  for large numbers of small objects, this overhead is greater than the
//  overhead of the objects themselves.
//
//  This smart streamer builds a map of type names, associating an identifier
//  with each one. So, it only has to stream out this identifier with each
//  object. The id is 16 bits (to minimize storage), so only a maximum of
//  64K different types can be streamed into a single file using this class.
//
//  The class type information is stored intermixed with the actual data.
//  When a new type is seen, its written out along with its id. When the file
//  is streamed back in, its known that any object's class will be seen before
//  any object of that type is seen. During the streamout, a hashed set
//  collection of classes written is maintained so that it can quickly see
//  whether a class has been written yet or not. During the stream in, it is
//  built up and used to map ids back to class types.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
template <typename TElem, class TKey, class TKeyOps> class TKeyedHashSet;


// ---------------------------------------------------------------------------
//  CLASS: TClassInfo
//
//  This is a simple class that allows us to store a TClass object
//  and an indentifier number that maps to that class.
// ---------------------------------------------------------------------------
class TClassInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TClassInfo() = default;

        TClassInfo(const TClass& clsToStore, const tCIDLib::TCard2 c2Id) :

            m_c2Id(c2Id)
            , m_clsRepresented(clsToStore)
        {
        }

        TClassInfo(const TClassInfo&)  = default;
        TClassInfo(TClassInfo&&)  = default;

        ~TClassInfo() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TClassInfo& operator=(const TClassInfo&) = default;
        TClassInfo& operator=(TClassInfo&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2Id() const
        {
            return m_c2Id;
        }

        const TClass& clsRepresented() const
        {
            return m_clsRepresented;
        }


    private :
        // -------------------------------------------------------------
        //  Private data members
        //
        //  m_c2Id
        //      The id that is stored in the file for this class
        //
        //  m_clsRepresented
        //      This is the class that this object represents
        // -------------------------------------------------------------
        tCIDLib::TCard2 m_c2Id = 0;
        TClass          m_clsRepresented;
};


// ---------------------------------------------------------------------------
//  A key ops object for a TClass object
// ---------------------------------------------------------------------------
class TClassKeyOps
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsGetKey(const TClassInfo& clsiSrc)
        {
            return clsiSrc.clsRepresented();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtaul methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean
        bCompKeys(const TClass& cls1, const TClass& cls2) const
        {
            return (cls1 == cls2);
        }

        tCIDLib::THashVal hshKey(const  TClass&         clsToHash
                                , const tCIDLib::TCard4) const
        {
            return clsToHash.hshInternal();
        }
};



// ---------------------------------------------------------------------------
//  CLASS: TPolyStreamer
// PREFIX: pstmr
// ---------------------------------------------------------------------------
template <typename TElem> class TPolyStreamer : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPolyStreamer(  TBinInStream* const     pstrmIn = nullptr
                        , TBinOutStream* const  pstrmOut = nullptr) :

            m_c2CurId(1)
            , m_pcolClassSet(nullptr)
            , m_pstrmIn(pstrmIn)
            , m_pstrmOut(pstrmOut)
        {
            m_pcolClassSet = new TKeyedHashSet<TClassInfo,TClass,TClassKeyOps>
            (
                kCIDLib::c4ClassModulus
                , TClassKeyOps()
                , &TClassKeyOps::clsGetKey
            );
        }

        TPolyStreamer(const TPolyStreamer<TElem>&) = delete;
        TPolyStreamer(TPolyStreamer<TElem>&&) = delete;

        ~TPolyStreamer()
        {
            delete m_pcolClassSet;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPolyStreamer<TElem>& operator=(const TPolyStreamer<TElem>&) = delete;
        TPolyStreamer<TElem>& operator=(TPolyStreamer<TElem>&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEndOfStream() const
        {
            CheckHaveStream(kCIDLib::True);
            return m_pstrmIn->bEndOfStream();
        }

        tCIDLib::TVoid FullReset()
        {
            m_c2CurId = 1;
            if (m_pstrmIn)
                m_pstrmIn->Reset();
            if (m_pstrmOut)
                m_pstrmOut->Reset();
            m_pcolClassSet->RemoveAll();
        }

        [[nodiscard]] TElem* pobjStreamIn()
        {
            CheckHaveStream(kCIDLib::True);

            //
            //  Stream in the next record type. There could be a class record
            //  before the next object type.
            //
            tCIDLib::TCard1 c1RecordType;
            *m_pstrmIn >> c1RecordType;

            tCIDLib::TBoolean   bNewClass = kCIDLib::False;
            TClass              clsNewType;
            tCIDLib::TCard2     c2NewId = 0;
            if (c1RecordType == static_cast<tCIDLib::TCard1>(0xAC))
            {
                //
                //  Stream in the new record object. First we pull in the new
                //  id and then the class info. We use these to create a new
                //  class info object to add to the collection of classes.
                //
                *m_pstrmIn >> c2NewId;
                clsNewType = m_pstrmIn->clsReadClassInfo();

                // Add this one to the set if its not already
                bNewClass = bCheckOrAdd(clsNewType, c2NewId);

                // Now get the next record type
                *m_pstrmIn >> c1RecordType;
            }

            // Make sure its a legal record type
            if (c1RecordType != static_cast<tCIDLib::TCard1>(0xEA))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStrm_BadRecordType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(c1RecordType)
                );
            }

            //
            //  Read in the next id. This will be the id of the next object. We
            //  have to map it to a class.
            //
            tCIDLib::TCard2 c2NextId;
            *m_pstrmIn >> c2NextId;

            //
            //  If we just loaded a new class, then it has to be that this object
            //  is of that type. So make sure.
            //
            if (bNewClass && (c2NextId != c2NewId))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStrm_NotOfLastId
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(c2NextId)
                    , TCardinal(c2NewId)
                    , clsNewType
                );
            }

            // If we did not just load a class, then we need to look up this id
            if (!bNewClass)
                FindClassById(c2NextId, clsNewType);

            TObject* const      pobjNew = clsNewType.pobjMakeNew();
            TJanitor<TObject>   janNewObj(pobjNew);

            //
            //  Make sure that the new type has a valid relationship with the
            //  type of the pointer we have to fill in. The class type we read
            //  in must be a class of or derived from the instantiation type.
            //
            TBinInStream::CheckRelationship(pobjNew, TElem::clsThis());

            // Its ok so we can cast our pointer safely
            TElem* pobjTyped = static_cast<TElem*>(pobjNew);

            //
            //  And now let it stream itself in. If this class does not support
            //  the streaming API, it will be caught here at compile time.
            //
            *m_pstrmIn >> *pobjTyped;

            // Orphan the new object and give back the pointer
            janNewObj.Orphan();
            return pobjTyped;
        }

        tCIDLib::TVoid ResetIn()
        {
            CheckHaveStream(kCIDLib::True);
            m_pstrmIn->Reset();
        }

        tCIDLib::TVoid ResetOut()
        {
            CheckHaveStream(kCIDLib::False);
            m_pstrmOut->Reset();
        }

        tCIDLib::TVoid StreamOut(const TElem& objToStream)
        {
            CheckHaveStream(kCIDLib::False);

            //
            //  See if this object's type has been streamed out yet. If its a
            //  new class, then we stream out the class record first.
            //
            tCIDLib::TCard2 c2ThisId = 0;
            const TClass&   clsThisObj = objToStream.clsIsA();

            if (bCheckOrAdd(clsThisObj, c2ThisId))
            {
                // Stream out the class record id
                *m_pstrmOut << static_cast<tCIDLib::TCard1>(0xAC);

                // Now stream out the id for this class and the class itself
                *m_pstrmOut << c2ThisId;
                m_pstrmOut->WriteClassInfo(clsThisObj);
            }

            // Now write out the record id and the id of this type
            *m_pstrmOut << static_cast<tCIDLib::TCard1>(0xEA);
            *m_pstrmOut << c2ThisId;

            // And finally stream out the object itself
            *m_pstrmOut << objToStream << kCIDLib::FlushIt;
        }


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TClassList = TKeyedHashSet<TClassInfo,TClass,TClassKeyOps>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckOrAdd(  const   TClass&             clsNewType
                                        ,       tCIDLib::TCard2&    c2NewId)
        {
            //
            //  See if this key is in the list. If so, then we've got it so
            //  just return false now and fill in the id.
            //
            TClassInfo* pclsiFound = m_pcolClassSet->pobjFindByKey(clsNewType);
            if (pclsiFound)
            {
                c2NewId = pclsiFound->c2Id();
                return kCIDLib::False;
            }

            //
            //  Create a candidate entry with the next available id, then ask
            //  the hash set to add to add it.
            //
            c2NewId = m_c2CurId++;
            TClassInfo clsiTmp(clsNewType, c2NewId);
            m_pcolClassSet->objAdd(clsiTmp);

            return kCIDLib::True;
        }

        tCIDLib::TVoid CheckHaveStream(const tCIDLib::TBoolean bInput) const
        {
            if ((bInput && !m_pstrmIn) || (!bInput && !m_pstrmOut))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStrm_DontHaveStream
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TString(bInput ? L"input" : L"output")
                );
            }
        }

        tCIDLib::TVoid FindClassById(const  tCIDLib::TCard2 c2NextId
                                    ,       TClass&         clsNewType)
        {
            TClassList::TCursor cursFind(m_pcolClassSet);
            while (cursFind.bIsValid())
            {
                const TClassInfo& clsiCur = cursFind.objRCur();
                if (c2NextId == clsiCur.c2Id())
                {
                    clsNewType = clsiCur.clsRepresented();
                    return;
                }
                ++cursFind;
            }

            // This should never happen
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_TypeIdNotInList
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c2NextId)
            );
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2CurId
        //      This is the current id. Its bumped up for each new class. It
        //      starts at 1 so 0 is an invalid value.
        //
        //  m_pcolClassSet
        //      This is the hash set of TClassInfo objects that is used to
        //      track what types have been streamed in our out. This is a
        //      nested class that contains a class and an id.
        //
        //  m_pstrmIn
        //  m_pstrmOut
        //      These are pointers to the streams to use for input and output.
        //      We do not adopt them, just reference them. In general they should
        //      be set to the default platform endianness.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2CurId;
        TClassList*         m_pcolClassSet;
        TBinInStream*       m_pstrmIn;
        TBinOutStream*      m_pstrmOut;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TPolyStreamer<TElem>, TObject)
};

#pragma CIDLIB_POPPACK

