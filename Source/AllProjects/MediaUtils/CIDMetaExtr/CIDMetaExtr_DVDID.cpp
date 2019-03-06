//
// FILE NAME: CIDMetaExtr_DVDID.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the base meta extractor interface.
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
#include    "CIDMetaExtr_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDDVDID,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMetaExtr_DVDID
{
    // A lookup table with the CRC replacement values
    const tCIDLib::TCard8 ac8table[] =
    {
        0x0000000000000000, 0x0809E8A2969451E9, 0x1013D1452D28A3D2, 0x181A39E7BBBCF23B
      , 0x2027A28A5A5147A4, 0x282E4A28CCC5164D, 0x303473CF7779E476, 0x383D9B6DE1EDB59F
      , 0x404F4514B4A28F48, 0x4846ADB62236DEA1, 0x505C9451998A2C9A, 0x58557CF30F1E7D73
      , 0x6068E79EEEF3C8EC, 0x68610F3C78679905, 0x707B36DBC3DB6B3E, 0x7872DE79554F3AD7
      , 0x809E8A2969451E90, 0x8897628BFFD14F79, 0x908D5B6C446DBD42, 0x9884B3CED2F9ECAB
      , 0xA0B928A333145934, 0xA8B0C001A58008DD, 0xB0AAF9E61E3CFAE6, 0xB8A3114488A8AB0F
      , 0xC0D1CF3DDDE791D8, 0xC8D8279F4B73C031, 0xD0C21E78F0CF320A, 0xD8CBF6DA665B63E3
      , 0xE0F66DB787B6D67C, 0xE8FF851511228795, 0xF0E5BCF2AA9E75AE, 0xF8EC54503C0A2447
      , 0x24B1909974C84E69, 0x2CB8783BE25C1F80, 0x34A241DC59E0EDBB, 0x3CABA97ECF74BC52
      , 0x049632132E9909CD, 0x0C9FDAB1B80D5824, 0x1485E35603B1AA1F, 0x1C8C0BF49525FBF6
      , 0x64FED58DC06AC121, 0x6CF73D2F56FE90C8, 0x74ED04C8ED4262F3, 0x7CE4EC6A7BD6331A
      , 0x44D977079A3B8685, 0x4CD09FA50CAFD76C, 0x54CAA642B7132557, 0x5CC34EE0218774BE
      , 0xA42F1AB01D8D50F9, 0xAC26F2128B190110, 0xB43CCBF530A5F32B, 0xBC352357A631A2C2
      , 0x8408B83A47DC175D, 0x8C015098D14846B4, 0x941B697F6AF4B48F, 0x9C1281DDFC60E566
      , 0xE4605FA4A92FDFB1, 0xEC69B7063FBB8E58, 0xF4738EE184077C63, 0xFC7A664312932D8A
      , 0xC447FD2EF37E9815, 0xCC4E158C65EAC9FC, 0xD4542C6BDE563BC7, 0xDC5DC4C948C26A2E
      , 0x49632132E9909CD2, 0x416AC9907F04CD3B, 0x5970F077C4B83F00, 0x517918D5522C6EE9
      , 0x694483B8B3C1DB76, 0x614D6B1A25558A9F, 0x795752FD9EE978A4, 0x715EBA5F087D294D
      , 0x092C64265D32139A, 0x01258C84CBA64273, 0x193FB563701AB048, 0x11365DC1E68EE1A1
      , 0x290BC6AC0763543E, 0x21022E0E91F705D7, 0x391817E92A4BF7EC, 0x3111FF4BBCDFA605
      , 0xC9FDAB1B80D58242, 0xC1F443B91641D3AB, 0xD9EE7A5EADFD2190, 0xD1E792FC3B697079
      , 0xE9DA0991DA84C5E6, 0xE1D3E1334C10940F, 0xF9C9D8D4F7AC6634, 0xF1C03076613837DD
      , 0x89B2EE0F34770D0A, 0x81BB06ADA2E35CE3, 0x99A13F4A195FAED8, 0x91A8D7E88FCBFF31
      , 0xA9954C856E264AAE, 0xA19CA427F8B21B47, 0xB9869DC0430EE97C, 0xB18F7562D59AB895
      , 0x6DD2B1AB9D58D2BB, 0x65DB59090BCC8352, 0x7DC160EEB0707169, 0x75C8884C26E42080
      , 0x4DF51321C709951F, 0x45FCFB83519DC4F6, 0x5DE6C264EA2136CD, 0x55EF2AC67CB56724
      , 0x2D9DF4BF29FA5DF3, 0x25941C1DBF6E0C1A, 0x3D8E25FA04D2FE21, 0x3587CD589246AFC8
      , 0x0DBA563573AB1A57, 0x05B3BE97E53F4BBE, 0x1DA987705E83B985, 0x15A06FD2C817E86C
      , 0xED4C3B82F41DCC2B, 0xE545D32062899DC2, 0xFD5FEAC7D9356FF9, 0xF55602654FA13E10
      , 0xCD6B9908AE4C8B8F, 0xC56271AA38D8DA66, 0xDD78484D8364285D, 0xD571A0EF15F079B4
      , 0xAD037E9640BF4363, 0xA50A9634D62B128A, 0xBD10AFD36D97E0B1, 0xB5194771FB03B158
      , 0x8D24DC1C1AEE04C7, 0x852D34BE8C7A552E, 0x9D370D5937C6A715, 0x953EE5FBA152F6FC
      , 0x92C64265D32139A4, 0x9ACFAAC745B5684D, 0x82D59320FE099A76, 0x8ADC7B82689DCB9F
      , 0xB2E1E0EF89707E00, 0xBAE8084D1FE42FE9, 0xA2F231AAA458DDD2, 0xAAFBD90832CC8C3B
      , 0xD28907716783B6EC, 0xDA80EFD3F117E705, 0xC29AD6344AAB153E, 0xCA933E96DC3F44D7
      , 0xF2AEA5FB3DD2F148, 0xFAA74D59AB46A0A1, 0xE2BD74BE10FA529A, 0xEAB49C1C866E0373
      , 0x1258C84CBA642734, 0x1A5120EE2CF076DD, 0x024B1909974C84E6, 0x0A42F1AB01D8D50F
      , 0x327F6AC6E0356090, 0x3A76826476A13179, 0x226CBB83CD1DC342, 0x2A6553215B8992AB
      , 0x52178D580EC6A87C, 0x5A1E65FA9852F995, 0x42045C1D23EE0BAE, 0x4A0DB4BFB57A5A47
      , 0x72302FD25497EFD8, 0x7A39C770C203BE31, 0x6223FE9779BF4C0A, 0x6A2A1635EF2B1DE3
      , 0xB677D2FCA7E977CD, 0xBE7E3A5E317D2624, 0xA66403B98AC1D41F, 0xAE6DEB1B1C5585F6
      , 0x96507076FDB83069, 0x9E5998D46B2C6180, 0x8643A133D09093BB, 0x8E4A49914604C252
      , 0xF63897E8134BF885, 0xFE317F4A85DFA96C, 0xE62B46AD3E635B57, 0xEE22AE0FA8F70ABE
      , 0xD61F3562491ABF21, 0xDE16DDC0DF8EEEC8, 0xC60CE42764321CF3, 0xCE050C85F2A64D1A
      , 0x36E958D5CEAC695D, 0x3EE0B077583838B4, 0x26FA8990E384CA8F, 0x2EF3613275109B66
      , 0x16CEFA5F94FD2EF9, 0x1EC712FD02697F10, 0x06DD2B1AB9D58D2B, 0x0ED4C3B82F41DCC2
      , 0x76A61DC17A0EE615, 0x7EAFF563EC9AB7FC, 0x66B5CC84572645C7, 0x6EBC2426C1B2142E
      , 0x5681BF4B205FA1B1, 0x5E8857E9B6CBF058, 0x46926E0E0D770263, 0x4E9B86AC9BE3538A
      , 0xDBA563573AB1A576, 0xD3AC8BF5AC25F49F, 0xCBB6B212179906A4, 0xC3BF5AB0810D574D
      , 0xFB82C1DD60E0E2D2, 0xF38B297FF674B33B, 0xEB9110984DC84100, 0xE398F83ADB5C10E9
      , 0x9BEA26438E132A3E, 0x93E3CEE118877BD7, 0x8BF9F706A33B89EC, 0x83F01FA435AFD805
      , 0xBBCD84C9D4426D9A, 0xB3C46C6B42D63C73, 0xABDE558CF96ACE48, 0xA3D7BD2E6FFE9FA1
      , 0x5B3BE97E53F4BBE6, 0x533201DCC560EA0F, 0x4B28383B7EDC1834, 0x4321D099E84849DD
      , 0x7B1C4BF409A5FC42, 0x7315A3569F31ADAB, 0x6B0F9AB1248D5F90, 0x63067213B2190E79
      , 0x1B74AC6AE75634AE, 0x137D44C871C26547, 0x0B677D2FCA7E977C, 0x036E958D5CEAC695
      , 0x3B530EE0BD07730A, 0x335AE6422B9322E3, 0x2B40DFA5902FD0D8, 0x2349370706BB8131
      , 0xFF14F3CE4E79EB1F, 0xF71D1B6CD8EDBAF6, 0xEF07228B635148CD, 0xE70ECA29F5C51924
      , 0xDF3351441428ACBB, 0xD73AB9E682BCFD52, 0xCF20800139000F69, 0xC72968A3AF945E80
      , 0xBF5BB6DAFADB6457, 0xB7525E786C4F35BE, 0xAF48679FD7F3C785, 0xA7418F3D4167966C
      , 0x9F7C1450A08A23F3, 0x9775FCF2361E721A, 0x8F6FC5158DA28021, 0x87662DB71B36D1C8
      , 0x7F8A79E7273CF58F, 0x77839145B1A8A466, 0x6F99A8A20A14565D, 0x679040009C8007B4
      , 0x5FADDB6D7D6DB22B, 0x57A433CFEBF9E3C2, 0x4FBE0A28504511F9, 0x47B7E28AC6D14010
      , 0x3FC53CF3939E7AC7, 0x37CCD451050A2B2E, 0x2FD6EDB6BEB6D915, 0x27DF0514282288FC
      , 0x1FE29E79C9CF3D63, 0x17EB76DB5F5B6C8A, 0x0FF14F3CE4E79EB1, 0x07F8A79E7273CF58
    };

}


// A little inline to do the actual per-byte accumulation
inline static tCIDLib::TVoid
AccumByte(tCIDLib::TCard8& c8Tar, const tCIDLib::TCard1 c1Val)
{
    c8Tar = (c8Tar >> 8)
             ^ CIDMetaExtr_DVDID::ac8table[tCIDLib::TCard1(c8Tar) ^ c1Val];
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDDVDID::TPerFileInfo
// PREFIX: pfi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDVDID::TPerFileInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDVDID::TPerFileInfo::TPerFileInfo(  const   TFindBuf&       fndbSrc
                                        ,       TUTF8Converter& tcvtToUse
                                        ,       TMemBuf&        mbufTmp) :
    m_c4NameBytes(0)
    , m_c4Size(0)
    , m_c8Time(0)
    , m_pschName(0)
{
    //
    //  Convert the file name and allocate the buffer for it and fill it, and
    //  we only want the name.ext part.
    //
    TString strTmp;
    fndbSrc.pathFileName().bQueryNameExt(strTmp);
    tcvtToUse.c4ConvertTo(strTmp, mbufTmp, m_c4NameBytes);

    m_pschName = new tCIDLib::TSCh[m_c4NameBytes + 1];
    mbufTmp.CopyOut(m_pschName, m_c4NameBytes);
    m_pschName[m_c4NameBytes] = 0;

    // Get out the size, which we have to cast back to 32 bits
    m_c4Size = tCIDLib::TCard4(fndbSrc.c8Size());

    // And store the time
    m_c8Time = fndbSrc.tmCreated().enctTime();
}

TCIDDVDID::TPerFileInfo::~TPerFileInfo()
{
    // Clean up the file name
    delete [] m_pschName;
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDDVDID
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDVDID: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDVDID::TCIDDVDID() :

    m_colFiles(tCIDLib::EAdoptOpts::Adopt)
    , m_mbufTmp(0x10000, 0x10000)
{
}

TCIDDVDID::~TCIDDVDID()
{
}


// ---------------------------------------------------------------------------
//  TCIDDVDID: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TCIDDVDID::GenDVDID(const   TString&            strDVDPath
                    ,       tCIDLib::TCard8&    c8ID
                    ,       TString&            strFmtID)
{
    // Check that the path exists
    m_pathVTS = strDVDPath;
    m_pathVTS.AddLevel(L"VIDEO_TS");

    if (!TFileSys::bIsDirectory(m_pathVTS))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcDVDID_BadPath
            , m_pathVTS
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Reset everything
    m_c8Accum = kCIDLib::c8MaxCard;
    m_colFiles.RemoveAll();

    // Now look up all the files
    LoadFiles();

    // Process the files
    ProcessFiles();

    //
    //  Now we need to read in (up to) the first 64K the first two IFO files,
    //  and process them through the CRC.
    //
    TPathStr pathIFO;

    // Do the main IFO
    {
        pathIFO = m_pathVTS;
        pathIFO.AddLevel(L"VIDEO_TS.IFO");
        TBinaryFile bflIFO(pathIFO);
        bflIFO.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        tCIDLib::TCard4 c4Bytes = bflIFO.c4ReadBuffer(m_mbufTmp, 0x10000);
        ProcessBuf(m_mbufTmp.pc1Data(), c4Bytes);
        bflIFO.Close();
    }

    // Do the VTS IFO
    {
        pathIFO = m_pathVTS;
        pathIFO.AddLevel(L"VTS_01_0.IFO");
        TBinaryFile bflIFO(pathIFO);
        bflIFO.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        tCIDLib::TCard4 c4Bytes = bflIFO.c4ReadBuffer(m_mbufTmp, 0x10000);
        ProcessBuf(m_mbufTmp.pc1Data(), c4Bytes);
        bflIFO.Close();
    }

    // Give the check sum back as the id
    c8ID = m_c8Accum;

    //
    //  And give back the formatted version of it, which is the eight hex
    //  formatted nibbles, divided by a hyphen.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        // Get out the current byte
        const tCIDLib::TCard1 c1Cur = tCIDLib::TCard1(m_c8Accum >> 56);
        m_c8Accum <<= 8;

        // Get the first nibble
        tCIDLib::TCard1 c1Fmt = c1Cur >> 4;
        if (c1Fmt < 0xA)
            strFmtID.Append(tCIDLib::TCh(kCIDLib::chDigit0 + c1Fmt));
        else
            strFmtID.Append(tCIDLib::TCh(kCIDLib::chLatin_A + (c1Fmt - 10)));

        // And the second nibble
        c1Fmt = c1Cur & 0xF;
        if (c1Fmt < 0xA)
            strFmtID.Append(tCIDLib::TCh(kCIDLib::chDigit0 + c1Fmt));
        else
            strFmtID.Append(tCIDLib::TCh(kCIDLib::chLatin_A + (c1Fmt - 10)));
    }

    // Slip the hyphen in
    strFmtID.Insert(kCIDLib::chHyphenMinus, 8);
}


// ---------------------------------------------------------------------------
//  TCIDDVDID: Public, non-virtual methods
// ---------------------------------------------------------------------------

// These methods all just process bytes and update the CRC accumulator
tCIDLib::TVoid TCIDDVDID::ProcessBuf(const  tCIDLib::TCard1* const  pc1Data
                                    , const tCIDLib::TCard4         c4Size)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Size; c4Index++)
        AccumByte(m_c8Accum, pc1Data[c4Index]);
}

tCIDLib::TVoid TCIDDVDID::ProcessCard4(const tCIDLib::TCard4 c4Val)
{
    tCIDLib::TCard4 c4Src = c4Val;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
    {
        AccumByte(m_c8Accum, tCIDLib::TCard1(c4Src & 0xFF));
        c4Src >>= 8;
    }
}

tCIDLib::TVoid TCIDDVDID::ProcessCard8(const tCIDLib::TCard8 c8Val)
{
    tCIDLib::TCard8 c8Src = c8Val;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        AccumByte(m_c8Accum, tCIDLib::TCard1(c8Src & 0xFF));
        c8Src >>= 8;
    }
}


tCIDLib::TVoid TCIDDVDID::ProcessFiles()
{
    // OK, go through the files and add their info to the CRC
    tCIDLib::TEncodedTime enctActual;
    const tCIDLib::TCard4 c4Count = m_colFiles.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TCIDDVDID::TPerFileInfo& pfiCur = *m_colFiles[c4Index];

        //
        //  The time has to be the raw Windows file time, so we have to
        //  use a conversion for that.
        //
        TTime::LocalToNative(pfiCur.m_c8Time, enctActual);
        ProcessCard8(enctActual);
        ProcessCard4(pfiCur.m_c4Size);

        // Include the null terminator!
        ProcessBuf
        (
            reinterpret_cast<const tCIDLib::TCard1*>(pfiCur.m_pschName)
            , pfiCur.m_c4NameBytes + 1
        );
    }
}


// Loads up all of the files in the VIDEO_TS directory
tCIDLib::TVoid TCIDDVDID::LoadFiles()
{
    const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
    TDirIter diterFiles;
    TFindBuf fndbCur;

    if (!diterFiles.bFindFirst(m_pathVTS, kCIDLib::pszAllFilesSpec, fndbCur, eFlags))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcDVDID_NoFiles
            , m_pathVTS
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // Loop and load them all up
    do
    {
        m_colFiles.Add(new TCIDDVDID::TPerFileInfo(fndbCur, m_tcvtUTF8, m_mbufTmp));
    }   while(diterFiles.bFindNext(fndbCur));

    // Sort them by the file name (which is ASCII, so short chars)
    m_colFiles.Sort
    (
        [](const TPerFileInfo& pfi1, const TPerFileInfo& pfi2)
        {
            return TRawStr::eCompareStr(pfi1.m_pschName, pfi2.m_pschName);
        }
    );
}

