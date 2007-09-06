/*
 * $Id$
 */

#ifndef fastxml_nodelist_h
#define fastxml_nodelist_h
RUBY_EXTERN VALUE fastxml_nodelist_initialize(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_inspect(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_length(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_entry(VALUE self, long idx);
RUBY_EXTERN VALUE fastxml_nodelist_each(VALUE self);
RUBY_EXTERN VALUE fastxml_nodelist_to_ary(VALUE self);
#endif /*fastxml_nodelist_h*/
