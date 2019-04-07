//
// FILE NAME: CIDMath_GraphData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/2011
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
//  This is the header for the CIDMath_GraphData.cpp file. It provides the
//  TGraphData class. This class provides the kind of functionality that a
//  graphing program would tend to need. The ability to keep a list of
//  samples where you can push new ones in on one end and old ones get
//  pushed out the other end. And where you can read out X number of samples
//  from the new end back.
//
//  It also provides the functionality to scale the sample points to a set
//  of X/Y point coordinates within a given width/height, which will be used
//  by graph display programs to scale the available points to available
//  pixels. The vertical axis can be linear or logarithmic.
//
//  For various reasons it also maintains a 'serial number', which it bumps
//  by 1 for every new sample added. This helps the outside world know how
//  many samples have been added since the last time they checked, since
//  this guy gives back the current serial number when you ask for new
//  samples. See the Gotchas section below for more info, #2.
//
//  The caller must indicate the number of samples we will have to deal
//  with. Once we have that many samples, we start pushing old ones out.
//
//  When we give back new samples, they are in the display order, i.e. the
//  oldest first (on the left) and the newest on the right. This is the
//  natural order for a graph display.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  We implement a circular buffer in order to make this class efficient.
//      To make it easy to distinguish between a full and empty list, we
//      allocate one extra value, so that there is always a gap between the
//      head and tail, i.e. they are only equal when the list is empty, not
//      when it is full.
//
//      We are always adding at the tail and removing from the head, so the
//      data is stored with oldest to the 'left' and newest to the 'right',
//      which is how it will be displayed in a left scrolling graph.
//
//  2)  When using a graph data object as the source to push new samples, you
//      may have gotten them from somewhere, and you want to keep the serial
//      number you got from that source. But it may not be true if you are
//      the collector of the samples. So the push multiple samples methods both
//      allow you to indicate whether to take a new serial number, or to just
//      bump this object's own serial number for each new sample. In one it's
//      a boolean flag. In the other if you pass non-zero it takes that as a
//      new serial number to store, else a zero means just bump our own.
//
//      Going the other way, it's always assumed you want the new serial number
//      and that you pass it back in again next time. So in the query samples
//      method that takes a graph data object as the target, it will be updated
//      with the current serial number of this object. In the one that takes a
//      vector there's a separate in/out serial number parameter.
//
//      When pushing in single samples, it's always assumed that this object
//      is being used by the collector of samples and that this object's serial
//      number should be incremented for the new sample.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TGraphData
//  PREFIX: grdat
// ---------------------------------------------------------------------------
class CIDMATHEXP TGraphData : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphData() = delete;

        TGraphData
        (
            const   tCIDLib::TCard4         c4MaxSamples
            , const tCIDLib::TCard4         c4InitSerialNum = 0
        );

        TGraphData
        (
            const   TGraphData&             grdatToCopy
        );

        ~TGraphData();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsFull() const;

        tCIDLib::TBoolean bQuerySamples
        (
                    tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TFloat4List&   fcolToFill
            ,       tCIDLib::TCard4&        c4NewSamples
        )   const;

        tCIDLib::TBoolean bQuerySamples
        (
                    TGraphData&             grdatTar
            ,       tCIDLib::TCard4&        c4NewSamples
        )   const;

        tCIDLib::TCard4 c4MaxSamples() const;

        tCIDLib::TCard4 c4SamplesAvail() const;

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TFloat4 f4SampleAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid PushNewSample
        (
            const   tCIDLib::TFloat4        f4ToPush
        );

        tCIDLib::TVoid PushNewSamples
        (
            const   tCIDLib::TFloat4List&   fcolNew
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4SerialNum
        );

        tCIDLib::TVoid PushNewSamples
        (
            const   TGraphData&             grdatSrc
            , const tCIDLib::TBoolean       bTakeSerialNum
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4NewSerialNum = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TFloat4        f4FillWith
            , const tCIDLib::TCard4         c4NewSerialNum = kCIDLib::c4MaxCard
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MaxSamples;
        tCIDLib::TCard4     m_c4SerialNum;
        tCIDLib::TCard4     m_c4Head;
        tCIDLib::TCard4     m_c4Tail;
        tCIDLib::TFloat4*   m_pf4Data;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRange,TObject)
};

#pragma CIDLIB_POPPACK


