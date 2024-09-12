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
from sklearn.neighbors import BallTree
import numpy as np

def collect ():

  while True:

    yield tuple (map (int, input ().split (' ')))

def program ():

  ndwarfs, nstations = next (collect ())
  dwarfs = np.array ([ [x, y] for _, (x, y) in zip (range (ndwarfs), collect ()) ])
  stations = np.array ([ [x, y] for _, (x, y) in zip (range (nstations), collect ()) ])
  kdtree = BallTree (stations)

  print ('dwarfs: ', dwarfs)
  print ('stations: ', stations)

  _, indices = kdtree.query (dwarfs)
  nearest_station = [ stations [k [0]] for k in indices ]
  print (nearest_station)

program ()
