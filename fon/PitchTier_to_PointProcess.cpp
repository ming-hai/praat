/* PitchTier_to_PointProcess.cpp
 *
 * Copyright (C) 1992-2005,2011,2012,2015-2017,2019,2023,2024 Paul Boersma
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

#include "PitchTier_to_PointProcess.h"
#include "Pitch_to_PitchTier.h"

autoPointProcess PitchTier_to_PointProcess (PitchTier me) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 1000);
		double area = 0.5;   // imagine an event half a period before the beginning
		integer size = my points.size;
		if (size == 0)
			return thee;
		for (integer interval = 0; interval <= size; interval ++) {
			const double t1 = ( interval == 0 ? my xmin : my points.at [interval] -> number );
			Melder_require (isdefined (t1),
				U"Time of point ", interval, U" not defined.");
			const double t2 = ( interval == size ? my xmax : my points.at [interval + 1] -> number );
			Melder_require (isdefined (t2),
				U"Time of point ", interval + 1, U" not defined.");
			const double f1 = my points.at [interval == 0 ? 1 : interval] -> value;
			Melder_require (isdefined (f1),
				U"Pitch of point ", interval == 0 ? 1 : interval, U" not defined.");
			const double f2 = my points.at [interval == size ? size : interval + 1] -> value;
			Melder_require (isdefined (f2),
				U"Pitch of point ", interval == size ? size : interval + 1, U" not defined.");
			area += (t2 - t1) * 0.5 * (f1 + f2);
			while (area >= 1.0) {
				const double slope = (f2 - f1) / (t2 - t1);
				area -= 1.0;
				const double discriminant = f2 * f2 - 2.0 * area * slope;
				PointProcess_addPoint (thee.get(), t2 - 2.0 * area / (f2 + NUMsqrt_0 (discriminant)));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PointProcess.");
	}
}

autoPointProcess PitchTier_Pitch_to_PointProcess (PitchTier me, Pitch vuv) {
	try {
		autoPointProcess fullPoint = PitchTier_to_PointProcess (me);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, fullPoint -> nt);
		/*
			Copy only voiced parts to result.
		*/
		for (integer i = 1; i <= fullPoint -> nt; i ++) {
			const double t = fullPoint -> t [i];
			if (Pitch_isVoiced_t (vuv, t))
				PointProcess_addPoint (thee.get(), t);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", vuv, U": not converted to PointProcess.");
	}
}

static bool PointProcess_isVoiced_t (PointProcess me, const double t, const double maximumPeriod) {
	const integer imid = PointProcess_getNearestIndex (me, t);
	if (imid == 0)
		return false;
	const double tmid = my t [imid];
	const bool leftVoiced = ( imid > 1 && tmid - my t [imid - 1] <= maximumPeriod );
	const bool rightVoiced = ( imid < my nt && my t [imid + 1] - tmid <= maximumPeriod );
	if (leftVoiced && t <= tmid || rightVoiced && t >= tmid)
		return true;
	if (leftVoiced && t < 1.5 * tmid - 0.5 * my t [imid - 1])
		return true;
	if (rightVoiced && t > 1.5 * tmid - 0.5 * my t [imid + 1])
		return true;
	return false;
}

autoPointProcess PitchTier_Point_to_PointProcess (PitchTier me, PointProcess vuv, const double maximumPeriod) {
	try {
		autoPointProcess fullPoint = PitchTier_to_PointProcess (me);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, fullPoint -> nt);
		/*
			Copy only voiced parts to result.
		*/
		for (integer i = 1; i <= fullPoint -> nt; i ++) {
			const double t = fullPoint -> t [i];
			if (PointProcess_isVoiced_t (vuv, t, maximumPeriod))
				PointProcess_addPoint (thee.get(), t);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", vuv, U": not converted to PointProcess.");
	}
}

autoPitchTier PointProcess_to_PitchTier (PointProcess me, const double maximumPeriod) {
	try {
		autoPitchTier thee = PitchTier_create (my xmin, my xmax);
		for (integer i = 1; i < my nt; i ++) {
			const double interval = my t [i + 1] - my t [i];
			if (interval <= maximumPeriod)
				RealTier_addPoint (thee.get(), my t [i] + 0.5 * interval, 1.0 / interval);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PitchTier.");
	}
}

autoPitchTier Pitch_PointProcess_to_PitchTier (Pitch me, PointProcess pp) {
	try {
		autoPitchTier temp = Pitch_to_PitchTier (me);
		autoPitchTier thee = PitchTier_PointProcess_to_PitchTier (temp.get(), pp);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to PitchTier.");
	}
}

autoPitchTier PitchTier_PointProcess_to_PitchTier (PitchTier me, PointProcess pp) {
	try {
		Melder_require (my points.size > 0,
			U"No pitch points.");
		autoPitchTier thee = PitchTier_create (pp -> xmin, pp -> xmax);
		for (integer i = 1; i <= pp -> nt; i ++) {
			const double time = pp -> t [i];
			const double value = RealTier_getValueAtTime (me, time);
			RealTier_addPoint (thee.get(), time, value);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to PitchTier.");
	}
}

autoTableOfReal PitchTier_downto_TableOfReal (const constPitchTier me, const bool useSemitones) {
	try {
		autoTableOfReal thee = RealTier_downto_TableOfReal (me, U"Time", U"F0");
		if (useSemitones)
			for (integer i = 1; i <= thy numberOfRows; i ++)
				thy data [i] [2] = NUMhertzToSemitones (thy data [i] [2]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

/* End of file PitchTier_to_PointProcess.cpp */
