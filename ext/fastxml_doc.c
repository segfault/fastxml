/*
 *  $Id$
 */
#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"


/* {{{ fastxml_doc 
 */
VALUE fastxml_doc_inspect(VALUE self)
{
    VALUE *argv;
    argv = ALLOCA_N( VALUE, 3 );
    argv[0] = rb_str_new2( "#<%s:0x%x %s>" );
    argv[1] = CLASS_OF( self );
    argv[2] = rb_obj_id( self );
	argv[3] = fastxml_doc_to_s( self );

    return rb_f_sprintf( 4, argv );
}

VALUE fastxml_doc_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->doc->children == NULL)
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->doc->children, -1 );
}

VALUE fastxml_doc_stylesheet(VALUE self)
{
	return rb_iv_get( self, "@lxml_style" );
}

VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style)
{
	VALUE dv, xslt_doc;
	fxml_data_t *data;

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

	ret_doc = (xmlDocPtr)xsltApplyStylesheet( xf_data->xslt, my_data->doc, NULL );
	ret_str = rb_str_new2( "<shouldNeverBeSeen/>" );
	ret = rb_class_new_instance( 1, &ret_str, rb_cFastXmlDoc );
	ret_dv = rb_iv_get( ret, "@lxml_doc" );
	Data_Get_Struct( ret_dv, fxml_data_t, ret_data );
	xmlFree( ret_data->doc );
	ret_data->doc = ret_doc;
	
	return ret;
}

VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath, VALUE blk)
{
	return fastxml_xpath_search( self, raw_xpath, blk );
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
    VALUE dv;
    fxml_data_t *data;
    xmlNodePtr root;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    root = xmlDocGetRootElement( data->doc );

    return fastxml_raw_node_to_obj( root );
}

VALUE fastxml_doc_initialize(VALUE self, VALUE xml_doc_str)
{
    VALUE data_s, dv, lines;
    fxml_data_t *data;
    int parser_opts = XML_PARSE_NOERROR | XML_PARSE_NOWARNING;
    int parse_dtd = XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_DTDVALID;
    int parse_forgiving = XML_PARSE_RECOVER;

    if (NIL_P(xml_doc_str)) {
        rb_raise(rb_eArgError, "nil passed as xml document");
        return Qnil;
    }

	if (rb_respond_to( xml_doc_str, s_readlines )) {
		lines = rb_funcall( xml_doc_str, s_readlines, 0 );
		data_s = rb_funcall( lines, s_to_s, 0 );
	}
	else
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
