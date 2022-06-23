// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Settings/NamingConventionSettings.h"

#define LOCTEXT_NAMESPACE "NamingConventionSettings"

//---

FNamingConventionPlane::FNamingConventionPlane()
{
    mPatternKeywords.AddKeyword( ENamingConventionPlanePatternKeyword::PlaneIndex   , "plane-index" , LOCTEXT( "plane-pattern-keyword.plane-index", "an incremental index" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionPlanePatternKeyword::CameraName   , "camera-name" , LOCTEXT( "plane-pattern-keyword.camera-name", "the name of the camera (won't update if camera name changes)" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionPlanePatternKeyword::ShotName     , "shot-name"   , LOCTEXT( "plane-pattern-keyword.shot-name", "the name of the shot (won't update if shot name changes)" ) );

    Pattern = FString::Printf( TEXT( "plane_%s" )
                               , *mPatternKeywords.mKeywordList[ENamingConventionPlanePatternKeyword::PlaneIndex].mKeywordWithBraces
    );
}

FNamingConventionCamera::FNamingConventionCamera()
{
    mPatternKeywords.AddKeyword( ENamingConventionCameraPatternKeyword::CameraIndex , "camera-index", LOCTEXT( "camera-pattern-keyword.camera-index", "an incremental index" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionCameraPatternKeyword::ShotName    , "shot-name"   , LOCTEXT( "camera-pattern-keyword.shot-name", "the name of the shot (won't update if shot name changes)" ) );

    Pattern = FString::Printf( TEXT( "camera_%s" )
                               , *mPatternKeywords.mKeywordList[ENamingConventionCameraPatternKeyword::CameraIndex].mKeywordWithBraces
    );
}

FNamingConventionShot::FNamingConventionShot()
{
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::ShotIndex         , "shot-index"          , LOCTEXT( "shot-pattern-keyword.shot-index", "an incremental index" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::TakeIndex         , "take-index"          , LOCTEXT( "shot-pattern-keyword.take-index", "an incremental index for take" ) );

    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::StudioName        , "studio-name"         , LOCTEXT( "shot-pattern-keyword.studio-name", "the full studio name" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::StudioAcronym     , "studio-acronym"      , LOCTEXT( "shot-pattern-keyword.studio-acronym", "the studio name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::LicenseName       , "license-name"        , LOCTEXT( "shot-pattern-keyword.license-name", "the full license name" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::LicenseAcronym    , "license-acronym"     , LOCTEXT( "shot-pattern-keyword.license-acronym", "the license name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::ProductionName    , "production-name"     , LOCTEXT( "shot-pattern-keyword.production-name", "the full production title" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::ProductionAcronym , "production-acronym"  , LOCTEXT( "shot-pattern-keyword.production-acronym", "the production title acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::Season            , "season"              , LOCTEXT( "shot-pattern-keyword.season", "the season number" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::Episode           , "episode"             , LOCTEXT( "shot-pattern-keyword.episode", "the episode number" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::Part              , "part"                , LOCTEXT( "shot-pattern-keyword.part", "the part (A, B, C, ...)" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::DepartmentName    , "department-name"     , LOCTEXT( "shot-pattern-keyword.department-name", "the full department name" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::DepartmentAcronym , "department-acronym"  , LOCTEXT( "shot-pattern-keyword.department-acronym", "the department name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionShotPatternKeyword::Initials          , "initials"            , LOCTEXT( "shot-pattern-keyword.initials", "some initials" ) );

    Pattern = FString::Printf( TEXT( "shot_%s_%s" )
                               , *mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::ShotIndex].mKeywordWithBraces
                               , *mPatternKeywords.mKeywordList[ENamingConventionShotPatternKeyword::TakeIndex].mKeywordWithBraces
    );
}

FNamingConventionBoard::FNamingConventionBoard()
{
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::BoardIndex       , "board-index"         , LOCTEXT( "board-pattern-keyword.board-index", "an incremental index" ) );

    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::StudioName       , "studio-name"         , LOCTEXT( "board-pattern-keyword.studio-name", "the full studio name" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::StudioAcronym    , "studio-acronym"      , LOCTEXT( "board-pattern-keyword.studio-acronym", "the studio name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::LicenseName      , "license-name"        , LOCTEXT( "board-pattern-keyword.license-name", "the full license name" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::LicenseAcronym   , "license-acronym"     , LOCTEXT( "board-pattern-keyword.license-acronym", "the license name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::ProductionName   , "production-name"     , LOCTEXT( "board-pattern-keyword.production-name", "the full production title" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::ProductionAcronym, "production-acronym"  , LOCTEXT( "board-pattern-keyword.production-acronym", "the production title acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::Season           , "season"              , LOCTEXT( "board-pattern-keyword.season", "the season number" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::Episode          , "episode"             , LOCTEXT( "board-pattern-keyword.episode", "the episode number" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::Part             , "part"                , LOCTEXT( "board-pattern-keyword.part", "the part (A, B, C, ...)" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::DepartmentName   , "department-name"     , LOCTEXT( "board-pattern-keyword.department-name", "the full department name" ) );
    //mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::DepartmentAcronym, "department-acronym"  , LOCTEXT( "board-pattern-keyword.department-acronym", "the department name acronym" ) );
    mPatternKeywords.AddKeyword( ENamingConventionBoardPatternKeyword::Initials         , "initials"            , LOCTEXT( "board-pattern-keyword.initials", "some initials" ) );

    Pattern = FString::Printf( TEXT( "board_%s" )
                               , *mPatternKeywords.mKeywordList[ENamingConventionBoardPatternKeyword::BoardIndex].mKeywordWithBraces
    );
}

//---

FName
UNamingConventionSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UNamingConventionSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}

#undef LOCTEXT_NAMESPACE
