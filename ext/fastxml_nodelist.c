/*
 *  $Id: fastxml_node.c 29 2007-08-16 05:16:47Z segfault $
 */

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"

/* {{{ fastxml_nodelist
 */
VALUE fastxml_nodelist_inspect(VALUE self)
{
    VALUE dv;
    VALUE *argv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

    argv = ALLOCA_N( VALUE, 4 );
    argv[0] = rb_str_new2( "#<%s:0x%x %d>" );
    argv[1] = CLASS_OF( self );
    argv[2] = rb_obj_id( self );
    argv[3] = fastxml_nodelist_length( self );
    return rb_f_sprintf( 4, argv );
}

VALUE fastxml_nodelist_initialize(VALUE self)
{
    return self;
}

VALUE fastxml_nodelist_length(VALUE self)
{
    VALUE dv;
    xmlNodePtr cur;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	
	if (data->list_len == EMPTY_NODELIST) {              
		data->list_len = 0;
		
        cur = data->list;
		while (cur != NULL) {
			data->list_len++;
			cur = cur->next;
		}
	}
	
	return rb_int2inum( data->list_len );
}

VALUE fastxml_nodelist_obj_to_ary(fxml_data_t *root)
{
	VALUE ret;
    xmlNodePtr cur = root->list;	
	
 	ret = rb_ary_new();
    while (cur != NULL) {        
        rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
		cur = cur->next;
	}	
	
	return ret;
}

VALUE fastxml_nodeset_obj_to_ary(fxml_data_t *root)
{
	VALUE ret;
	xmlNodePtr cur, sub = NULL;
	int i;

 	ret = rb_ary_new();
    if (root->xpath_obj->nodesetval->nodeTab != NULL) {
        cur = *root->xpath_obj->nodesetval->nodeTab;	
        for (i = 0; i < root->list_len; i++) {
            if (cur->type != XML_ELEMENT_NODE)
                continue;
            
            rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
            sub = cur->next;
            while (sub != NULL) {
                rb_ary_push( ret, fastxml_raw_node_to_obj( sub ) );
                sub = sub->next;
            }
		    cur++;
        }
    }
    
	
	return ret;
}

VALUE fastxml_nodelist_gen_list(VALUE self, fxml_data_t *data)
{
	VALUE lst = rb_iv_get( self, "@list" );

	if (lst == Qnil) {
        if (data->xpath_obj != NULL) {
			lst = fastxml_nodeset_obj_to_ary( data );
			rb_iv_set( self, "@list", lst );
		} else {
			lst = fastxml_nodelist_obj_to_ary( data );
			rb_iv_set( self, "@list", lst );
		}
	}

	return lst;
}

VALUE fastxml_nodelist_to_ary(VALUE self)
{
	VALUE dv;
	fxml_data_t *data;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	return fastxml_nodelist_gen_list( self, data );		
}

VALUE fastxml_nodelist_each(VALUE self)
{
	VALUE lst, dv;
	fxml_data_t *data;
	int i;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	lst = fastxml_nodelist_gen_list( self, data );
	
	for (i=0; i<RARRAY(lst)->len; i++) {
		rb_yield( RARRAY(lst)->ptr[i] );
    }
	
	return self;
}

VALUE fastxml_nodelist_entry(VALUE self, long idx)
{
	VALUE lst, dv;
	fxml_data_t *data;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	lst = fastxml_nodelist_gen_list( self, data );
	if (idx > 0) // this comes in offset by 1
		idx = idx-1;
	// TODO: find out why this is provided offset by 1 and not 0-based

	return rb_ary_entry( lst, idx );
}

/* }}} fastxml_nodelist
 */
