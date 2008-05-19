/*
 *  $Id$
 */

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"
#include "fastxml_attrlist.h"

/* {{{ fastml_attr_list
*/

VALUE fastxml_attrlist_initialize(VALUE self)
{
    return self;
}

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