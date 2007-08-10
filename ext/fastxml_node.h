#include <ruby.h>

#ifndef fastxml_node_h
#define fastxml_node_h
extern VALUE fastxml_node_initialize(VALUE self);
extern VALUE fastxml_node_search(VALUE self,VALUE raw_xpath);
extern VALUE fastxml_node_name(VALUE self);
extern VALUE fastxml_node_value(VALUE self);
extern VALUE fastxml_node_value_set(VALUE self, VALUE new_val);
extern VALUE fastxml_node_innerxml(VALUE self);
extern VALUE fastxml_node_to_s(VALUE self);
extern VALUE fastxml_node_xpath(VALUE self);
extern VALUE fastxml_node_attr(VALUE self, VALUE attr_name);
extern VALUE fastxml_node_children(VALUE self);
extern VALUE fastxml_node_next(VALUE self);
extern VALUE fastxml_node_prev(VALUE self);
extern VALUE fastxml_node_parent(VALUE self);
#endif
