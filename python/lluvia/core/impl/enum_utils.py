"""
    lluvia.core.impl.enum_utils
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

__all__ = ['validateFlagStrings']


def validateFlagStrings(correctFlags, userFlags):

    for uFlag in userFlags:
        if not uFlag in correctFlags:
            raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(uFlag, correctFlags))
