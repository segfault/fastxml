/*
 * Prototypes for everything in fastxml_attrlist.c
 *
 * Please see the LICENSE file for copyright, licensing and distribution information
 */

#ifndef fastxml_attrlist_h
#define fastxml_attrlist_h
extern void Init_fastxml_attrlist();

RUBY_EXTERN VALUE fastxml_attrlist_initialize(VALUE self);
RUBY_EXTERN VALUE fastxml_attrlist_indexer(VALUE self, VALUE idx);
RUBY_EXTERN VALUE fastxml_attrlist_indexer_set(VALUE self, VALUE idx, VALUE val);
RUBY_EXTERN VALUE fastxml_attrlist_include(VALUE self, VALUE attr_name);
#endif