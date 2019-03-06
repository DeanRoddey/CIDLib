class TVideoDemoIntfImpl : public TVideoDemoServerBase
{
    public :
        TVideoDemoIntfImpl() {}
        ~TVideoDemoIntfImpl() {}

        // Inherited from the ORB generated server side stub
        tCIDLib::TBoolean bQueryByName
        (
            const   TString&                strToFind
            ,       TDemoRecord&            recFound
        )   override;

        tCIDLib::TBoolean bQueryAllStatus
        (
            const   tVideoDemoSh::EStatus   eToFind
            ,       TVector<TDemoRecord>&   colFound
        )   override;


    protected :
        // ORB callbacks
        tCIDLib::TVoid Initialize() override;
        tCIDLib::TVoid Terminate()  override;

    private :
        // Filled with some dummy records for this demo
        TVector<TDemoRecord> m_colPatients;

        RTTIDefs(TVideoDemoIntfImpl,TVideoDemoServerBase)
};

