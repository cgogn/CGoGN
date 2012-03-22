/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                  *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __COLOR_MAPS_H__
#define __COLOR_MAPS_H__

namespace CGoGN
{

namespace Utils
{

inline Geom::Vec3f color_map_blue_green_red(float x)
{
	Geom::Vec3f r(0.0f) ;
	if (x < 0.0f)
		r[2] = 1.0f;
	else if (x < 0.5f)
	{
		r[1] = 2.0f * x;
		r[2] = 1.0f - 2.0f * x;
	}
	else if (x < 1.0f)
	{
		r[0] = 2.0f * x - 1.0f;
		r[1] = 2.0f - 2.0f * x;
	}
	else
		r[0] = 1.0f;
	return r;
}

inline Geom::Vec3f color_map_BCGYR(float x)
{
	Geom::Vec3f r(0.0f) ;
	if (x < 0.0f)
		r[2] = 1.0f;
	else if (x < 0.25f)
	{
		r[0] = 0.0f;
		r[1] = 4.0f * x;
		r[2] = 1.0f;
	}
	else if (x < 0.5f)
	{
		r[0] = 0.0f;
		r[1] = 1.0f;
		r[2] = 2.0f - 4.0f * x;
	}
	else if (x < 0.75f)
	{
		r[0] = 4.0f * x - 2.0f;
		r[1] = 1.0f;
		r[2] = 0.0f;
	}
	else if (x < 1.0f)
	{
		r[0] = 1.0f;
		r[1] = 4.0f - 4.0f * x;
		r[2] = 0.0f;
	}
	else
		r[0] = 1.0f;
	return r;
}

inline Geom::Vec3f color_map_blue_white_red(float x)
{
	Geom::Vec3f r(0.0f);
	if (x < 0.0f)
		r[2] = 1.0f;
	else if (x < 0.5f)
	{
		r[0] = 2.0f * x;
		r[1] = 2.0f * x;
		r[2] = 1.0f;
	}
	else if (x < 1.0f)
	{
		r[0] = 1.0f;
		r[1] = 2.0f - 2.0f * x;
		r[2] = 2.0f - 2.0f * x;
	}
	else
		r[0] = 1.0f;
	return r;
}

inline Geom::Vec3f color_map_cyan_white_red(float x)
{
	Geom::Vec3f r(0.0f);
	if (x < 0.0f)
		r[2] = 1.0f;
	else if (x < 0.5f)
	{
		r[0] = 2.0f * x;
		r[1] = 1.0f;
		r[2] = 1.0f;
	}
	else if (x < 1.0f)
	{
		r[0] = 1.0f;
		r[1] = 2.0f - 2.0f * x;
		r[2] = 2.0f - 2.0f * x;
	}
	else
		r[0] = 1.0f;
	return r;
}

inline float scale_expand_within_0_1(float x, int n)
{
	for (int i = 1; i <= n; i++)
		x = (1.0f - cos(M_PI * x)) / 2.0f;
	for (int i = -1; i >= n; i--)
		x = acos(1.0f - 2.0f * x) / M_PI;
	return x;
}

inline float scale_expand_towards_1(float x, int n)
{
	for (int i = 1; i <= n; i++)
		x = sin(x * M_PI / 2.0f);
	for (int i = -1; i >= n; i--)
		x = asin(x) * 2.0f / M_PI;
	return x;
}

inline float scale_to_0_1(float x, float min, float max)
{
	return (x - min) / (max - min);
}

inline float scale_and_clamp_to_0_1(float x, float min, float max)
{
	float v = (x - min) / (max - min);
	return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

inline void scale_centering_around_0(float& min, float& max)
{
	float new_max = std::max(max, -min);
	min = std::min(min, -max);
	max = new_max;
}

inline float scale_to_0_1_around_one_half(float x, float min, float max)
{
	float ma = std::max(max, -min);
	float mi = std::min(min, -max);
	return (x - mi) / (ma - mi);
}

} // namespace Utils

} // namespace CGoGN

#endif
