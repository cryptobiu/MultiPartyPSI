#!/bin/sh
#  This file is part of the gf2x library.
#
#  Copyright 2007, 2008, 2009, 2010
#  Richard Brent, Pierrick Gaudry, Emmanuel Thome', Paul Zimmermann
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the
#  Free Software Foundation; either version 2 of the License, or (at your
#  option) any later version.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; see the file COPYING.  If not, write to the Free
#  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
#  02111-1307, USA.



cat "$srcdir/check-mul.res" | while read n1 n2 v s ; do
    echo -n "${n1}x${n2} "
    got=`./check-mul $n1 $n2`
    expected="$n1 $n2 $v $s"
    if [ "$got" != "$expected" ] ; then
        echo "failed check for ${n1}x${n2} : '$got' != '$expected'" >&2
        echo "failed : '$got' != '$expected'"
        exit 1
    fi
done
echo

./check-addmul

exit $?
