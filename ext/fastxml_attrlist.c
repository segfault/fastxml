/*
 * Document-class: FastXml::AttrList
 *
 * call-seq:
 *  doc = FastXml( docfile ) # from FastXml::Doc
 *  n = doc.root             # pull the root node 
 *  attrlist = n.attr        # pull the FastXml::AttrList for the node
 *  puts attrlist[:some_attr]
 *  puts attrlist["some_attr"]
 */
// Please see the LICENSE file for copyright, licensing and distribution information

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"
#include "fastxml_attrlist.h"

/* {{{ fastml_attr_list
*/
void Init_fastxml_attrlist()
{
	#ifdef RDOC_SHOULD_BE_SMARTER__THIS_IS_NEVER_RUN
    rb_mFastXml = rb_define_module( "FastXml" );
	#endif
	rb_cFastXmlAttrList = rb_define_class_under( rb_mFastXml, "AttrList", rb_cObject );		
		
	rb_include_module( rb_cFastXmlAttrList, rb_mEnumerable );
	rb_define_method( rb_cFastXmlAttrList, "initialize", fastxml_attrlist_initialize, 0 );
	rb_define_method( rb_cFastXmlAttrList, "[]", fastxml_attrlist_indexer, 1 );
	rb_define_method( rb_cFastXmlAttrList, "[]=", fastxml_attrlist_indexer_set, 2 );
	rb_define_method( rb_cFastXmlAttrList, "include?", fastxml_attrlist_include, 1 );	
}

VALUE fastxml_attrlist_initialize(VALUE self)
{
    return self;
}

/* Returns the value of the attribute with the provided attr_name
 * as a string.
 * nil if the attribute does not exist.
 *
 * call-seq:
 *   puts node.attr[:an_attr_name]
 */
VALUE fastxml_attrlist_indexer(VALUE self, VALUE attr_name)
{
	VALUE ret, dv, attr_raw_str;
	fxml_data_t *data;
	xmlChar *raw_ret, *name_str;
	
	if (attr_name == Qnil)
		return Qnil;	
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	attr_raw_str = rb_funcall( attr_name, s_to_s, 0 );
	name_str = (xmlChar*)StringValuePtr( attr_raw_str );
	raw_ret = xmlGetProp( data->node, name_str );
	if (raw_ret == NULL)
		return Qnil;
		
	ret = rb_str_new2( (const char*)raw_ret );
	xmlFree( raw_ret );
	
	return ret;
}

/* Assignes a value to the attribute with the provided attr_name.
 * if the value provided is nil, the attribute is removed from the element.
 * 
 * call-seq:
 *  node.attr[:an_attr_name] = "testing" # adds the attribute if it doesn't exist
 *  node.attr[:an_attr_name] = nil       # removes the attribute if it exists
 */
VALUE fastxml_attrlist_indexer_set(VALUE self, VALUE attr_name, VALUE attr_value)
{
	VALUE dv, attr_raw_str;
	fxml_data_t *data;
	xmlChar *val, *name_str;
	
	if (attr_name == Qnil)
		return Qnil;	
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	attr_raw_str = rb_funcall( attr_name, s_to_s, 0 );
	name_str = (xmlChar*)StringValuePtr( attr_raw_str );

	if (attr_value == Qnil) {
		xmlUnsetProp( data->node, name_str ); // don't care if the node doesn't exist, as the end result is the same.
	} else {
		val = (xmlChar*)StringValuePtr( attr_value );

    	xmlSetProp( data->node, name_str, val );
	}


	return attr_value;
}

/* Returns True if the FastXml::AttrList contains an attribute
 * with the specified attr_name. Returns nil otherwise.
 *
 * call-seq:
 *  puts "FAIL" unless node.attr.include?(:a_required_node)
 */
VALUE fastxml_attrlist_include(VALUE self, VALUE attr_name)
{
	VALUE ret, dv, attr_raw_str;
	fxml_data_t *data;
	xmlChar *raw_ret, *name_str;
	
	if (attr_name == Qnil)
		return Qnil;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	attr_raw_str = rb_funcall( attr_name, s_to_s, 0 );
	name_str = (xmlChar*)StringValuePtr( attr_raw_str );
	raw_ret = xmlGetProp( data->node, name_str );
	if (raw_ret == NULL)
		return Qnil;	
	
	return Qtrue;
}


/* }}} fastxml_attr_list
*/