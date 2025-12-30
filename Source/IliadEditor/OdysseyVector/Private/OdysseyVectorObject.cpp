// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorObject.h"
#include "OdysseyVector.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorObject::~FOdysseyVectorObject()
{
    mTagList.remove_if( []( FOdysseyVectorTag *tag ) -> bool
                          {
                              delete tag;

                              return true;
                          } );

    mChildrenList.remove_if( []( FOdysseyVectorObject *obj )
                                {
                                    delete obj;

                                    return true;
                                } );
}

FOdysseyVectorObject::FOdysseyVectorObject( const FString& iName )
    : mParent( nullptr )
    , bSelected( false )
    , bExpanded( false )
    , bSystem( false )
    , bVisible ( true )
    , mBackgroundBucket( this, 0.0f, 0.0f, false )
    , mForegroundBucket( this, 0.0f, 0.0f, false )
    , mInvalidationFlags ()
{
    mLocalMatrix.reset();
    mWorldMatrix.reset();
    mInverseLocalMatrix.reset();
    mInverseWorldMatrix.reset();

    SetName( iName );
    SetOpacity( 1.0f );
    SetExpanded(true);
    SetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f );

    mForegroundBucket.SetColorMode( eBucketColorMode::SolidColor );
    mForegroundBucket.SetSolidColor( FOREGROUNDCOLOR_DEFAULT_R
                                   , FOREGROUNDCOLOR_DEFAULT_G
                                   , FOREGROUNDCOLOR_DEFAULT_B
                                   , FOREGROUNDCOLOR_DEFAULT_A );

    mBackgroundBucket.SetColorMode( eBucketColorMode::SolidColor );
    mBackgroundBucket.SetSolidColor( BACKGROUNDCOLOR_DEFAULT_R
                                   , BACKGROUNDCOLOR_DEFAULT_G
                                   , BACKGROUNDCOLOR_DEFAULT_B
                                   , BACKGROUNDCOLOR_DEFAULT_A );
}

FOdysseyVectorBucket&
FOdysseyVectorObject::GetBackgroundBucket()
{
    return mBackgroundBucket;
}

FOdysseyVectorBucket&
FOdysseyVectorObject::GetForegroundBucket()
{
    return mForegroundBucket;
}

double
FOdysseyVectorObject::GetOpacity()
{
    return mOpacity;
}

void
FOdysseyVectorObject::SetOpacity( double iOpacity )
{
    mOpacity = iOpacity;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::COLOR) );
}

void
FOdysseyVectorObject::AddTag( FOdysseyVectorTag* iTag )
{
    mTagList.push_back( iTag );

    iTag->Added();

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_LIST) );
}

void
FOdysseyVectorObject::RemoveTag( FOdysseyVectorTag* iTag )
{
    mTagList.remove( iTag );

    iTag->Removed();

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_LIST) );
}

void
FOdysseyVectorObject::SetName( const FString& iName )
{
    mName = iName;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::PARAM) );
}

FString&
FOdysseyVectorObject::GetName()
{
    return mName;
}

void
FOdysseyVectorObject::SetExpanded( bool iIsExpanded )
{
    bExpanded = iIsExpanded;
}

bool
FOdysseyVectorObject::IsExpanded()
{
    return bExpanded;
}

static uint32
CheckCommonClass( std::list<FOdysseyVectorObject*>& iObjectList, uint32 iCommonClass )
{
    if( iObjectList.size() )
    {
        for( FOdysseyVectorObject* object : iObjectList )
        {
            if( object->HasBaseClass( iCommonClass ) )
            {
                uint32 objectClass = object->GetClass();

                if( objectClass != iCommonClass )
                {
                    uint32 newCommonClass = CheckCommonClass( iObjectList, objectClass );

                    if( newCommonClass )
                    {
                        return newCommonClass;
                    }
                }
            }
            else
            {
                return 0;
            }
        }

        return iCommonClass;
    }

    return 0;
}

FColor
FOdysseyVectorObject::GetHUDColor()
{
    FOdysseyVectorObject* group = GetAncestorByClass( FOdysseyVectorGroup::StaticClass()
                                                    , true
                                                    , false );

    return group ? group->GetHUDColor() : FOdysseyVectorHUD::GetForegroundColor();
}

uint32
FOdysseyVectorObject::GetCommonClass( std::list<FOdysseyVectorObject*>& iObjectList )
{
    return CheckCommonClass( iObjectList, FOdysseyVectorObject::StaticClass() );
}

bool
FOdysseyVectorObject::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return false;
}

void
FOdysseyVectorObject::Added()
{
    for( FOdysseyVectorTag* tag : mTagList )
    {
        tag->ObjectAdded();
    }
}

bool
FOdysseyVectorObject::IsSystem()
{
    return bSystem;
}

void
FOdysseyVectorObject::Removed()
{
    for( FOdysseyVectorTag* tag : mTagList )
    {
        tag->ObjectRemoved();
    }
}

void
FOdysseyVectorObject::UpdateShape( uint32 iUpdateFlags )
{
/*
    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidationFlags = 0;
    }
*/
}

void
FOdysseyVectorObject::UpdateBBox()
{
    mBBox = ::ULIS::FRectD( 0, 0, 0, 0 );
}

void
FOdysseyVectorObject::MakeInDepthBBox()
{
    ::ULIS::FRectD childrenBBox = ::ULIS::FRectD( 0, 0, 0, 0 );

    mInDepthBBox = ::ULIS::FRectD( 0, 0, 0, 0 );

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        ::ULIS::FRectD bbox = child->GetBBox( true, false );
        ::ULIS::FVec2D p0 = FOdysseyVector::MapPoint( child->GetLocalMatrix(), ::ULIS::FVec2D( bbox.x         , bbox.y          ) );
        ::ULIS::FVec2D p1 = FOdysseyVector::MapPoint( child->GetLocalMatrix(), ::ULIS::FVec2D( bbox.x + bbox.w, bbox.y          ) );
        ::ULIS::FVec2D p2 = FOdysseyVector::MapPoint( child->GetLocalMatrix(), ::ULIS::FVec2D( bbox.x + bbox.w, bbox.y + bbox.h ) );
        ::ULIS::FVec2D p3 = FOdysseyVector::MapPoint( child->GetLocalMatrix(), ::ULIS::FVec2D( bbox.x         , bbox.y + bbox.h ) );
        ::ULIS::FRectD relativeBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                                , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                                , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                                , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        if( relativeBBox.Area() )
        {
            childrenBBox = childrenBBox.Area() ? ( childrenBBox | relativeBBox ) : relativeBBox;
        }
    }

    if( childrenBBox.Area() )
    {
        mInDepthBBox = childrenBBox;
    }

    if( mBBox.Area() )
    {
        mInDepthBBox = mInDepthBBox.Area() ?  mInDepthBBox | mBBox : mBBox;
    }
}

void
FOdysseyVectorObject::Update( uint32 iUpdateFlags )
{
    uint32 childUpdateFlags = iUpdateFlags;

    LockDrawing();

    if( mInvalidationFlags.bits.any() )
    {
        // update children first by recursively calling the Update function and, if needed,
        // removing the object from the invalidated object list, in the same call.
        mInvalidatedChildrenList.remove_if( [childUpdateFlags] ( FOdysseyVectorObject* child )
                                            {
                                                child->Update( childUpdateFlags );

                                                return child->IsInvalidated() == false;
                                            } );

        UpdateShape( iUpdateFlags );

        if( ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE]       )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE] ) )
        {
            UpdateBBox();
        }

        if( ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE]           )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX]          )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::HIERARCHY]       )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE]     )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX  ]  )
         || ( mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_HIERARCHY] ) )
        {
            MakeInDepthBBox();
        }

        // update tags
        for( FOdysseyVectorTag* tag : mTagList )
        {
            tag->Update( iUpdateFlags, mInvalidationFlags );
        }

        if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
        {
            mInvalidationFlags.bits.reset();
        }
    }

    UnlockDrawing();
}

// static
void
FOdysseyVectorObject::GetBucketsFromPaletteEntryRecursively( FOdysseyVectorObject* iObject
                                                           , UOdysseyPaletteEntry* iPaletteEntry
                                                           , TArray<FOdysseyVectorBucket*>& oBucketArray )
{
    std::list<FOdysseyVectorBucket*> bucketList;

    GetBucketsFromPaletteEntryRecursively( iObject, iPaletteEntry, bucketList );

    oBucketArray.Reserve( oBucketArray.Num() + bucketList.size() );

    for( FOdysseyVectorBucket* bucket : bucketList )
    {
        oBucketArray.Add( bucket );
    }
}

// static
void
FOdysseyVectorObject::GetBucketsFromPaletteEntryRecursively( FOdysseyVectorObject* iObject
                                                           , UOdysseyPaletteEntry* iPaletteEntry
                                                           , std::list<FOdysseyVectorBucket*>& oBucketList )
{
    if( iObject->GetForegroundBucket().GetPaletteEntry() == iPaletteEntry )
    {
        oBucketList.push_back( &iObject->GetForegroundBucket() );
    }

    if( iObject->GetBackgroundBucket().GetPaletteEntry() == iPaletteEntry )
    {
        oBucketList.push_back( &iObject->GetBackgroundBucket() );
    }

    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        for( FOdysseyVectorBucket* bucket : paintGroup->GetBucketList() )
        {
            if( bucket->GetPaletteEntry() == iPaletteEntry )
            {
                oBucketList.push_back( bucket );
            }
        }
    }

    // recurse
    for( FOdysseyVectorObject* child : iObject->GetChildrenList() )
    {
        GetBucketsFromPaletteEntryRecursively( child, iPaletteEntry, oBucketList );
    }
}

void
FOdysseyVectorObject::SetSelected( bool iIsSelected )
{
    bSelected = iIsSelected;
}

void FOdysseyVectorObject::ApplyMatrix( BLMatrix2D& iMatrix )
{

}

void FOdysseyVectorObject::ApplyTransformations()
{
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        BLMatrix2D& parentInverseWorldMatrix = mParent->GetInverseWorldMatrix();
        BLMatrix2D& childWorldMatrix = child->GetWorldMatrix();
        BLMatrix2D localMatrix;
        double translationX, translationY, rotation, scalingX, scalingY, skewX, skewY;

        FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, childWorldMatrix, localMatrix );

        FOdysseyVector::ExtractTransformations( localMatrix
                                              , &translationX
                                              , &translationY
                                              , &rotation
                                              , &scalingX
                                              , &scalingY
                                              , &skewX
                                              , &skewY
                                              , true ); // in Degrees

        child->SetTransform( translationX
                           , translationY
                           , rotation
                           , scalingX
                           , scalingY
                           , skewX
                           , skewY );
    }

    SetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f );

    UpdateMatrix();
}

void
FOdysseyVectorObject::Transfer( const BLMatrix2D& iMatrix )
{
    BLMatrix2D inverseMatrix = iMatrix;
    BLMatrix2D localMatrix;

    inverseMatrix.invert();

    FOdysseyVector::MatrixMultiply( inverseMatrix, mWorldMatrix, localMatrix );
    FOdysseyVector::ExtractTransformations( localMatrix
                                          , &mTranslationX
                                          , &mTranslationY
                                          , &mRotation
                                          , &mScalingX
                                          , &mScalingY
                                          , &mSkewX
                                          , &mSkewY
                                          , true );

    //UpdateMatrix();
}

void
FOdysseyVectorObject::Translate( double iX, double iY )
{
    mTranslationX = iX;
    mTranslationY = iY;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::MATRIX) );
}

void
FOdysseyVectorObject::Rotate( double iAngle )
{
    mRotation = iAngle;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::MATRIX) );
}

void
FOdysseyVectorObject::Scale( double iX, double iY )
{
    mScalingX = iX;
    mScalingY = iY;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::MATRIX) );
}

void
FOdysseyVectorObject::Skew( double iX, double iY )
{
    mSkewX = iX;
    mSkewY = iY;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::MATRIX) );
}

double
FOdysseyVectorObject::GetScalingX()
{
    return mScalingX;
}

double
FOdysseyVectorObject::GetScalingY()
{
    return mScalingY;
}

double
FOdysseyVectorObject::GetSkewX()
{
    return mSkewX;
}

double
FOdysseyVectorObject::GetSkewY()
{
    return mSkewY;
}

double
FOdysseyVectorObject::GetRotation()
{
    return mRotation;
}

void
FOdysseyVectorObject::GetTransform( double& oTranslationX
                                  , double& oTranslationY
                                  , double& oRotation
                                  , double& oScalingX
                                  , double& oScalingY
                                  , double& oSkewX
                                  , double& oSkewY )
{
    oTranslationX = mTranslationX;
    oTranslationY = mTranslationY;
    oRotation = mRotation;
    oScalingX = mScalingX;
    oScalingY = mScalingY;
    oSkewX = mSkewX;
    oSkewY = mSkewY;
}

void
FOdysseyVectorObject::SetTransform( double iTranslationX
                                  , double iTranslationY
                                  , double iRotation
                                  , double iScalingX
                                  , double iScalingY
                                  , double iSkewX
                                  , double iSkewY )
{
    mTranslationX = iTranslationX;
    mTranslationY = iTranslationY;
    mRotation = iRotation;
    mScalingX = iScalingX;
    mScalingY = iScalingY;
    mSkewX = iSkewX;
    mSkewY = iSkewY;

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::MATRIX) );
}

void
FOdysseyVectorObject::ResetTransform()
{
    Translate( 0.0f, 0.0f );
    Scale( 1.0f, 1.0f );
    Rotate( 0.0f );
    Skew( 0.0f, 0.0f );
}

void
FOdysseyVectorObject::RecursiveRemoveTagByType( uint32 iTagType
                                              , std::list<FOdysseyVectorTag*>& oRemovedTagList )
{
    FOdysseyVectorTag* tag = GetTagByType( iTagType );

    if( tag )
    {
        oRemovedTagList.push_back( tag );

        RemoveTag( tag );
    }

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        child->RecursiveRemoveTagByType( iTagType, oRemovedTagList );
    }
}

void
FOdysseyVectorObject::InvalidateTree( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags  )
{
    Invalidate( iInvalidationFlags );

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        child->InvalidateTree( iInvalidationFlags );
    }
}

void
FOdysseyVectorObject::SetVisible( bool iVisible )
{
    FOdysseyVectorCell* cell = GetCell();

    bVisible = iVisible;

    // visibility has consequences on the whole tree. We then have to invalidate the whole tree.
    InvalidateTree( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::PARAM) );

    // auto invalidation of the whole region that needs to be redrawn
    if( cell )
    {
        cell->InvalidateRect( GetBBox( true, true ) );
    }
}

bool
FOdysseyVectorObject::IsVisible( bool iHierarchical )
{
    if( iHierarchical )
    {
        FOdysseyVectorObject* object = this;

        while( object )
        {
            if( object->IsVisible( false ) == false )
            {
                return false;
            }

            object = object->GetParent();
        }

    }
    else
    {
        return bVisible;
    }

    return true;
}

FOdysseyVectorObject*
FOdysseyVectorObject::CopyShape( uint64 iCopyFlags )
{
    return new FOdysseyVectorObject( mName );
}

FOdysseyVectorObject*
FOdysseyVectorObject::Copy( uint64 iCopyFlags
                          , std::function<uint64(FOdysseyVectorObject*,uint64)> iPreCallback
                          , std::function<uint64(FOdysseyVectorObject*
                                               , FOdysseyVectorObject*,uint64)> iPostCallback )
{
    uint64 copyFlags = iPreCallback( this, iCopyFlags );

    FOdysseyVectorObject* objectCopy = CopyShape( copyFlags );

    if( objectCopy )
    {
        CopySettings( objectCopy, iCopyFlags );

        // recurse
        for( FOdysseyVectorObject *child : mChildrenList )
        {
            FOdysseyVectorObject *childCopy = child->Copy( copyFlags, iPreCallback, iPostCallback );

            objectCopy->AppendChild( childCopy );
        }

        // copy tags
        if( ( iCopyFlags & COPY_NOTAG ) == 0 )
        {
            for( FOdysseyVectorTag* tag : mTagList )
            {
                FOdysseyVectorTag* tagCopy = tag->Copy( objectCopy );

                objectCopy->AddTag( tagCopy );
            }
        }
    }

    iPostCallback( this, objectCopy, copyFlags );

    return objectCopy;
}

FOdysseyVectorObject*
FOdysseyVectorObject::Copy()
{
    return Copy( 0
               , []( FOdysseyVectorObject* object, uint64 ){ return 0; }
               , []( FOdysseyVectorObject* sourceObject
                   , FOdysseyVectorObject* objectCopy, uint64 ){ return 0; } );
}

// TODO: export flags
void
FOdysseyVectorObject::ExportParam( FOdysseyVectorObject* iDestinationObject, bool iInvalidate )
{
    iDestinationObject->mForegroundBucket.SetPaletteEntry( mForegroundBucket.GetPaletteEntry() );
    iDestinationObject->mForegroundBucket.SetPaletteSet( mForegroundBucket.GetPaletteSet() );
    iDestinationObject->mForegroundBucket.SetSolidColor( mForegroundBucket.GetSolidColor() );
    iDestinationObject->mForegroundBucket.SetColorMode( mForegroundBucket.GetColorMode() );
    iDestinationObject->mForegroundBucket.SetGradientColor0( mForegroundBucket.GetGradientColor0() );
    iDestinationObject->mForegroundBucket.SetGradientColor1( mForegroundBucket.GetGradientColor1() );

    iDestinationObject->mBackgroundBucket.SetPaletteEntry( mBackgroundBucket.GetPaletteEntry() );
    iDestinationObject->mBackgroundBucket.SetPaletteSet( mBackgroundBucket.GetPaletteSet() );
    iDestinationObject->mBackgroundBucket.SetSolidColor( mBackgroundBucket.GetSolidColor() );
    iDestinationObject->mBackgroundBucket.SetColorMode( mBackgroundBucket.GetColorMode() );
    iDestinationObject->mBackgroundBucket.SetGradientColor0( mBackgroundBucket.GetGradientColor0() );
    iDestinationObject->mBackgroundBucket.SetGradientColor1( mBackgroundBucket.GetGradientColor1() );

    iDestinationObject->SetOpacity( mOpacity );
    iDestinationObject->SetExpanded( IsExpanded() );

    if( iInvalidate )
    {
        iDestinationObject->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::COLOR)  );
    }
}

void
FOdysseyVectorObject::CopySettings( FOdysseyVectorObject* iDestinationObject, uint64 iCopyFlags )
{
    CopyTransformation( *iDestinationObject );

    iDestinationObject->UpdateMatrix();

    ExportParam( iDestinationObject, false );

    iDestinationObject->mBBox = mBBox;

    iDestinationObject->SetName( mName );
}

double
FOdysseyVectorObject::GetTranslationX()
{
    return mTranslationX;
}

double
FOdysseyVectorObject::GetTranslationY()
{
    return mTranslationY;
}

void
FOdysseyVectorObject::UpdateMatrix()
{
    mLocalMatrix.reset();
    mLocalMatrix.translate( mTranslationX, mTranslationY );
    mLocalMatrix.rotate( mRotation * M_PI / 180.0f );
    mLocalMatrix.scale( mScalingX, mScalingY );
    mLocalMatrix.skew( mSkewX, mSkewY );

    BLMatrix2D::invert( mInverseLocalMatrix, mLocalMatrix );

    if( mParent)
    {
        mWorldMatrix = mParent->mWorldMatrix;
        mWorldMatrix.transform( mLocalMatrix );

        BLMatrix2D::invert( mInverseWorldMatrix, mWorldMatrix );
    }
        else
    {
        memcpy( &mWorldMatrix       , &mLocalMatrix       , sizeof ( mLocalMatrix        ) );
        memcpy( &mInverseWorldMatrix, &mInverseLocalMatrix, sizeof ( mInverseLocalMatrix ) );
    }

    // recurse
    for( FOdysseyVectorObject *child : mChildrenList )
    {
        child->UpdateMatrix();
    }

    //update tags
    for( FOdysseyVectorTag* tag : mTagList )
    {
        tag->UpdateMatrix();
    }

    // Commented-out : flag reset is handle by Update()
    //mInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] = 0;
}

//static
/*
::ULIS::FRectD
FOdysseyVectorObject::GetBoundingBoxFromList( std::list<FOdysseyVectorObject*>& iObjectList )
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( FOdysseyVectorObject *obj : iObjectList )
    {
        ::ULIS::FRectD objBBox = obj->GetBBox( true );

        bbox = ( init == 0 ) ? objBBox : bbox | objBBox;

        init = 1;
    }

    return bbox;
}
*/

bool
FOdysseyVectorObject::HasAncestor( FOdysseyVectorObject* iCandidateAncestor )
{
    FOdysseyVectorObject* parent = mParent;

    while ( parent )
    {
        if( parent == iCandidateAncestor )
        {
            return true;
        }

        parent = parent->GetParent();
    }

    return false;
}

void
FOdysseyVectorObject::LockDrawing()
{
    mDrawingMutex.lock();
}

void
FOdysseyVectorObject::UnlockDrawing()
{
    mDrawingMutex.unlock();
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetAncestorByClass( uint32 iClass, bool iBaseClass, bool iSelf )
{
    FOdysseyVectorObject* parent = iSelf ? this : mParent;

    while ( parent )
    {
        if( ( iBaseClass && parent->HasBaseClass( iClass )  ) || parent->GetClass() == iClass )
        {
            return parent;
        }

        parent = parent->GetParent();
    }

    return nullptr;
}

::ULIS::FRectD
FOdysseyVectorObject::GetBBox( bool iInDepth, bool iWorld )
{
    ::ULIS::FRectD bbox = iInDepth ? mInDepthBBox : mBBox;

    if ( iWorld == true )
    {
        BLPoint p0 = mWorldMatrix.mapPoint( bbox.x         , bbox.y          );
        BLPoint p1 = mWorldMatrix.mapPoint( bbox.x + bbox.w, bbox.y          );
        BLPoint p2 = mWorldMatrix.mapPoint( bbox.x + bbox.w, bbox.y + bbox.h );
        BLPoint p3 = mWorldMatrix.mapPoint( bbox.x         , bbox.y + bbox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return bbox;
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetPreviousChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorObject* previousItem = nullptr;

    for( FOdysseyVectorObject* item : mChildrenList )
    {
        if( item == iChild )
        {
            return previousItem;
        }

        previousItem = item;
    }

    return previousItem;
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetLastChild()
{
    return mChildrenList.size() ? mChildrenList.back() : nullptr;
}

void
FOdysseyVectorObject::DrawChildren( BLContext* iBLContext
                                  , FOdysseyVectorEngine* iEngine
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iCombinedOpacity
                                  , uint64 iFlags )
{
    for( FOdysseyVectorObject *child : mChildrenList )
    {
        child->Draw( iBLContext, iEngine, iInvalidationArea, iCombinedOpacity, iFlags );
    }
}

void
FOdysseyVectorObject::DrawTags( BLContext* iBLContext
                              , FOdysseyVectorEngine* iEngine
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iCombinedOpacity
                              , uint64 iFlags )
{
    for( FOdysseyVectorTag *tag : mTagList )
    {
        tag->Draw( iBLContext, iEngine, iInvalidationArea, iCombinedOpacity, iFlags );
    }
}

void
FOdysseyVectorObject::Draw( BLContext* iBLContext
                          , FOdysseyVectorEngine* iEngine
                          , const ::ULIS::FRectD& iInvalidationArea
                          , double iAncestorsOpacity
                          , uint64 iFlags )
{
    double visible = bVisible ? 1.0f : 0.0f;
    double combinedOpacity = ( iAncestorsOpacity *= mOpacity ) * visible;

    LockDrawing();

    iBLContext->save();
    iBLContext->transform( mLocalMatrix );

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
    //Get sure everything is drawn before we draw in the BLend2D buffer.
    iBLContext->flush( BL_CONTEXT_FLUSH_SYNC  );

    DrawShape( iBLContext, iEngine, iInvalidationArea, combinedOpacity, iFlags );

    // get sure the parent has finished drawing before drawing its children
    iBLContext->flush( BL_CONTEXT_FLUSH_SYNC  );

    DrawChildren( iBLContext, iEngine, iInvalidationArea, combinedOpacity, iFlags );

    DrawTags( iBLContext, iEngine, iInvalidationArea, combinedOpacity, iFlags );

    iBLContext->restore();

    UnlockDrawing();
}

bool
FOdysseyVectorObject::IsInvalidated()
{
    return mInvalidationFlags.bits.any();
}

bool
FOdysseyVectorObject::IsSelected()
{
    return bSelected;
}

FOdysseyVectorTag*
FOdysseyVectorObject::GetTagByType( uint32 iTagClass )
{
    for( FOdysseyVectorTag* tag : mTagList )
    {
        if( tag->GetClass() == iTagClass )
        {
            return tag;
        }
    }

    return nullptr;
}

FOdysseyVectorObjectInvalidationFlags
FOdysseyVectorObject::GetInvalidationFlags()
{
    return mInvalidationFlags;
}

void
FOdysseyVectorObject::InvalidateChild( FOdysseyVectorObject* iChild
                                     , const FOdysseyVectorObjectInvalidationFlags& iChildInvalidationFlags )
{
    // this is temporary and should be optimized somehow
    if( std::find( mInvalidatedChildrenList.begin(), mInvalidatedChildrenList.end(), iChild ) == mInvalidatedChildrenList.end() )
        /*mInvalidationFlags & INVALIDATE_PARENT ) == 0*/
    {
        mInvalidatedChildrenList.push_back( iChild );
    }

    for( uint32 i = 0; i < iChildInvalidationFlags.bits.size(); i++ )
    {
        if( iChildInvalidationFlags.bits[i] )
        {
            mInvalidationFlags.Set( ( i < FOdysseyVectorObjectInvalidationFlags::FLAG_COUNT ) ? FOdysseyVectorObjectInvalidationFlags::FLAG_COUNT + i
                                                                                              : i );
        }
    }

    if( mParent )
    {
        mParent->InvalidateChild( this, iChildInvalidationFlags );
    }
}

void
FOdysseyVectorObject::Invalidate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags )
{
    if ( mParent )
    {
        mParent->InvalidateChild( this, iInvalidationFlags );
    }

    mInvalidationFlags.bits |= iInvalidationFlags.bits;
}

FOdysseyVectorCell*
FOdysseyVectorObject::GetCell()
{
    FOdysseyVectorObject* candidate = this;

    while ( candidate )
    {
        if( candidate->GetClass() == FOdysseyVectorCell::StaticClass() )
        {
            return static_cast<FOdysseyVectorCell*>(candidate);
        }

        candidate = candidate->GetParent();
    }

    return nullptr;
}

FOdysseyVectorLayer*
FOdysseyVectorObject::GetLayer()
{
    FOdysseyVectorObject* candidate = this;

    while ( candidate )
    {
        if( candidate->GetClass() == FOdysseyVectorLayer::StaticClass() )
        {
            return static_cast<FOdysseyVectorLayer*>(candidate);
        }

        candidate = candidate->GetParent();
    }

    return nullptr;
}

FOdysseyVectorGroupPaint*
FOdysseyVectorObject::GetScene()
{
    FOdysseyVectorCell* cell = GetCell();

    return cell ? cell->GetScene() : nullptr;
}

void
FOdysseyVectorObject::SetParent( FOdysseyVectorObject* iObject )
{
    mParent = iObject;
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetParent()
{
    return mParent;
}

void
FOdysseyVectorObject::SendBackward()
{
    if ( mParent )
    {
        std::list<FOdysseyVectorObject*>::iterator it1 = mParent->mChildrenList.begin();
        std::list<FOdysseyVectorObject*>::iterator it2 = it1++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it1 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                FOdysseyVectorObject *child = static_cast<FOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY) );

                    return;
                }
            }
        }
    }
}

void
FOdysseyVectorObject::BringForward()
{
    if ( mParent )
    {
        std::list<FOdysseyVectorObject*>::iterator it2 = mParent->mChildrenList.begin();
        std::list<FOdysseyVectorObject*>::iterator it1 = it2++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it2 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                FOdysseyVectorObject *child = static_cast<FOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY) );

                    return;
                }
            }
        }


    }
}

bool
FOdysseyVectorObject::PickShape( const ::ULIS::FRectD &iRoi
                               , const BLImage& iMaskImage )
{
    return false;
}

FOdysseyVectorObject*
FOdysseyVectorObject::Pick( FOdysseyVectorGroup* iSelectionSpace
                          , const ::ULIS::FRectD &iRoi
                          , const BLImage& iMaskImage )
{
    if( HasAncestor( iSelectionSpace ) )
    {
        bool picked = PickShape( iRoi, iMaskImage );

        // returns parent only if the parent is of Group type and is different from the selection space
        if( picked )
        {
            FOdysseyVectorObject* pickedObject = this;
            FOdysseyVectorObject* pickedObjectParent = pickedObject->GetParent();

            while( pickedObjectParent && ( pickedObjectParent != iSelectionSpace ) )
            {
                if( pickedObjectParent->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
                {
                    pickedObject = pickedObjectParent;
                }

                pickedObjectParent = pickedObjectParent->GetParent();
            }

            return pickedObject;
        }
    }

    return nullptr;
}

void
FOdysseyVectorObject::Recurse( void (FOdysseyVectorObject::*Func)() )
{
    (this->*Func)();

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        child->Recurse( Func );
    }
}

uint32
FOdysseyVectorObject::AppendChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorObject* lastItem = mChildrenList.size() ? mChildrenList.back() : nullptr;

    return AddChild( iChild, lastItem );
}

uint32
FOdysseyVectorObject::PrependChild( FOdysseyVectorObject* iChild )
{
    return AddChild( iChild, nullptr );
}

uint32
FOdysseyVectorObject::AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorCell* cell = GetCell();
    FOdysseyVectorObject* lastItem = GetLastChild();
    uint32 ret = HIERARCHY_CHANGE_ERROR;

    LockDrawing();

    if( HasAncestor( iChild ) == false )
    {
        iChild->mParent = this;

        if( iInsertAfter == nullptr )
        {
            mChildrenList.push_front( iChild );

            ret = HIERARCHY_CHANGE_SUCCESS;
        }
        else
        if( iInsertAfter == lastItem )
        {
            mChildrenList.push_back( iChild );

            ret = HIERARCHY_CHANGE_SUCCESS;
        }
        else
        {
            std::list<FOdysseyVectorObject*>::iterator it;

            for( it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
            {
                FOdysseyVectorObject* item = (*it);

                if( item == iInsertAfter )
                {
                    mChildrenList.insert( ++it, iChild );

                    ret = HIERARCHY_CHANGE_SUCCESS;

                    break;
                }
            }
        }

        iChild->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY) );
    }

    if( ret == HIERARCHY_CHANGE_SUCCESS )
    {
        iChild->Recurse( &FOdysseyVectorObject::Added );
    }

    UnlockDrawing();

    // auto invalidation of the whole region that needs to be redrawn
    if( cell )
    {
        cell->InvalidateRect();
    }

    return ret;
}

FOdysseyVectorObject*
FOdysseyVectorObject::GetOldParent()
{
    return mOldParent;
}

uint32
FOdysseyVectorObject::RemoveChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorCell* cell = GetCell();
    uint32 ret = HIERARCHY_CHANGE_ERROR;

    LockDrawing();

    if( iChild->mParent == this )
    {
        mChildrenList.remove( iChild );
        mInvalidatedChildrenList.remove( iChild );

        Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::CHILD_HIERARCHY) );
        // needed for undoing
        iChild->mOldParent = this;
        iChild->mParent = nullptr;

        // update now
        iChild->mInvalidationFlags.Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY);
        iChild->Update( 0 );

        ret = HIERARCHY_CHANGE_SUCCESS; // removal succeeded
    }

    if( ret == HIERARCHY_CHANGE_SUCCESS )
    {
        iChild->Recurse( &FOdysseyVectorObject::Removed );
    }

    UnlockDrawing();

    // auto invalidation of the whole region that needs to be redrawn
    if( cell )
    {
        cell->InvalidateRect();
    }

    return ret;
}

uint32
FOdysseyVectorObject::RemoveAllChildren()
{
    FOdysseyVectorCell* cell = GetCell();
    uint32 ret = HIERARCHY_CHANGE_ERROR;

    LockDrawing();

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY) );

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        // needed for undoing
        child->mOldParent = this;
        child->mParent = nullptr;

        // update now
        child->mInvalidationFlags.Set(FOdysseyVectorObjectInvalidationFlags::HIERARCHY);
        child->Update( 0 );

        child->Recurse( &FOdysseyVectorObject::Removed );
    }

    mChildrenList.clear();
    mInvalidatedChildrenList.clear();

    UnlockDrawing();

    // auto invalidation of the whole region that needs to be redrawn
    if( cell )
    {
        cell->InvalidateRect();
    }

    return HIERARCHY_CHANGE_SUCCESS;
}

uint32
FOdysseyVectorObject::TransferChild( FOdysseyVectorObject* iFosterChild
                                   , FOdysseyVectorObject* iInsertAfter )
{
    if( ( iFosterChild != this ) && ( iFosterChild != iInsertAfter ) )
    {
        FOdysseyVectorObject* formerParent = iFosterChild->GetParent();
        FOdysseyVectorObject* previousChild = formerParent->GetPreviousChild( iFosterChild );

        uint32 removalFlags = iFosterChild->GetParent()->RemoveChild( iFosterChild );

        // removal succeeded
        if( removalFlags == HIERARCHY_CHANGE_SUCCESS )
        {
            BLMatrix2D childFormerWorldMatrix = iFosterChild->mWorldMatrix;
            uint32 additionFlags = AddChild( iFosterChild, iInsertAfter );

            if( additionFlags == HIERARCHY_CHANGE_SUCCESS )
            {
                double translationX, translationY, rotation, scalingX, scalingY, skewX, skewY;
                BLMatrix2D localMatrix;

                FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, childFormerWorldMatrix, localMatrix );
                FOdysseyVector::ExtractTransformations( localMatrix
                                                      , &translationX
                                                      , &translationY
                                                      , &rotation
                                                      , &scalingX
                                                      , &scalingY
                                                      , &skewX
                                                      , &skewY
                                                      , true ); // in degrees

                iFosterChild->SetTransform( translationX
                                          , translationY
                                          , rotation
                                          , scalingX
                                          , scalingY
                                          , skewX
                                          , skewY );

                iFosterChild->UpdateMatrix();
            }
            else // add back
            {
                formerParent->AddChild( iFosterChild, previousChild );
            }

            return additionFlags; // transfer succeeded
        }

        return removalFlags;
    }

    return HIERARCHY_CHANGE_ERROR;
}

bool
FOdysseyVectorObject::HasChild( FOdysseyVectorObject* iChild )
{
    if( std::find( mChildrenList.begin(), mChildrenList.end(), iChild ) != mChildrenList.end() )
    {
        return true;
    }

    return false;
}

void
FOdysseyVectorObject::SetForegroundSolidColor( FColor& iColor )
{
    mForegroundBucket.SetSolidColor( iColor );
}

void
FOdysseyVectorObject::SetBackgroundSolidColor( FColor& iColor )
{
    mBackgroundBucket.SetSolidColor( iColor );
}

void
FOdysseyVectorObject::SetForegroundSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mForegroundBucket.SetSolidColor( iR, iG, iB, iA );
}

void
FOdysseyVectorObject::SetBackgroundSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mBackgroundBucket.SetSolidColor( iR, iG, iB, iA );
}

void
FOdysseyVectorObject::CopyTransformation( FOdysseyVectorObject& iDestinationObject )
{
    iDestinationObject.SetTransform( mTranslationX
                                   , mTranslationY
                                   , mRotation
                                   , mScalingX
                                   , mScalingY
                                   , mSkewX
                                   , mSkewY );
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorObject::GetChildrenList()
{
    return mChildrenList;
}

BLMatrix2D&
FOdysseyVectorObject::GetLocalMatrix()
{
    return mLocalMatrix;
}

BLMatrix2D&
FOdysseyVectorObject::GetWorldMatrix()
{
    return mWorldMatrix;
}

BLMatrix2D&
FOdysseyVectorObject::GetInverseLocalMatrix()
{
    return mInverseLocalMatrix;
}

BLMatrix2D&
FOdysseyVectorObject::GetInverseWorldMatrix()
{
    return mInverseWorldMatrix;
}

uint32
FOdysseyVectorObject::TreeToList( FOdysseyVectorObject* iObject, std::list<FOdysseyVectorObject*>& iOutList )
{
    iOutList.push_back( iObject );

    for( FOdysseyVectorObject* childObject : iObject->mChildrenList )
    {
        TreeToList( childObject, iOutList );
    }

    return iOutList.size();
}

std::list<FOdysseyVectorTag*>&
FOdysseyVectorObject::GetTagList()
{
    return mTagList;
}

//static
uint32
FOdysseyVectorObject::TreeToArray( FOdysseyVectorObject* iObject, std::vector<FOdysseyVectorObject*>& iOutArray )
{
    uint32 size = iObject->mChildrenList.size();

    iOutArray.push_back( iObject );

    for( FOdysseyVectorObject* childObject : iObject->mChildrenList )
    {
        TreeToArray( childObject, iOutArray );
    }

    return iOutArray.size();
}

FColor
FOdysseyVectorObject::GetForegroundColor()
{
    return mForegroundBucket.GetColor();
}

FColor
FOdysseyVectorObject::GetBackgroundColor()
{
    return mBackgroundBucket.GetColor();
}

void
FOdysseyVectorObject::SetForegroundBucket( const FOdysseyVectorBucket& iBucket  )
{
    mForegroundBucket = iBucket;
}

void
FOdysseyVectorObject::SetBackgroundBucket( const FOdysseyVectorBucket& iBucket )
{
    mBackgroundBucket = iBucket;
}

uint32
FOdysseyVectorObject::GetID()
{
    return mID;
}

void
FOdysseyVectorObject::SetID( uint32 iID )
{
    mID = iID;
}

// static
void
FOdysseyVectorObject::FlipObjectsHorizontal( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    FlipObjects( iObjectList, -1.0f, 1.0f );
}

// static
void
FOdysseyVectorObject::FlipObjectsVertical( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    FlipObjects( iObjectList, 1.0f, -1.0f );
}

// static
void
FOdysseyVectorObject::FlipObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                 , double iXFactor
                                 , double iYFactor )
{
    ::ULIS::FVec2D axisPosition = GetPositionFromObjects( iObjectList );
    BLMatrix2D inverseAxisMatrix;
    BLMatrix2D axisMatrix;
    BLMatrix2D flippingMatrix;

    axisMatrix.reset();
    axisMatrix.translate( axisPosition.x, axisPosition.y );

    BLMatrix2D::invert( inverseAxisMatrix, axisMatrix );

    flippingMatrix.resetToScaling( iXFactor, iYFactor );

    for( FOdysseyVectorObject *object : iObjectList )
    {
        BLMatrix2D& objectWorldMatrix = object->GetWorldMatrix();
        BLPoint objectWorldCenter = objectWorldMatrix.mapPoint( 0.0f, 0.0f );
        BLPoint objectLocalCenter = inverseAxisMatrix.mapPoint( objectWorldCenter );
        BLPoint objectLocalFlippedCenter = flippingMatrix.mapPoint( objectLocalCenter );
        BLPoint objectWorldFlippedCenter = axisMatrix.mapPoint( objectLocalFlippedCenter );

        objectLocalCenter = object->GetParent()->GetInverseWorldMatrix().mapPoint( objectWorldFlippedCenter );

        object->Translate( objectLocalCenter.x, objectLocalCenter.y );
        object->Scale( iXFactor * object->GetScalingX(), iYFactor * object->GetScalingY() );
        object->Rotate( -object->GetRotation() );

        object->UpdateMatrix();
    }
}

// static
FOdysseyVectorGroup*
FOdysseyVectorObject::GroupObjects( FOdysseyVectorObject* iParent
                                  , const std::list<FOdysseyVectorObject*>& iObjectList
                                  , std::vector<FOdysseyVectorObject*>& oObjectArray )
{
    FOdysseyVectorGroupPaint* scene = iParent->GetScene();
    BLPoint averageTranslation = { 0.0f, 0.0f };

    if ( iObjectList.size() )
    {
        FOdysseyVectorGroup* group = new FOdysseyVectorGroup( FString("Group") );

        iParent->AppendChild ( group );

        //group->Translate( averageTranslation.x, averageTranslation.y );
        group->UpdateMatrix();

        oObjectArray.reserve( iObjectList.size() );

        for( FOdysseyVectorObject *obj : iObjectList )
        {
            if( obj != scene )
            {
                oObjectArray.push_back( obj );

                group->TransferChild( obj, group->GetLastChild() );
            }
        }

        //group->Invalidate();

        return group;
    }

    return nullptr;
}

// Execute callback on object tree
// static
uint64
FOdysseyVectorObject::Traverse( FOdysseyVectorObject* iObject
                              , uint64 iTraversalFlags
                              , std::function<uint64(FOdysseyVectorObject*,uint64)> iCallback )
{
    uint64 objectTraversalFlags = iCallback( iObject, iTraversalFlags );

    if( objectTraversalFlags & TRAVERSE_STOP )
    {
        return TRAVERSE_STOP;
    }

    if( objectTraversalFlags & TRAVERSE_OBJECT_ACCEPTED )
    {
        iTraversalFlags |= TRAVERSE_PARENT_HASFOCUS;
    }

    if( ( objectTraversalFlags & TRAVERSE_OBJECT_IGNORE_CHILDREN ) == 0 )
    {
        for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
        {
            uint64 childTraversalFlags = Traverse( childObject, iTraversalFlags, iCallback );

            if( childTraversalFlags & TRAVERSE_STOP )
            {
                return TRAVERSE_STOP;
            }
        }
    }

    return 0;
}

// static
FOdysseyVectorGroupPaint*
FOdysseyVectorObject::MakePaintGroupFromObjects( FOdysseyVectorObject* iParent
                                               , const std::list<FOdysseyVectorObject*>& iObjectList
                                               , std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                               , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray )
{
    // this array will help us to transfer buckets as well
    std::vector<FOdysseyVectorGroupPaint*> parentPaintGroupArray;

    if( iObjectList.size() )
    {
        FOdysseyVectorGroupPaint* paintGroup = new FOdysseyVectorGroupPaint( "Paint Group" );

        iParent->AppendChild( paintGroup );

        paintGroup->UpdateMatrix();

        for( FOdysseyVectorObject* selectedObject : iObjectList )
        {
            if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* selectedCubicPath = static_cast<FOdysseyVectorPath*>( selectedObject );

                oCubicPathArray.push_back( selectedCubicPath );
            }
        }

        for( int i = 0; i < oCubicPathArray.size(); i++ )
        {
            FOdysseyVectorObject* parentObject = oCubicPathArray[i]->GetParent();

            paintGroup->TransferChild( oCubicPathArray[i], paintGroup->GetLastChild() );

            // take andvantge of this loop to also extract buckets if over
            if( parentObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(parentObject);

                if( std::find( parentPaintGroupArray.begin(), parentPaintGroupArray.end(), parentPaintGroup ) == parentPaintGroupArray.end() )
                {
                    parentPaintGroupArray.push_back( parentPaintGroup );
                    // the gap tolerance will be set multiple times
                    // if there are multiples former parent paintgroups
                    // but this does not matter.
                    paintGroup->SetGapTolerance( parentPaintGroup->GetGapTolerance() );
                }
            }
        }

        // update paths and detect cycles for bucket matching
        paintGroup->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        // extract buckets if over
        for( FOdysseyVectorGroupPaint* parentPaintGroup : parentPaintGroupArray )
        {
            std::list<FOdysseyVectorBucket*>& bucketList = parentPaintGroup->GetBucketList();
            BLMatrix2D& parentWorldMatrix = parentPaintGroup->GetWorldMatrix();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                ::ULIS::FVec2D& bucketCoords = bucket->GetCoords();
                BLPoint bucketWorldCoords = parentWorldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );

                // TODO: that's a lot of conversion, kowing that both PickCycle
                // and FOdysseyVectorBucket() convert to their own space.
                // We can optimize by creating a PickCycle
                // with local coordinates as parameter. Same for FOdysseyVectorBucket()
                if( paintGroup->PickCycle( bucketWorldCoords.x, bucketWorldCoords.y ) )
                {
                    FOdysseyVectorBucket* importedBucket = new FOdysseyVectorBucket( paintGroup, bucket );

                    paintGroup->AddBucket( importedBucket );

                    oRemovedBucketArray.push_back( bucket );
                }
            }
        }

        // we can't remove the bucket in the previous loop as it would alter the std:list
        // we are looping into (unless we copy the list, yes I know).
        for( int i = 0; i < oRemovedBucketArray.size(); i++ )
        {
            FOdysseyVectorObject* ownerObject = oRemovedBucketArray[i]->GetOwner();

            if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

                parentPaintGroup->RemoveBucket( oRemovedBucketArray[i] );
            }
        }

        return paintGroup;
    }

    return nullptr;
}

// static
::ULIS::FVec2D
FOdysseyVectorObject::GetPositionFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    BLPoint averagePosition = BLPoint( 0.0f, 0.0f );

    if ( iObjectList.size() )
    {
        for( FOdysseyVectorObject* obj : iObjectList )
        {
            ::ULIS::FRectD bbox = obj->GetBBox( false, true );
            BLPoint middle = BLPoint( bbox.x + bbox.w * 0.5f
                                    , bbox.y + bbox.h * 0.5f );

            averagePosition.x += middle.x;
            averagePosition.y += middle.y;
        }

        averagePosition.x /= iObjectList.size();
        averagePosition.y /= iObjectList.size();
    }

    return ::ULIS::FVec2D( averagePosition.x, averagePosition.y );
}
