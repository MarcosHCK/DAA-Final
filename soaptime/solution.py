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
from sklearn.neighbors import KDTree
from sys import stderr
import numpy as np

Point = tuple [int, int]
Line = tuple [Point, Point]
Rect = tuple [Point, Point, Point, Point]

def collect ():

  while True:

    yield tuple (map (int, input ().split (' ')))

def convex_hull (points: list[tuple[int,int]]):

  def orientation (p, q, r):

    if (val := (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1])) == 0:

      return 0
    else:
      return 1 if val > 0 else 2

  def recurse (points):

    if (n := len (points)) < 3:

      return points
    else:

      left_half = recurse (points [:n // 2])
      right_half = recurse (points [n // 2:])

      left_convex = [ ]
      right_convex = [ ]

      for i in range (len (left_half)):

        p = left_half [i]

        while len (right_convex) >= 2 and orientation (p, right_convex [-2], right_convex [-1]) != 2:
          right_convex.pop ()

        right_convex.append (p)

      for i in range (len (right_half) - 1, -1, -1):

        p = right_half [i]

        while len (left_convex) >= 2 and orientation (p, left_convex [-2], left_convex [-1]) != 2:
          left_convex.pop ()

        left_convex.append (p)

      return sorted (left_convex + right_convex)

  return recurse (points)

def intersection_point (a: Line, b: Line):

  s0 = np.array (a [0], np.float32)
  s1 = np.array (b [0], np.float32)
  e0 = np.array (a [1], np.float32)
  e1 = np.array (b [1], np.float32)

  r = e0 - s0
  s = e1 - s1
  q = s1 - s0

  if (d := np.cross (r, s)) != 0:

    t = np.dot (q, s) / d
    u = np.dot (q, r) / d

    if (0 <= t <= 1) and (0 <= u <= 1):

      return tuple (s0 + t * r)
  return None

def intersection_frame (a: Rect, b: Rect):

  print (f'rectangles {a} and {b}')

  ap = [ (s, e) for s, e in zip (a, map (lambda i: a[-1-i], range (len (a)))) ]
  bp = [ (s, e) for s, e in zip (b, map (lambda i: a[-1-i], range (len (b)))) ]
  ip = [ ]

  for al in ap:

    for bl in bp:

      if (point := intersection_point (al, bl)) != None:
        ip.append (point)

  print (ip)

  return None

def manhattan_distance (a: tuple[int, int], b: tuple[int, int]):

  return abs (a[0] - b[0]) + abs (a[1] - b[1])

def mid (s: int, t: int):

  return s + (t - s) // 2

def rotating_calipers (hull):

  n = len (hull)
  max_distance = 0
  points = None

  for i in range (n):

    for j in range (1 + i, n):

      if (distance := manhattan_distance (hull [i], hull [j])) > max_distance:

        max_distance = distance
        points = (hull [i], hull [j])

  return points, max_distance

def program ():

  ndwarfs, nstations = next (collect ())
  dwarfs = [ (x, y) for _, (x, y) in zip (range (ndwarfs), collect ()) ]
  stations = [ (x, y) for _, (x, y) in zip (range (nstations), collect ()) ]

  print ('dwarfs: ', dwarfs, file = stderr)
  print ('stations: ', stations, file = stderr)

  if ndwarfs < 2:

    print (0)
  else:

    if nstations < 2:

      hull = convex_hull (dwarfs)
      _, distance = rotating_calipers (hull)

      print (1 + (distance - 1) // 2)
    else:

      kdtree = KDTree (np.asarray (stations), metric = 'manhattan')
      distance, nearest = kdtree.query (dwarfs)

      dwarfs = [ (int (d), int (n), (x, y)) for [d], [n], (x, y) in zip (distance, nearest, dwarfs) ]
      dwarfs = sorted (dwarfs, key = lambda x: x[0])

      def too_far_to_board (e: int):

        for dwarf in dwarfs:

          if dwarf [0] > e: yield dwarf

      def nearest_station (e: int):

        for distance, n, _ in dwarfs:

          if distance <= e: return stations [n]

      def intersection_points (e: int):

        any = False

        for _, _, point in too_far_to_board (e):
          any = True
          yield point
        if (point := nearest_station (e)) != None and any:
          yield point

      def intersects (e: int):

        first = True

        for x, y in intersection_points (e):

          pb = (x - e, y)
          pl = (x, y - e)
          pr = (x, y + e)
          pt = (x + e, y)

          if first:

            first = False
            frame = (pt, pl, pb, pr)
          else:

            if (nextf := intersection_frame (frame, (pt, pl, pb, pr))) != None:

              frame = nextf
            else:
              return False

        return True

      def search_least (s: int, t: int):

        if (t - s) < 3:

          for e in range (s, t + 1):

            if intersects (e): return e
        else:

          m = mid (s, t)

          if intersects (m):

            return search_least (s, m)
          else:
            return search_least (m + 1, t)

        return None

      print (search_least (0, dwarfs [-1] [0]))

program ()
