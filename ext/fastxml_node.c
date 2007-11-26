/*
 *  $Id$
 */

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"

/* {{{ fastxml_node 
 */

VALUE fastxml_node_inspect(VALUE self)
{
    VALUE dv;
    VALUE *argv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

    argv = ALLOCA_N( VALUE, 4 );
    argv[0] = rb_str_new2( "#<%s:0x%x %s>" );
    argv[1] = CLASS_OF( self );
    argv[2] = rb_obj_id( self );
    argv[3] = rb_str_new2( (char*) data->node->name );
    return rb_f_sprintf( 4, argv );
}

VALUE fastxml_node_initialize(VALUE self)
{
    return self;
}

VALUE fastxml_node_innerxml(VALUE self)
{
	VALUE dv, ret;
    fxml_data_t *data;
	xmlBufferPtr buf = xmlBufferCreate();

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	
	xmlNodeDump( buf, data->doc, data->node, 0, 0 );
	ret = rb_str_new2( (char*)xmlBufferContent( buf ) );
	xmlBufferFree( buf );	
	
    return ret;
}

VALUE fastxml_node_next(VALUE self)
{
	VALUE dv, next;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->next == NULL))
		return Qnil;
		
	next = rb_iv_get( self, "@next" );
	if (next == Qnil) {
		next = fastxml_raw_node_to_obj( data->node->next );
		rb_iv_set( self, "@next", next );
	}
	
	return next;
}

VALUE fastxml_node_prev(VALUE self)
{
	VALUE dv, prev;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->prev == NULL))
		return Qnil;
		
	prev = rb_iv_get( self, "@prev" );
	if (prev == Qnil) {
		prev = fastxml_raw_node_to_obj( data->node->prev );
		rb_iv_set( self, "@prev", prev );
	}		
	
	return prev;
}

VALUE fastxml_node_parent(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->parent == NULL))
		return Qnil;
	
	return fastxml_raw_node_to_obj( data->node->parent );
}

VALUE fastxml_node_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->children == NULL))
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->node->children, -1 );
}

VALUE fastxml_node_name(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    if (data->node == NULL || (data->node != NULL && data->node->name == NULL))
        return Qnil;

    ret = rb_str_new2( (const char*)data->node->name );

    return ret;
}


VALUE fastxml_node_attr(VALUE self)
{
	VALUE self_dv, ret;
	fxml_data_t *data;
	xmlChar *raw_ret;

	ret = rb_iv_get( self, "@attrs" );
	if (ret == Qnil) {
		self_dv = rb_iv_get( self, "@lxml_doc" );
		Data_Get_Struct( self_dv, fxml_data_t, data );
		ret = rb_class_new_instance( 0, 0, rb_cFastXmlAttrList ); 

		rb_iv_set( ret, "@lxml_doc", self_dv );
    	rb_iv_set( self, "@attrs", ret );
	}

	return ret;
}


VALUE fastxml_node_xpath(VALUE self)
{
	VALUE ret, dv;
	fxml_data_t *data;
	xmlChar *raw_ret;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	raw_ret = xmlGetNodePath( data->node );
	if (raw_ret == NULL)
		return Qnil;
	
	ret = rb_str_new2( (const char*)raw_ret );
	xmlFree( raw_ret );
	
	return ret;
}

VALUE fastxml_node_value_set(VALUE self, VALUE new_val)
{
    VALUE dv, val_s;
    fxml_data_t *data;
    xmlChar *ents, *spec;


    val_s = rb_obj_as_string( new_val );
    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    ents = xmlEncodeEntitiesReentrant( data->doc, (const xmlChar*)StringValuePtr(val_s) );
    spec = xmlEncodeSpecialChars( data->doc, ents );

    xmlNodeSetContent( data->node, spec );
    xmlFree( ents );

    return new_val;
}

VALUE fastxml_node_value(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;
	xmlChar *cont;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 
	
	cont = xmlNodeGetContent( data->node );

    if (cont == NULL)
        return Qnil;

    ret = rb_str_new2( (const char*)cont );

    return ret;
}

VALUE fastxml_node_to_s(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;
    xmlBufferPtr buf;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    buf = xmlBufferCreate();
    ret = Qnil;

    if (xmlNodeDump(buf, data->doc, data->node, 0, 0) != -1) 
        ret = rb_str_new( (const char*)buf->content, buf->use );

    xmlBufferFree( buf );
    return ret;
}

VALUE fastxml_node_search(VALUE self, VALUE raw_xpath, VALUE blk)
{
    return fastxml_xpath_search( self, raw_xpath, blk );
}


/* }}} fastxml_node
 */
