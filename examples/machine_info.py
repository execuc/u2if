import machine

print('S/N: %s' % machine.unique_id())
print('V/N: %s' % machine.firmware_version())

