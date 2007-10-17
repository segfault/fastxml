/*
 * $Id$
 */

#ifndef fastxml_doc_h
#define fastxml_doc_h
RUBY_EXTERN VALUE fastxml_doc_initialize(int argc, VALUE* argv, VALUE self);
RUBY_EXTERN VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath, VALUE blk);
RUBY_EXTERN VALUE fastxml_doc_to_s(VALUE self);
RUBY_EXTERN VALUE fastxml_doc_root(VALUE self);
RUBY_EXTERN VALUE fastxml_doc_transform(VALUE self, VALUE xform);
RUBY_EXTERN VALUE fastxml_doc_stylesheet(VALUE self);
RUBY_EXTERN VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style);
RUBY_EXTERN VALUE fastxml_doc_children(VALUE self);
RUBY_EXTERN VALUE fastxml_doc_inspect(VALUE self);
#endif
