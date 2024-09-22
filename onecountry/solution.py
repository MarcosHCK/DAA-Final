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
from typing import Any, Callable, Generator, Generic, Iterable, NamedTuple, NoReturn, TypeVar, TypeVarTuple
import sys

class Point (NamedTuple):

  x: int
  y: int

  @classmethod
  def __default__ (cls):

    return Point (0, 0)

class Rectangle:

  @classmethod
  def __default__ (cls):

    b = Point.__default__ ()
    t = Point.__default__ ()
    return Rectangle (b, t)

  def __init__ (self, bottomleft: Point, topright: Point):

    self._deleted = False

    self.bottomleft = bottomleft
    self.topright = topright

  def __str__ (self) -> str:

    b = f'({self.bottomleft.x}, {self.bottomleft.y})'
    t = f'({self.topright.x}, {self.topright.y})'
    return f'({b}, {t})'

  def copy (self):

    b = Point (self.bottomleft.x, self.bottomleft.y)
    t = Point (self.topright.x, self.topright.y)
    return Rectangle (b, t)

  @property
  def deleted (self):

    return self._deleted

  @deleted.setter
  def deleted (self, value):

    self._deleted = value

def left (node: int):

  return 2 * node + 1

def mid (l: int, r: int):

  return l + (r - l) // 2

def right (node: int):

  return 2 * node + 2

T = TypeVar ('T')
Ts = TypeVarTuple ('Ts')

def recorder (func: Callable [[*Ts], T]) -> Callable [[*Ts], T]:

  def ff (*args: *Ts, **kwargs):

    _a = ', '.join ([ str (a) for a in args ])
    _b = ', '.join ([ f'{str (k)} = {str (v)}' for k, v in kwargs.items () ])

    print (f'{func.__name__} (' + (_a + (', ' if len (_b) > 0 else '') + _b) + ')')
    return func (*args, **kwargs)

  return ff

class SegmentTree ():

  def __init__ (self, rects: list[Rectangle]):

    self.rects: list [Rectangle] = rects
    self.tree: list [Rectangle] = [Rectangle.__default__ ()] * (len (rects) * 4)

    self.build (0, 0, len (rects) - 1)

  @recorder
  def build (self, node: int, s: int, e: int):

    if s == e:

      self.tree [node] = self.copyNode (self.rects [s])
    else:

      m = mid (s, e)
      self.build (l := left (node), s, m)
      self.build (r := right (node), m + 1, e)

      self.tree [node] = self.buildNode (self.tree [l], self.tree [r])

  def buildNode (self, l: Rectangle, r: Rectangle):

    m0 = min (l.bottomleft.x, r.bottomleft.x)
    m1 = min (l.bottomleft.y, r.bottomleft.y)
    m2 = max (l.topright.x, r.topright.x)
    m3 = max (l.topright.y, r.topright.y)

    return Rectangle (Point (m0, m1), Point (m2, m3))

  def copyNode (self, r: Rectangle):

    m0 = r.bottomleft.x
    m1 = r.bottomleft.y
    m2 = r.topright.x
    m3 = r.topright.y

    return Rectangle (Point (m0, m1), Point (m2, m3))

  def query (self, l: int, r: int):

    return self.queryNode (0, 0, len (self.rects) - 1, l, r)

  def queryNode (self, node: int, s: int, e: int, l: int, r: int):

    if r < s or e < l:
      return Rectangle.__default__ ()

    if (l <= s and e <= r):

      return self.copyNode (self.tree [node])
    else:

      m = mid (s, e)
      self.queryNode (l_ := left (node), s, m, l, r)
      self.queryNode (r_ := right (node), m + 1, e, l, r)

      return self.buildNode (self.tree [l_], self.tree [r_])

  def updateNode (self, node: int, s: int, e: int, idx: None | int = None):

    if s == e:

      v = self.rects [s]
      self.tree [node] = self.copyNode (v)
    elif idx == None:

      m = mid (s, e)
      self.updateNode (l := left (node), s, m, idx)
      self.updateNode (r := right (node), m + 1, e, idx)

      self.tree [node] = self.buildNode (self.tree [l], self.tree [r])
    else:

      if (m := mid (s, e)) > idx:

        self.updateNode (l := left (node), m + 1, e, idx)
        r = right (node)
      else:
        self.updateNode (r := right (node), m + 1, e, idx)
        l = left (node)

      self.tree [node] = self.buildNode (self.tree [l], self.tree [r])

def collect () -> Generator[tuple[int, ...], Any, NoReturn]:

  while True:

    yield tuple (map (int, input ().split (' ')))

T = TypeVar ('T')

def halver (from_: list[T]) -> Generator[T, Any, None]:

  for value, _ in zip (from_, range ((1 + len (from_)) // 2)):

    yield value

def jumper (from_: Iterable[Rectangle]) -> Generator[int, Any, None]:

  for i, value in enumerate (from_):

    if value.deleted == False: yield i

def isGood (rects: list [Rectangle]) -> bool:

  if len (rects) < 2:

    return True
  else:

    cut = 1
    x0r = jumper (halver (x0r_ := sorted (rects, key = lambda r: r.bottomleft.x, reverse = False)))
    x1r = jumper (halver (x1r_ := sorted (rects, key = lambda r: r.topright.x, reverse = False)))
    y0r = jumper (halver (y0r_ := sorted (rects, key = lambda r: r.bottomleft.y, reverse = True)))
    y1r = jumper (halver (y1r_ := sorted (rects, key = lambda r: r.topright.y, reverse = True)))
    x0tree = SegmentTree (x0r_)
    x1tree = SegmentTree (x1r_)
    y0tree = SegmentTree (y0r_)
    y1tree = SegmentTree (y1r_)

    firstx0r: None | int = None
    firstx1r: None | int = None
    firsty0r: None | int = None
    firsty1r: None | int = None

    def cutAt (p: int):

      print (p)

    while cut > 0:

      cut = 0
      if firstx0r == None or rects [firstx0r].deleted: firstx0r = next (x0r)
      if firstx1r == None or rects [firstx1r].deleted: firstx1r = next (x1r)
      if firsty0r == None or rects [firsty0r].deleted: firsty0r = next (y0r)
      if firsty1r == None or rects [firsty1r].deleted: firsty1r = next (y1r)

      while True:

        try:
          second = next (x0r)
          if (rects [second].bottomleft.x >= x0tree.query (0, second - 1).topright.x):
            print (f'cut at left x ...|{second}', file = sys.stderr)
            cutAt (second)
        except StopIteration:
          break

        try:
          second = next (y0r)
          if (rects [second].bottomleft.y >= y0tree.query (0, second - 1).topright.y):
            print (f'cut at left y ...|{second}', file = sys.stderr)
            cutAt (second)
        except StopIteration:
          break

        try:
          second = next (x1r)
          if (rects [second].topright.x <= x1tree.query (second + 1, len (rects) - 1).bottomleft.x):
            print (f'cut at right x ...|{second}', file = sys.stderr)
            cutAt (second)
        except StopIteration:
          break

        try:
          second = next (y1r)
          if (rects [second].topright.y <= y1tree.query (second + 1, len (rects) - 1).bottomleft.y):
            print (f'cut at right y ...|{second}', file = sys.stderr)
            cutAt (second)
        except StopIteration:
          break

    return False
    return isGood (list (filter (lambda r: not r.deleted, rects)))

def program ():

  (ncastles,) = next (collect ())
  rectangles = [ Rectangle (Point (x1, y1), Point (x2, y2)) for _, (x1, y1, x2, y2) in zip (range (ncastles), collect ()) ]

  print ('NO' if not isGood (rectangles) else 'YES')

program ()
