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

report='report'
solver='python3 solution.py'
tester='python3 ../test.py'
time='time\n  user: %U\n  system: %S\n  elapsed: %E\nmemory\n  shared: %X Kb\n  unshared: %D Kb\n  total: %M Kb'

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
    got=`echo -ne "$2" | time -f "$time" -ao $report -- $sol`

    check "$got" "$3" "$2"
  }

echo 'Tests' > $report

echo 'case0.1:' >> $report
test "$solver" '1 0\n2 -2\n' '0'
echo 'case0.2:' >> $report
test "$solver" '2 0\n5 -3\n-4 -5\n' '6'
echo 'case0.3:' >> $report
test "$solver" '2 2\n5 -3\n-4 -5\n-4 0\n-3 -2\n' '6'
exit 0

for ((i = 0; i < 1; i++));
do
  arg=`$tester`
  echo "case$((i+1)):" >> $report
  test "$solver" "$arg" 'YES' "testcase$i"
done
