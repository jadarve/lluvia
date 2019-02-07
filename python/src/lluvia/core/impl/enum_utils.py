"""
    lluvia.core.impl.enum_utils
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

__all__ = ['validateFlagStrings']


def validateFlagStrings(correctFlags, userFlags, forceList=False):

    # if type(userFlags) is str or type(userFlags) is bytes:

    isStringLike = type(userFlags) is str or type(userFlags) is bytes

    if isStringLike:

        userFlags = bytes(userFlags, 'utf-8') if type(userFlags) is str else userFlags
        
        if not userFlags in correctFlags:
            raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(userFlags, correctFlags))

        return [userFlags] if forceList else userFlags

    # if type(userFlags) is str:

    #     if not userFlags in correctFlags:
    #         raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(userFlags, correctFlags))

    #     flagBytes = bytes(userFlags, 'utf-8')

    #     return [flagBytes] if forceList else flagBytes

    # if type(userFlags) is bytes:

    #     correctFlagsBytes = [bytes(s, 'utf-8') for s in correctFlags]

    #     if not userFlags in correctFlagsBytes:
    #         raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(userFlags, correctFlags))

    #     return [userFlags] if forceList else userFlags

    userFlags = [bytes(s, 'utf-8') if type(s) is str else s for s in userFlags]

    for uFlag in userFlags:

        # uFlag = bytes(uFlag, 'utf-8') if type(uFlag) is str else uFlag

        if not uFlag in correctFlags:
            raise ValueError('Incorrect flag value \'{0}\'. The value must be one of {1}.'.format(uFlag, correctFlags))

    return userFlags
    # return [bytes(s, 'utf-8') for s in userFlags]
