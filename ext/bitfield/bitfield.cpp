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

/*
 * Document-method: new
 *
 * call-seq: new(size)
 *
 * Creates a new BitField with the _size_ that was given.
 */
static VALUE bf_new(VALUE self, VALUE size)
{
    VALUE *argv;
    Check_Type(size, T_FIXNUM);
    BitField *ptr;
    argv[0] = size;
    rb_obj_call_init(self, 1, argv);
    return self;
}

/*
 * Document-method: size
 *
 * call-seq: size()
 *
 * Returns the size of the BitField
 */
static VALUE bf_size(VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    return INT2NUM(ptr->data.size());
}

/*
 * Document-method: flip
 *
 * call-seq: flip(n=nil) -> nil
 *
 * Flips all bits in the BitField if _n_ is nil
 * Flips n if n <> nil
 */
static VALUE bf_flip(int argc, VALUE *argv, VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    if(argc == 1)
    {
        Check_Type(argv[0], T_FIXNUM);
        long pos = FIX2INT(argv[0]);
        if(pos < 0 || pos >= ptr->data.size()) {
          return Qfalse;
        } else {
            ptr->data[pos].flip();
        }
    }
    else if(argc == 0)
    {
        ptr->data.flip();
    }
    else
    {
        rb_raise(rb_eArgError, "wrong number of arguments(%d for 1 or 0)", argc);
    }
    return Qnil;
}

/*
 * Document-method: count
 *
 * call-seq: count()
 *
 * Returns the number of bits that are set
 */
static VALUE bf_count(VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    return INT2NUM(ptr->data.count());
}

/*
 * Document-method: []=
 *
 * call-seq:
 *   [index]= value
 *   [range]= value-array
 *
 * Element Assignment - Sets the element at _index_ or replaces the subset
 * specified by _range_. Values can be only be positive numerics. Even
 * values will be set to 0 (n % 2 = 0) and uneven values will be set to 1
 * (n % 2 = 1).
 */
static VALUE bf_bit_set(VALUE self, VALUE position, VALUE value)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);

    if(rb_obj_is_kind_of(position, rb_cRange)
            && rb_obj_is_kind_of(value, rb_cArray))
    {
        long beg, len;
        VALUE tmp;
        switch(rb_range_beg_len(position, &beg, &len, (long int)ptr->data.size()-1, 2))
        {
            case Qfalse:
            case Qnil:
                return Qfalse;
            default:
                for(long i = beg+len-1; i >= beg; i--)
                {
                    tmp = rb_ary_pop(value);
                    if(tmp != Qnil)
                    {
                        /* Is the array value a number? */
                        Check_Type(tmp, T_FIXNUM);
                        ptr->data[i] = FIX2INT(tmp) % 2;
                    }
                    else
                    {
                        rb_raise(rb_eRangeError, "Array is smaller than given range.");
                    }
                }
        }
    }
    else
    {
        /* Sanity checks for position and value */
        Check_Type(position, T_FIXNUM);
        Check_Type(value, T_FIXNUM);

        int pos = FIX2INT(position);
        if(pos < 0 || pos >= ptr->data.size())
        {
            rb_raise(rb_eRangeError, "BitField out of range with value %d.", pos);
        }
        ptr->data[pos] = FIX2INT(value) % 2;
    }
    return Qnil;
}

/*
 * Document-method: []
 *
 * call-seq:
 *   [index] -> value
 *   [range] -> new_ary
 *
 * Element Reference - Returns the element at _index_ or the subset
 * specified by _range_. Negative or indices greater than the size of
 * the BitField will be nil.
 */
static VALUE bf_bit_get(VALUE self, VALUE position)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);

    /* Is position a range? */
    if(rb_obj_is_kind_of(position, rb_cRange))
    {
        long beg, len;
        VALUE range_ary = rb_ary_new();
        switch(rb_range_beg_len(position, &beg, &len, (long int)ptr->data.size()-1, 2))
        {
            case Qfalse:
            case Qnil:
                return Qnil;
            default:
                for(long i = beg; i < beg+len; i++)
                {
                    if(i < 0 || i >= ptr->data.size()) {
                        rb_ary_push(range_ary, Qnil);
                    } else {
                        rb_ary_push(range_ary, LONG2NUM(ptr->data[i]));
                    }
                }
        }
        return range_ary;
    }
    else
    {
        /* Sanity checks for position and value */
        Check_Type(position, T_FIXNUM);
        long pos = FIX2INT(position);
        if(pos < 0 || pos >= ptr->data.size()) {
            return Qnil;
        } else {
            return LONG2NUM(ptr->data[pos]);
        }
    }
}

/*
 * Document-method: to_s
 *
 * call-seq:
 *   inspect -> string
 *   to_s -> string
 *
 * Creates a string representation of +self+.
 */
static VALUE bf_to_s(VALUE self)
{
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    std::string buffer;
    to_string(ptr->data, buffer);
    return rb_str_new2(buffer.c_str());
}


/*
 * call-seq:
 *   each {|item| block } -> ary
 *   each -> an_enumerator
 *
 * Calls block once for each element in +self+, passing that
 * element as a parameter. If no block is given, an
 * enumerator is returned instead.
 */
static VALUE bf_each(VALUE self)
{
    long i, size;
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    size = ptr->data.size();

    RETURN_ENUMERATOR(self, 0, 0);
    for (i=0; i<size; i++) {
        rb_yield(bf_bit_get(self, LONG2NUM(i)));
    }
    return self;
}

/*
 * call-seq:
 *   each_index {|item| block } -> ary
 *   each_index -> an_enumerator
 *
 * Same as BitField#each, but passes the index of the element
 * instead of the element itself.
 */
static VALUE bf_each_index(VALUE self)
{
    long i, size;
    BitField *ptr;
    Data_Get_Struct(self, BitField, ptr);
    size = ptr->data.size();

    RETURN_ENUMERATOR(self, 0, 0);
    for (i=0; i<size; i++) {
        rb_yield(LONG2NUM(i));
    }
    return self;
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
    void Init_bitfield()
    {
        rb_cBitField = rb_define_class("BitField", rb_cObject);

        rb_define_alloc_func(rb_cBitField, BitField_allocate);

        /* We do the same for .clone and .dup */
        rb_define_method(
            rb_cBitField,
            "initialize_clone",
            RUBY_METHOD_FUNC(bf_initialize_clone),
            1
        );
        rb_define_method(
            rb_cBitField,
            "initialize_dup",
            RUBY_METHOD_FUNC(bf_initialize_clone),
            1
        );

        /* Returns the size of the bitset */
        rb_define_method(
            rb_cBitField,
            "size",
            RUBY_METHOD_FUNC(bf_size),
            0
        );
        rb_define_method(
            rb_cBitField,
            "initialize",
            RUBY_METHOD_FUNC(bf_init),
            1
        );
        rb_define_method(
            rb_cBitField,
            "[]=",
            RUBY_METHOD_FUNC(bf_bit_set),
            2
        );
        rb_define_method(
            rb_cBitField,
            "[]",
            reinterpret_cast<VALUE(*)(...)>(RUBY_METHOD_FUNC(bf_bit_get)),
            1
        );
        rb_define_method(
            rb_cBitField,
            "to_s",
            RUBY_METHOD_FUNC(bf_to_s),
            0
        );
        rb_define_method(
            rb_cBitField,
            "flip",
            RUBY_METHOD_FUNC(bf_flip),
            -1
        );
        rb_define_method(
            rb_cBitField,
            "count",
            RUBY_METHOD_FUNC(bf_count),
            0
        );
        rb_define_method(
            rb_cBitField,
            "each",
            RUBY_METHOD_FUNC(bf_each),
            0
        );
        rb_define_method(
            rb_cBitField,
            "each_index",
            RUBY_METHOD_FUNC(bf_each_index),
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
