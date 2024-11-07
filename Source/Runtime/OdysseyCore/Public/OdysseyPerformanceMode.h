// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

enum class eOdysseyPerformanceMode
{
    Memory, //Enhance memory efficiency when reading/editing, will cache values and release memory whenever possible
    Speed, //Enhance speed when reading/editing, will force load cached values in memory for fast access
    Shutdown //Immediately caches everything and stops all activity, like if it was removed from memory
};