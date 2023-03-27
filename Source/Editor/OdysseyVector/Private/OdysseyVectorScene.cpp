#include "OdysseyVectorScene.h"

FOdysseyVectorScene::~FOdysseyVectorScene()
{

}

FOdysseyVectorScene::FOdysseyVectorScene()
{
    mFillBucket.SetColor( 0, 0, 0, 0 );
}

void FOdysseyVectorScene::Init( std::string iName )
{
    SetName( iName );
}

void
FOdysseyVectorScene::SetEngine( FOdysseyVectorEngine* iEngine )
{
    mEngine = iEngine;
}

FOdysseyVectorEngine*
FOdysseyVectorScene::GetEngine()
{
    return mEngine;
}

void
FOdysseyVectorScene::ClearSelection()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorScene::Unselect( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorScene::Select( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

FOdysseyVectorObject*
FOdysseyVectorScene::CopyShape()
{
    FOdysseyVectorScene* rootCopy = new FOdysseyVectorScene();

    rootCopy->Init( Name );

    return static_cast<FOdysseyVectorObject*>(rootCopy);
}

FOdysseyVectorGroup*
FOdysseyVectorScene::GroupSelectedObjects( )
{
    FOdysseyVectorGroup* group = new FOdysseyVectorGroup();
    BLPoint averageTranslation = { 0.0f, 0.0f };
    // We don't use the mSelectedObjectList because we want to keep the same order
    // and we work on a copy to be able to delete the objects while iterating
    std::list<FOdysseyVectorObject*> objectList = mChildrenList;
/*
    if ( mSelectedObjectList.size() )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject *obj = (*it);
            BLPoint origin = obj->GetWorldMatrix().mapPoint( 0.0f, 0.0f );

            averageTranslation.x += origin.x;
            averageTranslation.y += origin.y;
        }

        averageTranslation.x /= mSelectedObjectList.size();
        averageTranslation.y /= mSelectedObjectList.size();

        averageTranslation = this->GetInverseWorldMatrix().mapPoint ( averageTranslation.x, averageTranslation.y );
    }
*/
    AppendChild ( group );

    //group->Translate( averageTranslation.x, averageTranslation.y );
    group->UpdateMatrix();

    while( objectList.size () )
    {
        FOdysseyVectorObject *obj = objectList.back();

        if ( obj->IsSelected() == true )
        {
            obj->GetParent()->RemoveChild( obj );

            group->PrependChild( obj );
        }

        objectList.pop_back();
    }

    group->Invalidate();
    group->Update( 0 );

    return group;
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorScene::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

void
FOdysseyVectorScene::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetEngine()->GetBLContext();
    static ::ULIS::FRectD zeroRectangle; // static variables are always zeroed by default
    BLRgba32 blFillColor;
    FColor& fillColor = mFillBucket.GetColor();
    uint32 width, height;

    GetEngine()->GetColorImageSize( &width, &height );

    blctx->setCompOp( BL_COMP_OP_SRC_COPY );

    // Note: Blend2D color format is 0xAARRGGBB
    blFillColor.r = fillColor.B;
    blFillColor.g = fillColor.G;
    blFillColor.b = fillColor.R;
    blFillColor.a = fillColor.A;

    blctx->setFillStyle( blFillColor );

    blctx->save();
    blctx->resetMatrix();
    blctx->fillRect( 0, 0, width, height );
    blctx->restore();

    // view the updated zone ( testing purpose only )
    /*blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
    blctx.setStrokeWidth(1.0f);
    blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
}

void
FOdysseyVectorScene::InvalidateObject( FOdysseyVectorObject* iObject )
{
    mInvalidatedObjectList.push_back( iObject );
}

void
FOdysseyVectorScene::UpdateShape( uint32 iUpdateFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mInvalidatedObjectList.begin(); it != mInvalidatedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->Update( iUpdateFlags );
    }

    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidatedObjectList.clear();
    }
}

FOdysseyVectorObject*
FOdysseyVectorScene::GetLastSelected()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

uint32
FOdysseyVectorScene::GetType()
{
    return FOdysseyVectorObject::VECTORROOTTYPE;
}

void
FOdysseyVectorScene::RemoveSelectedObjects()
{
    for(std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it)
    {
        FOdysseyVectorObject* selectedObject = (*it);

        // prevent nested removal
        if( selectedObject->HasSelectedAncestor() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
