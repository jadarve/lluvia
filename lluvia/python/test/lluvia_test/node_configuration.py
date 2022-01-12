
__all__ = [
    'loadNode'
]

def loadNode(session, programPath, programName, scriptPath):
    """
    Load a node made of a program and a builder script into a ll.Session

    Parameters
    ----------
    session : lluvia.Session
        The session into which the node will be loaded
    
    programPath : str
        The path of the compiled program file (e.g., .comp.spv extension).
    
    programName : str
        The name given to the given program.
    
    scriptPath : str
        The path to the script file defining the node's builder code.
    
    Internally this method uses `rules_python.python.runfiles` to resolve the
    absolute paths of the program and script files.
    """

    from rules_python.python.runfiles import runfiles

    r = runfiles.Create()
    programAbsolutePath = r.Rlocation(programPath)
    scriptAbsolutePath = r.Rlocation(scriptPath)

    program = session.createProgram(programAbsolutePath)
    session.setProgram(programName, program)
    session.scriptFile(scriptAbsolutePath)
