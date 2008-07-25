# encoding: utf-8
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Node, ' functionality' do
  before(:all) do
    data_raw = open( "./test_data/hasno_feed.xml" )
    @data_ary = data_raw.readlines
    data_raw.close
    @data_str = @data_ary.join('')
    @doc = FastXml::Doc.new( @data_str )
    @node = @doc.root
  end
  
  it 'should be able to handle xpath searches' do
    @node.should respond_to( :search )
    @node.search( '/feed' ).length.should >= 1
  end
  
  it 'should support hpricot-style xpath queries using the / operator' do
    @node.should respond_to( '/' )
    (@node/"feed").length.should >= 1
    (@node/"feed").length.should == @node.search( '/feed' ).length
  end  


  it 'should provide a children accessor' do
    @node.should respond_to( :children )
    @node.children.should_not be_nil
  end
  
  it 'should provide a children accessor that includes Enumerable' do
    @node.children.kind_of?( Enumerable )
    @node.children.respond_to?( :each ).should == true
    @node.children.respond_to?( :first ).should == true
    @node.children.respond_to?( :last ).should == true
    @node.children.respond_to?( :length ).should == true
  end
  
  it 'should provide a name accessor' do
    @node.should respond_to( :name )
    @node.name.should_not be_nil
    @node.name.kind_of?(String).should == true
    @node.name.length.should > 0
  end
  
  it 'should provide a to_s method' do
    @node.should respond_to( :to_s )
    @node.to_s.should_not be_nil
    @node.to_s.kind_of?(String).should == true
    @node.to_s.length.should > 0
  end

  it 'should provide valid nodes from searches' do
    entries = @node.search( '//entry' )
    entries.should_not be_nil
    entries.length.should > 0
    entries.first.should_not be_nil
    entries.first.to_s.should_not be_nil
  end
  
  it 'should provide an inspect method' do
    @node.should respond_to( :inspect )
    @node.inspect.should_not be_nil
    @node.inspect.kind_of?(String)
    @node.inspect.length.should > 0
  end  
  
  it 'should provide an attribute accessor method named attr' do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( '[]' )
    @node.attr[:ab].should be_nil
    @node.attr["ab"].should be_nil
  end
  
  it 'should provide an attribute mutator method that responds to symbols' do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( '[]' )
    @node.attr[:ab] = "test"
    @node.attr[:ab].should == "test"
    @node.attr["ab"].should == "test"
    @node.attr["ab"].kind_of?(String).should == true
  end
  
  it 'should provide a next method' do
    @node.should respond_to( :next )
    first_chld = @node.children.first
    first_chld.next.should_not be_nil
  end
  
  it 'should provide a prev method' do
    @node.should respond_to( :prev )
    first_chld = @node.children.first    
    first_chld.prev.should be_nil
    first_chld.next.prev.should_not be_nil
  end
  
  it 'should provide an xpath method returning the xpath to the node' do
    @node.should respond_to( :xpath )
    @node.xpath.should_not be_nil
    @node.xpath.kind_of?(String).should == true
  end
  
  it 'should provide a parent method' do
    @node.should respond_to( :parent )
    @node.parent.should_not be_nil
    @node.parent.kind_of?(FastXml::Node).should == true
  end
  
  it 'should provide an inner_xml method' do
    @node.should respond_to( :inner_xml )
    @node.inner_xml.should_not be_nil
    @node.inner_xml.kind_of?(String).should == true
  end
  
  it 'should provide a children_of_type method' do
    @node.should respond_to( :children_of_type )
  end  
  
  it 'should provide an at method' do
    @node.should respond_to( :at )
    @node.at( "entry" ).should_not be_nil
  end  
  
  it 'should provide a content accessor' do
    @node.should respond_to( :content )
    @node.content.should_not be_nil
    @node.content.kind_of?(String).should == true
  end
  
  it 'should provide a content mutator' do
    @node.should respond_to( :content= )
    @node.content = "test"
    @node.content.should_not be_nil
    @node.content.should == "test"
    @node.content.kind_of?(String).should ==  true
  end
end
