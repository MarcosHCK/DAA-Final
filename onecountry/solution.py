# Copyright 2024-2025 MarcosHCK
# This file is part of DAA-Final-Project.
#
# DAA-Final-Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DAA-Final-Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DAA-Final-Project. If not, see <http://www.gnu.org/licenses/>.
#
from typing import Any, Generator, NoReturn

def collect () -> Generator[tuple[int, ...], Any, NoReturn]:

  while True:

    yield tuple (map (int, input ().split (' ')))

def isGood (recs: list[tuple[tuple[int, int], tuple[int, int]]], axis = 0) -> bool:

  if len (recs) < 2:

    return True
  else:

    lastp = 0
    recs = sorted (recs, key = lambda x: x [0] [axis])

    for i in range (len (recs) - 1):

      lastp = max (lastp, recs [i] [1] [axis])
      firstp = recs [i + 1] [0] [axis]

      if firstp >= lastp:

        return isGood (recs [:1 + i]) and isGood (recs [1 + i:])

    return False if axis > 0 else isGood (recs, 1 + axis)

def program ():

  (ncastles,) = next (collect ())
  rectangles = [ ((x1, y1), (x2, y2)) for _, (x1, y1, x2, y2) in zip (range (ncastles), collect ()) ]

  print ('NO' if not isGood (rectangles) else 'YES')

program ()
