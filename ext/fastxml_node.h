/*
 * Prototypes for everything in fastxml_node.c
 *
 * Please see the LICENSE file for copyright, licensing and distribution information
 */

#ifndef fastxml_node_h
#define fastxml_node_h
extern void Init_fastxml_node();

RUBY_EXTERN VALUE fastxml_node_initialize(VALUE self);
RUBY_EXTERN VALUE fastxml_node_search(VALUE self,VALUE raw_xpath, VALUE blk);
RUBY_EXTERN VALUE fastxml_node_name(VALUE self);
RUBY_EXTERN VALUE fastxml_node_value(VALUE self);
RUBY_EXTERN VALUE fastxml_node_value_set(VALUE self, VALUE new_val);
RUBY_EXTERN VALUE fastxml_node_innerxml(VALUE self);
RUBY_EXTERN VALUE fastxml_node_to_s(VALUE self);
RUBY_EXTERN VALUE fastxml_node_xpath(VALUE self);
RUBY_EXTERN VALUE fastxml_node_attr(VALUE self);
RUBY_EXTERN VALUE fastxml_node_children(VALUE self);
RUBY_EXTERN VALUE fastxml_node_next(VALUE self);
RUBY_EXTERN VALUE fastxml_node_prev(VALUE self);
RUBY_EXTERN VALUE fastxml_node_parent(VALUE self);
RUBY_EXTERN VALUE fastxml_node_inspect(VALUE self);
#endif
