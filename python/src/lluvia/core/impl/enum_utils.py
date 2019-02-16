"""
    lluvia.core.impl.enum_utils
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from . import str_utils

__all__ = ['validateFlagStrings']


def validateFlagStrings(correctFlags, userFlags, forceList=False):

    if str_utils.isStringLike(userFlags):

        userFlags = str_utils.encodeString(userFlags)
        
        if not userFlags in correctFlags:
            raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(userFlags, correctFlags))

        return [userFlags] if forceList else userFlags

    userFlags = [str_utils.encodeString(s) for s in userFlags]

    for uFlag in userFlags:

        if not uFlag in correctFlags:
            raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(uFlag, correctFlags))

    return userFlags
