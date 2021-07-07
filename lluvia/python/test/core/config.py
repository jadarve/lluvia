import sys
import os

from rules_python.python.runfiles import runfiles

def add_lluvia_import_path():

    r = runfiles.Create()
    llpath = r.Rlocation('lluvia/lluvia/python/src/')
    sys.path.append(llpath)

def print_file_tree(path, indent=0, c=False):

    if os.path.isfile(path):
        print('FILE:', path.ljust(indent))
        return

    elif os.path.isdir(path):
        for f in os.listdir(path):
            if f == 'lluvia' or c:
                print('FOLDER:', path.ljust(indent))
                print_file_tree(os.path.join(path, f), indent + 2, True)

    elif os.path.islink(path):
        print('LINK:', path)
