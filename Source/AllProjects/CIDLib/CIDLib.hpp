//
// FILE NAME: CIDLib.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get all that we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDLIB)
#define CIDLIBEXP   CID_DLLEXPORT
#else
#define CIDLIBEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDKernel.hpp"


// ---------------------------------------------------------------------------
//  Some needed forward references to deal with bootstrapping issues at this level
// ---------------------------------------------------------------------------
class MFormattable;
class TString;


// ---------------------------------------------------------------------------
//  Now subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDLib_Constants.hpp"
#include    "CIDLib_RTTI.hpp"
#include    "CIDLib_Duplicable.hpp"
#include    "CIDLib_Formattable.hpp"
#include    "CIDLib_RefCounted.hpp"
#include    "CIDLib_Streamable.hpp"
#include    "CIDLib_Lockable.hpp"
#include    "CIDLib_Type.hpp"

//
//  These may need to be seen by templated stuff below, and don't need to see any
//  streams headers, so we can put them up here.
//
StdEnumTricks(tCIDLib::ECorners)
StdEnumTricks(tCIDLib::ELogFlags)

#include    "CIDLib_Object.hpp"
#include    "CIDLib_Class.hpp"
#include    "CIDLib_ErrorIds.hpp"
#include    "CIDLib_MessageIds.hpp"
#include    "CIDLib_String.hpp"
#include    "CIDLib_StringId.hpp"
#include    "CIDLib_ResourceName.hpp"
#include    "CIDLib_CriticalSection.hpp"
#include    "CIDLib_Event.hpp"
#include    "CIDLib_Mutex.hpp"
#include    "CIDLib_ModuleInfo.hpp"
#include    "CIDLib_Module.hpp"
#include    "CIDLib_KeyValuePair.hpp"
#include    "CIDLib_Atomic.hpp"
#include    "CIDLib_SmartPointer.hpp"
#include    "CIDLib_ObjLocker.hpp"
#include    "CIDLib_SearchNSort.hpp"
#include    "CIDLib_PubSub.hpp"
#include    "CIDLib_ExpByteBuf.hpp"
#include    "CIDLib_TextConverter.hpp"
#include    "CIDLib_UTFConverter.hpp"

#include    "CIDLib_DLinkedList.hpp"
#include    "CIDLib_SLinkedList.hpp"
#include    "CIDLib_CollectionBase.hpp"
#include    "CIDLib_CollectCursor.hpp"
#include    "CIDLib_Collection.hpp"

#include    "CIDLib_CircularBuf.hpp"
#include    "CIDLib_FundArray.hpp"
#include    "CIDLib_FundDeque.hpp"
#include    "CIDLib_FundQueue.hpp"
#include    "CIDLib_FundStack.hpp"
#include    "CIDLib_FundVector.hpp"

#include    "CIDLib_BinInStream.hpp"
#include    "CIDLib_BinOutStream.hpp"


// ---------------------------------------------------------------------------
//  Because of the special nature of this facility at the base of the system, order of
//  inclusion problems arise and we don't have access to the IDL language down here.
//  So we manually do some enum streaming macros for some of the basic enums (most of
//  which come from the CIDKernel facility.)
// ---------------------------------------------------------------------------
#if !defined(CIDLIB_NOCLASSES)

EnumStreamMacros(tCIDLib::EAngleTypes)
EnumStreamMacros(tCIDLib::EAudioCues)
EnumStreamMacros(tCIDLib::EAutoModes)
EnumStreamMacros(tCIDLib::ECreateActs)
EnumStreamMacros(tCIDLib::EDelModes)
EnumStreamMacros(tCIDLib::EEndianModes)
EnumStreamMacros(tCIDLib::EEnds)
EnumStreamMacros(tCIDLib::EErrClasses)
EnumStreamMacros(tCIDLib::EEventStates)
EnumStreamMacros(tCIDLib::EExitCodes)
EnumStreamMacros(tCIDLib::EHJustify)
EnumStreamMacros(tCIDLib::EHVOrients)
EnumStreamMacros(tCIDLib::ELanguages)
EnumStreamMacros(tCIDLib::ELockStates)
EnumStreamMacros(tCIDLib::ELogMapModes)
EnumStreamMacros(tCIDLib::EMeasures)
EnumStreamMacros(tCIDLib::EMonths)
EnumStreamMacros(tCIDLib::EPathParts)
EnumStreamMacros(tCIDLib::EPressStates)
EnumStreamMacros(tCIDLib::EPrioClasses)
EnumStreamMacros(tCIDLib::EPrioLevels)
EnumStreamMacros(tCIDLib::ERadices)
EnumStreamMacros(tCIDLib::ERectlTypes)
EnumStreamMacros(tCIDLib::ESeverities)
EnumStreamMacros(tCIDLib::EShareStates)
EnumStreamMacros(tCIDLib::ESpecialTimes)
EnumStreamMacros(tCIDLib::ESymmetries)
EnumStreamMacros(tCIDLib::ESysExcepts)
EnumStreamMacros(tCIDLib::ETrailFmts)
EnumStreamMacros(tCIDLib::ETriStates)
EnumStreamMacros(tCIDLib::EVisible)
EnumStreamMacros(tCIDLib::EVJustify)
EnumStreamMacros(tCIDLib::EVolHWTypes)
EnumStreamMacros(tCIDLib::EWeekDays)

EnumBinStreamMacros(tCIDLib::EAdoptOpts)
EnumBinStreamMacros(tCIDLib::EAccessModes)
EnumBinStreamMacros(tCIDLib::EAllocTypes)
EnumBinStreamMacros(tCIDLib::ECSSides)
EnumBinStreamMacros(tCIDLib::EDirs)
EnumBinStreamMacros(tCIDLib::EFileInfoFlags)
EnumBinStreamMacros(tCIDLib::EForceOpts)
EnumBinStreamMacros(tCIDLib::ELoadRes)
EnumBinStreamMacros(tCIDLib::EMemAccFlags)
EnumBinStreamMacros(tCIDLib::EModFlags)
EnumBinStreamMacros(tCIDLib::EModTypes)
EnumBinStreamMacros(tCIDLib::EMTStates)
EnumBinStreamMacros(tCIDLib::ENextPrev)
EnumBinStreamMacros(tCIDLib::EQPrios)
EnumBinStreamMacros(tCIDLib::ERangeStates)
EnumBinStreamMacros(tCIDLib::ERenameRes)
EnumBinStreamMacros(tCIDLib::ESortDirs)
EnumBinStreamMacros(tCIDLib::EStartEnd)
EnumBinStreamMacros(tCIDLib::EStatItemTypes)
EnumBinStreamMacros(tCIDLib::EStdFiles)
EnumBinStreamMacros(tCIDLib::EStripModes)
EnumBinStreamMacros(tCIDLib::EVolumeFlags)

//
//  Do a couple that are really CIDImage, but are defined in the kernel types header since
//  we want folks to be able to use them without bringing in the CIDImage facility. And
//  also one from CIDCtrls.
//
EnumBinStreamMacros(tCIDImage::EBitDepths)
EnumBinStreamMacros(tCIDImage::EPixFmts)
EnumBinStreamMacros(tCIDCtrls::EExtKeys)

#endif

#include    "CIDLib_BasicDLinkedCol.hpp"

#include    "CIDLib_StreamFmt.hpp"
#include    "CIDLib_TextInStream.hpp"
#include    "CIDLib_TextOutStream.hpp"

#include    "CIDLib_StringTokenizer.hpp"
#include    "CIDLib_BasicTreeCol.hpp"

#include    "CIDLib_Enum.hpp"
#include    "CIDLib_StatsCache.hpp"
#include    "CIDLib_Facility.hpp"
#include    "CIDLib_ExpCharBuf.hpp"
#include    "CIDLib_NativeWCConverter.hpp"

#include    "CIDLib_Base64.hpp"
#include    "CIDLib_BitSet.hpp"
#include    "CIDLib_Color.hpp"
#include    "CIDLib_Point.hpp"
#include    "CIDLib_Size.hpp"
#include    "CIDLib_Area.hpp"
#include    "CIDLib_Boolean.hpp"
#include    "CIDLib_Cardinal.hpp"
#include    "CIDLib_Float.hpp"
#include    "CIDLib_Integer.hpp"
#include    "CIDLib_Time.hpp"
#include    "CIDLib_Random.hpp"
#include    "CIDLib_BitSeq.hpp"
#include    "CIDLib_PathString.hpp"
#include    "CIDLib_FileBase.hpp"
#include    "CIDLib_BinaryFile.hpp"
#include    "CIDLib_FileStreamImpl.hpp"
#include    "CIDLib_LogEvent.hpp"
#include    "CIDLib_VolumeInfo.hpp"
#include    "CIDLib_Locale.hpp"
#include    "CIDLib_Logger.hpp"
#include    "CIDLib_LowLevelPopUp.hpp"
#include    "CIDLib_MemBuf.hpp"
#include    "CIDLib_HeapMemBuf.hpp"
#include    "CIDLib_SysMemBuf.hpp"
#include    "CIDLib_MemoryStreamImpl.hpp"
#include    "CIDLib_Money.hpp"
#include    "CIDLib_FileLogger.hpp"
#include    "CIDLib_PopUpLogger.hpp"
#include    "CIDLib_Pointer.hpp"
#include    "CIDLib_Process.hpp"
#include    "CIDLib_SafeCounter.hpp"
#include    "CIDLib_Semaphore.hpp"
#include    "CIDLib_SharedMemory.hpp"
#include    "CIDLib_Signals.hpp"
#include    "CIDLib_StatusController.hpp"
#include    "CIDLib_StatusControllerTTY.hpp"
#include    "CIDLib_StringStreamImpl.hpp"
#include    "CIDLib_TextFileLogger.hpp"
#include    "CIDLib_StdErrLogger.hpp"
#include    "CIDLib_Thread.hpp"
#include    "CIDLib_UserContext.hpp"
#include    "CIDLib_UniqueName.hpp"
#include    "CIDLib_DataSrc.hpp"

#include    "CIDLib_BinaryFileStream.hpp"
#include    "CIDLib_BinaryMBufStream.hpp"
#include    "CIDLib_Console.hpp"
#include    "CIDLib_TextFileStream.hpp"
#include    "CIDLib_TextMBufStream.hpp"
#include    "CIDLib_TextStringStream.hpp"
#include    "CIDLib_PolyStreamer.hpp"
#include    "CIDLib_StreamLexer.hpp"

#include    "CIDLib_BackFileMgr.hpp"

#include    "CIDLib_ThreadWaitList.hpp"
#include    "CIDLib_EventTimer.hpp"
#include    "CIDLib_TypedMBuf.hpp"
#include    "CIDLib_USB.hpp"

#include    "CIDLib_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDLIBEXP TFacCIDLib& facCIDLib();


// ---------------------------------------------------------------------------
//  And now include some more headers. A lot of these are templates, so they must be able
//  to see any other classes that they use. This is why they are stuck at the end.
// ---------------------------------------------------------------------------
#include    "CIDLib_KeyOps.hpp"
#include    "CIDLib_Bag.hpp"
#include    "CIDLib_Deque.hpp"
#include    "CIDLib_NamedValMap.hpp"

#include    "CIDLib_ObjectArray.hpp"
#include    "CIDLib_Queue.hpp"
#include    "CIDLib_Stack.hpp"
#include    "CIDLib_HashSet.hpp"
#include    "CIDLib_KeyedHashSet.hpp"
#include    "CIDLib_HashMap.hpp"
#include    "CIDLib_SortedBag.hpp"
#include    "CIDLib_FindBuf.hpp"
#include    "CIDLib_Vector.hpp"
#include    "CIDLib_FileSystem.hpp"

#include    "CIDLib_BasicDLinkedRefCol.hpp"
#include    "CIDLib_RefBag.hpp"
#include    "CIDLib_RefDeque.hpp"
#include    "CIDLib_RefKeyedHashSet.hpp"
#include    "CIDLib_RefSortedBag.hpp"
#include    "CIDLib_RefQueue.hpp"
#include    "CIDLib_RefStack.hpp"
#include    "CIDLib_RefVector.hpp"

#include    "CIDLib_ColAlgo.hpp"
#include    "CIDLib_SystemInfo.hpp"
#include    "CIDLib_PubSub2.hpp"
#include    "CIDLib_GenCacheItem.hpp"
#include    "CIDLib_ChunkedMemStream.hpp"
#include    "CIDLib_LogLimiter.hpp"
#include    "CIDLib_DirChangeInfo.hpp"
#include    "CIDLib_DirIter.hpp"


// ---------------------------------------------------------------------------
//  Define common collection aliases. We also force most of these to be pre-instantiated
//  as well below.
// ---------------------------------------------------------------------------
namespace tCIDLib
{
    using TKVPTree          = TBasicTreeCol<TKeyValuePair>;
    using TStrArray         = TObjArray<TString>;
    using TStrBag           = TBag<TString>;
    using TStrCollect       = TCollection<TString>;
    using TKVPCollect       = TCollection<TKeyValuePair>;
    using TKVPFCollect      = TCollection<TKeyValFPair>;
    using TKValsCollect     = TCollection<TKeyValues>;
    using TStrList          = TVector<TString>;
    using TStrIdList        = TVector<TStringId>;
    using TKVPList          = TVector<TKeyValuePair>;
    using TKVPFList         = TVector<TKeyValFPair>;
    using TKValsList        = TVector<TKeyValues>;
    using TKNumList         = TVector<TKeyNumPair>;
    using TDirChanges       = TVector<TDirChangeInfo>;
    using TThreadList       = TRefVector<TThread>;
    using TLogEventQ        = TQueue<TLogEvent>;
    using TStringQ          = TQueue<TString>;
    using TKVPairQ          = TQueue<TKeyValuePair>;
    using TBoolArray        = TFundArray<tCIDLib::TBoolean>;
    using TCardArray        = TFundArray<tCIDLib::TCard4>;
    using TFloatArray       = TFundArray<tCIDLib::TFloat8>;
    using TIntArray         = TFundArray<tCIDLib::TInt4>;
    using TBoolList         = TFundVector<tCIDLib::TBoolean>;
    using TByteList         = TFundVector<tCIDLib::TCard1>;
    using TCardList         = TFundVector<tCIDLib::TCard4>;
    using TCard8List        = TFundVector<tCIDLib::TCard8>;
    using TFloat4List       = TFundVector<tCIDLib::TFloat4>;
    using TFloat8List       = TFundVector<tCIDLib::TFloat8>;
    using TChangeList       = TFundVector<tCIDLib::EChangeRes>;
    using TBoolStack        = TFundStack<tCIDLib::TBoolean>;
    using TByteStack        = TFundStack<tCIDLib::TCard1>;
    using TCardStack        = TFundStack<tCIDLib::TCard4>;
    using TKVHashSet        = TKeyedHashSet<TKeyValuePair,TString,TStringKeyOps>;
    using TStrHashSet       = THashSet<TString,TStringKeyOps>;
    using TGenCache         = TKeyedHashSet<TCIDGenCacheItem, TString, TStringKeyOps>;
}


#include    "CIDLib_DirChange.hpp"
#include    "CIDLib_Audio.hpp"
#include    "CIDLib_Speech.hpp"
#include    "CIDLib_TextQStream.hpp"
#include    "CIDLib_Environment.hpp"
#include    "CIDLib_ExternalProcess.hpp"
#include    "CIDLib_UndoCore.hpp"
#include    "CIDLib_FixedSizePool.hpp"
#include    "CIDLib_SimplePool.hpp"



// ---------------------------------------------------------------------------
//  Provide externs for two magic global methods that are used by the magic startup macro
//  CIDLib_MainModule(), which is provided on a per-platform basis.
// ---------------------------------------------------------------------------
extern tCIDLib::TVoid CIDLIBEXP CIDLib_Init();
extern tCIDLib::TVoid CIDLIBEXP CIDLib_MakePrimary(TThread* const);


// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDLIB_PREINST)
extern template class TBasicTreeCol<TKeyValuePair>;

extern template class TFundVector<tCIDLib::TBoolean>;
extern template class TFundVector<tCIDLib::TCard4>;
extern template class TFundVector<tCIDLib::TInt4>;
extern template class TFundVector<tCIDLib::TFloat8>;

extern template class TFundVector<tCIDLib::TBoolean>;
extern template class TFundVector<tCIDLib::TCard1>;
extern template class TFundVector<tCIDLib::TCard4>;
extern template class TFundVector<tCIDLib::TCard8>;
extern template class TFundVector<tCIDLib::TFloat4>;
extern template class TFundVector<tCIDLib::TFloat8>;

extern template class TFundStack<tCIDLib::TBoolean>;
extern template class TFundStack<tCIDLib::TCard1>;
extern template class TFundStack<tCIDLib::TCard4>;

extern template class TObjArray<TString>;

extern template class TBag<TString>;

extern template class TColCursor<TString>;
extern template class TColCursor<TKeyValuePair>;

extern template class TCollection<TString>;
extern template class TCollection<TKeyValuePair>;
extern template class TCollection<TKeyValFPair>;

extern template class TQueue<TString>;
extern template class TQueue<TKeyValuePair>;

extern template class TKeyedHashSet<TKeyValuePair,TString,TStringKeyOps>;
extern template class TKeyedHashSet<TCIDGenCacheItem, TString, TStringKeyOps>;

extern template class THashSet<TString,TStringKeyOps>;

extern template class TVector<TString>;
extern template class TVector<TStringId>;
extern template class TVector<TKeyValuePair>;
extern template class TVector<TKeyValFPair>;
extern template class TVector<TKeyValues>;
extern template class TVector<TKeyNumPair>;
extern template class TVector<TDirChangeInfo>;

#endif

