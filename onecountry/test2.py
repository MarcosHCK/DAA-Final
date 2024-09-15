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
from sys import stderr

def program ():

  width = 100000
  nrecs = randint (100, 10000)
  size = width // nrecs
  xoff = 0
  yoff = 0

  print (f'recs = {nrecs * 2}', file = stderr)
  print (f'size = {size}', file = stderr)
  print (nrecs * 2)

  for _ in range (nrecs):

    x0, y0 = xoff, yoff
    x1, y1 = (xoff := xoff + size), width

    print (x0, y0, x1, y1)

    x0, y0 = xoff, yoff
    x1, y1 = width, (yoff := yoff + size)

    print (x0, y0, x1, y1)

program ()
