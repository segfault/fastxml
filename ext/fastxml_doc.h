/*
 * $Id$
 */

#ifndef fastxml_doc_h
#define fastxml_doc_h
extern VALUE fastxml_doc_initialize(VALUE self, VALUE data);
extern VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath);
extern VALUE fastxml_doc_to_s(VALUE self);
extern VALUE fastxml_doc_root(VALUE self);
extern VALUE fastxml_doc_transform(VALUE self, VALUE xform);
extern VALUE fastxml_doc_stylesheet(VALUE self);
extern VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style);
extern VALUE fastxml_doc_children(VALUE self);
extern VALUE fastxml_doc_inspect(VALUE self);
#endif
