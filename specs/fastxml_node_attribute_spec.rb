$: << '../ext'
$: << './ext'
# encoding: utf-8
require 'fastxml'

describe FastXml::AttrList, ' functionality' do
  before(:all) do
    data_raw = open( "./test_data/hasno_feed.xml" )
    @data_ary = data_raw.readlines
    data_raw.close
    @data_str = @data_ary.join('')
    @doc = FastXml::Doc.new( @data_str )
    @node = @doc.root
  end
  
  it 'should provide an indexer [] method using both symbols and strings' do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( '[]' )
    @node.attr[:ab].should be_nil
    @node.attr["ab"].should be_nil
  end
  
  it 'should provide an indexer p[] mutator method using both symbols and strings' do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( '[]' )
    @node.attr[:ab] = "test"
    @node.attr[:ab].should == "test"
    @node.attr["ab"].should == "test"
  end
  
  it 'should provide an include? method' do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( 'include?' )
    @node.attr.include?(:ab).should == true
  end

  it "should remove an attribute when it's value is set to nil" do
    @node.should respond_to( 'attr' )
    @node.attr.should respond_to( '[]' )
    @node.attr[:ab].should == "test"
    @node.attr[:ab] = nil
    @node.attr[:ab].should == nil
    @node.attr.include?(:ab).should == nil    
  end
end
