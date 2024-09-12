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
import sys

def collect () -> Generator[tuple[int, ...], Any, NoReturn]:

  while True:

    yield tuple (map (int, input ().split (' ')))

Rectangles = list[tuple[tuple[int, int], tuple[int, int]]]

def isGood (recsx: Rectangles, recsy: Rectangles, axis = 0) -> bool:

  if len (recsx) < 2:

    return True
  else:

    assert (len (recsx) == len (recsy))
    lastp = 0

    match axis:

      case 0: recs = recsx
      case 1: recs = recsy
      case _: raise Exception ()

    for i in range (len (recs) - 1):

      lastp = max (lastp, recs [i] [1] [axis])
      firstp = recs [i + 1] [0] [axis]

      if firstp >= lastp:

        match axis:

          case 0:

            fs = set (recsx1 := recsx [:1 + i])
            recsy1 = list (filter (lambda x: x in fs, recsy))
            ss = set (recsx2 := recsx [1 + i:])
            recsy2 = list (filter (lambda x: x in ss, recsy))

          case 1:

            fs = set (recsy1 := recsy [:1 + i])
            recsx1 = list (filter (lambda x: x in fs, recsx))
            ss = set (recsy2 := recsy [1 + i:])
            recsx2 = list (filter (lambda x: x in ss, recsx))

        return isGood (recsx1, recsy1) and isGood (recsx2, recsy2)

    return False if axis > 0 else isGood (recsx, recsy, 1 + axis)

def program ():

  (ncastles,) = next (collect ())
  rectangles = [ ((x1, y1), (x2, y2)) for _, (x1, y1, x2, y2) in zip (range (ncastles), collect ()) ]
  rectangles = set (rectangles)

  recsx = sorted (rectangles, key = lambda x: x[0][0])
  recsy = sorted (rectangles, key = lambda x: x[0][1])

  print ('NO' if not isGood (recsx, recsy) else 'YES')

program ()
