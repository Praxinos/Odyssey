// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorGroup.h"
#include "OdysseyVectorEngine.h"
#include "HUD/OdysseyVectorHUD.h"

FOdysseyVectorGroup::FOdysseyVectorGroup( const FString& iName )
    : FOdysseyVectorObject( iName )
    , mHUDColor( FOdysseyVectorHUD::GetForegroundColor() )
{
}

void
FOdysseyVectorGroup::Init( FString& iName )
{
    SetName( iName );
}

bool
FOdysseyVectorGroup::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

/*
bool
FOdysseyVectorGroup::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.map_point( iRoi.x, iRoi.y );

        if( mBBox.HitTest( ::ULIS::FVec2D( pt.x, pt.y ) ) )
        {
            return true;
        }
    }

    return false;
}
*/

FOdysseyVectorObject*
FOdysseyVectorGroup::CopyShape( uint64 iCopyFlags )
{
    FOdysseyVectorGroup* groupCopy = new FOdysseyVectorGroup( mName + "_Copy" );

    groupCopy->SetHUDColor( mHUDColor );

    return groupCopy;
}

void
FOdysseyVectorGroup::UpdateBBox()
{
    mBBox = ::ULIS::FRectD( 0, 0, 0, 0 );

/*
    FOdysseyVectorEngine::Traverse( this
                                  , 0
                                  , [ this ] ( FOdysseyVectorObject* object, uint64 iTraverseFlags )
    {
        ::ULIS::FRectD bbox = object->GetBBox( true, true );

        mBBox = mBBox.Area() ? ( mBBox | bbox ) : bbox;
                                                                                   // Traverse until depth 1 only
        return ( iTraverseFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) ? FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN
                                                                                   : FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
    } );
*/
}

void
FOdysseyVectorGroup::UpdateShape( uint32 iUpdateFlags )
{

}

FColor
FOdysseyVectorGroup::GetHUDColor()
{
    return mHUDColor;
}

void
FOdysseyVectorGroup::SetHUDColor( const FColor& iHUDColor )
{
    mHUDColor = iHUDColor;
}
