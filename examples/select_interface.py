import machine

# if there are multiple u2if connected, we must specify its serial number by this command before using it.
# Test with your id (use machine_info.py test program to get it)
machine.select_interface("0xE66038B713644F31")

print('S/N: %s' % machine.unique_id())
print('V/N: %s' % machine.firmware_version())
