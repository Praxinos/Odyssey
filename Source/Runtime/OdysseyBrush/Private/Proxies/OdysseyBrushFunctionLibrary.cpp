// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushFunctionLibrary.h"
#include "OdysseyBrushAssetBase.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include "OdysseyBrushContext.h"
#include <ULIS_CORE>


#define LOCTEXT_NAMESPACE "OdysseyBrushFunctionLibrary"


//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushFunctionLibrary
//--------------------------------------------------------------------------------------
//--------------------------------------------- Odyssey Brush Blueprint Callable Methods

//static
void
UOdysseyBrushFunctionLibrary::DebugStamp()
{
    ODYSSEY_BRUSH_CONTEXT_CHECK

    int diameter = FMath::Max( 2.f, brush->GetSizeModifier() * brush->GetPressure() );
    int center = ( diameter / 2 );
    int radius = center - 2;
    FOdysseyBlock round( diameter, diameter );
    ::ULIS::CColor color = brush->GetState().color;
    color.SetAlphaF( brush->GetState().flow_modifier );
    ::ULIS::FPainterContext::DrawCircleAndres( round.GetIBlock(), ::ULIS::FPoint( center, center ), radius, color, true );
    ::ULIS::FRect invalidRect;
    invalidRect.x = brush->GetX() - radius;
    invalidRect.y = brush->GetY() - radius;
    invalidRect.w = diameter;
    invalidRect.h = diameter;
    ::ULIS::FBlendingContext::Blend( round.GetIBlock()
                                   , brush->GetState().target_temp_buffer->GetIBlock()
                                   , invalidRect.x
                                   , invalidRect.y
                                   , ::ULIS::eBlendingMode::kNormal
                                   , ::ULIS::eAlphaMode::kNormal
                                   , 1.f );
    brush->PushInvalidRect( invalidRect );
}


void
ComputeRectWithPivot( FOdysseyBlock* iBlock, const FOdysseyPivot& iPivot, float iX, float iY, ::ULIS::FRect* oRect )
{
    int width = iBlock->Width();
    int height = iBlock->Height();
    int width2 = width / 2;
    int height2 = height / 2;

    FVector2D computedOffset = iPivot.OffsetMode == EPivotOffsetMode::kAbsolute ? iPivot.Offset : iPivot.Offset * FVector2D( width, height );
    oRect->x = iX;
    oRect->y = iY;
    oRect->w = width;
    oRect->h = height;

    switch( iPivot.Reference )
    {
        case EPivotReference::kTopLeft:
        {
            oRect->x = iX;
            oRect->y = iY;
            break;
        }

        case EPivotReference::kTopMiddle:
        {
            oRect->x = iX - width2;
            oRect->y = iY;
            break;
        }

        case EPivotReference::kTopRight:
        {
            oRect->x = iX - width;
            oRect->y = iY;
            break;
        }

        case EPivotReference::kMiddleLeft:
        {
            oRect->x = iX;
            oRect->y = iY - height2;
            break;
        }

        case EPivotReference::kCenter:
        {
            oRect->x = iX - width2;
            oRect->y = iY - height2;
            break;
        }

        case EPivotReference::kMiddleRight:
        {
            oRect->x = iX - width;
            oRect->y = iY - height2;
            break;
        }

        case EPivotReference::kBotLeft:
        {
            oRect->x = iX;
            oRect->y = iY - height;
            break;
        }

        case EPivotReference::kBotMiddle:
        {
            oRect->x = iX - width2;
            oRect->y = iY - height;
            break;
        }

        case EPivotReference::kBotRight:
        {
            oRect->x = iX - width;
            oRect->y = iY - height;
            break;
        }
    }

    oRect->x += computedOffset.X;
    oRect->y += computedOffset.Y;
}


//static
void
UOdysseyBrushFunctionLibrary::SimpleStamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN

    /*
    FOdysseyBlock* block = Sample.m;
    int width = block->Width();
    int height = block->Height();
    int width2 = width / 2;
    int height2 = height / 2;

    FVector2D computedOffset = Pivot.OffsetMode == EPivotOffsetMode::kAbsolute ? Pivot.Offset : Pivot.Offset * FVector2D( width, height );
    ::ULIS::FRect invalidRect;

    invalidRect.x = X;
    invalidRect.y = Y;
    invalidRect.w = block->Width();
    invalidRect.h = block->Height();

    switch( Pivot.Reference )
    {
        case EPivotReference::kTopLeft:
        {
            invalidRect.x = X;
            invalidRect.y = Y;
            break;
        }

        case EPivotReference::kTopMiddle:
        {
            invalidRect.x = X - width2;
            invalidRect.y = Y;
            break;
        }

        case EPivotReference::kTopRight:
        {
            invalidRect.x = X - width;
            invalidRect.y = Y;
            break;
        }

        case EPivotReference::kMiddleLeft:
        {
            invalidRect.x = X;
            invalidRect.y = Y - height2;
            break;
        }

        case EPivotReference::kCenter:
        {
            invalidRect.x = X - width2;
            invalidRect.y = Y - height2;
            break;
        }

        case EPivotReference::kMiddleRight:
        {
            invalidRect.x = X - width;
            invalidRect.y = Y - height2;
            break;
        }

        case EPivotReference::kBotLeft:
        {
            invalidRect.x = X;
            invalidRect.y = Y - height;
            break;
        }

        case EPivotReference::kBotMiddle:
        {
            invalidRect.x = X - width2;
            invalidRect.y = Y - height;
            break;
        }

        case EPivotReference::kBotRight:
        {
            invalidRect.x = X - width;
            invalidRect.y = Y - height;
            break;
        }
    }

    invalidRect.x += computedOffset.X;
    invalidRect.y += computedOffset.Y;
    */

    FOdysseyBlock* block = Sample.m;
    ::ULIS::FRect invalidRect;
    ComputeRectWithPivot( block, Pivot, X, Y, &invalidRect );

    ::ULIS::FBlendingContext::Blend( block->GetIBlock()
                                   , brush->GetState().target_temp_buffer->GetIBlock()
                                   , invalidRect.x
                                   , invalidRect.y
                                   , ::ULIS::eBlendingMode::kNormal
                                   , ::ULIS::eAlphaMode::kNormal
                                   , FMath::Clamp( Flow, 0.f, 1.f ) );
    brush->PushInvalidRect( invalidRect );
}


//static
void
UOdysseyBrushFunctionLibrary::Stamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN

    FOdysseyBlock* block = Sample.m;
    ::ULIS::FRect invalidRect;
    ComputeRectWithPivot( block, Pivot, X, Y, &invalidRect );

    ::ULIS::FBlendingContext::Blend( block->GetIBlock()
                                   , brush->GetState().target_temp_buffer->GetIBlock()
                                   , invalidRect.x
                                   , invalidRect.y
                                   , (::ULIS::eBlendingMode)BlendingMode
                                   , (::ULIS::eAlphaMode)AlphaMode
                                   , FMath::Clamp( Flow, 0.f, 1.f ) );
    brush->PushInvalidRect( invalidRect );
}


//static
void
UOdysseyBrushFunctionLibrary::GenerateOrbitDelta( float AngleRad, float  Radius, float& DeltaX, float& DeltaY )
{
    float cosa = cos( AngleRad );
    float sina = sin( AngleRad );
    DeltaX = cosa * Radius;
    DeltaY = sina * Radius;
}


#undef LOCTEXT_NAMESPACE
