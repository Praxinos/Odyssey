// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"

#include "Editor.h"
#include "Editor/Transactor.h"
#include "Framework/Commands/GenericCommands.h"
#include "GeomTools.h"
#include "ToolMenu.h"
#include "ScopedTransaction.h"

#include "OdysseyBrushTransform.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyRasterBlock.h"

#include "ULISEventBuilder.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterTransformTool::~UOdysseyPainterEditorRasterTransformTool()
{
    if (mTransformedBlock)
    {
        mTransformedBlock.Reset();
        mTransformedBlock = nullptr;
    }

    if (mSelectionBlock)
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }

    mHandles.Empty();
    mTransformToolHUD->EmptyElements();
}

UOdysseyPainterEditorRasterTransformTool::UOdysseyPainterEditorRasterTransformTool()
    : mPaintEngine()
    , mRasterMutator(true)
    , mTransformCaptureMode(EOdysseyTransformCapture::NoCapture)
    , mTransformToolHUD(MakeShared<FOdysseyHUDElement>())
    , mTransformAreaHUD(nullptr)
    , mSelectionBlock(nullptr)
    , mTransformedBlock(nullptr)
    , mIndexTransaction(0)
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Transform64"));

    mHUD->AddElement(mTransformToolHUD);
}

bool
UOdysseyPainterEditorRasterTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

void
UOdysseyPainterEditorRasterTransformTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);

    section.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "FlipHorizontal",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        FlipHorizontal();
                    }
                )
            ),
            FText(),
            LOCTEXT("raster-transform-tool.top-tab.flip_horizontal", "Flip the selection horizontally"),
            FSlateIcon("OdysseyStyle", "PainterEditor.FlipHorizontal32"),
            EUserInterfaceActionType::Button
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "FlipVertical",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        FlipVertical();
                    }
                )
            ),
            FText(),
            LOCTEXT("raster-transform-tool.top-tab.flip_vertical", "Flip the selection vertically"),
            FSlateIcon("OdysseyStyle", "PainterEditor.FlipVertical32"),
            EUserInterfaceActionType::Button
        )
    );
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

    if( !mTransformAreaHUD )
        return false;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt( point.x );
    point.y = FMath::RoundToInt( point.y );

    mTransformCaptureMode = DetectCaptureMode( FVector2D( iPointInTexture.x, iPointInTexture.y ));
    mMouseLastReferencePoint = FVector2D(point.x, point.y);
    if( mTransformCaptureMode != EOdysseyTransformCapture::NoCapture )
    {
        RecordTransformTransaction();

        UpdateTransformBlock();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (!mTransformAreaHUD)
        return;

    switch (DetectCaptureMode(FVector2D(iPointInTexture.x, iPointInTexture.y)))
    {
        case EOdysseyTransformCapture::Inside:
            mMouseCursor = EMouseCursor::GrabHand;
            break;
        case EOdysseyTransformCapture::Rotation:
            mMouseCursor = EMouseCursorCustom::CircleClockwise;
            break;
        default:
            mMouseCursor = EMouseCursor::Crosshairs;
    }
}

void UOdysseyPainterEditorRasterTransformTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    if (!mTransformAreaHUD)
        return;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    if (mTransformCaptureMode == EOdysseyTransformCapture::Inside)
    {
        for (int i = 0; i < mHandles.Num(); i++)
        {
            mHandles[i]->SetPosition(mHandles[i]->GetPosition() - (mMouseLastReferencePoint - FVector2D(point.x, point.y)));
        }
        TArray<FVector2D>& points = mTransformAreaHUD->GetPoints();
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] = mHandles[i]->GetPosition();
        }

        mMouseLastReferencePoint = FVector2D(point.x, point.y);

        if (!Perspective)
            ConstrainToRectangle(FVector2D(point.x, point.y));
    }
    else if (mTransformCaptureMode == EOdysseyTransformCapture::Rotation)
    {
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();
        int rotationDelta = GetRotationAngleFromLastReference(FVector2D(point.x, point.y)) - mLastReferenceRotation;
        mLastReferenceRotation += rotationDelta;

        TArray<FVector2D>& points = mTransformAreaHUD->GetPoints();
        double cosAngle = FMath::Cos(::FMath::DegreesToRadians(-rotationDelta));
        double sinAngle = FMath::Sin(::FMath::DegreesToRadians(-rotationDelta));
        for (int i = 0; i < points.Num(); i++)
        {
            points[i] -= mHandles[4]->GetPosition();
            FVector2D newPt = FVector2D( points[i].X * cosAngle - points[i].Y * sinAngle, points[i].X * sinAngle + points[i].Y * cosAngle );
            points[i] = mHandles[4]->GetPosition() + newPt;
            mHandles[i]->SetPosition( points[i] );
        }

        if (!Perspective)
            ConstrainToRectangle(FVector2D(point.x, point.y));

        UpdateTransformBlock();
    }
}

bool UOdysseyPainterEditorRasterTransformTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if (!mTransformAreaHUD)
        return false;

    FOdysseyPoint point = iPointInTexture;
    point.x = FMath::RoundToInt(point.x);
    point.y = FMath::RoundToInt(point.y);

    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;

    return true;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyDown(const FKey& iKey)
{
    mToolChord = FInputChord( iKey,
        iKey == EKeys::LeftShift || iKey == EKeys::RightShift || mToolChord.bShift == true,
        iKey == EKeys::LeftControl || iKey == EKeys::RightControl || mToolChord.bCtrl == true,
        iKey == EKeys::LeftAlt || iKey == EKeys::RightAlt || mToolChord.bAlt == true,
        iKey == EKeys::LeftCommand || iKey == EKeys::RightCommand || mToolChord.bCmd == true);

    for (uint32 i = 0; i < static_cast<uint8>(EMultipleKeyBindingIndex::NumChords); ++i)
    {
        EMultipleKeyBindingIndex chordIndex = static_cast<EMultipleKeyBindingIndex>(i);
        const TSharedRef<const FInputChord> undoChord = FGenericCommands::Get().Undo->GetActiveChord(chordIndex);
        const TSharedRef<const FInputChord> redoChord = FGenericCommands::Get().Redo->GetActiveChord(chordIndex);
        if( mToolChord == undoChord.Get() )
        {
            if(mSelectionBlock)
            {
                UndoTransformTransaction();
                return true;
            }
            else
            {
                return false;
            }
        }
        else if( mToolChord == redoChord.Get() )
        {
            RedoTransformTransaction();
            return true;
        }
    }

    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    else if (iKey == EKeys::LeftAlt || iKey == EKeys::RightAlt)
    {
        Perspective = !Perspective;
        return true;
    }

    return false;
}

bool UOdysseyPainterEditorRasterTransformTool::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        mToolChord.bShift = false;
        return true;
    }
    else if (iKey == EKeys::LeftControl || iKey == EKeys::RightControl)
    {
        mToolChord.bCtrl = false;
    }
    else if (iKey == EKeys::LeftAlt || iKey == EKeys::RightAlt)
    {
        Perspective = !Perspective;
        mToolChord.bAlt = false;
        return true;
    }
    else if (iKey == EKeys::LeftCommand || iKey == EKeys::RightCommand)
    {
        mToolChord.bCmd = false;
    }
    else if (iKey == EKeys::Enter || iKey == EKeys::SpaceBar)
    {
        CommitTransform();
        mEditor->ActivateMainTool( mEditor->GetRasterDrawingTool() );
        return true;
    }
    else if( iKey == EKeys::Escape )
    {
        ClearTransform();
        mEditor->ActivateMainTool( mEditor->GetRasterDrawingTool() );
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterTransformTool::Tick(float iDeltaTime)
{
    //Only blend on the tick to synchronize with the FPS of the Editor
    BlendTransformAreaToPaintBlock();
}

void UOdysseyPainterEditorRasterTransformTool::Load()
{
    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnRasterSelectionChanged);
    rasterSelection->HideSelectionHUD();

    UpdateRasterSelection( true ); //Create new selection if empty

    UpdateTransformHUD();
    UOdysseyPainterEditorTool::Load();
}

void UOdysseyPainterEditorRasterTransformTool::Unload()
{
    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);

    CommitTransform();

    mSelectionBlock = nullptr;
    UOdysseyPainterEditorTool::Unload();
}

TOptional<FMouseCursor> UOdysseyPainterEditorRasterTransformTool::GetMouseCursorOverride() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if( mediaProvider.IsLocked() )
        return FMouseCursor( EMouseCursor::SlashedCircle );

    return Super::GetMouseCursorOverride();
}

int
UOdysseyPainterEditorRasterTransformTool::GetRotationAngleFromLastReference( FVector2D iPointInTexture )
{
    FVector2D point = iPointInTexture;
    point.X = FMath::RoundToInt(point.X);
    point.Y = FMath::RoundToInt(point.Y);

    if( !mTransformAreaHUD )
        return 0;

    FVector2D refPoint = mHandles[4]->GetPosition() - point;

    return FMath::RadiansToDegrees(FMath::Atan2(refPoint.X, refPoint.Y));
}

void UOdysseyPainterEditorRasterTransformTool::UpdateTransformHUD()
{
    if( !mSelectionBlock )
        return;

    FOdysseyMediaProvider mediaProvider = mEditor->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    ::ULIS::FRectI boundingBox = rasterSelection->GetMaskBoundingRect();

    if (boundingBox.Area() <= 0)
        boundingBox = ::ULIS::FRectI::FromMinMax(0, 0, mediaRasters[0]->GetRasterBlock()->GetWidth(), mediaRasters[0]->GetRasterBlock()->GetHeight());

    mTransformAreaHUD = MakeShared<FOdysseyHUDPolygon>();
    TArray<FVector2D>& areaPoints = mTransformAreaHUD->GetPoints();
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y ) );
    areaPoints.Add( FVector2D( boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h ) );
    areaPoints.Add( FVector2D( boundingBox.x, boundingBox.y + boundingBox.h ) );

    mTransformToolHUD->EmptyElements(); //Deleting the HUD of the selection to create the one for the transform
    mTransformToolHUD->AddElement(mTransformAreaHUD);

    TSharedPtr<FOdysseyHUDHandle> handleTopLeft = MakeShared<FOdysseyHUDHandle>(areaPoints[0]);
    TSharedPtr<FOdysseyHUDHandle> handleTopRight = MakeShared<FOdysseyHUDHandle>(areaPoints[1]);
    TSharedPtr<FOdysseyHUDHandle> handleBottomRight = MakeShared<FOdysseyHUDHandle>(areaPoints[2]);
    TSharedPtr<FOdysseyHUDHandle> handleBottomLeft = MakeShared<FOdysseyHUDHandle>(areaPoints[3]);
    TSharedPtr<FOdysseyHUDHandle> pivot = MakeShared<FOdysseyHUDHandle>(FVector2D(boundingBox.x + boundingBox.w / 2, boundingBox.y + boundingBox.h / 2));

    handleTopLeft->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnTopLeftHandleDragged);
    handleTopRight->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnTopRightHandleDragged);
    handleBottomRight->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnBottomRightHandleDragged);
    handleBottomLeft->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnBottomLeftHandleDragged);
    pivot->OnDragged().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::OnPivotHandleDragged);
    handleTopLeft->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction);
    handleTopRight->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction);
    handleBottomRight->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction);
    handleBottomLeft->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction);
    pivot->OnDragBegin().AddUObject(this, &UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction);

    mTransformAreaHUD->AddElement(handleTopLeft);
    mTransformAreaHUD->AddElement(handleTopRight);
    mTransformAreaHUD->AddElement(handleBottomRight);
    mTransformAreaHUD->AddElement(handleBottomLeft);
    mTransformAreaHUD->AddElement(pivot);

    mHandles.Add(handleTopLeft);
    mHandles.Add(handleTopRight);
    mHandles.Add(handleBottomRight);
    mHandles.Add(handleBottomLeft);
    mHandles.Add(pivot);

    //Creating the data
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

    ::ULIS::eFormat format = rasterBlock->GetFormat();

    mRasterMutator.SetRasterBlock(rasterBlock);
    mRasterMutator.EditTilesFromRects(
        { boundingBox },
        [&, referenceBlock = mSelectionBlock](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

            ctx.Blend(
                *referenceBlock,
                *iBlock,
                referenceBlock->Rect(),
                ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
                ::ULIS::Blend_Normal,
                ::ULIS::Alpha_Sub,
                1.f,
                ::ULIS::FSchedulePolicy::MultiScanlines,
                0,
                nullptr,
                nullptr
            );

            ctx.Finish();

            return { };
        }
    );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    ctx.Blend(
        *mSelectionBlock,
        *paintBlock,
        mSelectionBlock->Rect(),
        ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
        ::ULIS::Blend_Normal,
        ::ULIS::Alpha_Normal,
        1.f,
        ::ULIS::FSchedulePolicy::MultiScanlines,
        0,
        nullptr,
        nullptr
    );

    ctx.Finish();

    paintBlock->Dirty();
    mPaintEngine.Update(FOdysseyBlendParameters());
}


EOdysseyTransformCapture UOdysseyPainterEditorRasterTransformTool::DetectCaptureMode( FVector2D iPoint )
{
    if (FGeomTools2D::IsPointInPolygon( iPoint, mTransformAreaHUD->GetPoints()))
    {
        return EOdysseyTransformCapture::Inside;
    }
    else
    {
        mLastReferenceRotation = GetRotationAngleFromLastReference( iPoint );
        return EOdysseyTransformCapture::Rotation;
    }
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToRectangle(FVector2D iPosition)
{
    if( !mTransformAreaHUD )
        return;


    if (Uniform)
    {
        for (int i = 0; i < 4; i++)
        {
            if (mHandles[i]->IsCaptured())
            {
                FVector point = FVector(iPosition.X, iPosition.Y, 0);

                FVector diagonalPt1 = FVector(mTransformAreaHUD->GetPoints()[i].X, mTransformAreaHUD->GetPoints()[i].Y, 0);
                FVector diagonalPt2 = FVector(mTransformAreaHUD->GetPoints()[(i+2)%4].X, mTransformAreaHUD->GetPoints()[(i+2)%4].Y, 0);

                FVector closestDiag = FMath::ClosestPointOnInfiniteLine(diagonalPt1, diagonalPt2, point);

                iPosition = FVector2D(closestDiag.X, closestDiag.Y);
            }
        }
    }

    FVector2D centerRect;

    double rotationRectangle = 0;
    double rotationDiag = 0;

    for (int i = 0; i < 4; i++)
    {
        if (mHandles[i]->IsCaptured())
        {
            centerRect = FVector2D(iPosition + mHandles[(i + 2) % 4]->GetPosition()) / 2.f;

            FVector2D rotRectPoint = mTransformAreaHUD->GetPoints()[1] - mTransformAreaHUD->GetPoints()[0];
            rotationRectangle = FMath::Atan2(rotRectPoint.X, rotRectPoint.Y) - PI / 2.f;

            FVector2D rotDiagPoint = mHandles[(i+2)%4]->GetPosition() - iPosition;
            rotationDiag = FMath::Atan2(rotDiagPoint.X, rotDiagPoint.Y);
        }
    }

    int next;
    int opposite;
    int previous;

    double cosRotation = cos( - 2 * (rotationDiag - rotationRectangle));
    double sinRotation = sin( - 2 * (rotationDiag - rotationRectangle));
    const FMatrix2x2 rotationMatrix = FMatrix2x2( cosRotation, -sinRotation,
                                                  sinRotation, cosRotation);

    FTransform2D diagTransform = FTransform2D(rotationMatrix);

    for (int i = 0; i < 4; i++)
    {
        if (mHandles[i]->IsCaptured())
        {
            next = (i + 1) % 4;
            opposite = (i + 2) % 4;
            previous = (i + 3) % 4;
            if (i % 2 == 0)
            {
                mHandles[i]->SetPosition( iPosition );
                mTransformAreaHUD->GetPoints()[i] = mHandles[i]->GetPosition();
                mHandles[next]->SetPosition(diagTransform.TransformPoint(mHandles[i]->GetPosition() - centerRect) + centerRect);
                mTransformAreaHUD->GetPoints()[next] = mHandles[next]->GetPosition();
                mHandles[previous]->SetPosition(diagTransform.TransformPoint(mHandles[opposite]->GetPosition() - centerRect) + centerRect);
                mTransformAreaHUD->GetPoints()[previous] = mHandles[previous]->GetPosition();
            }
            else
            {
                mHandles[i]->SetPosition( iPosition );
                mTransformAreaHUD->GetPoints()[i] = mHandles[i]->GetPosition();
                mHandles[next]->SetPosition(diagTransform.TransformPoint(mHandles[opposite]->GetPosition() - centerRect) + centerRect);
                mTransformAreaHUD->GetPoints()[next] = mHandles[next]->GetPosition();
                mHandles[previous]->SetPosition(diagTransform.TransformPoint(mHandles[i]->GetPosition() - centerRect) + centerRect);
                mTransformAreaHUD->GetPoints()[previous] = mHandles[previous]->GetPosition();
            }

            mHandles[4]->SetPosition(centerRect);
        }
    }
}

void UOdysseyPainterEditorRasterTransformTool::ConstrainToParallelogram(FVector2D iPosition)
{

}

void UOdysseyPainterEditorRasterTransformTool::TransformBlockPerspective(TSharedPtr<::ULIS::FBlock> iBaseBlock, TSharedPtr<::ULIS::FBlock> iDestBlock)
{
    if (!iBaseBlock || !iDestBlock || !mTransformAreaHUD || iBaseBlock->Format() != iDestBlock->Format() )
        return;

    ::ULIS::eFormat format = iBaseBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

    TArray<FVector2D> basePoints;
    basePoints.Add(FVector2D(0, 0));
    basePoints.Add(FVector2D(iBaseBlock->Width(), 0));
    basePoints.Add(FVector2D(iBaseBlock->Width(), iBaseBlock->Height()));
    basePoints.Add(FVector2D(0, iBaseBlock->Height()));

    TArray<FVector2D> vertices = mTransformAreaHUD->GetPoints();
    for (int i = 0; i < vertices.Num(); i++)
    {
        vertices[i] -= FVector2D(boundingBox.x, boundingBox.y);
    }

    if (!IsPolygonConvex(vertices))
        return;

    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
        basePoints[0],
        basePoints[1],
        basePoints[2],
        basePoints[3],
        vertices[0],
        vertices[1],
        vertices[2],
        vertices[3]
    );

    // This is a very slow operation unfortunately
    ctx.TransformPerspective(
        *iBaseBlock
        , *iDestBlock
        , ::ULIS::FRectI::Auto
        , transformation.m
    );

    ctx.Finish();
}

void UOdysseyPainterEditorRasterTransformTool::UpdateTransformBlock()
{
    if( !mSelectionBlock )
        return;

    mTransformedBlock = nullptr;

    ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

    mTransformedBlock = MakeShareable(new ::ULIS::FBlock(boundingBox.w, boundingBox.h, mSelectionBlock->Format()));
    ClearBlock(mTransformedBlock);

    TransformBlockPerspective(mSelectionBlock, mTransformedBlock);
}

::ULIS::FRectI UOdysseyPainterEditorRasterTransformTool::GetTransformAreaBoundingRect()
{
    if( mTransformAreaHUD )
    {
        int minX = mTransformAreaHUD->GetPoints()[0].X;
        int maxX = mTransformAreaHUD->GetPoints()[0].X;
        int minY = mTransformAreaHUD->GetPoints()[0].Y;
        int maxY = mTransformAreaHUD->GetPoints()[0].Y;
        for( int i = 1; i < mTransformAreaHUD->GetPoints().Num(); i++ )
        {
            minX = FMath::Min( minX, mTransformAreaHUD->GetPoints()[i].X );
            maxX = FMath::Max( maxX, mTransformAreaHUD->GetPoints()[i].X );
            minY = FMath::Min( minY, mTransformAreaHUD->GetPoints()[i].Y );
            maxY = FMath::Max( maxY, mTransformAreaHUD->GetPoints()[i].Y );
        }
        return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
    }

    return ::ULIS::FRectI::FromXYWH( 0,0,0,0 );
}

void UOdysseyPainterEditorRasterTransformTool::BlendTransformAreaToPaintBlock()
{
    if( mTransformAreaHUD )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();

        if( !paintBlock || !mSelectionBlock || !mTransformedBlock )
            return;

        ::ULIS::eFormat format = paintBlock->Format();
        ::ULIS::FRectI boundingBox = GetTransformAreaBoundingRect();

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        ::ULIS::FEvent clearEvent, blendEvent;

        ctx.Clear(
            *paintBlock,
            paintBlock->Rect(),
            ::ULIS::FSchedulePolicy::MultiScanlines,
            0,
            nullptr,
            &clearEvent);

        ctx.Blend(
            *mTransformedBlock,
            *paintBlock,
            mTransformedBlock->Rect(),
            ::ULIS::FVec2I(boundingBox.x, boundingBox.y),
            ::ULIS::Blend_Normal,
            ::ULIS::Alpha_Normal,
            1.f,
            ::ULIS::FSchedulePolicy::MultiScanlines,
            1,
            &clearEvent,
            &blendEvent
        );

        ctx.Finish();

        paintBlock->Dirty();
        mPaintEngine.Update( FOdysseyBlendParameters() );
    }
}

void UOdysseyPainterEditorRasterTransformTool::CommitTransform()
{
    if( !mTransformAreaHUD )
        return;

    if( mRasterMutator.GetRasterBlock() != nullptr )
    {
        FScopedTransaction scopedTransaction(LOCTEXT("actions.raster.transform.apply", "Apply transform"));
        {
            mRasterMutator.Commit();
            mPaintEngine.Commit(FOdysseyBlendParameters());

            FOdysseyPainterEditor* editor = GetEditor();
            TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();

            // --- Change the raster selection to cover the transformed pixels only ---
            FOdysseyRasterBlockMutator mutator(GetEditor()->RasterSelection()->GetRasterBlock());

            ::ULIS::FRectI boundingRect = ::ULIS::FRectI::FromXYWH(0, 0, GetEditor()->RasterSelection()->GetBlock()->Width(), GetEditor()->RasterSelection()->GetBlock()->Height());
            mutator.EditTilesFromRects(
                { boundingRect },
                [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                {
                    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

                    ::ULIS::eFormat format = iBlock->Format();
                    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

                    TSharedPtr<::ULIS::FBlock> baseSelectionBlock = MakeShareable(new ::ULIS::FBlock(iBlock->Width(), iBlock->Height(), format));

                    ::ULIS::FRectI selectionBoundingBox = rasterSelection->GetMaskBoundingRect();

                    TArray<FVector2D> basePoints;
                    basePoints.Add(FVector2D(selectionBoundingBox.x, selectionBoundingBox.y));
                    basePoints.Add(FVector2D(selectionBoundingBox.x + selectionBoundingBox.w, selectionBoundingBox.y));
                    basePoints.Add(FVector2D(selectionBoundingBox.x + selectionBoundingBox.w, selectionBoundingBox.y + selectionBoundingBox.h));
                    basePoints.Add(FVector2D(selectionBoundingBox.x, selectionBoundingBox.y + selectionBoundingBox.h));

                    TArray<FVector2D> vertices = mTransformAreaHUD->GetPoints();

                    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
                        basePoints[0],
                        basePoints[1],
                        basePoints[2],
                        basePoints[3],
                        vertices[0],
                        vertices[1],
                        vertices[2],
                        vertices[3]
                    );

                    ctx.Copy(
                        *iBlock,
                        *baseSelectionBlock);

                    ctx.Finish();

                    ctx.Clear(
                        *iBlock,
                        iBlock->Rect());

                    ctx.Finish();

                    // This is a very slow operation unfortunately
                    ctx.TransformPerspective(
                        *baseSelectionBlock
                        , *iBlock
                        , ::ULIS::FRectI::Auto
                        , transformation.m);

                    ctx.Finish();

                    ctx.FilterInto(
                        [this](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
                        {
                            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                            {
                                if (iSrcPixel.GreyF() != 0.f)
                                    iDstPixel.SetGreyF(1.f);
                            }
                        }
                        , *iBlock
                        , *iBlock
                    );

                    ctx.Finish();

                    return { };
                }
            );
            // --- Change the raster selection to cover the transformed pixels only ---
            mutator.Commit();
        }
        GetEditor()->RasterSelection()->OnChanged().Broadcast();
    }
    ClearTransform();
}

void UOdysseyPainterEditorRasterTransformTool::ClearTransform()
{
    if (mRasterMutator.GetRasterBlock() != nullptr)
    {
        mPaintEngine.Abort();
        mRasterMutator.Abort();
        mRasterMutator.SetRasterBlock(nullptr);
    }

    mIndexTransaction = 0;
    mTransformTransactions.Empty();
    mSelectionBlock = nullptr;
    mTransformedBlock = nullptr;
    mLastReferenceRotation = 0;
    mHandles.Empty();
    mTransformToolHUD->EmptyElements();
    mTransformAreaHUD = nullptr;
    mTransformCaptureMode = EOdysseyTransformCapture::NoCapture;
    GetEditor()->RasterSelection()->ShowSelectionHUD();
}

void UOdysseyPainterEditorRasterTransformTool::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if (!iBlock)
        return;

    ::ULIS::eFormat format = iBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    ctx.Clear(
        *iBlock,
        iBlock->Rect(),
        ::ULIS::FSchedulePolicy::MultiScanlines,
        0,
        nullptr,
        nullptr);

    ctx.Finish();
}


bool UOdysseyPainterEditorRasterTransformTool::IsPolygonConvex(const TArray<FVector2D>& Points)
{
    const int PointCount = Points.Num();
    float Sign = 0;
    for (int32 PointIndex = 0; PointIndex < PointCount; ++PointIndex)
    {
        const FVector2D& A = Points[PointIndex];
        const FVector2D& B = Points[(PointIndex + 1) % PointCount];
        const FVector2D& C = Points[(PointIndex + 2) % PointCount];
        float Det = (B.X - A.X) * (C.Y - B.Y) - (B.Y - A.Y) * (C.X - B.X);
        float DetSign = FMath::Sign(Det);
        if (DetSign != 0)
        {
            if (Sign == 0)
            {
                Sign = DetSign;
            }
            else if (Sign != DetSign)
            {
                return false;
            }
        }
    }

    return true;
}

void UOdysseyPainterEditorRasterTransformTool::RecordTransformTransaction()
{
    TStaticArray<FVector2D, 5> transaction;
    for (int i = 0; i < 5; i++)
        transaction[i] = mHandles[i]->GetPosition();

    if (mIndexTransaction >= mTransformTransactions.Num())
        mTransformTransactions.Add(transaction);
    else
    {
        mTransformTransactions[mIndexTransaction] = transaction;
        mTransformTransactions.SetNum(mIndexTransaction + 1);
    }

    mIndexTransaction++;
}

void UOdysseyPainterEditorRasterTransformTool::UndoTransformTransaction()
{
    if( mIndexTransaction <= 0 )
    {
        mTransformTransactions.Empty();
        ClearTransform();
        return;
    }

    if( mIndexTransaction == mTransformTransactions.Num() )
    {
        RecordTransformTransaction();
        mIndexTransaction--;
    }

    mIndexTransaction--;

    for( int i = 0; i < 4; i++ )
    {
        mHandles[i]->SetPosition( mTransformTransactions[mIndexTransaction][i]);
        mTransformAreaHUD->GetPoints()[i] = mHandles[i]->GetPosition();
    }

    mHandles[4]->SetPosition(mTransformTransactions[mIndexTransaction][4]);

    UpdateTransformBlock();
}

void UOdysseyPainterEditorRasterTransformTool::RedoTransformTransaction()
{
    if (mIndexTransaction >= mTransformTransactions.Num() - 1 )
        return;

    mIndexTransaction++;

    for (int i = 0; i < 4; i++)
    {
        mHandles[i]->SetPosition(mTransformTransactions[mIndexTransaction][i]);
        mTransformAreaHUD->GetPoints()[i] = mHandles[i]->GetPosition();
    }

    mHandles[4]->SetPosition(mTransformTransactions[mIndexTransaction][4]);

    UpdateTransformBlock();
}

FText
UOdysseyPainterEditorRasterTransformTool::GetTooltip() const
{
    return LOCTEXT("raster-transform-tool.tooltip", "Transform Tool");
}

bool UOdysseyPainterEditorRasterTransformTool::FlipHorizontal()
{
    if( !mSelectionBlock )
        return false;

    RecordTransformTransaction();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( mSelectionBlock->Format() );

    TArray<FVector2D> referencePoints;
    referencePoints.Add(FVector2D(0, 0));
    referencePoints.Add(FVector2D(mSelectionBlock->Width(), 0));
    referencePoints.Add(FVector2D(mSelectionBlock->Width(), mSelectionBlock->Height()));
    referencePoints.Add(FVector2D(0, mSelectionBlock->Height()));

    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
        referencePoints[0],
        referencePoints[1],
        referencePoints[2],
        referencePoints[3],
        referencePoints[1],
        referencePoints[0],
        referencePoints[3],
        referencePoints[2]
    );

    ::ULIS::FBlock copyBlock = ::ULIS::FBlock(mSelectionBlock->Width(), mSelectionBlock->Height(), mSelectionBlock->Format() );

    ctx.Copy(
        *mSelectionBlock,
        copyBlock,
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I(0, 0),
        ::ULIS::FSchedulePolicy::MultiScanlines,
        0,
        nullptr,
        nullptr
    );

    ctx.Finish();

    ctx.TransformPerspective(
        copyBlock
        , *mSelectionBlock
        , ::ULIS::FRectI::Auto
        , transformation.m
    );

    ctx.Finish();

    UpdateTransformBlock();

    return true;
}

bool UOdysseyPainterEditorRasterTransformTool::FlipVertical()
{
    if (!mSelectionBlock)
        return false;

    RecordTransformTransaction();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mSelectionBlock->Format());

    TArray<FVector2D> referencePoints;
    referencePoints.Add(FVector2D(0, 0));
    referencePoints.Add(FVector2D(mSelectionBlock->Width(), 0));
    referencePoints.Add(FVector2D(mSelectionBlock->Width(), mSelectionBlock->Height()));
    referencePoints.Add(FVector2D(0, mSelectionBlock->Height()));

    FOdysseyMatrix transformation = UOdysseyTransformProxyLibrary::MakePerspectiveMatrix(
        referencePoints[0],
        referencePoints[1],
        referencePoints[2],
        referencePoints[3],
        referencePoints[3],
        referencePoints[2],
        referencePoints[1],
        referencePoints[0]
    );

    ::ULIS::FBlock copyBlock = ::ULIS::FBlock(mSelectionBlock->Width(), mSelectionBlock->Height(), mSelectionBlock->Format());

    ctx.Copy(
        *mSelectionBlock,
        copyBlock,
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I(0, 0),
        ::ULIS::FSchedulePolicy::MultiScanlines,
        0,
        nullptr,
        nullptr
    );

    ctx.Finish();

    ctx.TransformPerspective(
        copyBlock
        , *mSelectionBlock
        , ::ULIS::FRectI::Auto
        , transformation.m
    );

    ctx.Finish();

    UpdateTransformBlock();

    return true;
}

void
UOdysseyPainterEditorRasterTransformTool::OnTopLeftHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[0]->GetPosition() );
    else
        mTransformAreaHUD->GetPoints()[0] = mHandles[0]->GetPosition();

    UpdateTransformBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnTopRightHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[1]->GetPosition() );
    else
        mTransformAreaHUD->GetPoints()[1] = mHandles[1]->GetPosition();

    UpdateTransformBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnBottomRightHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[2]->GetPosition() );
    else
        mTransformAreaHUD->GetPoints()[2] = mHandles[2]->GetPosition();

    UpdateTransformBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnBottomLeftHandleDragged()
{
    if( !Perspective )
        ConstrainToRectangle( mHandles[3]->GetPosition() );
    else
        mTransformAreaHUD->GetPoints()[3] = mHandles[3]->GetPosition();

    UpdateTransformBlock();
}

void
UOdysseyPainterEditorRasterTransformTool::OnPivotHandleDragged()
{
}

void
UOdysseyPainterEditorRasterTransformTool::UpdateRasterSelection( bool iCreateNewIfEmpty )
{
    FOdysseyMediaProvider mediaProvider = mEditor->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasters.Num() <= 0)
        return;

    TObjectPtr<UOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        if( !iCreateNewIfEmpty )
        {
            mSelectionBlock = nullptr;
            mTransformedBlock = nullptr;
            return;
        }
        else //If we have no selection, by default, transform tool will select the smallest block that contains all pixels in the layer/frame
        {
            ::ULIS::FRectI boundingRect;
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
            TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());

            ctx.AnalyzeSmallestVisibleRect( *block, &boundingRect );
            ctx.Finish();

            TArray<FVector2D> polyPoints;
            polyPoints.Add(FVector2D(boundingRect.x, boundingRect.y));
            polyPoints.Add(FVector2D(boundingRect.x + boundingRect.w, boundingRect.y));
            polyPoints.Add(FVector2D(boundingRect.x + boundingRect.w, boundingRect.y + boundingRect.h));
            polyPoints.Add(FVector2D(boundingRect.x, boundingRect.y + boundingRect.h));

            mEditor->RasterSelection()->Add(polyPoints);
        }
    }

    ::ULIS::FRectI boundingBox = rasterSelection->GetMaskBoundingRect();

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

    mSelectionBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, rasterBlock->GetFormat());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
    ::ULIS::FEvent clearEvent, copyEvent;
    ctx.Clear(*mSelectionBlock);
    ctx.Finish();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = mEditor->RasterSelection()->GetBlock();

    if (maskBlock)
    {
        ctx.Copy(
            *block,
            *mSelectionBlock,
            boundingBox,
            ::ULIS::FVec2I(0,0),
            ::ULIS::FSchedulePolicy::MultiScanlines,
            0,
            nullptr,
            &copyEvent
        );

        ctx.FilterInto(
            [](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
            {
                for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                {
                    iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                }
            }
                , *maskBlock
                , *mSelectionBlock
                , boundingBox
                , ::ULIS::FVec2I(0,0)
                , ::ULIS::FSchedulePolicy::MultiScanlines
                , 1
                , &copyEvent
                , nullptr
                );

        ctx.Finish();
    }
}

void
UOdysseyPainterEditorRasterTransformTool::OnRasterSelectionChanged()
{
    UpdateRasterSelection();
    ClearTransform();
    UpdateTransformHUD();
}

void
UOdysseyPainterEditorRasterTransformTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().RasterTranformToolTogglePerspectiveMode,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterTransformTool::TogglePerspectiveMode )
    );
}

void
UOdysseyPainterEditorRasterTransformTool::TogglePerspectiveMode()
{
    Perspective = !Perspective;
}

#undef LOCTEXT_NAMESPACE
