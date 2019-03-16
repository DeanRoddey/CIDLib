//
// FILE NAME: CIDMath_4By4Matrix.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the T4By4Matrix class.
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
#include    "CIDMath_.hpp"



// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(T4By4Matrix,TObject)


// ---------------------------------------------------------------------------
//  Our global binary operators
// ---------------------------------------------------------------------------
T4By4Matrix operator*(const T4By4Matrix& mtrx1, const T4By4Matrix& mtrx2)
{
    T4By4Matrix mtrxRet;

    mtrxRet.Zero();
    for (tCIDLib::TCard4 c4Outer = 0; c4Outer < 4; c4Outer++)
    {
        for (tCIDLib::TCard4 c4Inner = 0; c4Inner < 4; c4Inner++)
        {
            for (tCIDLib::TCard4 c4Element = 0; c4Element < 4; c4Element++)
            {
                mtrxRet.m_af8Matrix[c4Outer][c4Inner]
                        += (mtrx1.m_af8Matrix[c4Outer][c4Element]
                            * mtrx2.m_af8Matrix[c4Element][c4Inner]);
            }
        }
    }
    return mtrxRet;
}




// ---------------------------------------------------------------------------
//   CLASS: T4By4Matrix
//  PREFIX: mtrx
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T4By4Matrix: Constructor and Destructors
// ---------------------------------------------------------------------------
T4By4Matrix::T4By4Matrix()
{
    SetToIdentity();
}


T4By4Matrix::T4By4Matrix(   const   tCIDLib::TFloat8&   f81_1
                            , const tCIDLib::TFloat8&   f81_2
                            , const tCIDLib::TFloat8&   f81_3
                            , const tCIDLib::TFloat8&   f82_1
                            , const tCIDLib::TFloat8&   f82_2
                            , const tCIDLib::TFloat8&   f82_3
                            , const tCIDLib::TFloat8&   f83_1
                            , const tCIDLib::TFloat8&   f83_2
                            , const tCIDLib::TFloat8&   f83_3
                            , const tCIDLib::TFloat8&   f8Xt
                            , const tCIDLib::TFloat8&   f8Yt
                            , const tCIDLib::TFloat8&   f8Zt)
{
    TRawMem::SetMemBuf(m_af8Matrix, tCIDLib::TCard1(0), sizeof(m_af8Matrix));
    m_af8Matrix[0][0]     = f81_1;
    m_af8Matrix[0][1]     = f81_2;
    m_af8Matrix[0][2]     = f81_3;
    m_af8Matrix[1][0]     = f82_1;
    m_af8Matrix[1][1]     = f82_2;
    m_af8Matrix[1][2]     = f82_3;
    m_af8Matrix[2][0]     = f83_1;
    m_af8Matrix[2][1]     = f83_2;
    m_af8Matrix[2][2]     = f83_3;
    m_af8Matrix[3][0]     = f8Xt;
    m_af8Matrix[3][1]     = f8Yt;
    m_af8Matrix[3][2]     = f8Zt;
}

T4By4Matrix::T4By4Matrix(const T4By4Matrix& mtrxSrc)
{
    TRawMem::CopyMemBuf(m_af8Matrix, mtrxSrc.m_af8Matrix, sizeof(m_af8Matrix));
}


// ---------------------------------------------------------------------------
//  T4By4Matrix: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TFloat8 T4By4Matrix::operator[](const tCIDMath::EMatrixElems eElem) const
{
    return m_af8Array[tCIDLib::TCard4(eElem)];
}

tCIDLib::TFloat8& T4By4Matrix::operator[](const tCIDMath::EMatrixElems eElem)
{
    return m_af8Array[tCIDLib::TCard4(eElem)];
}


tCIDLib::TVoid T4By4Matrix::operator*=(const T4By4Matrix& mtrxSrc)
{
    // Make a copy of this matrix
    T4By4Matrix mtrxTmp(*this);

    // Zero out this matrix as the new empty matrix to fill in
    Zero();
    for (tCIDLib::TCard4 c4Outer = 0; c4Outer < 4; c4Outer++)
    {
        for (tCIDLib::TCard4 c4Inner = 0; c4Inner < 4; c4Inner++)
        {
            for (tCIDLib::TCard4 c4Element = 0; c4Element < 4; c4Element++)
            {
                m_af8Matrix[c4Outer][c4Inner]
                            += (mtrxTmp.m_af8Matrix[c4Outer][c4Element]
                                * mtrxSrc.m_af8Matrix[c4Element][c4Inner]);
            }
        }
    }
}


tCIDLib::TBoolean T4By4Matrix::operator==(const T4By4Matrix& mtrxSrc) const
{
    if (&mtrxSrc != this)
    {
        // Compare the values of the objects
        if (!TRawMem::bCompareMemBuf(mtrxSrc.m_af8Matrix, m_af8Matrix, sizeof(m_af8Matrix)))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean T4By4Matrix::operator!=(const T4By4Matrix& mtrxSrc) const
{
    return !operator==(mtrxSrc);
}


T4By4Matrix& T4By4Matrix::operator=(const T4By4Matrix& mtrxSrc)
{
    if (this != &mtrxSrc)
        TRawMem::CopyMemBuf(m_af8Matrix, mtrxSrc.m_af8Matrix, sizeof(m_af8Matrix));
    return *this;
}


// ---------------------------------------------------------------------------
//  T4By4Matrix: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T4By4Matrix::AppendRotation(const T3DVector& vecAngles)
{
    // Get a copy of the vector and convert to radians
    T3DVector vecTmp(vecAngles);
    vecTmp.ToRadians();

    // Get the sine and cosine of each value
    tCIDLib::TFloat8 f8SinX, f8SinY, f8SinZ;
    tCIDLib::TFloat8 f8CosX, f8CosY, f8CosZ;
    f8SinX = TMathLib::f8Sine(vecTmp.f8X());
    f8CosX = TMathLib::f8Cosine(vecTmp.f8X());
    f8SinY = TMathLib::f8Sine(vecTmp.f8Y());
    f8CosY = TMathLib::f8Cosine(vecTmp.f8Y());
    f8SinZ = TMathLib::f8Sine(vecTmp.f8Z());
    f8CosZ = TMathLib::f8Cosine(vecTmp.f8Z());

    // Concatenate the X rotation
    T4By4Matrix mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::M22] = f8CosX;
    mtrxTmp[tCIDMath::EMatrixElems::M33] = f8CosX;
    mtrxTmp[tCIDMath::EMatrixElems::M23] = f8SinX;
    mtrxTmp[tCIDMath::EMatrixElems::M32] = -f8SinX;
    *this *= mtrxTmp;

    // Concatenate the Y rotation
    mtrxTmp.SetToIdentity();
    mtrxTmp[tCIDMath::EMatrixElems::M11] = f8CosY;
    mtrxTmp[tCIDMath::EMatrixElems::M33] = f8CosY;
    mtrxTmp[tCIDMath::EMatrixElems::M13] = -f8SinY;
    mtrxTmp[tCIDMath::EMatrixElems::M31] = f8SinY;
    *this *= mtrxTmp;

    // Concatenate the Z rotation
    mtrxTmp.SetToIdentity();
    mtrxTmp[tCIDMath::EMatrixElems::M11] = f8CosZ;
    mtrxTmp[tCIDMath::EMatrixElems::M22] = f8CosZ;
    mtrxTmp[tCIDMath::EMatrixElems::M12] = f8SinZ;
    mtrxTmp[tCIDMath::EMatrixElems::M21] = -f8SinZ;
    *this *= mtrxTmp;
}

tCIDLib::TVoid
T4By4Matrix::AppendRotation(const   tCIDLib::TFloat8&   f8XAngle
                            , const tCIDLib::TFloat8&   f8YAngle
                            , const tCIDLib::TFloat8&   f8ZAngle)
{
    // Convert the angles to radians
    tCIDLib::TFloat8 f8XA = f8XAngle * kCIDLib::f8PI / 180.0;
    tCIDLib::TFloat8 f8YA = f8YAngle * kCIDLib::f8PI / 180.0;
    tCIDLib::TFloat8 f8ZA = f8ZAngle * kCIDLib::f8PI / 180.0;

    // Get the sine and cosine of each value
    tCIDLib::TFloat8 f8SinX, f8SinY, f8SinZ;
    tCIDLib::TFloat8 f8CosX, f8CosY, f8CosZ;
    f8SinX = TMathLib::f8Sine(f8XA);
    f8CosX = TMathLib::f8Cosine(f8XA);
    f8SinY = TMathLib::f8Sine(f8YA);
    f8CosY = TMathLib::f8Cosine(f8YA);
    f8SinZ = TMathLib::f8Sine(f8ZA);
    f8CosZ = TMathLib::f8Cosine(f8ZA);

    // Concatenate the X rotation
    T4By4Matrix mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::M22] = f8CosX;
    mtrxTmp[tCIDMath::EMatrixElems::M33] = f8CosX;
    mtrxTmp[tCIDMath::EMatrixElems::M23] = f8SinX;
    mtrxTmp[tCIDMath::EMatrixElems::M32] = -f8SinX;
    *this *= mtrxTmp;

    // Concatenate the Y rotation
    mtrxTmp.SetToIdentity();
    mtrxTmp[tCIDMath::EMatrixElems::M11] = f8CosY;
    mtrxTmp[tCIDMath::EMatrixElems::M33] = f8CosY;
    mtrxTmp[tCIDMath::EMatrixElems::M13] = -f8SinY;
    mtrxTmp[tCIDMath::EMatrixElems::M31] = f8SinY;
    *this *= mtrxTmp;

    // Concatenate the Z rotation
    mtrxTmp.SetToIdentity();
    mtrxTmp[tCIDMath::EMatrixElems::M11] = f8CosZ;
    mtrxTmp[tCIDMath::EMatrixElems::M22] = f8CosZ;
    mtrxTmp[tCIDMath::EMatrixElems::M12] = f8SinZ;
    mtrxTmp[tCIDMath::EMatrixElems::M21] = -f8SinZ;
    *this *= mtrxTmp;
}


tCIDLib::TVoid T4By4Matrix::AppendScale(const T3DVector& vecScale)
{
    // Create a temporary matrix and set up the scaling
    T4By4Matrix mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::M11] = vecScale.f8X();
    mtrxTmp[tCIDMath::EMatrixElems::M22] = vecScale.f8Y();
    mtrxTmp[tCIDMath::EMatrixElems::M33] = vecScale.f8Z();

    // Concatenate it
    *this *= mtrxTmp;
}

tCIDLib::TVoid
T4By4Matrix::AppendScale(   const   tCIDLib::TFloat8&   f8XScale
                            , const tCIDLib::TFloat8&   f8YScale
                            , const tCIDLib::TFloat8&   f8ZScale)
{
    // Create a temporary matrix and set up the scaling
    T4By4Matrix    mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::M11] = f8XScale;
    mtrxTmp[tCIDMath::EMatrixElems::M22] = f8YScale;
    mtrxTmp[tCIDMath::EMatrixElems::M33] = f8ZScale;

    // Concatenate it
    *this *= mtrxTmp;
}


tCIDLib::TVoid T4By4Matrix::AppendTranslation(const T3DVector& vecTrans)
{
    // Create a temporary matrix and set up the translation
    T4By4Matrix mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::MXt] = vecTrans.f8X();
    mtrxTmp[tCIDMath::EMatrixElems::MYt] = vecTrans.f8Y();
    mtrxTmp[tCIDMath::EMatrixElems::MZt] = vecTrans.f8Z();

    // Concatenate it
    *this *= mtrxTmp;
}

tCIDLib::TVoid
T4By4Matrix::AppendTranslation( const   tCIDLib::TFloat8&   f8XTrans
                                , const tCIDLib::TFloat8&   f8YTrans
                                , const tCIDLib::TFloat8&   f8ZTrans)
{
    // Create a temporary matrix and set up the translation
    T4By4Matrix mtrxTmp;
    mtrxTmp[tCIDMath::EMatrixElems::MXt] = f8XTrans;
    mtrxTmp[tCIDMath::EMatrixElems::MYt] = f8YTrans;
    mtrxTmp[tCIDMath::EMatrixElems::MZt] = f8ZTrans;

    // Concatenate it
    *this *= mtrxTmp;
}


tCIDLib::TVoid T4By4Matrix::SetToIdentity()
{
    TRawMem::SetMemBuf(m_af8Matrix, tCIDLib::TCard1(0), sizeof(m_af8Matrix));
    m_af8Matrix[0][0]   = 1.0;
    m_af8Matrix[1][1]   = 1.0;
    m_af8Matrix[2][2]   = 1.0;
    m_af8Matrix[3][3]   = 1.0;
}


tCIDLib::TVoid
T4By4Matrix::SetToRotation( const   tCIDLib::TFloat8&   f8XDeg
                            , const tCIDLib::TFloat8&   f8YDeg
                            , const tCIDLib::TFloat8&   f8ZDeg)
{
    SetToIdentity();
    AppendRotation(f8XDeg, f8YDeg, f8ZDeg);
}


tCIDLib::TVoid T4By4Matrix::SetToRotation(const T3DVector& vecAngles)
{
    SetToIdentity();
    AppendRotation(vecAngles);
}


tCIDLib::TVoid
T4By4Matrix::SetToScale(const   tCIDLib::TFloat8&   f8XScale
                        , const tCIDLib::TFloat8&   f8YScale
                        , const tCIDLib::TFloat8&   f8ZScale)
{
    SetToIdentity();
    AppendScale(f8XScale,f8YScale,f8ZScale);
}


tCIDLib::TVoid T4By4Matrix::SetToScale(const T3DVector& vecScale)
{
    SetToIdentity();
    AppendScale(vecScale);
}


tCIDLib::TVoid
T4By4Matrix::SetToTranslation(  const   tCIDLib::TFloat8&   f8XTrans
                                , const tCIDLib::TFloat8&   f8YTrans
                                , const tCIDLib::TFloat8&   f8ZTrans)
{
    SetToIdentity();
    AppendTranslation(f8XTrans, f8YTrans, f8ZTrans);
}


tCIDLib::TVoid T4By4Matrix::SetToTranslation(const T3DVector& vecTrans)
{
    SetToIdentity();
    AppendTranslation(vecTrans);
}


tCIDLib::TVoid T4By4Matrix::SetToTransposed(const T4By4Matrix& mtrxSrc)
{
    // Flip the order of the indices
    for (tCIDLib::TInt4 i4Outer = 0; i4Outer < 4; i4Outer++)
    {
        for (tCIDLib::TInt4 i4Inner = 0; i4Inner < 4; i4Inner++)
        {
            m_af8Matrix[i4Outer][i4Inner]
                            = mtrxSrc.m_af8Matrix[i4Inner][i4Outer];
        }
    }
}


tCIDLib::TVoid T4By4Matrix::Transform(T3DVector& vecTarget) const
{
    tCIDLib::TFloat8 f8X, f8Y, f8Z;

    f8X =   (vecTarget.m_f8XMag * m_af8Matrix[0][0])
            + (vecTarget.m_f8YMag * m_af8Matrix[1][0])
            + (vecTarget.m_f8ZMag * m_af8Matrix[2][0])
            + m_af8Matrix[3][0];

    f8Y =   (vecTarget.m_f8XMag * m_af8Matrix[0][1])
            + (vecTarget.m_f8YMag * m_af8Matrix[1][1])
            + (vecTarget.m_f8ZMag * m_af8Matrix[2][1])
            + m_af8Matrix[3][1];

    f8Z =   (vecTarget.m_f8XMag * m_af8Matrix[0][2])
            + (vecTarget.m_f8YMag * m_af8Matrix[1][2])
            + (vecTarget.m_f8ZMag * m_af8Matrix[2][2])
            + m_af8Matrix[3][2];

    vecTarget.m_f8XMag = f8X;
    vecTarget.m_f8YMag = f8Y;
    vecTarget.m_f8ZMag = f8Z;
}

tCIDLib::TVoid
T4By4Matrix::Transform(const T3DVector& vecSource, T3DVector& vecTarget) const
{
    vecTarget.m_f8XMag =
    (
        (vecSource.m_f8XMag * m_af8Matrix[0][0])
        + (vecSource.m_f8YMag * m_af8Matrix[1][0])
        + (vecSource.m_f8ZMag * m_af8Matrix[2][0])
        + m_af8Matrix[3][0]
    );

    vecTarget.m_f8YMag =
    (
        (vecSource.m_f8XMag * m_af8Matrix[0][1])
        + (vecSource.m_f8YMag * m_af8Matrix[1][1])
        + (vecSource.m_f8ZMag * m_af8Matrix[2][1])
        + m_af8Matrix[3][1]
    );

    vecTarget.m_f8ZMag =
    (
        (vecSource.m_f8XMag * m_af8Matrix[0][2])
        + (vecSource.m_f8YMag * m_af8Matrix[1][2])
        + (vecSource.m_f8ZMag * m_af8Matrix[2][2])
        + m_af8Matrix[3][2]
    );
}


tCIDLib::TVoid T4By4Matrix::Transpose()
{
    tCIDLib::TFloat8 af8Tmp[4][4];

    // Copy the current matrix to the temp array
    TRawMem::CopyMemBuf(af8Tmp, m_af8Matrix, sizeof(m_af8Matrix));

    // Now put them back in the opposite order
    for (tCIDLib::TInt4 i4Outer = 0; i4Outer < 4; i4Outer++)
    {
        for (tCIDLib::TInt4 i4Inner = 0; i4Inner < 4; i4Inner++)
            m_af8Matrix[i4Outer][i4Inner] = af8Tmp[i4Inner][i4Outer];
    }
}


tCIDLib::TVoid T4By4Matrix::Zero()
{
    TRawMem::SetMemBuf(m_af8Matrix, tCIDLib::TCard1(0), sizeof(m_af8Matrix));
}


// ---------------------------------------------------------------------------
//  T4By4Matrix: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T4By4Matrix::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Save the stream state
    TStreamJanitor janWrite(&strmToWriteTo);

    strmToWriteTo   << TTextOutStream::Width(10)
                    << TTextOutStream::Precision(4)
                    << TTextOutStream::Justify(tCIDLib::EHJustify::Right)
                    << TTextOutStream::Fill(kCIDLib::chSpace);

    tCIDMath::EMatrixElems eIndex = tCIDMath::EMatrixElems::Min;
    while (eIndex < tCIDMath::EMatrixElems::Max)
    {
        strmToWriteTo << m_af8Array[tCIDLib::c4EnumOrd(eIndex)];

        if (eIndex != tCIDMath::EMatrixElems::Max)
            strmToWriteTo << L", ";

        eIndex++;
    }
}


tCIDLib::TVoid T4By4Matrix::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom.ReadArray(m_af8Array, tCIDLib::c4EnumOrd(tCIDMath::EMatrixElems::Count));
}

tCIDLib::TVoid T4By4Matrix::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo.WriteArray(m_af8Array, tCIDLib::c4EnumOrd(tCIDMath::EMatrixElems::Count));
}

