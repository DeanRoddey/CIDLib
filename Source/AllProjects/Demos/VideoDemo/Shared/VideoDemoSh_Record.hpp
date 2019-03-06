#pragma CIDLIB_PACK(CIDLIBPACK)

class VIDEODEMOSHEXP TDemoRecord :  public TObject
                                    , public MStreamable
                                    , public MFormattable
{
    public :
        TDemoRecord();
        TDemoRecord
        (
            const   TString&                strFirst
            , const TString&                strLast
            , const tVideoDemoSh::EStatus   eStatus
            , const tCIDLib::TCard4         c4Age
            , const tCIDLib::TCard4         c4RecordNum
        );

        TDemoRecord(const TDemoRecord& recSrc);
        ~TDemoRecord();
        TDemoRecord& operator=(const TDemoRecord& recSrc);

        tCIDLib::TCard4         m_c4RecordNum;
        tCIDLib::TCard4         m_c4Age;
        tVideoDemoSh::EStatus   m_eStatus;
        TString                 m_strFullName;
        TString                 m_strFirst;
        TString                 m_strLast;

    protected :
        tCIDLib::TVoid FormatTo(TTextOutStream& strmDest) const override;
        tCIDLib::TVoid StreamFrom(TBinInStream& strmToReadFrom) override;
        tCIDLib::TVoid StreamTo(TBinOutStream& strmToWriteTo) const override;

    private:
        RTTIDefs(TDemoRecord,TObject)
};

#pragma CIDLIB_POPPACK
