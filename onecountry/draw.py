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
from matplotlib.patches import Rectangle
from typing import Any, Generator, NoReturn
import matplotlib.pyplot as plt

def collect () -> Generator[tuple[int, ...], Any, NoReturn]:

  while True:

    yield tuple (map (int, input ().split (' ')))

def program ():

  (ncastles,) = next (collect ())
  rectangles = [ ((x1, y1), (x2, y2)) for _, (x1, y1, x2, y2) in zip (range (ncastles), collect ()) ]

  fig, ax = plt.subplots()

  for (x0, y0), (x1, y1) in rectangles:

    width = abs (x1 - x0)
    height = abs (y1 - y0)

    rect = Rectangle ((min (x0, x1), min (y0, y1)), width, height, fill = False, edgecolor = 'black')
    ax.add_patch (rect)

  ax.set_xlim (-1, max (max (x0, x1) for (x0, _), (x1, _) in rectangles) + 1)
  ax.set_ylim (-1, max (max (y0, y1) for (_, y0), (_, y1) in rectangles) + 1)
  ax.set_aspect ('equal')

  plt.show ()

program ()
