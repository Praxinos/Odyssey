#include "OdysseyVectorObject.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorObject::~FOdysseyVectorObject()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        delete obj;
    }

    mChildrenList.clear();
}

FOdysseyVectorObject::FOdysseyVectorObject( const FString& iName )
    : mParent( nullptr )
    , mIsSelected( false )
    , mIsExpanded( false )
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

void
FOdysseyVectorObject::SetOpacity( double iOpacity )
{
    mObjectParam.Opacity = iOpacity;
}

void
FOdysseyVectorObject::SetName( const FString& iName )
{
    mObjectParam.Name = iName;
}

FString&
FOdysseyVectorObject::GetName()
{
    return mObjectParam.Name;
}

void
FOdysseyVectorObject::SetExpanded( bool iIsExpanded )
{
    mIsExpanded = iIsExpanded;
}

bool
FOdysseyVectorObject::IsExpanded()
{
    return mIsExpanded;
}

static uint32
CheckCommonClass( std::list<FOdysseyVectorObject*>& iObjectList, uint32 iCommonClass )
{
    std::list<FOdysseyVectorObject*>::iterator it;

    if( iObjectList.size() )
    {
        for( it = iObjectList.begin(); it != iObjectList.end(); ++it )
        {
            FOdysseyVectorObject* object = (*it);

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
FOdysseyVectorObject::UpdateShape( uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidationFlags = 0;
    }
}

void
FOdysseyVectorObject::Update( uint32 iUpdateFlags )
{
    if( mInvalidationFlags )
    {
        // update children first by recursively calling the Update function and, if needed,
        // removing the object from the invalidated object list, in the same call.
        mInvalidatedChildrenList.remove_if( [iUpdateFlags] ( FOdysseyVectorObject* child )
                                            {
                                                child->Update( iUpdateFlags );

                                                return child->IsInvalidated() == false;
                                            } );

        UpdateShape( iUpdateFlags );
    }
}

void
FOdysseyVectorObject::SetIsSelected( bool iIsSelected )
{
    mIsSelected = iIsSelected;
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
                                          , &mObjectParam.TranslationX
                                          , &mObjectParam.TranslationY
                                          , &mObjectParam.Rotation
                                          , &mObjectParam.ScalingX
                                          , &mObjectParam.ScalingY );

    //UpdateMatrix();
}

void
FOdysseyVectorObject::Translate( double iX, double iY )
{
    mObjectParam.TranslationX = iX;
    mObjectParam.TranslationY = iY;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::Rotate( double iAngle )
{
    mObjectParam.Rotation = iAngle;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::Scale( double iX, double iY )
{
    mObjectParam.ScalingX = iX;
    mObjectParam.ScalingY = iY;

    Invalidate( INVALIDATE_MATRIX );
}

double
FOdysseyVectorObject::GetScalingX()
{
    return mObjectParam.ScalingX;
}

double
FOdysseyVectorObject::GetScalingY()
{
    return mObjectParam.ScalingY;
}

double
FOdysseyVectorObject::GetRotation()
{
    return mObjectParam.Rotation;
}

void
FOdysseyVectorObject::GetTransform( double& oTranslationX
                                  , double& oTranslationY
                                  , double& oRotation
                                  , double& oScalingX
                                  , double& oScalingY )
{
    oTranslationX = mObjectParam.TranslationX;
    oTranslationY = mObjectParam.TranslationY;
    oRotation = mObjectParam.Rotation;
    oScalingX = mObjectParam.ScalingX;
    oScalingY = mObjectParam.ScalingY;
}

void
FOdysseyVectorObject::SetTransform( double iTranslationX
                                  , double iTranslationY
                                  , double iRotation
                                  , double iScalingX
                                  , double iScalingY )
{
    mObjectParam.TranslationX = iTranslationX;
    mObjectParam.TranslationY = iTranslationY;
    mObjectParam.Rotation = iRotation;
    mObjectParam.ScalingX = iScalingX;
    mObjectParam.ScalingY = iScalingY;

    Invalidate( INVALIDATE_MATRIX );
}

void
FOdysseyVectorObject::ResetTransform()
{
    Translate( 0.0f, 0.0f );
    Scale( 1.0f, 1.0f );
    Rotate( 0.0f );
}

FOdysseyVectorObject*
FOdysseyVectorObject::Copy()
{
    FOdysseyVectorObject* objectCopy = CopyShape();

   // TODO, update matrices once we get a BLContext object

    if( objectCopy )
    {
        CopySettings( *objectCopy ); // we need the matrices to properly import the child

        // recurse
        for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            FOdysseyVectorObject *child = (*it);
            FOdysseyVectorObject *childCopy = child->Copy() ;

            objectCopy->AppendChild( childCopy );
        }
    }

    return objectCopy;
}

void
FOdysseyVectorObject::CopySettings( FOdysseyVectorObject& iDestinationObject )
{
    CopyTransformation( iDestinationObject );

    iDestinationObject.UpdateMatrix();

    iDestinationObject.mForegroundBucket.SetPaletteEntry( mForegroundBucket.GetPaletteEntry() );
    iDestinationObject.mForegroundBucket.SetSolidColor( mForegroundBucket.GetSolidColor() );
    iDestinationObject.mForegroundBucket.SetColorMode( mForegroundBucket.GetColorMode() );
    iDestinationObject.mForegroundBucket.SetGradientColor0( mForegroundBucket.GetGradientColor0() );
    iDestinationObject.mForegroundBucket.SetGradientColor1( mForegroundBucket.GetGradientColor1() );

    iDestinationObject.mBackgroundBucket.SetPaletteEntry( mBackgroundBucket.GetPaletteEntry() );
    iDestinationObject.mBackgroundBucket.SetSolidColor( mBackgroundBucket.GetSolidColor() );
    iDestinationObject.mBackgroundBucket.SetColorMode( mBackgroundBucket.GetColorMode() );
    iDestinationObject.mBackgroundBucket.SetGradientColor0( mBackgroundBucket.GetGradientColor0() );
    iDestinationObject.mBackgroundBucket.SetGradientColor1( mBackgroundBucket.GetGradientColor1() );

    iDestinationObject.mBBox = mBBox;

    iDestinationObject.SetName( mObjectParam.Name );
    iDestinationObject.SetOpacity( mObjectParam.Opacity );
}

double
FOdysseyVectorObject::GetTranslationX()
{
    return mObjectParam.TranslationX;
}

double
FOdysseyVectorObject::GetTranslationY()
{
    return mObjectParam.TranslationY;
}

void
FOdysseyVectorObject::UpdateMatrix()
{
    UpdateMatrix( true );
}

void
FOdysseyVectorObject::UpdateMatrix( bool iInvalidate )
{
    FOdysseyVectorScene* scene = GetScene();

    if( scene )
    {
        BLContext* blctx = scene->GetEngine()->GetBLContext();

        blctx->save();

        blctx->resetMatrix();
        blctx->translate( mObjectParam.TranslationX, mObjectParam.TranslationY );
        blctx->rotate( mObjectParam.Rotation * M_PI / 180.0f );
        blctx->scale( mObjectParam.ScalingX, mObjectParam.ScalingY );
        mLocalMatrix = blctx->userMatrix();

        BLMatrix2D::invert( mInverseLocalMatrix, mLocalMatrix );

        if( mParent)
        {
            blctx->setMatrix( mParent->mWorldMatrix );
            blctx->transform( mLocalMatrix );
            mWorldMatrix = blctx->userMatrix();

            BLMatrix2D::invert( mInverseWorldMatrix, mWorldMatrix );
        }
         else
        {
            memcpy( &mWorldMatrix       , &mLocalMatrix       , sizeof ( mLocalMatrix        ) );
            memcpy( &mInverseWorldMatrix, &mInverseLocalMatrix, sizeof ( mInverseLocalMatrix ) );
        }

        // recurse
        for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            FOdysseyVectorObject *child = (*it);

            child->UpdateMatrix( false );
        }

        blctx->restore();

        mInvalidationFlags &= (~INVALIDATE_MATRIX);
    }
/*
    if( iInvalidate )
    {
        if( mParent )
        {
            mParent->Invalidate( mParent->mInvalidationFlags | INVALIDATE_CHILD );
        }
    }
*/
}

//static
::ULIS::FRectD
FOdysseyVectorObject::GetBoundingBoxFromList( std::list<FOdysseyVectorObject*>& iObjectList )
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);
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

bool
FOdysseyVectorObject::HasSelectedAncestor()
{
    FOdysseyVectorObject* parent = mParent;

    while ( parent )
    {
        if( parent->IsSelected() == true )
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
FOdysseyVectorObject::TransferChild( FOdysseyVectorObject* iFosterChild, FOdysseyVectorObject* iInsertAfter )
{
    double translationX, translationY, rotation, scalingX, scalingY;
    BLMatrix2D localMatrix;

    FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, iFosterChild->mWorldMatrix, localMatrix );
    FOdysseyVector::ExtractTransformations( localMatrix, &translationX, &translationY, &rotation, &scalingX, &scalingY );

    iFosterChild->GetParent()->RemoveChild( iFosterChild );
    
    AddChild( iFosterChild, iInsertAfter );

    iFosterChild->SetTransform( translationX
                              , translationY
                              , rotation / M_PI * 180.0f
                              , scalingX
                              , scalingY );

    iFosterChild->UpdateMatrix();
}

void
FOdysseyVectorObject::DrawChildren( uint64 iFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        child->Draw( iFlags );
    }
}

void
FOdysseyVectorObject::Draw( uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    blctx->save();
    blctx->transform( mLocalMatrix );

    blctx->setCompOp( BL_COMP_OP_SRC_OVER );

    DrawShape( iFlags );

    DrawChildren( iFlags );

    blctx->restore();
}

bool
FOdysseyVectorObject::IsInvalidated()
{
    return mInvalidationFlags != 0 ? true : false;
}

bool
FOdysseyVectorObject::IsSelected()
{
    return mIsSelected;
}

void
FOdysseyVectorObject::InvalidateChild( FOdysseyVectorObject* iChild )
{
    // this is temporary and should be optimized somehow
    if( std::find( mInvalidatedChildrenList.begin(), mInvalidatedChildrenList.end(), iChild ) == mInvalidatedChildrenList.end() )
        /*mInvalidationFlags & INVALIDATE_PARENT ) == 0*/
    {
        mInvalidatedChildrenList.push_back( iChild );
    }

    Invalidate( INVALIDATE_CHILD );
}

void
FOdysseyVectorObject::Invalidate()
{
    Invalidate( FOdysseyVectorObject::INVALIDATE_ALL );
}

void
FOdysseyVectorObject::Invalidate( uint32 iInvalidationFlags )
{
    if ( mParent )
    {
        mParent->InvalidateChild( this );
    }

    mInvalidationFlags |= iInvalidationFlags;
}

FOdysseyVectorScene*
FOdysseyVectorObject::GetScene()
{
    FOdysseyVectorObject* parent = mParent;
    FOdysseyVectorObject* root = this;

    while ( parent )
    {
        root = parent;

        parent = parent->GetParent();
    }

    return ( root->GetClass() == FOdysseyVectorEngine::StaticClass() ) ?  static_cast<FOdysseyVectorEngine*>(root)->GetScene() : nullptr;
}

void
FOdysseyVectorObject::SetParent( FOdysseyVectorObject* iObject )
{
    mParent = iObject;
}

::ULIS::FVec2D
FOdysseyVectorObject::WorldCoordinatesToLocal( double iX, double iY )
{
    BLPoint localCoords;
    ::ULIS::FVec2D localPoint;

    localCoords = mInverseWorldMatrix.mapPoint( iX, iY );

    localPoint.x = localCoords.x;
    localPoint.y = localCoords.y;

    return localPoint;
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
FOdysseyVectorObject::AppendChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorObject* lastItem = mChildrenList.size() ? mChildrenList.back() : nullptr;

    AddChild( iChild, lastItem );
}

void
FOdysseyVectorObject::PrependChild( FOdysseyVectorObject* iChild )
{
    AddChild( iChild, nullptr );
}

void
FOdysseyVectorObject::AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorObject* lastItem = GetLastChild();

    iChild->mParent = this;

    if( iInsertAfter == nullptr )
    {
        mChildrenList.push_front( iChild );
    }
    else
    if( iInsertAfter == lastItem )
    {
        mChildrenList.push_back( iChild );
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

                break;
            }
        }
    }

    iChild->Invalidate();
}

void
FOdysseyVectorObject::RemoveChild( FOdysseyVectorObject* iChild )
{
    //iChild->mParent = nullptr;

    mChildrenList.remove(iChild);
    mInvalidatedChildrenList.remove(iChild);

    Invalidate();
}

void
FOdysseyVectorObject::SetForegroundColor( FColor& iColor )
{
    mForegroundBucket.SetSolidColor( iColor );
}

void
FOdysseyVectorObject::SetBackgroundColor( FColor& iColor )
{
    mBackgroundBucket.SetSolidColor( iColor );
}

void
FOdysseyVectorObject::SetForegroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mForegroundBucket.SetSolidColor( iR, iG, iB, iA );
}

void
FOdysseyVectorObject::SetBackgroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mBackgroundBucket.SetSolidColor( iR, iG, iB, iA );
}

void 
FOdysseyVectorObject::CopyTransformation( FOdysseyVectorObject& iObject )
{
    iObject.SetTransform( mObjectParam.TranslationX
                        , mObjectParam.TranslationY
                        , mObjectParam.Rotation
                        , mObjectParam.ScalingX
                        , mObjectParam.ScalingY );
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

    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->mChildrenList.begin(); it != iObject->mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject* childObject = (*it);

        TreeToList( childObject, iOutList );
    }

    return iOutList.size();
}

//static
uint32
FOdysseyVectorObject::TreeToArray( FOdysseyVectorObject* iObject, std::vector<FOdysseyVectorObject*>& iOutArray )
{
    uint32 size = iObject->mChildrenList.size();

    iOutArray.push_back( iObject );

    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->mChildrenList.begin(); it != iObject->mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject* childObject = (*it);

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

uint32
FOdysseyVectorObject::GetType()
{
    return FOdysseyVectorObject::VECTOROBJECTTYPE;
}
