# From https://github.com/adafruit/Adafruit_CircuitPython_VL53L0X which is originally licensed : see below.
# Minor changes to work on micropython
# ********************************************************************************************************

# SPDX-FileCopyrightText: 2017 Tony DiCola for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Caution: this version is modified to work with u2if python library

"""
`adafruit_vl53l0x`
====================================================

CircuitPython driver for the VL53L0X distance sensor.  This code is adapted
from the pololu driver here:
https://github.com/pololu/vl53l0x-arduino

See usage in the examples/vl53l0x_simpletest.py file.

* Author(s): Tony DiCola

Implementation Notes
--------------------

**Hardware:**

* Adafruit `VL53L0X Time of Flight Distance Sensor - ~30 to 1000mm
  <https://www.adafruit.com/product/3317>`_ (Product ID: 3317)

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the ESP8622 and M0-based boards:
  https://github.com/adafruit/circuitpython/releases
* Adafruit's Bus Device library: https://github.com/adafruit/Adafruit_CircuitPython_BusDevice
"""
import math
import time

#import adafruit_bus_device.i2c_device as i2c_device
from micropython import const

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_VL53L0X.git"

# Configuration constants:
_SYSRANGE_START = const(0x00)
_SYSTEM_THRESH_HIGH = const(0x0C)
_SYSTEM_THRESH_LOW = const(0x0E)
_SYSTEM_SEQUENCE_CONFIG = const(0x01)
_SYSTEM_RANGE_CONFIG = const(0x09)
_SYSTEM_INTERMEASUREMENT_PERIOD = const(0x04)
_SYSTEM_INTERRUPT_CONFIG_GPIO = const(0x0A)
_GPIO_HV_MUX_ACTIVE_HIGH = const(0x84)
_SYSTEM_INTERRUPT_CLEAR = const(0x0B)
_RESULT_INTERRUPT_STATUS = const(0x13)
_RESULT_RANGE_STATUS = const(0x14)
_RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN = const(0xBC)
_RESULT_CORE_RANGING_TOTAL_EVENTS_RTN = const(0xC0)
_RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF = const(0xD0)
_RESULT_CORE_RANGING_TOTAL_EVENTS_REF = const(0xD4)
_RESULT_PEAK_SIGNAL_RATE_REF = const(0xB6)
_ALGO_PART_TO_PART_RANGE_OFFSET_MM = const(0x28)
_I2C_SLAVE_DEVICE_ADDRESS = const(0x8A)
_MSRC_CONFIG_CONTROL = const(0x60)
_PRE_RANGE_CONFIG_MIN_SNR = const(0x27)
_PRE_RANGE_CONFIG_VALID_PHASE_LOW = const(0x56)
_PRE_RANGE_CONFIG_VALID_PHASE_HIGH = const(0x57)
_PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT = const(0x64)
_FINAL_RANGE_CONFIG_MIN_SNR = const(0x67)
_FINAL_RANGE_CONFIG_VALID_PHASE_LOW = const(0x47)
_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH = const(0x48)
_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = const(0x44)
_PRE_RANGE_CONFIG_SIGMA_THRESH_HI = const(0x61)
_PRE_RANGE_CONFIG_SIGMA_THRESH_LO = const(0x62)
_PRE_RANGE_CONFIG_VCSEL_PERIOD = const(0x50)
_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = const(0x51)
_PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO = const(0x52)
_SYSTEM_HISTOGRAM_BIN = const(0x81)
_HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT = const(0x33)
_HISTOGRAM_CONFIG_READOUT_CTRL = const(0x55)
_FINAL_RANGE_CONFIG_VCSEL_PERIOD = const(0x70)
_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = const(0x71)
_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO = const(0x72)
_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS = const(0x20)
_MSRC_CONFIG_TIMEOUT_MACROP = const(0x46)
_SOFT_RESET_GO2_SOFT_RESET_N = const(0xBF)
_IDENTIFICATION_MODEL_ID = const(0xC0)
_IDENTIFICATION_REVISION_ID = const(0xC2)
_OSC_CALIBRATE_VAL = const(0xF8)
_GLOBAL_CONFIG_VCSEL_WIDTH = const(0x32)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = const(0xB0)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_1 = const(0xB1)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_2 = const(0xB2)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_3 = const(0xB3)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_4 = const(0xB4)
_GLOBAL_CONFIG_SPAD_ENABLES_REF_5 = const(0xB5)
_GLOBAL_CONFIG_REF_EN_START_SELECT = const(0xB6)
_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = const(0x4E)
_DYNAMIC_SPAD_REF_EN_START_OFFSET = const(0x4F)
_POWER_MANAGEMENT_GO1_POWER_FORCE = const(0x80)
_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = const(0x89)
_ALGO_PHASECAL_LIM = const(0x30)
_ALGO_PHASECAL_CONFIG_TIMEOUT = const(0x30)
_VCSEL_PERIOD_PRE_RANGE = const(0)
_VCSEL_PERIOD_FINAL_RANGE = const(1)


def _decode_timeout(val):
    # format: "(LSByte * 2^MSByte) + 1"
    return float(val & 0xFF) * math.pow(2.0, ((val & 0xFF00) >> 8)) + 1


def _encode_timeout(timeout_mclks):
    # format: "(LSByte * 2^MSByte) + 1"
    timeout_mclks = int(timeout_mclks) & 0xFFFF
    ls_byte = 0
    ms_byte = 0
    if timeout_mclks > 0:
        ls_byte = timeout_mclks - 1
        while ls_byte > 255:
            ls_byte >>= 1
            ms_byte += 1
        return ((ms_byte << 8) | (ls_byte & 0xFF)) & 0xFFFF
    return 0


def _timeout_mclks_to_microseconds(timeout_period_mclks, vcsel_period_pclks):
    macro_period_ns = ((2304 * (vcsel_period_pclks) * 1655) + 500) // 1000
    return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns // 2)) // 1000


def _timeout_microseconds_to_mclks(timeout_period_us, vcsel_period_pclks):
    macro_period_ns = ((2304 * (vcsel_period_pclks) * 1655) + 500) // 1000
    return ((timeout_period_us * 1000) + (macro_period_ns // 2)) // macro_period_ns


class VL53L0X:
    """Driver for the VL53L0X distance sensor."""

    # Class-level buffer for reading and writing data with the sensor.
    # This reduces memory allocations but means the code is not re-entrant or
    # thread safe!
    _BUFFER = bytearray(3)

    def __init__(self, i2c, address=41, io_timeout_s=0):
        # pylint: disable=too-many-statements
        #self._i2c = i2c
        #self._device = i2c_device.I2CDevice(i2c, address)
        self._device = i2c
        self._address = address
        self.io_timeout_s = io_timeout_s
        # Check identification registers for expected values.
        # From section 3.2 of the datasheet.
        if (
                self._read_u8(0xC0) != 0xEE
                or self._read_u8(0xC1) != 0xAA
                or self._read_u8(0xC2) != 0x10
        ):
            raise RuntimeError(
                "Failed to find expected ID register values. Check wiring!"
            )
        # Initialize access to the sensor.  This is based on the logic from:
        #   https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
        # Set I2C standard mode.
        for pair in ((0x88, 0x00), (0x80, 0x01), (0xFF, 0x01), (0x00, 0x00)):
            self._write_u8(pair[0], pair[1])
        self._stop_variable = self._read_u8(0x91)
        for pair in ((0x00, 0x01), (0xFF, 0x00), (0x80, 0x00)):
            self._write_u8(pair[0], pair[1])
        # disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4)
        # limit checks
        config_control = self._read_u8(_MSRC_CONFIG_CONTROL) | 0x12
        self._write_u8(_MSRC_CONFIG_CONTROL, config_control)
        # set final range signal rate limit to 0.25 MCPS (million counts per
        # second)
        self.signal_rate_limit = 0.25
        self._write_u8(_SYSTEM_SEQUENCE_CONFIG, 0xFF)
        spad_count, spad_is_aperture = self._get_spad_info()
        # The SPAD map (RefGoodSpadMap) is read by
        # VL53L0X_get_info_from_device() in the API, but the same data seems to
        # be more easily readable from GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through
        # _6, so read it from there.
        ref_spad_map = bytearray(7)
        ref_spad_map[0] = _GLOBAL_CONFIG_SPAD_ENABLES_REF_0
        #with self._device:
            # self._device.write(ref_spad_map, end=1)
            # self._device.readinto(ref_spad_map, start=1)
        self._device.writeto(self._address, bytes([ref_spad_map[0]]))
        res = self._device.readfrom(self._address, 6)
        for i in range(len(res)):
            ref_spad_map[i+1] = res[i]

        for pair in (
                (0xFF, 0x01),
                (_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00),
                (_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C),
                (0xFF, 0x00),
                (_GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4),
        ):
            self._write_u8(pair[0], pair[1])

        first_spad_to_enable = 12 if spad_is_aperture else 0
        spads_enabled = 0
        for i in range(48):
            if i < first_spad_to_enable or spads_enabled == spad_count:
                # This bit is lower than the first one that should be enabled,
                # or (reference_spad_count) bits have already been enabled, so
                # zero this bit.
                ref_spad_map[1 + (i // 8)] &= ~(1 << (i % 8))
            elif (ref_spad_map[1 + (i // 8)] >> (i % 8)) & 0x1 > 0:
                spads_enabled += 1
        #with self._device:
        #    self._device.write(ref_spad_map)
        self._device.writeto(self._address, ref_spad_map)
        for pair in (
                (0xFF, 0x01),
                (0x00, 0x00),
                (0xFF, 0x00),
                (0x09, 0x00),
                (0x10, 0x00),
                (0x11, 0x00),
                (0x24, 0x01),
                (0x25, 0xFF),
                (0x75, 0x00),
                (0xFF, 0x01),
                (0x4E, 0x2C),
                (0x48, 0x00),
                (0x30, 0x20),
                (0xFF, 0x00),
                (0x30, 0x09),
                (0x54, 0x00),
                (0x31, 0x04),
                (0x32, 0x03),
                (0x40, 0x83),
                (0x46, 0x25),
                (0x60, 0x00),
                (0x27, 0x00),
                (0x50, 0x06),
                (0x51, 0x00),
                (0x52, 0x96),
                (0x56, 0x08),
                (0x57, 0x30),
                (0x61, 0x00),
                (0x62, 0x00),
                (0x64, 0x00),
                (0x65, 0x00),
                (0x66, 0xA0),
                (0xFF, 0x01),
                (0x22, 0x32),
                (0x47, 0x14),
                (0x49, 0xFF),
                (0x4A, 0x00),
                (0xFF, 0x00),
                (0x7A, 0x0A),
                (0x7B, 0x00),
                (0x78, 0x21),
                (0xFF, 0x01),
                (0x23, 0x34),
                (0x42, 0x00),
                (0x44, 0xFF),
                (0x45, 0x26),
                (0x46, 0x05),
                (0x40, 0x40),
                (0x0E, 0x06),
                (0x20, 0x1A),
                (0x43, 0x40),
                (0xFF, 0x00),
                (0x34, 0x03),
                (0x35, 0x44),
                (0xFF, 0x01),
                (0x31, 0x04),
                (0x4B, 0x09),
                (0x4C, 0x05),
                (0x4D, 0x04),
                (0xFF, 0x00),
                (0x44, 0x00),
                (0x45, 0x20),
                (0x47, 0x08),
                (0x48, 0x28),
                (0x67, 0x00),
                (0x70, 0x04),
                (0x71, 0x01),
                (0x72, 0xFE),
                (0x76, 0x00),
                (0x77, 0x00),
                (0xFF, 0x01),
                (0x0D, 0x01),
                (0xFF, 0x00),
                (0x80, 0x01),
                (0x01, 0xF8),
                (0xFF, 0x01),
                (0x8E, 0x01),
                (0x00, 0x01),
                (0xFF, 0x00),
                (0x80, 0x00),
        ):
            self._write_u8(pair[0], pair[1])

        self._write_u8(_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04)
        gpio_hv_mux_active_high = self._read_u8(_GPIO_HV_MUX_ACTIVE_HIGH)
        self._write_u8(
            _GPIO_HV_MUX_ACTIVE_HIGH, gpio_hv_mux_active_high & ~0x10
        )  # active low
        self._write_u8(_SYSTEM_INTERRUPT_CLEAR, 0x01)
        self._measurement_timing_budget_us = self.measurement_timing_budget
        self._write_u8(_SYSTEM_SEQUENCE_CONFIG, 0xE8)
        self.measurement_timing_budget = self._measurement_timing_budget_us
        self._write_u8(_SYSTEM_SEQUENCE_CONFIG, 0x01)
        self._perform_single_ref_calibration(0x40)
        self._write_u8(_SYSTEM_SEQUENCE_CONFIG, 0x02)
        self._perform_single_ref_calibration(0x00)
        # "restore the previous Sequence Config"
        self._write_u8(_SYSTEM_SEQUENCE_CONFIG, 0xE8)

    def _read_u8(self, address):
        # Read an 8-bit unsigned value from the specified 8-bit address.
        # with self._device:
        #     self._BUFFER[0] = address & 0xFF
        #     self._device.write(self._BUFFER, end=1)
        #     self._device.readinto(self._BUFFER, end=1)
        return self._device.readfrom_mem(self._address, address & 0xFF, 1)[0]

    def _read_u16(self, address):
        # Read a 16-bit BE unsigned value from the specified 8-bit address.
        # with self._device:
        #     self._BUFFER[0] = address & 0xFF
        #     self._device.write(self._BUFFER, end=1)
        #     self._device.readinto(self._BUFFER)
        #return (self._BUFFER[0] << 8) | self._BUFFER[1]
        buf = self._device.readfrom_mem(self._address, address & 0xFF, 2)
        return (buf[0] << 8) | buf[1]

    def _write_u8(self, address, val):
        # Write an 8-bit unsigned value to the specified 8-bit address.
        # with self._device:
        #     self._BUFFER[0] = address & 0xFF
        #     self._BUFFER[1] = val & 0xFF
        #     self._device.write(self._BUFFER, end=2)
        return self._device.writeto_mem(self._address, address & 0xFF, [val & 0xFF])

    def _write_u16(self, address, val):
        # Write a 16-bit BE unsigned value to the specified 8-bit address.
        # with self._device:
        #     self._BUFFER[0] = address & 0xFF
        #     self._BUFFER[1] = (val >> 8) & 0xFF
        #     self._BUFFER[2] = val & 0xFF
        #     self._device.write(self._BUFFER)
        #self._device.write(self._BUFFER)
        return self._device.writeto_mem(self._address, address & 0xFF, [(val >> 8) & 0xFF, val & 0xFF])

    def _get_spad_info(self):
        # Get reference SPAD count and type, returned as a 2-tuple of
        # count and boolean is_aperture.  Based on code from:
        #   https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
        for pair in ((0x80, 0x01), (0xFF, 0x01), (0x00, 0x00), (0xFF, 0x06)):
            self._write_u8(pair[0], pair[1])
        self._write_u8(0x83, self._read_u8(0x83) | 0x04)
        for pair in (
                (0xFF, 0x07),
                (0x81, 0x01),
                (0x80, 0x01),
                (0x94, 0x6B),
                (0x83, 0x00),
        ):
            self._write_u8(pair[0], pair[1])
        start = time.monotonic()
        while self._read_u8(0x83) == 0x00:
            if (
                    self.io_timeout_s > 0
                    and (time.monotonic() - start) >= self.io_timeout_s
            ):
                raise RuntimeError("Timeout waiting for VL53L0X!")
        self._write_u8(0x83, 0x01)
        tmp = self._read_u8(0x92)
        count = tmp & 0x7F
        is_aperture = ((tmp >> 7) & 0x01) == 1
        for pair in ((0x81, 0x00), (0xFF, 0x06)):
            self._write_u8(pair[0], pair[1])
        self._write_u8(0x83, self._read_u8(0x83) & ~0x04)
        for pair in ((0xFF, 0x01), (0x00, 0x01), (0xFF, 0x00), (0x80, 0x00)):
            self._write_u8(pair[0], pair[1])
        return (count, is_aperture)

    def _perform_single_ref_calibration(self, vhv_init_byte):
        # based on VL53L0X_perform_single_ref_calibration() from ST API.
        self._write_u8(_SYSRANGE_START, 0x01 | vhv_init_byte & 0xFF)
        start = time.monotonic()
        while (self._read_u8(_RESULT_INTERRUPT_STATUS) & 0x07) == 0:
            if (
                    self.io_timeout_s > 0
                    and (time.monotonic() - start) >= self.io_timeout_s
            ):
                raise RuntimeError("Timeout waiting for VL53L0X!")
        self._write_u8(_SYSTEM_INTERRUPT_CLEAR, 0x01)
        self._write_u8(_SYSRANGE_START, 0x00)

    def _get_vcsel_pulse_period(self, vcsel_period_type):
        # pylint: disable=no-else-return
        # Disable should be removed when refactor can be tested
        if vcsel_period_type == _VCSEL_PERIOD_PRE_RANGE:
            val = self._read_u8(_PRE_RANGE_CONFIG_VCSEL_PERIOD)
            return (((val) + 1) & 0xFF) << 1
        elif vcsel_period_type == _VCSEL_PERIOD_FINAL_RANGE:
            val = self._read_u8(_FINAL_RANGE_CONFIG_VCSEL_PERIOD)
            return (((val) + 1) & 0xFF) << 1
        return 255

    def _get_sequence_step_enables(self):
        # based on VL53L0X_GetSequenceStepEnables() from ST API
        sequence_config = self._read_u8(_SYSTEM_SEQUENCE_CONFIG)
        tcc = (sequence_config >> 4) & 0x1 > 0
        dss = (sequence_config >> 3) & 0x1 > 0
        msrc = (sequence_config >> 2) & 0x1 > 0
        pre_range = (sequence_config >> 6) & 0x1 > 0
        final_range = (sequence_config >> 7) & 0x1 > 0
        return (tcc, dss, msrc, pre_range, final_range)

    def _get_sequence_step_timeouts(self, pre_range):
        # based on get_sequence_step_timeout() from ST API but modified by
        # pololu here:
        #   https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
        pre_range_vcsel_period_pclks = self._get_vcsel_pulse_period(
            _VCSEL_PERIOD_PRE_RANGE
        )
        msrc_dss_tcc_mclks = (self._read_u8(_MSRC_CONFIG_TIMEOUT_MACROP) + 1) & 0xFF
        msrc_dss_tcc_us = _timeout_mclks_to_microseconds(
            msrc_dss_tcc_mclks, pre_range_vcsel_period_pclks
        )
        pre_range_mclks = _decode_timeout(
            self._read_u16(_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI)
        )
        pre_range_us = _timeout_mclks_to_microseconds(
            pre_range_mclks, pre_range_vcsel_period_pclks
        )
        final_range_vcsel_period_pclks = self._get_vcsel_pulse_period(
            _VCSEL_PERIOD_FINAL_RANGE
        )
        final_range_mclks = _decode_timeout(
            self._read_u16(_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI)
        )
        if pre_range:
            final_range_mclks -= pre_range_mclks
        final_range_us = _timeout_mclks_to_microseconds(
            final_range_mclks, final_range_vcsel_period_pclks
        )
        return (
            msrc_dss_tcc_us,
            pre_range_us,
            final_range_us,
            final_range_vcsel_period_pclks,
            pre_range_mclks,
        )

    @property
    def signal_rate_limit(self):
        """The signal rate limit in mega counts per second."""
        val = self._read_u16(_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT)
        # Return value converted from 16-bit 9.7 fixed point to float.
        return val / (1 << 7)

    @signal_rate_limit.setter
    def signal_rate_limit(self, val):
        assert 0.0 <= val <= 511.99
        # Convert to 16-bit 9.7 fixed point value from a float.
        val = int(val * (1 << 7))
        self._write_u16(_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, val)

    @property
    def measurement_timing_budget(self):
        """The measurement timing budget in microseconds."""
        budget_us = 1910 + 960  # Start overhead + end overhead.
        tcc, dss, msrc, pre_range, final_range = self._get_sequence_step_enables()
        step_timeouts = self._get_sequence_step_timeouts(pre_range)
        msrc_dss_tcc_us, pre_range_us, final_range_us, _, _ = step_timeouts
        if tcc:
            budget_us += msrc_dss_tcc_us + 590
        if dss:
            budget_us += 2 * (msrc_dss_tcc_us + 690)
        elif msrc:
            budget_us += msrc_dss_tcc_us + 660
        if pre_range:
            budget_us += pre_range_us + 660
        if final_range:
            budget_us += final_range_us + 550
        self._measurement_timing_budget_us = budget_us
        return budget_us

    @measurement_timing_budget.setter
    def measurement_timing_budget(self, budget_us):
        # pylint: disable=too-many-locals
        assert budget_us >= 20000
        used_budget_us = 1320 + 960  # Start (diff from get) + end overhead
        tcc, dss, msrc, pre_range, final_range = self._get_sequence_step_enables()
        step_timeouts = self._get_sequence_step_timeouts(pre_range)
        msrc_dss_tcc_us, pre_range_us, _ = step_timeouts[:3]
        final_range_vcsel_period_pclks, pre_range_mclks = step_timeouts[3:]
        if tcc:
            used_budget_us += msrc_dss_tcc_us + 590
        if dss:
            used_budget_us += 2 * (msrc_dss_tcc_us + 690)
        elif msrc:
            used_budget_us += msrc_dss_tcc_us + 660
        if pre_range:
            used_budget_us += pre_range_us + 660
        if final_range:
            used_budget_us += 550
            # "Note that the final range timeout is determined by the timing
            # budget and the sum of all other timeouts within the sequence.
            # If there is no room for the final range timeout, then an error
            # will be set. Otherwise the remaining time will be applied to
            # the final range."
            if used_budget_us > budget_us:
                raise ValueError("Requested timeout too big.")
            final_range_timeout_us = budget_us - used_budget_us
            final_range_timeout_mclks = _timeout_microseconds_to_mclks(
                final_range_timeout_us, final_range_vcsel_period_pclks
            )
            if pre_range:
                final_range_timeout_mclks += pre_range_mclks
            self._write_u16(
                _FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                _encode_timeout(final_range_timeout_mclks),
            )
            self._measurement_timing_budget_us = budget_us

    @property
    def range(self):
        """Perform a single reading of the range for an object in front of
        the sensor and return the distance in millimeters.
        """
        # Adapted from readRangeSingleMillimeters &
        # readRangeContinuousMillimeters in pololu code at:
        #   https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
        for pair in (
                (0x80, 0x01),
                (0xFF, 0x01),
                (0x00, 0x00),
                (0x91, self._stop_variable),
                (0x00, 0x01),
                (0xFF, 0x00),
                (0x80, 0x00),
                (_SYSRANGE_START, 0x01),
        ):
            self._write_u8(pair[0], pair[1])
        start = time.monotonic()
        while (self._read_u8(_SYSRANGE_START) & 0x01) > 0:
            if (
                    self.io_timeout_s > 0
                    and (time.monotonic() - start) >= self.io_timeout_s
            ):
                raise RuntimeError("Timeout waiting for VL53L0X!")
        start = time.monotonic()
        while (self._read_u8(_RESULT_INTERRUPT_STATUS) & 0x07) == 0:
            if (
                    self.io_timeout_s > 0
                    and (time.monotonic() - start) >= self.io_timeout_s
            ):
                raise RuntimeError("Timeout waiting for VL53L0X!")
        # assumptions: Linearity Corrective Gain is 1000 (default)
        # fractional ranging is not enabled
        range_mm = self._read_u16(_RESULT_RANGE_STATUS + 10)
        self._write_u8(_SYSTEM_INTERRUPT_CLEAR, 0x01)
        return range_mm

    # def set_address(self, new_address):
    #     """Set a new I2C address to the instantaited object. This is only called when using
    #     multiple VL53L0X sensors on the same I2C bus (SDA & SCL pins). See also the
    #     `example <examples.html#multiple-vl53l0x-on-same-i2c-bus>`_ for proper usage.
    #
    #     :param int new_address: The 7-bit `int` that is to be assigned to the VL53L0X sensor.
    #         The address that is assigned should NOT be already in use by another device on the
    #         I2C bus.
    #
    #     .. important:: To properly set the address to an individual VL53L0X sensor, you must
    #         first ensure that all other VL53L0X sensors (using the default address of ``0x29``)
    #         on the same I2C bus are in their off state by pulling the "SHDN" pins LOW. When the
    #         "SHDN" pin is pulled HIGH again the default I2C address is ``0x29``.
    #     """
    #     self._write_u8(_I2C_SLAVE_DEVICE_ADDRESS, new_address & 0x7F)
    #     self._device = i2c_device.I2CDevice(self._i2c, new_address)

    def start_measure(self):
        """Perform a single reading of the range for an object in front of
        the sensor and return the distance in millimeters.
        """
        # Adapted from readRangeSingleMillimeters &
        # readRangeContinuousMillimeters in pololu code at:
        #   https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
        for pair in (
                (0x80, 0x01),
                (0xFF, 0x01),
                (0x00, 0x00),
                (0x91, self._stop_variable),
                (0x00, 0x01),
                (0xFF, 0x00),
                (0x80, 0x00),
                (_SYSRANGE_START, 0x01),
        ):
            self._write_u8(pair[0], pair[1])
        start = time.monotonic()
        while (self._read_u8(_SYSRANGE_START) & 0x01) > 0:
            if (
                    self.io_timeout_s > 0
                    and (time.monotonic() - start) >= self.io_timeout_s
            ):
                raise RuntimeError("Timeout waiting for VL53L0X!")

    def read_measure(self):
        # assumptions: Linearity Corrective Gain is 1000 (default)
        # fractional ranging is not enabled
        range_mm = self._read_u16(_RESULT_RANGE_STATUS + 10)
        self._write_u8(_SYSTEM_INTERRUPT_CLEAR, 0x01)
        return range_mm
