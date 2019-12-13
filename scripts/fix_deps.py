#
# Utility to automatically patch build issues in submodules.
#

import os
import re
import sys
import fileinput

def fix_solidity(path):
    target = re.compile("\$\{CMAKE_SOURCE_DIR\}(?!/solidity)")
    newstr = "${CMAKE_SOURCE_DIR}/solidity"
    for (root, dirs, files) in os.walk(path):
        if "CMakeLists.txt" in files:
            cmakefn = os.path.join(root, "CMakeLists.txt")
            with fileinput.FileInput(cmakefn, inplace=True) as cmakefile:
                for line in cmakefile:
                    print(re.sub(target, newstr, line), end="")

def main():
    filepath = os.path.realpath(__file__)
    reporoot = os.path.dirname(os.path.dirname(filepath))
    fix_solidity(os.path.join(reporoot, "solidity"))

if __name__ == "__main__":
    main()

