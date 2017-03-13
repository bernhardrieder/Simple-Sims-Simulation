// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <unordered_map>
#include "Hexagon.h"

/**
 * 
 */
static class MYSIMS_API Pathfinding
{
public:
	static void A_StarSearch(AHexagon* start, AHexagon* goal, std::unordered_map<AHexagon*, AHexagon*>& came_from, std::unordered_map<AHexagon*, float>& cost_so_far, bool considerThreat = true);
	static std::vector<AHexagon*> ReconstructPath(AHexagon* start, AHexagon* goal, std::unordered_map<AHexagon*, AHexagon*> came_from, bool shouldBeSortedStartToEnd = false);
private:
	static float A_StarSearchHeuristic(AHexagon* start, AHexagon* goal);
};
