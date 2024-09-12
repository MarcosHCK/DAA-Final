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
from random import randint

def program ():

  x = 0
  y = 0

  nrecs = randint (0, 100000)
  recs = []

  for _ in range (nrecs):

    rec1 = (x, y)
    rec2 = (x := 1 + randint (x, x + 10), y := 1 + randint (y, y + 10))

    recs.append ((rec1, rec2))

  for _ in range (nrecs):

    i = randint (0, nrecs - 1)
    j = randint (0, nrecs - 1)

    if i != j:

      t = recs [i]
      recs [i] = recs [j]
      recs [j] = t

  print (nrecs)

  for r in recs:

    (x1, y1), (x2, y2) = r
    print (f'{x1} {y1} {x2} {y2}')

program ()
