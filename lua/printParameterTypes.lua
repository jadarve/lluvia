
print('int', type(0))
print('float', type(0.5))
print('bool', type(false))
print('string', type('moni'))

print('type(type)', type(type(0)))

param = ll.Parameter.new()
param:set(156.89)

print(param:get())


desc = ll.ContainerNodeDescriptor.new()


-- parameter with default value
desc:setParameter('levels', 10)
print('getParam', desc:getParameter('levels'))

for i=1,desc:getParameter('levels') do
    print(i)
end


