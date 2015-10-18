/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#include "unitsconvert.h"

int to20(int nbr) { return (int)((float)nbr/(float)16*20+0.5); }
int to16(int nbr) { return (int)((float)nbr/(float)20*16+0.5); }
int toNext10(int nbr) { return ((nbr + 5) / 10 * 10); }
int toNext20(int nbr) { return ((nbr + 10) / 20 * 20); }

