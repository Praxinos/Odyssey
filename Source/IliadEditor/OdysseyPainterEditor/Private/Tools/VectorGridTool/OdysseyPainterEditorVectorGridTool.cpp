// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridToolHUD.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "ISinglePropertyView.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

// testing
#include "Import/svg/OdysseyVectorImportSVG.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
    : UOdysseyPainterEditorVectorSelectionTool( new FOdysseyPainterEditorVectorGridToolHUD( this ) )
    , mMultipleSelectionMode( false )
    , DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
    , World ( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");

    mGridHUD = static_cast<FOdysseyPainterEditorVectorGridToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorGridTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

uint64
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    mGridHUD->Export( mPointArray );



    //MakeTest( iScene );
    //#ifdef _DEBUG
    //FOdysseyVectorImportSVG svgReader( iScene, TEXT("C:\\CODE\\tiger.svg") );
    //#endif



    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // force redraw
    iScene->GetEngine()->Invalidate( 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey
                                                      , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-grid-tool.transaction.edit-grid","Vector Grid Tool"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPointArray, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        // multiple selection mode
        if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
        {
            mMultipleSelectionMode = true;

            mGridHUD->StartSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            bool picked;

            mGridHUD->GetSelection( mGridNodeArray );

            picked = mGridHUD->PickNodes( iPointInTexture.x
                                        , iPointInTexture.y
                                        , PickingRadius
                                        , FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );

            if( picked == true )
            {
                mGridHUD->GetSelection( mGridNodeArray );
            }
        }
    }

    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , uint64& oSignalFlags )
{
    // TODO: highlight grid handles ?
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , uint64& oSignalFlags )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        if( mMultipleSelectionMode == true )
        {
            mGridHUD->DragSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            FSelectionBox& selectionBox = mGridHUD->GetSelectionBox();

            if( selectionBox.rect.Area() )
            {
                BLPoint spaceDif = selectionBox.inverseWorldMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                                            , iPointInTexture.deltaPosition.Y );

                for( int i = 0; i < mGridNodeArray.size(); i++ )
                {
                    mGridNodeArray[i]->Set( mGridNodeArray[i]->GetX() + spaceDif.x, mGridNodeArray[i]->GetY() + spaceDif.y );
                }

                mGridHUD->Deform();

                // update invalidated objects
                iScene->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                              | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
            }
        }
    }

    iScene->GetEngine()->Invalidate( FOdysseyVectorEngine::INVALIDATE_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey
                                                    , uint64& oSignalFlags )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    uint64 notificationFlags = 0;

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        if( mMultipleSelectionMode == true )
        {
            mGridHUD->EndSelectionRectangle( FSlateApplication::Get().GetModifierKeys().IsControlDown() ? false : true );
        }

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        mMultipleSelectionMode = false;
    }

    iScene->GetEngine()->Invalidate( 0 );
    return true;
}

uint64
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    iEngine->ResetHUD();

    // redraw
    iScene->GetEngine()->Invalidate( 0 );

    return UOdysseyPainterEditorVectorSelectionTool::PropertyChangedVector( iScene, iPropertyName );
}

/* TSharedRef<SWidget>
UOdysseyPainterEditorVectorGridTool::CreateTopTabWidget()
{
    FPropertyEditorModule& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FSinglePropertyParams defaultPropertyParams;
    const TSharedPtr<ISinglePropertyView> XDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsX", defaultPropertyParams);
    const TSharedPtr<ISinglePropertyView> YDivPropertyView = propertyEditorModule.CreateSingleProperty(this, "DivisionsY", defaultPropertyParams);
    TSharedPtr<class IPropertyHandle> XDivHandle = XDivPropertyView->GetPropertyHandle();
    TSharedPtr<class IPropertyHandle> YDivHandle = YDivPropertyView->GetPropertyHandle();

    return SNew(SUniformWrapPanel)
        .SlotPadding(FVector2D(3.f, 0.f))
        .EvenRowDistribution(true)
        .HAlign(HAlign_Left)
        + SUniformWrapPanel::Slot()
        [
            SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(XDivHandle, XDivPropertyView).ToSharedRef()
        ]
        + SUniformWrapPanel::Slot()
        [
            CreatePropertyWidget(YDivHandle, YDivPropertyView).ToSharedRef()
        ];
} */

FText
UOdysseyPainterEditorVectorGridTool::GetTooltip() const
{
    return LOCTEXT("vector-grid-tool.tooltip", "Grid Tool");
}

#undef LOCTEXT_NAMESPACE
