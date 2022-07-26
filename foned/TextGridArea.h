#ifndef _TextGridArea_h_
#define _TextGridArea_h_
/* TextGridArea.h
 *
 * Copyright (C) 1992-2005,2007-2022 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FunctionArea.h"
#include "TextGrid.h"
#include "SpellingChecker.h"
#include "Preferences.h"

#include "TextGridArea_enums.h"

Thing_declare (SoundArea);
Thing_declare (SoundAnalysisArea);

Thing_define (TextGridArea, FunctionArea) {
	TextGrid textGrid() const { return static_cast <TextGrid> (our function()); }
	SoundArea borrowedSoundArea = nullptr;
	SoundAnalysisArea borrowedSoundAnalysisArea = nullptr;

	integer selectedTier = 1;
	bool suppressRedraw;

private:
	void v_specializedHighlightBackground () const
		override;

private:
	/* only in v_mouse: */
	double anchorTime = undefined;
	integer clickedLeftBoundary = 0;
	double leftDraggingBoundary = undefined, rightDraggingBoundary = undefined;   // initial dragging range
	double draggingTime = undefined;
	autoBOOLVEC draggingTiers;
	bool hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
public:
	void v1_info ()
		override;
	void v_drawInside ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
	GuiMenuItem extractSelectedTextGridPreserveTimesButton, extractSelectedTextGridTimeFromZeroButton;
	void v_createMenuItems_file (EditorMenu menu)
		override;
	void v_createMenuItems_view_timeDomain (EditorMenu menu)
		override;
	void v_createMenuItems_query (EditorMenu menu)
		override;
	void v_createMenus ()
		override;
	void v_updateMenuItems ()
		override;

	#include "TextGridArea_prefs.h"
};

DEFINE_FunctionArea_create (TextGridArea, TextGrid)

/*
	BUG: The following should move to TextGridArea.cpp, once AnyTextGridEditor.cpp no longer needs it.
*/
static void checkTierSelection (constTextGridArea me, conststring32 verbPhrase) {
	if (my selectedTier < 1 || my selectedTier > my textGrid() -> tiers->size)
		Melder_throw (U"To ", verbPhrase, U", first select a tier by clicking anywhere inside it.");
}
static integer getSelectedInterval (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const IntervalTier tier = (IntervalTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection());
}

static integer getSelectedLeftBoundary (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const IntervalTier tier = (IntervalTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection());
}
static integer getSelectedPoint (constTextGridArea me) {
	Melder_assert (my selectedTier >= 1 || my selectedTier <= my textGrid() -> tiers->size);
	const TextTier tier = (TextTier) my textGrid() -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	Melder_assert (isdefined (my startSelection()));
	return AnyTier_hasPoint (tier->asAnyTier(), my startSelection());
}

/* End of file TextGridArea.h */
#endif
