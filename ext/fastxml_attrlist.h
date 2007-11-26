/*
 * $Id$
 */

#ifndef fastxml_attrlist_h
#define fastxml_attrlist_h

RUBY_EXTERN VALUE fastxml_attrlist_initialize(VALUE self);
RUBY_EXTERN VALUE fastxml_attrlist_indexer(VALUE self, VALUE idx);
RUBY_EXTERN VALUE fastxml_attrlist_indexer_set(VALUE self, VALUE idx, VALUE val);
#endif