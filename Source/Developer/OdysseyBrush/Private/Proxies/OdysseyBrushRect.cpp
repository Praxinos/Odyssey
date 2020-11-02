// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "Proxies/OdysseyBrushRect.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushRect"
/////////////////////////////////////////////////////
// Odyssey Brush Rect

FOdysseyBrushRect::FOdysseyBrushRect()
    : m()
    , mIsInitialized(false)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( const ::ul3::FRect& iVal )
    : m( iVal )
    , mIsInitialized(true)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( int iX, int iY, int iW, int iH )
    : m( iX, iY, iW, iH )
    , mIsInitialized(true)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( ::ul3::FRect&& iVal )
    : m( std::move( iVal ) )
    , mIsInitialized(true)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( const FOdysseyBrushRect& iOther )
    : m( iOther.m )
    , mIsInitialized(iOther.mIsInitialized)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( FOdysseyBrushRect&& iOther )
    : m( std::move( iOther.m ) )
    , mIsInitialized( std::move(iOther.mIsInitialized) )
{
}

FOdysseyBrushRect&
FOdysseyBrushRect::operator=( const FOdysseyBrushRect& iOther ) {
    m = iOther.m;
    mIsInitialized = iOther.mIsInitialized;
    return  *this;
}

void
FOdysseyBrushRect::SetValue( int iX, int iY, int iW, int iH ) {
    m = ::ul3::FRect(iX, iY, iW, iH);
    mIsInitialized = true;
}

void
FOdysseyBrushRect::SetValue( const ::ul3::FRect& iVal ) {
    m = iVal;
    mIsInitialized = true;
}

const ::ul3::FRect&
FOdysseyBrushRect::GetValue() const {
    return  m;
}

int
FOdysseyBrushRect::X() const
{
	return m.x;
}

int
FOdysseyBrushRect::Y() const
{
	return m.y;
}

int
FOdysseyBrushRect::Width() const
{
	return m.w;
}

int
FOdysseyBrushRect::Height() const
{
	return m.h;
}

bool
FOdysseyBrushRect::IsInitialized() const {
    return  mIsInitialized;
}

//static
FOdysseyBrushRect
FOdysseyBrushRect::FromTemp( ::ul3::FRect&& iVal ) {
    return  FOdysseyBrushRect( std::move( iVal ) );
}

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushRectFunctionLibrary
UOdysseyBrushRectFunctionLibrary::UOdysseyBrushRectFunctionLibrary( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
}


//static
FOdysseyBrushRect
UOdysseyBrushRectFunctionLibrary::MakeFromXYWH(int X, int Y, int W, int H)
{
    return  FOdysseyBrushRect( X, Y, W, H );
}

#undef LOCTEXT_NAMESPACE
