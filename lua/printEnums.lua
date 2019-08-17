function printEnum(name)

    print('------------------------------------------------')
    print('           '.. name)
    print('------------------------------------------------')
    enum = ll[name]

    for k, v in pairs(enum) do
        print(string.format('%40s - %s', k, v))
    end
    print('------------------------------------------------')
end

enums = {
    'ChannelCount',
    'ChannelType',
    'ImageAddressMode',
    'ImageAxis',
    'ImageFilterMode',
    'NodeState',
    'ObjectType',
    'PortDirection',
    'PortType',
    'BufferUsageFlagBits',
    'ImageLayout',
    'ImageUsageFlagBits'
}


for i, name in ipairs(enums) do
    printEnum(name)
end
