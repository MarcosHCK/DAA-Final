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

BOTTOM = 0
TOP = 1

def collect () -> Generator[tuple[int, ...], Any, NoReturn]:

  while True:

    yield tuple (map (int, input ().split (' ')))

def isGood (recs: list[tuple[tuple[int, int], tuple[int, int]]], axis = 0) -> bool:

  if (size := len (recs)) < 2:

    return True
  else:

    recs = sorted (recs, key = lambda x: x [BOTTOM] [axis])
    rightmost = []

    for i in range (size):

      rightmost.append (max (0 if i == 0 else rightmost [i - 1], recs [i] [TOP] [axis]))

    i, j = [size // 2] * 2

    while (i > 0 or j < size - 1):

      if i > 0:

        if recs [i] [BOTTOM] [axis] < rightmost [i - 1]:

          i -= 1
        else:
          return isGood (recs [:i]) and isGood (recs [i:])

      if j < size - 1:

        if recs [j + 1] [BOTTOM] [axis] < rightmost [j]:

          j += 1
        else:
          return isGood (recs [:j + 1]) and isGood (recs [j + 1:])

    return False if axis > 0 else isGood (recs, 1 + axis)

def program ():

  (ncastles,) = next (collect ())
  rectangles = [ ((x1, y1), (x2, y2)) for _, (x1, y1, x2, y2) in zip (range (ncastles), collect ()) ]

  print ('NO' if not isGood (rectangles) else 'YES')

program ()
