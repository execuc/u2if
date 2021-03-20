from machine import I2C, SPI, u2if, Pin

# Run a scan on the second I2C (i2_index=1)


i2c = I2C(i2c_index=0, frequency=400000) # , pullup=True
slaves = i2c.scan()
print("i2c slaves found %s" % slaves)
