/*
 *  $Id$
 */
#include "fastxml.h"


/* {{{ fastxml_doc 
 */

VALUE fastxml_doc_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->doc->children == NULL)
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->doc->children );
}

VALUE fastxml_doc_stylesheet(VALUE self)
{
	return rb_iv_get( self, "@lxml_style" );
}

VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style)
{
	VALUE ret, dv, data_s, xslt_doc;
	fxml_data_t *data;
	char *cstr;		

    xslt_doc = rb_class_new_instance(1, &style, rb_cFastXmlDoc );
	
    dv = rb_iv_get( xslt_doc, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	data->xslt = xsltParseStylesheetDoc( data->doc );
	rb_iv_set( self, "@lxml_style", xslt_doc );
	
	return Qnil;	
}

VALUE fastxml_doc_transform(VALUE self, VALUE xform)
{
	VALUE ret, dv, xform_dv, ret_str, ret_dv;
	fxml_data_t *my_data, *xf_data, *ret_data;
	xmlDocPtr ret_doc;

	if (xform == Qnil)
		return Qnil;
	
    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, my_data );
	xform_dv = rb_iv_get( xform, "@lxml_doc" );
	Data_Get_Struct( xform_dv, fxml_data_t, xf_data );
	
	if (xf_data->xslt == NULL)
		return Qnil;

	ret_doc = xsltApplyStylesheet( xf_data->xslt, my_data->doc, NULL );
	ret_str = rb_str_new2( "<shouldNeverBeSeen/>" );
	ret = rb_class_new_instance( 1, &ret_str, rb_cFastXmlDoc );
	ret_dv = rb_iv_get( ret, "@lxml_doc" );
	Data_Get_Struct( ret_dv, fxml_data_t, ret_data );
	xmlFree( ret_data->doc );
	ret_data->doc = ret_doc;
	
	return ret;
}

VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath)
{
	return fastxml_xpath_search( self, raw_xpath );
}

VALUE fastxml_doc_to_s(VALUE self)
{
    VALUE ret, dv;
    xmlChar *xs;
    fxml_data_t *data;
    int xs_len;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    xmlDocDumpFormatMemory( data->doc, &xs, &xs_len, 0 );

    ret = rb_str_new( (const char*)xs, xs_len );
    xmlFree( xs );

    return ret; 
}

VALUE fastxml_doc_root(VALUE self)
{
    VALUE ret, dv, dv_chld;
    fxml_data_t *data;
    fxml_data_t *chld;
    xmlNodePtr root;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    root = xmlDocGetRootElement( data->doc );

    return fastxml_raw_node_to_obj( root );
}

VALUE fastxml_doc_initialize(VALUE self, VALUE xml_doc_str)
{
    VALUE data_s, dv;
    char *cstr;
    fxml_data_t *data;
    int parser_opts = XML_PARSE_NOERROR | XML_PARSE_NOWARNING;
    int parse_dtd = XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_DTDVALID;
    int parse_forgiving = XML_PARSE_RECOVER;

    if (NIL_P(xml_doc_str)) {
        rb_raise(rb_eArgError, "nil passed as xml document");
        return Qnil;
    }


    data_s = rb_obj_as_string( xml_doc_str );
    rb_iv_set( self, "@raw_data", data_s );

    data = ALLOC(fxml_data_t);
    memset( data, (int)NULL, sizeof(fxml_data_t) );

    data->doc = xmlReadMemory( RSTRING(data_s)->ptr, RSTRING(data_s)->len, 
                               "noname.xml", NULL, parser_opts );
    // if we're mallformed we might want to use xmlRecoverMemcory(char*, int)
    if (data->doc == NULL) {
        rb_raise( rb_eRuntimeError, "Failed to parse document" );
	    return Qnil;
    }
    
    dv = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, data );
    rb_iv_set(self, "@lxml_doc", dv );

    return self;
}


/* }}} fastxml_doc 
 */