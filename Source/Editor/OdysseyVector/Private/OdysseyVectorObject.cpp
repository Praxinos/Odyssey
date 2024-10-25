#include "OdysseyVectorObject.h"
#include "OdysseyVector.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorRoot.h"
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
    , bIsSystem( false )
    , mBackgroundBucket( this, 0.0f, 0.0f, false )
    , mForegroundBucket( this, 0.0f, 0.0f, false )
    , mInvalidationFlags ( 0 )
{
    mLocalMatrix.reset();
    mWorldMatrix.reset();
    mInverseLocalMatrix.reset();
    mInverseWorldMatrix.reset();

    SetName( iName );
    SetOpacity( 1.0f );
    SetExpanded(true);
    SetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f );

    mForegroundBucket.SetColorMode( eBucketColorMode::SolidColor );
    mForegroundBucket.SetSolidColor( 0, 0, 0, 255 );

    mBackgroundBucket.SetColorMode( eBucketColorMode::SolidColor );
    mBackgroundBucket.SetSolidColor( 0, 0, 0, 0 );
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
}

void
FOdysseyVectorObject::AddTag( FOdysseyVectorTag* iTag )
{
    mTagList.push_back( iTag );

    iTag->Added();

    Invalidate( FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST );
}

void
FOdysseyVectorObject::RemoveTag( FOdysseyVectorTag* iTag )
{
    mTagList.remove( iTag );

    iTag->Removed();

    Invalidate( FOdysseyVectorObject::INVALIDATE_CHILD_TAG_LIST );
}

void
FOdysseyVectorObject::SetName( const FString& iName )
{
    mName = iName;
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
    return bIsSystem;
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
FOdysseyVectorObject::Update( uint32 iUpdateFlags )
{

    if( mInvalidationFlags )
    {
        // prevents DrawShape() to be called while the object has'nt been updated
        // Indeed, it could be called by the animation proxy
        mDrawingMutex.lock();

        // update children first by recursively calling the Update function and, if needed,
        // removing the object from the invalidated object list, in the same call.
        mInvalidatedChildrenList.remove_if( [iUpdateFlags] ( FOdysseyVectorObject* child )
                                            {
                                                child->Update( iUpdateFlags );

                                                return child->IsInvalidated() == false;
                                            } );
        UpdateShape( iUpdateFlags );

        // update tags
        for( FOdysseyVectorTag* tag : mTagList )
        {
            tag->Update( iUpdateFlags, mInvalidationFlags );
        }

        if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
        {
            mInvalidationFlags = 0;
        }

        mDrawingMutex.unlock();
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
        double translationX, translationY, rotation, scalingX, scalingY;

        FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, childWorldMatrix, localMatrix );

        FOdysseyVector::ExtractTransformations( localMatrix
                                              , &translationX
                                              , &translationY
                                              , &rotation
                                              , &scalingX
                                              , &scalingY );

        child->SetTransform( translationX
                           , translationY
                           , rotation / M_PI * 180.0f
                           , scalingX
                           , scalingY );
    }

    SetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f );

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
                                          , &mScalingY );

    //UpdateMatrix();
}

void
FOdysseyVectorObject::Translate( double iX, double iY )
{
    mTranslationX = iX;
    mTranslationY = iY;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::Rotate( double iAngle )
{
    mRotation = iAngle;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::Scale( double iX, double iY )
{
    mScalingX = iX;
    mScalingY = iY;

    Invalidate( INVALIDATE_MATRIX );
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
FOdysseyVectorObject::GetRotation()
{
    return mRotation;
}

void
FOdysseyVectorObject::GetTransform( double& oTranslationX
                                  , double& oTranslationY
                                  , double& oRotation
                                  , double& oScalingX
                                  , double& oScalingY )
{
    oTranslationX = mTranslationX;
    oTranslationY = mTranslationY;
    oRotation = mRotation;
    oScalingX = mScalingX;
    oScalingY = mScalingY;
}

void
FOdysseyVectorObject::SetTransform( double iTranslationX
                                  , double iTranslationY
                                  , double iRotation
                                  , double iScalingX
                                  , double iScalingY )
{
    mTranslationX = iTranslationX;
    mTranslationY = iTranslationY;
    mRotation = iRotation;
    mScalingX = iScalingX;
    mScalingY = iScalingY;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::ResetTransform()
{
    Translate( 0.0f, 0.0f );
    Scale( 1.0f, 1.0f );
    Rotate( 0.0f );
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
        CopySettings( objectCopy );

        // recurse
        for( FOdysseyVectorObject *child : mChildrenList )
        {
            FOdysseyVectorObject *childCopy = child->Copy( copyFlags, iPreCallback, iPostCallback );

            objectCopy->AppendChild( childCopy );
        }

        // copy tags
        for( FOdysseyVectorTag* tag : mTagList )
        {
            FOdysseyVectorTag* tagCopy = tag->Copy( objectCopy );

            objectCopy->AddTag( tagCopy );
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
    iDestinationObject->mForegroundBucket.SetSolidColor( mForegroundBucket.GetSolidColor() );
    iDestinationObject->mForegroundBucket.SetColorMode( mForegroundBucket.GetColorMode() );
    iDestinationObject->mForegroundBucket.SetGradientColor0( mForegroundBucket.GetGradientColor0() );
    iDestinationObject->mForegroundBucket.SetGradientColor1( mForegroundBucket.GetGradientColor1() );

    iDestinationObject->mBackgroundBucket.SetPaletteEntry( mBackgroundBucket.GetPaletteEntry() );
    iDestinationObject->mBackgroundBucket.SetSolidColor( mBackgroundBucket.GetSolidColor() );
    iDestinationObject->mBackgroundBucket.SetColorMode( mBackgroundBucket.GetColorMode() );
    iDestinationObject->mBackgroundBucket.SetGradientColor0( mBackgroundBucket.GetGradientColor0() );
    iDestinationObject->mBackgroundBucket.SetGradientColor1( mBackgroundBucket.GetGradientColor1() );

    iDestinationObject->SetOpacity( mOpacity );

    if( iInvalidate )
    {
        iDestinationObject->Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR  );
    }
}

void
FOdysseyVectorObject::CopySettings( FOdysseyVectorObject* iDestinationObject )
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
    FOdysseyVectorGroupPaint* scene = GetScene();

    if( scene )
    {
        mLocalMatrix.reset();
        mLocalMatrix.translate( mTranslationX, mTranslationY );
        mLocalMatrix.rotate( mRotation * M_PI / 180.0f );
        mLocalMatrix.scale( mScalingX, mScalingY );

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

        mInvalidationFlags &= (~INVALIDATE_MATRIX);
    }
}

//static
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

::ULIS::FRectD
FOdysseyVectorObject::GetBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint p0 = mWorldMatrix.mapPoint( mBBox.x          , mBBox.y           );
        BLPoint p1 = mWorldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y           );
        BLPoint p2 = mWorldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y + mBBox.h );
        BLPoint p3 = mWorldMatrix.mapPoint( mBBox.x          , mBBox.y + mBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return mBBox;
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
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iCombinedOpacity
                                  , uint64 iFlags )
{
    for( FOdysseyVectorObject *child : mChildrenList )
    {
        child->Draw( iBLContext, iInvalidationArea, iCombinedOpacity, iFlags );
    }
}

void
FOdysseyVectorObject::DrawTags( BLContext* iBLContext
                              , const ::ULIS::FRectD& iInvalidationArea
                              , double iCombinedOpacity
                              , uint64 iFlags )
{
    for( FOdysseyVectorTag *tag : mTagList )
    {
        tag->Draw( iBLContext, iInvalidationArea, iCombinedOpacity, iFlags );
    }
}

void
FOdysseyVectorObject::Draw( BLContext* iBLContext
                          , const ::ULIS::FRectD& iInvalidationArea
                          , double iAncestorsOpacity
                          , uint64 iFlags )
{
    double combinedOpacity = iAncestorsOpacity *= mOpacity;

    iBLContext->save();
    iBLContext->transform( mLocalMatrix );

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );
    //Get sure everything is drawn before we draw in the BLend2D buffer.
    iBLContext->flush( BL_CONTEXT_FLUSH_SYNC  );

    mDrawingMutex.lock();

    DrawShape( iBLContext, iInvalidationArea, combinedOpacity, iFlags );

    mDrawingMutex.unlock();

    // get sure the parent has finished drawing before drawing its children
    iBLContext->flush( BL_CONTEXT_FLUSH_SYNC  );

    DrawChildren( iBLContext, iInvalidationArea, combinedOpacity, iFlags );

    DrawTags( iBLContext, iInvalidationArea, combinedOpacity, iFlags );

    iBLContext->restore();
}

bool
FOdysseyVectorObject::IsInvalidated()
{
    return mInvalidationFlags != 0 ? true : false;
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

uint64
FOdysseyVectorObject::GetInvalidationFlags()
{
    return mInvalidationFlags;
}

void
FOdysseyVectorObject::InvalidateChild( FOdysseyVectorObject* iChild
                                     , uint64 iChildInvalidationFlags )
{
    // this is temporary and should be optimized somehow
    if( std::find( mInvalidatedChildrenList.begin(), mInvalidatedChildrenList.end(), iChild ) == mInvalidatedChildrenList.end() )
        /*mInvalidationFlags & INVALIDATE_PARENT ) == 0*/
    {
        mInvalidatedChildrenList.push_back( iChild );
    }

    Invalidate(   ( iChildInvalidationFlags & INVALIDATE_HIERARCHY )
              | ( ( iChildInvalidationFlags & INVALIDATE_SHAPE     ) << INVALIDATE_CHILD_SHIFT )
              | ( ( iChildInvalidationFlags & INVALIDATE_COLOR     ) << INVALIDATE_CHILD_SHIFT )
              | ( ( iChildInvalidationFlags & INVALIDATE_TOPOLOGY  ) << INVALIDATE_CHILD_SHIFT )
              | ( ( iChildInvalidationFlags & INVALIDATE_TAG       ) << INVALIDATE_CHILD_SHIFT )
              | ( ( iChildInvalidationFlags & INVALIDATE_TAG_LIST  ) << INVALIDATE_CHILD_SHIFT )
              | ( ( iChildInvalidationFlags & INVALIDATE_MATRIX    ) << INVALIDATE_CHILD_SHIFT )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_SHAPE    )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_COLOR    )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_TAG      )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_TAG_LIST )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_TOPOLOGY )
              |   ( iChildInvalidationFlags & INVALIDATE_CHILD_MATRIX   ) );
}

void
FOdysseyVectorObject::InvalidateTag( FOdysseyVectorTag* iTag )
{
    Invalidate( FOdysseyVectorObject::INVALIDATE_TAG );
}

void
FOdysseyVectorObject::Invalidate( uint64 iInvalidationFlags )
{
    if ( mParent )
    {
        mParent->InvalidateChild( this, iInvalidationFlags );
    }

    mInvalidationFlags |= ( INVALIDATE_DEFAULT | iInvalidationFlags );
}

FOdysseyVectorEngine*
FOdysseyVectorObject::GetEngine()
{
    FOdysseyVectorRoot* root = GetRoot();

    return root ? root->GetEngine() : nullptr;
}

FOdysseyVectorRoot*
FOdysseyVectorObject::GetRoot()
{
    FOdysseyVectorObject* candidate = this;

    while ( candidate )
    {
        if( candidate->GetClass() == FOdysseyVectorRoot::StaticClass() )
        {
            return static_cast<FOdysseyVectorRoot*>(candidate);
        }

        candidate = candidate->GetParent();
    }

    return nullptr;
}

FOdysseyVectorSharedEnv*
FOdysseyVectorObject::GetSharedEnv()
{
    FOdysseyVectorObject* candidate = this;

    while ( candidate )
    {
        if( candidate->GetClass() == FOdysseyVectorSharedEnv::StaticClass() )
        {
            return static_cast<FOdysseyVectorSharedEnv*>(candidate);
        }

        candidate = candidate->GetParent();
    }

    return nullptr;
}

FOdysseyVectorGroupPaint*
FOdysseyVectorObject::GetScene()
{
    FOdysseyVectorEngine* engine = GetEngine();

    return engine ? engine->GetScene() : nullptr;
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

                    return;
                }
            }
        }
    } 
}

FOdysseyVectorObject*
FOdysseyVectorObject::Pick( FOdysseyVectorGroup* iSelectionSpace, const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( HasAncestor( iSelectionSpace ) )
    {
        bool picked = PickShape( iRoi, iSelectionFlags );

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
    /*
            // Note: We cannot use dynamic casting with UE, this is not an ideal bypass
            if( ( this->mParent->GetClass() == FOdysseyVectorGroup::StaticClass()      )
                || ( this->mParent->GetClass() == FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                if( this->mParent != iSelectionSpace )
                {
                    return this->mParent;
                }
            }

            return this;
    */
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
    FOdysseyVectorObject* lastItem = GetLastChild();
    uint32 ret = HIERARCHY_CHANGE_ERROR;

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

        iChild->Invalidate( INVALIDATE_HIERARCHY );
    }

    if( ret == HIERARCHY_CHANGE_SUCCESS )
    {
        iChild->Recurse( &FOdysseyVectorObject::Added );
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
    uint32 ret = HIERARCHY_CHANGE_ERROR;

    if( iChild->mParent == this )
    {
        mChildrenList.remove( iChild );
        mInvalidatedChildrenList.remove( iChild );

        Invalidate( INVALIDATE_HIERARCHY );
        // needed for undoing
        iChild->mOldParent = this;
        iChild->mParent = nullptr;

        // update now
        iChild->mInvalidationFlags |= INVALIDATE_HIERARCHY;
        iChild->Update( 0 );

        ret = HIERARCHY_CHANGE_SUCCESS; // removal succeeded
    }

    if( ret == HIERARCHY_CHANGE_SUCCESS )
    {
        iChild->Recurse( &FOdysseyVectorObject::Removed );
    }

    return ret;
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
                double translationX, translationY, rotation, scalingX, scalingY;
                BLMatrix2D localMatrix;

                FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, childFormerWorldMatrix, localMatrix );
                FOdysseyVector::ExtractTransformations( localMatrix, &translationX, &translationY, &rotation, &scalingX, &scalingY );

                iFosterChild->SetTransform( translationX
                                          , translationY
                                          , rotation / M_PI * 180.0f
                                          , scalingX
                                          , scalingY );

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
                                   , mScalingY );
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
