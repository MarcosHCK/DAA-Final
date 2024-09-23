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
import random

class Rectangle:

  def __init__ (self, x, y, width, height):

    self.x = x
    self.y = y
    self.width = width
    self.height = height

def check_collision (rect1: Rectangle, rect2: Rectangle):
  
  return not (rect1.x + rect1.width <= rect2.x or
              rect2.x + rect2.width <= rect1.x or
              rect1.y + rect1.height <= rect2.y or
              rect2.y + rect2.height <= rect1.y)

def generate_non_overlapping_rectangles (num_rects: int, area_width: int, area_height: int, max_size: int):
  
  rectangles: list[Rectangle] = []
  tries = 0

  for _ in range (num_rects):

    new_rect = generate_random_rectangle (area_width, area_height, max_size)
    
    collides = False
    
    for rect in rectangles:

      if check_collision (new_rect, rect):

        collides = True
        tries += 1
        break

    if not collides:

      tries = 0
      rectangles.append (new_rect)
    elif tries > 9:
      break

  return rectangles

def generate_random_rectangle (area_width: int, area_height: int, max_size: int):

  rect_width = random.randint (1, max_size)
  rect_height = random.randint (1, max_size)
  
  # Ensure the rectangle fits within the area
  rect_x = random.randint (0, area_width - rect_width)
  rect_y = random.randint (0, area_height - rect_height)
  
  return Rectangle (rect_x, rect_y, rect_width, rect_height)

if __name__ == '__main__':

  area_factor = random.randint (1, 30)
  area_height = 1000
  area_width = 1000
  rectangles = []
  max_rectangle_size = 100
  num_rectangles = 50

  for xfactor in range (area_factor):

    for yfactor in range (area_factor):

      xstart = area_width * xfactor
      ystart = area_height * yfactor

      chunk = generate_non_overlapping_rectangles (num_rectangles, area_width, area_height, max_rectangle_size)
      chunk = [ Rectangle (xstart + r.x, ystart + r.y, r.width, r.height) for r in chunk ]

      rectangles += chunk

  print (len (rectangles))

  for rect in rectangles:

    print (f'{rect.x} {rect.y} {rect.x + rect.width} {rect.y + rect.height}')
