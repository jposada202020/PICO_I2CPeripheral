/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jose David Montoya
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/i2cperipheral/I2CPeripheral.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "lib/utils/interrupt_char.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"


STATIC mp_obj_t mp_obj_new_i2cperipheral_i2c_peripheral_request(i2cperipheral_i2c_peripheral_obj_t *peripheral, uint8_t address, bool is_read, bool is_restart) {
    i2cperipheral_i2c_peripheral_request_obj_t *self = m_new_obj(i2cperipheral_i2c_peripheral_request_obj_t);
    self->base.type = &i2cperipheral_i2c_peripheral_request_type;
    self->peripheral = peripheral;
    self->address = address;
    self->is_read = is_read;
    self->is_restart = is_restart;
    return (mp_obj_t)self;
}

//| class I2CPeripheral:
//|     """Two wire serial protocol peripheral"""
//|
//|     def __init__(self, scl: microcontroller.Pin, sda: microcontroller.Pin, addresses: Sequence[int], smbus: bool = False) -> None:
//|         """I2C is a two-wire protocol for communicating between devices.
//|         This implements the peripheral (sensor, secondary) side.
//|
//|         :param ~microcontroller.Pin scl: The clock pin
//|         :param ~microcontroller.Pin sda: The data pin
//|         :param frequency: The I2C frequency.
//|         :param timeout: timeout
//|         :param address: address of the salve device"""
//|         ...
//|

STATIC mp_obj_t i2cperipheral_i2c_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    i2cperipheral_i2c_peripheral_obj_t *self = m_new_obj(i2cperipheral_i2c_peripheral_obj_t);
    self->base.type = &i2cperipheral_i2c_peripheral_type;
    enum {ARG_scl, ARG_sda, ARG_frequency, ARG_timeout, ARG_address};
    static const mp_arg_t allowed args[] = {
    { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_timeout, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t* scl = validate_obj_is_free_pin(args[ARG_scl].u_obj);
    const mcu_pin_obj_t* sda = validate_obj_is_free_pin(args[ARG_sda].u_obj);

    uint32_t *frequency = NULL;
    uint32_t *timeout = NULL; //TODO:verify if the * goes here
    uint8_t *address = NULL; //TODO: verify if the * goes here

    //TODO verify if adress given is not out of bounds
    common_hal_i2cperipheral_i2c_peripheral_construct(self, scl, sda, frequency, timeout, address);
    return (mp_obj_t)self;
    }

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_obj_deinit(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &i2cperipheral_i2c_peripheral_type));
    i2cperipheral_i2c_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_i2cperipheral_i2c_peripheral_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cperipheral_i2c_peripheral_deinit_obj, i2cperipheral_i2c_peripheral_obj_deinit);

//|     def __enter__(self) -> I2CPeripheral:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_obj___exit__(size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_TYPE(args[0], &i2cperipheral_i2c_peripheral_type));
    i2cperipheral_i2c_peripheral_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_i2cperipheral_i2c_peripheral_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2cperipheral_i2c_peripheral___exit___obj, 4, 4, i2cperipheral_i2c_peripheral_obj___exit__);