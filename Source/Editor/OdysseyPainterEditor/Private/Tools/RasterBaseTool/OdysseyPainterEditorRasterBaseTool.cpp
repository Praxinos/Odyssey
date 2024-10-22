// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "ISinglePropertyView.h"
#include "OdysseyMediaRaster.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterBaseTool::~UOdysseyPainterEditorRasterBaseTool()
{
}

UOdysseyPainterEditorRasterBaseTool::UOdysseyPainterEditorRasterBaseTool()
    : mHasContextMenu( true )
{
}

void
UOdysseyPainterEditorRasterBaseTool::ExtendMenu(TSharedRef<FExtender> iExtender)
{
	TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
	iExtender->AddMenuExtension(
		"OdysseyEdit",
		EExtensionHook::After,
		commandList,
		FMenuExtensionDelegate::CreateLambda(
			[this](FMenuBuilder& iBuilder)
			{
				if (!IsActivated())
					return;
				
				iBuilder.BeginSection("ToolOptions", LOCTEXT("raster-base-tool.edit-menu.tool-options", "Tool Options"));
				{
					ExtendContextMenu( iBuilder );
				}
				iBuilder.EndSection();
			}
		)
	);
	
}

//static
bool
UOdysseyPainterEditorRasterBaseTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

TSharedRef<SWidget> UOdysseyPainterEditorRasterBaseTool::CreateTopTabWidget()
{
    return UOdysseyPainterEditorTool::CreateTopTabWidget();
}


TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView)
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    iView->SetVisibility(EVisibility::Collapsed);

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            //PATCH:
            iView.ToSharedRef()
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(0.f, 0.f, 3.f, 0.f)
        [
            nameWidget
        ]
        + SHorizontalBox::Slot()
        [
            valueWidget
        ];
}

bool
UOdysseyPainterEditorRasterBaseTool::OnKeyDown( const FKey& iKey )
{
    return UOdysseyPainterEditorTool::OnKeyDown( iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnKeyUp( const FKey& iKey )
{
    return UOdysseyPainterEditorTool::OnKeyUp( iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    bool ret = false;

    if (hasRaster)
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRaster = mediaProvider.GetMedias<FOdysseyMediaRaster>();

        if (mediaRaster.Num())
        {
            if (iKey == EKeys::RightMouseButton)
            {
                if (mHasContextMenu)
                {
                    PopupContextMenu();
                    return true;
                }
            }
        }
    }

    return UOdysseyPainterEditorTool::OnMouseDown( iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorRasterBaseTool::OnMouseUp(const FOdysseyPoint& iPointInTexture
    , const FKey& iKey)
{
    return UOdysseyPainterEditorTool::OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{

}

void UOdysseyPainterEditorRasterBaseTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<SWindow> window = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (!window)
		return;

    FSlateApplication::Get().PushMenu( window.ToSharedRef(),
        FWidgetPath(),
        contextMenu.ToSharedRef(),
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
}

TSharedPtr<SWidget> UOdysseyPainterEditorRasterBaseTool::CreateContextMenu()
{
    FMenuBuilder menu(true, nullptr);

    ExtendContextMenu(menu);

    return menu.MakeWidget();
}


void
UOdysseyPainterEditorRasterBaseTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{

}

void
UOdysseyPainterEditorRasterBaseTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    UOdysseyPainterEditorTool::PostEditChangeProperty( PropertyChangedEvent );
}


void UOdysseyPainterEditorRasterBaseTool::ExtendContextMenu(FMenuBuilder& menu)
{

}


void UOdysseyPainterEditorRasterBaseTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}


#undef LOCTEXT_NAMESPACE
