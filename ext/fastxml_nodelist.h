/*
 * Prototypes for everything in fastxml_nodelist.c
 *
 * Please see the LICENSE file for licensing and distribution information
 */

#ifndef fastxml_nodelist_h
#define fastxml_nodelist_h

#define EMPTY_NODELIST -1
#define EMPTY_NODESET -2

extern void Init_fastxml_nodelist();

RUBY_EXTERN VALUE fastxml_nodelist_initialize(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_inspect(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_length(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_entry(VALUE self, long idx);
RUBY_EXTERN VALUE fastxml_nodelist_each(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_to_ary(VALUE self);
#endif /*fastxml_nodelist_h*/
