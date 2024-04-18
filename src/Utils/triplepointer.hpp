/*
 * This program source code file is part of rpi-rgb-matrix-sink-server
 *
 * Copyright (C) 2024 Zachary Walden zachary.walden@eagles.oc.edu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/gpl-2.0.en.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

//Allocate Triple Pointer
template <typename T>
T*** allocTriplePointer(int y, int x, int depth, T initial_value)
{
	T*** ret_value = new T**[y];
	for(int i = 0; i < y; i++)
	{
		ret_value[i] = new T*[x];
		for(int j = 0; j < x; j++)
		{
			ret_value[i][j] = new T[depth];
			//Initialize values
			for(int k = 0; k < depth; k++)
			{
				ret_value[i][j][k] = initial_value;
			}
		}
	}
	return ret_value;
}

//De-Allocate Triple Pointer
template <typename T>
void deallocTriplePointer(T*** pointer, int y, int x)
{
	for(int i = 0; i < y; i++)
	{
		for(int j = 0; j < x; j++)
		{
			delete [] pointer[i][j];
		}
		delete [] pointer[i];
	}
	delete [] pointer;
}
