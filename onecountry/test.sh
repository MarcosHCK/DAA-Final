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

check ()
  {
    if [ "$1" = "$2" ];
    then
      echo 'ok'
    else
      echo "not ok: got $1"
      echo -ne "$3" > failed
      exit 1
    fi
  }

test ()
  {
    sol=$1
    got=`echo -ne "$2" | $sol`

    check "$got" "$3" "$2"
  }

test './solution' '3\n0 0 1 1\n1 1 2 2\n0 1 1 2\n' 'YES'
test './solution' '4\n0 0 1 2\n0 2 1 3\n1 0 2 1\n1 1 2 3\n' 'YES'
test './solution' '4\n0 0 2 1\n1 2 3 3\n2 0 3 2\n0 1 1 3\n' 'NO'

for ((i = 0; i < 10; i++));
do
  arg=`python3 ../test.py`
  test './solution' "$arg" 'YES'
done
