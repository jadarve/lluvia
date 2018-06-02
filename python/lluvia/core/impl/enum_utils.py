"""
    lluvia.core.impl.enum_utils
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

__all__ = ['createEnumeration', 'validateFlagStrings']


def validateFlagStrings(correctFlags, userFlags):

    for uFlag in userFlags:
        if not uFlag in correctFlags:
            raise ValueError('Incorrect flag value [{0}]. The value must be one of {1}.'.format(uFlag, correctFlags))


def createEnumeration(name, enableReverseMapping, **kwargs):
    """
    Creates a new enumeration type.

    Parameters
    ----------
    name : string.
        name of the enumeration.

    reverseMapping : bool.
        If True, adds `reserve_mapping` method
        to convert from value to enum name.

    **kwargs : keyword arguments
        Each kwarg will be converted to one enum name-value pair.

    Example
    -------
    ```
        MemoryFlags = createEnumeration('MemoryFlags', False,
            DeviceLocal = 'DEVICE_LOCAL',
            HostVisible = 'HOST_VISIBLE')
    ```
    Code above will create enumeration MemoryFlags with names
    DeviceLocal and HostVisible.

    Calling
    ```
        print(MemoryFlags.DeviceLocal)
    ```
    will print 'DEVICE_LOCAL'

    Returns
    -------
    E : new enumeration type
    """
    assert(type(name) == str)
    

    body = dict(**kwargs)
    
    if enableReverseMapping:
        reverse = dict((value, key) for key, value in body.iteritems())
        body['reverse_mapping'] = reverse
    
    return type(name, (), body)
