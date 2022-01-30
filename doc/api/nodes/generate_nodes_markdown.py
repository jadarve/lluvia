#!/usr/bin/env python3

import argparse
import jinja2
import os
from pathlib import Path

import sys
sys.path.append('build/python/')

import lluvia as ll


# Template to render each node
templateSource = """---
title: {{NAME}}
linkTitle: {{NAME}}
description: {{DESCRIPTION}}
collapsible: true
---

{{DOC_STRING}}
"""

# Template to render _index.md files for each folder
indexTemplateSource = """---
title: {{NAME}}
linkTitle: {{NAME}}
collapsible: true
---
"""

def foldersToCreate(descriptors):
    """Returns the folder paths that need to be created
    """

    dirSet = set()

    for desc in descriptors:
        head, tail = os.path.split(desc.name)

        while len(head) != 0:
            dirSet.add(head)
            head, tail = os.path.split(head)

    return dirSet



def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('output', type=str, help='output base path.')
    
    args = parser.parse_args()
    basePath = args.output

    session = ll.createSession()

    indexTemplate = jinja2.Template(source=indexTemplateSource, trim_blocks=True)
    template = jinja2.Template(source=templateSource, trim_blocks=True)

    for dirPathToCreate in foldersToCreate(session.getNodeBuilderDescriptors()):

        dirPath = os.path.join(basePath, dirPathToCreate)

        # create folder
        p = Path(dirPath)
        p.mkdir(parents=True, exist_ok=True)

        # for lluvia/opticalflow, subpackageName will be opticalflow
        _, subpackageName = os.path.split(dirPath)

        templateArgs = {
            'NAME': subpackageName
        }
        with open(os.path.join(dirPath, '_index.md'), mode='w') as f:
            renderedOut = indexTemplate.render(templateArgs)
            f.write(renderedOut)


    for desc in session.getNodeBuilderDescriptors():
        print('processing: {0}'.format(desc.name))

        head, tail = os.path.split(desc.name)
        dirPath = os.path.join(basePath, head)

        templateArgs = {
            'NAME': tail,
            'DOC_STRING': session.help(desc.name),
            'DESCRIPTION': desc.summary
        }

        templateOut = template.render(templateArgs)

        with open(os.path.join(dirPath, '{0}.md'.format(tail)), mode='w') as f:
            f.write(templateOut)

if __name__ == '__main__':
    main()
