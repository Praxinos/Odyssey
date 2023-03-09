#include "OdysseyVectorScene.h"

void UOdysseyVectorScene::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorScene::SetEngine( FOdysseyVectorEngine* iEngine )
{
    mEngine = iEngine;
}

FOdysseyVectorEngine*
UOdysseyVectorScene::GetEngine()
{
    return mEngine;
}

void
UOdysseyVectorScene::ClearSelection()
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
UOdysseyVectorScene::Unselect( UOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
UOdysseyVectorScene::Select( UOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

UOdysseyVectorObject*
UOdysseyVectorScene::CopyShape()
{
    UOdysseyVectorScene* rootCopy = NewObject<UOdysseyVectorScene>();

    rootCopy->Init( Name );

    return Cast<UOdysseyVectorObject>(rootCopy);
}

UOdysseyVectorGroup*
UOdysseyVectorScene::GroupSelectdObjects( )
{
    UOdysseyVectorGroup* group = NewObject<UOdysseyVectorGroup>();
    BLPoint averageTranslation = { 0.0f, 0.0f };
    // We don't use the mSelectedObjectList because we want to keep the same order
    // and we work on a copy to be able to delete the objects while iterating
    std::list<UOdysseyVectorObject*> objectList = mChildrenList;

    if ( mSelectedObjectList.size() )
    {
        for( std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            UOdysseyVectorObject *obj = (*it);
            BLPoint origin = obj->GetWorldMatrix().mapPoint( 0.0f, 0.0f );

            averageTranslation.x += origin.x;
            averageTranslation.y += origin.y;
        }

        averageTranslation.x /= mSelectedObjectList.size();
        averageTranslation.y /= mSelectedObjectList.size();

        averageTranslation = this->GetInverseWorldMatrix().mapPoint ( averageTranslation.x, averageTranslation.y );
    }

    AppendChild ( group );

    group->Translate( averageTranslation.x, averageTranslation.y );
    group->UpdateMatrix();

    while( objectList.size () )
    {
        UOdysseyVectorObject *obj = objectList.back();

        if ( obj->IsSelected() == true )
        {
            obj->GetParent()->RemoveChild( obj );

            group->PrependChild( obj );
        }

        objectList.pop_back();
    }

    group->Invalidate();


    return group;
}

std::list<UOdysseyVectorObject*>&
UOdysseyVectorScene::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

void
UOdysseyVectorScene::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{

}

void
UOdysseyVectorScene::InvalidateObject( UOdysseyVectorObject* iObject )
{
    mInvalidatedObjectList.push_back( iObject );
}

void
UOdysseyVectorScene::UpdateShape( uint32 iUpdateFlags )
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mInvalidatedObjectList.begin(); it != mInvalidatedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);

        obj->Update( iUpdateFlags );
    }

    if( ( iUpdateFlags & UOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidatedObjectList.clear();
    }
}

UOdysseyVectorObject*
UOdysseyVectorScene::GetLastSelected()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

uint32
UOdysseyVectorScene::GetType()
{
    return UOdysseyVectorObject::VECTORROOTTYPE;
}

void
UOdysseyVectorScene::RemoveSelectedObjects()
{
    for(std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it)
    {
        UOdysseyVectorObject* selectedObject = (*it);

        // prevent nested removal
        if( selectedObject->HasSelectedParent() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
