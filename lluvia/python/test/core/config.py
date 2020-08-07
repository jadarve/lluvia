import re
import sys
import os


def find_runfiles():
    """
    Find the runfiles tree (useful when _not_ run from a zip file)
    
    See https://stackoverflow.com/questions/41553609/what-is-the-right-way-to-refer-bazel-data-files-in-python
    """

    # Follow symlinks, looking for my module space
    stub_filename = os.path.abspath(sys.argv[0])
    while True:
        # Found it?
        module_space = stub_filename + '.runfiles'
        if os.path.isdir(module_space):
            break

        runfiles_pattern = r"(.*\.runfiles)"
        matchobj = re.match(runfiles_pattern, os.path.abspath(sys.argv[0]))
        if matchobj:
            module_space = matchobj.group(1)
            break

        raise RuntimeError('Cannot find .runfiles directory for %s' %
                           sys.argv[0])
    return module_space


def add_lluvia_import_path():

    runfile_root = find_runfiles()

    # remove the workspace root from the python path
    sys.path.remove(os.path.join(runfile_root, 'lluvia'))

    # add inner path where lluvia Python package starts
    llpath = os.path.join(runfile_root, 'lluvia/lluvia/python/src/')
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
