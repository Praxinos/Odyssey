// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#include "Proxies/OdysseyBrushRect.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushRect"
/////////////////////////////////////////////////////
// Odyssey Brush Rect

FOdysseyBrushRect::FOdysseyBrushRect()
    : m()
    , mIsInitialized(false)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( const ::ULIS::FRectI& iVal )
    : m( iVal )
    , mIsInitialized(true)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( int iX, int iY, int iW, int iH )
    : m( iX, iY, iW, iH )
    , mIsInitialized(true)
{
}

FOdysseyBrushRect::FOdysseyBrushRect( ::ULIS::FRectI&& iVal )
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
    m = ::ULIS::FRectI(iX, iY, iW, iH);
    mIsInitialized = true;
}

void
FOdysseyBrushRect::SetValue( const ::ULIS::FRectI& iVal ) {
    m = iVal;
    mIsInitialized = true;
}

const ::ULIS::FRectI&
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
FOdysseyBrushRect::FromTemp( ::ULIS::FRectI&& iVal ) {
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

//static
void
UOdysseyBrushRectFunctionLibrary::BreakToXYWH(FOdysseyBrushRect Rectangle, int& X, int& Y, int& Width, int& Height)
{
    if (!Rectangle.IsInitialized())
    {
        X = 0;
        Y = 0;
        Width = 0;
        Height = 0;
        return;
    }
    const ::ULIS::FRectI& rect = Rectangle.GetValue();
    X = rect.x;
    Y = rect.y;
    Width = rect.w;
    Height = rect.h;
}

#undef LOCTEXT_NAMESPACE
