#include "OdysseyVectorRoot.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorRoot::FOdysseyVectorRoot( IOdysseyVectorAnimationCell* iAnimationCell
                                      , FOdysseyVectorGroupPaint* iScene
                                      , uint32 iPreferredWidth
                                      , uint32 iPreferredHeight )
    : FOdysseyVectorObject("ROOT")
    , mEngine ( iAnimationCell, this, iPreferredWidth, iPreferredHeight )
{
    SetScene( iScene );
}

bool
FOdysseyVectorRoot::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

FOdysseyVectorGroupPaint*
FOdysseyVectorRoot::GetScene()
{
    return static_cast<FOdysseyVectorGroupPaint*>(mChildrenList.front());
}

FOdysseyVectorEngine*
FOdysseyVectorRoot::GetEngine()
{
    return &mEngine;
}

void
FOdysseyVectorRoot::Invalidate( uint64 iInvalidationFlags )
{
    FOdysseyVectorObject::Invalidate( iInvalidationFlags );
}

void
FOdysseyVectorRoot::SetScene( FOdysseyVectorGroupPaint* iScene )
{
    // We dont use AddChild or RemoveChild because they are
    // overlodaded to prevent manual addition or removal of child objects.

    mChildrenList.clear();
    mInvalidatedChildrenList.clear();

    iScene->SetParent( this );
    mChildrenList.push_back( iScene );

    mEngine.ResetHUD();

    iScene->UpdateMatrix();
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

// forbid child removal
uint32
FOdysseyVectorRoot::RemoveChild( FOdysseyVectorObject* iChild )
{
    return FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN;
}

// forbid child addition
uint32
FOdysseyVectorRoot::AddChild( FOdysseyVectorObject* iChild
                            , FOdysseyVectorObject* iInsertAfter )
{
    return FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN;
}