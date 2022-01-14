
__all__ = [
    'loadNode'
]


def loadNode(session, builderPath, programPath=None, programName=None):
    """
    Load a node made of a program and a builder script into a ll.Session

    Parameters
    ----------
    session : lluvia.Session
        The session into which the node will be loaded
    
    builderPath : str
        The path to the script file defining the node's builder code.
    
    programPath : str. Defaults to None.
        The path of the compiled program file (e.g., .comp.spv extension).
        If None, no program is loaded.
    
    programName : str. Defaults to None.
        The name given to the given program.
    
    Internally this method uses `rules_python.python.runfiles` to resolve the
    absolute paths of the program and script files.
    """

    from rules_python.python.runfiles import runfiles

    r = runfiles.Create()

    scriptAbsolutePath = r.Rlocation(builderPath)
    session.scriptFile(scriptAbsolutePath)

    if programPath is not None:
        programAbsolutePath = r.Rlocation(programPath)
        program = session.createProgram(programAbsolutePath)
        session.setProgram(programName, program)
    
