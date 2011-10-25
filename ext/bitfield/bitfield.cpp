/**
 * Copyright (c) 2011, Fabian Becker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "ruby.h"
#include <boost/dynamic_bitset.hpp>
#include "bitfield.hpp"

static VALUE bf_init(VALUE self, VALUE size)
{
    Check_Type(size, T_FIXNUM);
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    ptr->data.resize(FIX2INT(size));
	return self;
}

static VALUE bf_new(VALUE self, VALUE size)
{
    VALUE *argv;
	Check_Type(size, T_FIXNUM);
    BitField *ptr;
    argv[0] = size;
	rb_obj_call_init(self, 1, argv);
	return self;
}

/**
 * Returns the size of the BitField
 *
 */
static VALUE bf_size(VALUE self)
{
	BitField *ptr;
	Data_Get_Struct(self, BitField, ptr);
	return INT2NUM(ptr->data.size());
}


/**
 * Flips the n-th bit if position is given 
 * flips all bits otherwise (TODO)
 */
static VALUE bf_flip(int argc, VALUE *argv, VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    if(argc == 1) {
	Check_Type(argv[0], T_FIXNUM);
	ptr->data[FIX2INT(argv[0])].flip();
    } else if(argc == 0) {
	ptr->data.flip();
    } else {
	rb_raise(rb_eArgError, "wrong number of arguments(%d for 1 or 0)", argc);
    }
    return Qnil;
}

/**
 * Returns the number of bits that are set
 *
 */
static VALUE bf_count(VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    return INT2NUM(ptr->data.count());
}

static VALUE bf_bit_set(VALUE self, VALUE position, VALUE value)
{
	BitField *ptr;
	Data_Get_Struct(self, BitField, ptr);

	if(rb_obj_is_kind_of(position, rb_cRange) 
		&& rb_obj_is_kind_of(value, rb_cArray)) {
		long beg, len;
		VALUE tmp;
		switch(rb_range_beg_len(position, &beg, &len, (long int)ptr->data.size()-1, 2)) {
			case Qfalse:
			case Qnil:
				return Qfalse;
			default:
				for(long i = beg+len-1; i >= beg; i--) {
					tmp = rb_ary_pop(value);
					if(tmp != Qnil) {
						ptr->data[i] = FIX2INT(tmp) % 2;
					} else {
						rb_raise(rb_eRangeError, "Array is smaller than given range.");
					}
				}
		}
	} else {
		/* Sanity checks for position and value */
		Check_Type(position, T_FIXNUM);
		Check_Type(value, T_FIXNUM);

		int pos = FIX2INT(position);
		if(pos < 0 || pos >= ptr->data.size()) {
			rb_raise(rb_eRangeError, "BitField out of range with value %d.", pos);
		}
		ptr->data[pos] = FIX2INT(value) % 2;
	}
	return Qnil;
}

static VALUE bf_bit_get(VALUE self, VALUE position)
{
	BitField *ptr;
	Data_Get_Struct(self, BitField, ptr);

	/* Is position a range? */
	if(rb_obj_is_kind_of(position, rb_cRange)) {
        long beg, len;
        VALUE range_ary = rb_ary_new();
        switch(rb_range_beg_len(position, &beg, &len, (long int)ptr->data.size()-1, 2)) {
            case Qfalse:
            case Qnil:
                return Qnil;
            default:
                for(long i = beg; i < beg+len; i++) {
                    rb_ary_push(range_ary, INT2NUM(ptr->data[i]));
                }
        }
		return range_ary;
	} else {
		/* Sanity checks for position and value */
		Check_Type(position, T_FIXNUM);
		int pos = FIX2INT(position);
		return INT2NUM(ptr->data[pos]);
	}
}

static VALUE bf_to_s(VALUE self)
{
	BitField *ptr;
	Data_Get_Struct(self, BitField, ptr);
	std::string buffer;
	to_string(ptr->data, buffer);
	return rb_str_new2(buffer.c_str());
}

static VALUE bf_initialize_clone(VALUE self, VALUE orig)
{
    BitField *ptr, *ptr2;
    Data_Get_Struct(self, BitField, ptr);
    Data_Get_Struct(orig, BitField, ptr2);
    ptr->data.resize(ptr2->data.size());
    ptr->data = ptr2->data;
    return self;
}

/* 
 *Correctly free the struct 
 */
void bf_free(BitField *ptr)
{
    delete ptr;
}

static VALUE BitField_allocate(VALUE klass)
{
    BitField *bf;
	bf = ALLOC(BitField);
	new(bf) BitField();
    VALUE tdata = Data_Wrap_Struct(klass, 0, bf_free, bf);
	return tdata;
}

VALUE rb_cBitField;

extern "C" {
	void Init_bitfield() {
		rb_cBitField = rb_define_class("BitField", rb_cObject);

        rb_define_alloc_func(rb_cBitField, BitField_allocate);

        /* We do the same for .clone and .dup */
		rb_define_method(
			rb_cBitField,
			"initialize_clone",
			reinterpret_cast<VALUE(*)(...)>(bf_initialize_clone),
			1
		);
        rb_define_method(
			rb_cBitField,
			"initialize_dup",
			reinterpret_cast<VALUE(*)(...)>(bf_initialize_clone),
			1
		);

        /* Returns the size of the bitset */
		rb_define_method(
			rb_cBitField,
			"size",
			reinterpret_cast<VALUE(*)(...)>(bf_size),
			0
		);
		rb_define_method(
			rb_cBitField,
			"initialize",
			reinterpret_cast<VALUE(*)(...)>(bf_init),
			1
		);
		rb_define_method(
			rb_cBitField,
			"[]=",
			reinterpret_cast<VALUE(*)(...)>(bf_bit_set),
			2
		);
		rb_define_method(
			rb_cBitField,
			"[]",
			reinterpret_cast<VALUE(*)(...)>(bf_bit_get),
			1
		);
		rb_define_method(
			rb_cBitField,
			"to_s",
			reinterpret_cast<VALUE(*)(...)>(bf_to_s),
			0
		);
		rb_define_method(
			rb_cBitField,
			"flip",
			reinterpret_cast<VALUE(*)(...)>(bf_flip),
			-1
		);
		rb_define_method(
			rb_cBitField,
			"count",
			reinterpret_cast<VALUE(*)(...)>(bf_count),
			0
		);
	}
}

/* Library Code */
BitField *BitFieldNew(int size)
{
	BitField *bf;
	bf = ALLOC(BitField);
	new(bf) BitField();
	bf->data.resize(size);
	return bf;
}
