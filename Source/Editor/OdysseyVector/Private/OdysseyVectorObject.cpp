#include "OdysseyVectorObject.h"

UOdysseyVectorObject::~UOdysseyVectorObject()
{
}

UOdysseyVectorObject::UOdysseyVectorObject()
    : TranslationX( 0.0f )
    , TranslationY( 0.0f )
    , Rotation( 0.0f )
    , ScalingX ( 1.0f )
    , ScalingY ( 1.0f )
    , Foreground(   0,   0,   0, 255 )
    , Background( 255, 255, 255, 255 )
    , mParent ( nullptr )
    , mIsFilled ( false )
    , mIsSelected ( false )
    , mIsInvalidated ( false )
    , mDependsOnChildren ( false )
{
    mLocalMatrix.reset();
    mWorldMatrix.reset();
    mInverseLocalMatrix.reset();
    mInverseWorldMatrix.reset();
}

void
UOdysseyVectorObject::SetName( std::string iName )
{
    Name.assign( iName );
}

void
UOdysseyVectorObject::Update( uint32 iUpdateFlags )
{
    UpdateShape( iUpdateFlags );

    if( ( iUpdateFlags & UOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mIsInvalidated = false;
    }
}

void
UOdysseyVectorObject::SetIsSelected( bool iIsSelected )
{
    mIsSelected = iIsSelected;
}

void
UOdysseyVectorObject::Translate( double iX, double iY )
{
    TranslationX = iX;
    TranslationY = iY;
}

void
UOdysseyVectorObject::Rotate( double iAngle )
{
    Rotation = iAngle;
}

void
UOdysseyVectorObject::Scale( double iX, double iY )
{
    ScalingX = iX;
    ScalingY = iY;
}

double
UOdysseyVectorObject::GetScalingX()
{
    return ScalingX;
}

double
UOdysseyVectorObject::GetScalingY()
{
    return ScalingY;
}

double
UOdysseyVectorObject::GetRotation()
{
    return Rotation;
}

void
UOdysseyVectorObject::ResetTransform()
{
    Translate( 0.0f, 0.0f );
    Scale( 1.0f, 1.0f );
    Rotate( 0.0f );
}

UOdysseyVectorObject*
UOdysseyVectorObject::Copy() {
    UOdysseyVectorObject* objectCopy = CopyShape();

   // TODO, update matrices once we get a BLContext object

    if( objectCopy )
    {
        CopySettings( *objectCopy ); // we need the matrices to properly import the child

        // recurse
        for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            UOdysseyVectorObject *child = (*it);

            objectCopy->AppendChild( child->Copy() );
        }

        objectCopy->Invalidate();
    }

    return objectCopy;
}

void
UOdysseyVectorObject::CopySettings( UOdysseyVectorObject& iDestinationObject )
{
    CopyTransformation( iDestinationObject );

    iDestinationObject.UpdateMatrix();

    iDestinationObject.Foreground = Foreground;
    iDestinationObject.Background = Background;
    iDestinationObject.mIsFilled = mIsFilled;

    iDestinationObject.mBBox = mBBox;

    iDestinationObject.Name = Name;
    iDestinationObject.Name.append("_Copy");
}

double
UOdysseyVectorObject::GetTranslationX()
{
    return TranslationX;
}

double
UOdysseyVectorObject::GetTranslationY()
{
    return TranslationY;
}

void
UOdysseyVectorObject::UpdateMatrix()
{
    if( GetRoot() )
    {
        BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();

        blctx->save();

        blctx->resetMatrix();
        blctx->translate( TranslationX, TranslationY );
        blctx->rotate( Rotation );
        blctx->scale( ScalingX, ScalingY );
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
        for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
        {
            UOdysseyVectorObject *child = (*it);

            child->UpdateMatrix( );
        }

        blctx->restore();
    }
}

//static
::ULIS::FRectD
UOdysseyVectorObject::GetBoundingBoxFromList( std::list<UOdysseyVectorObject*>& iObjectList )
{
    ::ULIS::FRectD bbox;
    int init = 0;

    for( std::list<UOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);
        ::ULIS::FRectD objBBox = obj->GetBBox( true );

        bbox = ( init == 0 ) ? objBBox : bbox | objBBox;

        init = 1;
    }

    return bbox;
}

bool
UOdysseyVectorObject::HasSelectedParent()
{
    UOdysseyVectorObject* parent = mParent;

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
UOdysseyVectorObject::GetBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint p0 = mWorldMatrix.mapPoint( mBBox.x, mBBox.y );
        BLPoint p1 = mWorldMatrix.mapPoint( mBBox.x + mBBox.w,  mBBox.y + mBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min( p0.x, p1.x )
                                                             , ::ULIS::FMath::Min( p0.y, p1.y )
                                                             , ::ULIS::FMath::Max( p0.x, p1.x )
                                                             , ::ULIS::FMath::Max( p0.y, p1.y ) );

        return worldBBox;
    }

    return mBBox;
}

void
UOdysseyVectorObject::DrawChildren( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        child->Draw( iRoi, iFlags );
    }
}

void
UOdysseyVectorObject::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    ::ULIS::FRectD localRoi = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Adapt the Region-Of-Interest to the local coordinates
    if( iRoi.Area() != 0.0f )
    {
        BLPoint roiCornerOrigin = { iRoi.x, iRoi.y };
        BLPoint roiCornerSize = { iRoi.w, iRoi.h };
        BLPoint localRoiCornerOrigin = mInverseLocalMatrix.mapPoint( roiCornerOrigin.x, roiCornerOrigin.y );
        BLPoint localRoiCornerSize = mInverseLocalMatrix.mapVector( roiCornerSize.x, roiCornerSize.y );

        localRoi.x = localRoiCornerOrigin.x;
        localRoi.y = localRoiCornerOrigin.y;
        localRoi.w = localRoiCornerSize.x;
        localRoi.h = localRoiCornerSize.y;
    }

            /*printf("%s : %f %f %f %f\n",Name.c_str(), iRoi.x,iRoi.y,iRoi.w,iRoi.h);*/

    blctx->save();
    blctx->transform( mLocalMatrix );
    DrawShape( localRoi, iFlags );

    DrawChildren( localRoi, iFlags );

    blctx->restore();
}

bool
UOdysseyVectorObject::IsFilled()
{
    return mIsFilled;
}

bool
UOdysseyVectorObject::IsInvalidated()
{
    return mIsInvalidated;
}

bool
UOdysseyVectorObject::IsSelected()
{
    return mIsSelected;
}

void
UOdysseyVectorObject::Invalidate()
{
    if ( mIsInvalidated == false )
    {
        UOdysseyVectorObject* obj = GetRoot();

        if ( obj && ( obj != this ) )
        {
            if ( obj->GetClass() == UOdysseyVectorRoot::StaticClass() )
            {
                UOdysseyVectorRoot* root = Cast<UOdysseyVectorRoot>(obj);

                root->InvalidateObject( this );

                mIsInvalidated = true;
            }
        }
    }

    if( mParent )
    {
        if( mParent->mDependsOnChildren == true )
        {
            mParent->Invalidate();
        }
    }
}

UOdysseyVectorRoot*
UOdysseyVectorObject::GetRoot()
{
    UOdysseyVectorObject* parent = mParent;
    UOdysseyVectorObject* root = this;

    while ( parent )
    {
        root = parent;

        parent = parent->GetParent();
    }

    return Cast<UOdysseyVectorRoot>(root);
}

void
UOdysseyVectorObject::SetParent( UOdysseyVectorObject* iObject )
{
    mParent = iObject;
}

::ULIS::FVec2D
UOdysseyVectorObject::WorldCoordinatesToLocal( double iX, double iY )
{
    BLPoint localCoords;
    ::ULIS::FVec2D localPoint;

    localCoords = mInverseWorldMatrix.mapPoint( iX, iY );

    localPoint.x = localCoords.x;
    localPoint.y = localCoords.y;

    return localPoint;
}

UOdysseyVectorObject*
UOdysseyVectorObject::GetParent()
{
    return mParent;
}

void
UOdysseyVectorObject::MoveBack()
{
    if ( mParent )
    {
        std::list<UOdysseyVectorObject*>::iterator it1 = mParent->mChildrenList.begin();
        std::list<UOdysseyVectorObject*>::iterator it2 = it1++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it1 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                UOdysseyVectorObject *child = static_cast<UOdysseyVectorObject*>(*it1);

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
UOdysseyVectorObject::MoveFront()
{
    if ( mParent )
    {
        std::list<UOdysseyVectorObject*>::iterator it2 = mParent->mChildrenList.begin();
        std::list<UOdysseyVectorObject*>::iterator it1 = it2++;

        if ( mParent->mChildrenList.size() )
        {
            for( ; it2 != mParent->mChildrenList.end(); it1++, it2++ )
            {
                UOdysseyVectorObject *child = static_cast<UOdysseyVectorObject*>(*it1);

                if ( this == child )
                {
                    std::swap(*it1, *it2);

                    return;
                }
            }
        }
    } 
}

UOdysseyVectorObject*
UOdysseyVectorObject::Pick( UOdysseyVectorGroup* iSelectionSpace, ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    bool picked = PickShape( iRoi, iSelectionFlags );

    // returns parent only if the parent is of Group type and is different from the selection space
    if( picked )
    {
        if( dynamic_cast<UOdysseyVectorGroup*>(this->mParent) )
        {
            if( this->mParent != iSelectionSpace )
            {
                return this->mParent;
            }
        }

        return this;
    }

    return nullptr;
}

void
UOdysseyVectorObject::AppendChild( UOdysseyVectorObject* iChild )
{
    AddChild ( iChild, false );
}

void
UOdysseyVectorObject::PrependChild( UOdysseyVectorObject* iChild )
{
    AddChild ( iChild, true );
}

void
UOdysseyVectorObject::AddChild( UOdysseyVectorObject* iChild, bool iPrepend )
{
/*
    BLMatrix2D localMatrix = this->GetInverseWorldMatrix();

    localMatrix.transform( iChild->GetWorldMatrix() );

    ExtractTransformations ( localMatrix
                           , &iChild->TranslationX
                           , &iChild->TranslationY
                           , &iChild->Rotation
                           , &iChild->ScalingX
                           , &iChild->ScalingY );

    iChild->UpdateMatrix();
*/
    iChild->mParent = this;

    if ( iPrepend == true )
    {
        mChildrenList.push_front( iChild );
    }
    else
    {
        mChildrenList.push_back( iChild );
    }
}

void
UOdysseyVectorObject::RemoveChild( UOdysseyVectorObject* iChild )
{
    iChild->mParent = nullptr;

    mChildrenList.remove(iChild);
}

void
UOdysseyVectorObject::SetForegroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    Foreground.R = iR;
    Foreground.G = iG;
    Foreground.B = iB;
    Foreground.A = iA;
}

void
UOdysseyVectorObject::SetBackgroundColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    Background.R = iR;
    Background.G = iG;
    Background.B = iB;
    Background.A = iA;
}

void
UOdysseyVectorObject::SetFilled( bool iIsFilled)
{
    mIsFilled = iIsFilled;
}

void 
UOdysseyVectorObject::CopyTransformation( UOdysseyVectorObject& iObject )
{
    iObject.Rotation     = Rotation;
    iObject.ScalingX     = ScalingX;
    iObject.ScalingY     = ScalingY;
    iObject.TranslationX = TranslationX;
    iObject.TranslationY = TranslationY;
}

std::list<UOdysseyVectorObject*>&
UOdysseyVectorObject::GetChildrenList()
{
    return mChildrenList;
}

BLMatrix2D&
UOdysseyVectorObject::GetLocalMatrix()
{
    return mLocalMatrix;
}

BLMatrix2D&
UOdysseyVectorObject::GetWorldMatrix()
{
    return mWorldMatrix;
}

BLMatrix2D&
UOdysseyVectorObject::GetInverseWorldMatrix()
{
    return mInverseWorldMatrix;
}

uint32
UOdysseyVectorObject::TreeToList( UOdysseyVectorObject* iObject, std::list<UOdysseyVectorObject*>& iOutList )
{
    iOutList.push_back( iObject );

    for( std::list<UOdysseyVectorObject*>::iterator it = iObject->mChildrenList.begin(); it != iObject->mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject* childObject = (*it);

        TreeToList( childObject, iOutList );
    }

    return iOutList.size();
}

//static
uint32
UOdysseyVectorObject::TreeToArray( UOdysseyVectorObject* iObject, std::vector<UOdysseyVectorObject*>& iOutArray )
{
    uint32 size = iObject->mChildrenList.size();

    iOutArray.push_back( iObject );

    for( std::list<UOdysseyVectorObject*>::iterator it = iObject->mChildrenList.begin(); it != iObject->mChildrenList.end(); ++it )
    {
        UOdysseyVectorObject* childObject = (*it);

        TreeToArray( childObject, iOutArray );
    }

    return iOutArray.size();
}

FColor
UOdysseyVectorObject::GetForegroundColor()
{
    return Foreground;
}

FColor
UOdysseyVectorObject::GetBackgroundColor()
{
    return Background;
}

uint32
UOdysseyVectorObject::GetID()
{
    return mID;
}

void
UOdysseyVectorObject::SetID( uint32 iID )
{
    mID = iID;
}

uint32
UOdysseyVectorObject::GetType()
{
    return UOdysseyVectorObject::VECTOROBJECTTYPE;
}


void
UOdysseyVectorObject::PropertyChanged(const FName& iPropertyName)
{
/*
    if ( iPropertyName == "Name" )
        NameChanged();
    if ( iPropertyName == "IsActivated" )
        IsActivatedChanged();
    if ( iPropertyName == "IsLocked" )
        IsLockedChanged();
    if ( iPropertyName == "IsExpanded" )
        IsExpandedChanged();
    if ( iPropertyName == "Parent" )
        ParentChanged();
    if ( iPropertyName == "Children" )
        ChildrenChanged();
*/
}

void
UOdysseyVectorObject::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}
/*
void
UOdysseyVectorObject::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}
*/