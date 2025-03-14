#!/usr/bin/env python

#
# Seccomp Library test program
#
# Copyright (c) 2023 Microsoft Corporation <paulmoore@microsoft.com>
# Author: Paul Moore <paul@paul-moore.com>
# Author: Tom Hromatka <tom.hromatka@oracle.com>
#

#
# This library is free software; you can redistribute it and/or modify it
# under the terms of version 2.1 of the GNU Lesser General Public License as
# published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, see <http://www.gnu.org/licenses>.
#

import argparse
import sys

import util

from seccomp import *

def test(args):
    f = SyscallFilter(ALLOW)

    f.remove_arch(Arch())
    f.add_arch(Arch("x86_64"))

    f.start_transaction()
    f.start_transaction()
    f.add_rule(KILL, "read")
    f.commit_transaction()
    f.add_arch(Arch("x86"))
    f.commit_transaction()

    f.add_rule(KILL, "write")

    f.start_transaction()
    f.start_transaction()
    f.remove_arch(Arch("x86"))
    f.commit_transaction()
    f.add_rule(KILL, "open")
    f.commit_transaction()

    return f

args = util.get_opt()
ctx = test(args)
util.filter_output(args, ctx)

# kate: syntax python;
# kate: indent-mode python; space-indent on; indent-width 4; mixedindent off;
