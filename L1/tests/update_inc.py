#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

import argparse
import os
import sys
import re

common_dir = os.path.dirname(os.path.abspath(__file__))

def update_srcs(file_name, dry_run=False):
    with open(file_name, "r") as f:
        lines = f.readlines()
    newlines = []
    p = re.compile("(.*)-I\s*\$\{CASE_ROOT\}[/.]*include(.*)")
    for l in lines:
        m = p.match(l)
        if m:
            nl = m.group(1) + "${XF_PROJ_ROOT}/L1/include" + m.group(2) + "\n"
            newlines.append(nl)
        else:
            newlines.append(l)
    if dry_run:
        for l in newlines:
             print(l, end="")
    else:
        with open(file_name, "w") as f:
            for l in newlines:
                f.write(l)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--dry_run", help="Dry run, just print to stdout",
                        action="store_true")
    parser.add_argument("file", help="File to be updated")
    args = parser.parse_args()

    if args.file is None or not os.path.isfile(args.file):
        parser.print_help()
        return 1

    update_srcs(args.file, args.dry_run)

if __name__ == '__main__':
    main()
