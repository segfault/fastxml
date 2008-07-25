# encoding: utf-8
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::NodeList, ' functionality' do
  before(:all) do
    data_raw = open( "./test_data/hasno_feed.xml" )
    @data_ary = data_raw.readlines
    data_raw.close
    @data_str = @data_ary.join('')
    @doc = FastXml::Doc.new( @data_str )
    @list = @doc.children
  end
  
  it 'should provide a length method' do
    @list.length.should_not be_nil
    @list.length.should >= 1
  end
  
  it 'should have an index accessor' do
    @list[0].should_not be_nil
  end
  
  it 'should have an entry method ala Array' do
    @list.entry(0).should_not be_nil
    @list.entry(0).should == @list[0]
  end
  
  it 'entry should not explode when faced with insane indexes' do
    @list.entry(99999999**99999).should be_nil
    @list.entry(0x3fffffff).should be_nil
  end
  
  it 'should provide an each method' do
    @list.should respond_to(:each)
    cnt = 0
    @list.each do |x|
      x.should_not be_nil
      cnt += 1
    end
    @list.length.should == cnt
  end
  
  it 'should provide an inspect method' do
    @list.should respond_to( :inspect )
    @list.inspect.should_not be_nil
  end
  
  it 'should provide a first method' do
    @list.should respond_to( :first )
    @list.first.should_not be_nil
    @list.first.should == @list[0]
    @list.first.to_s.should == @list[0].to_s
  end
  
  it 'should provide a last method' do
    @list.should respond_to( :last )
    @list.last.should_not be_nil
    @list.last.should == @list[-1]
    @list.last.to_s.should == @list[-1].to_s
  end

end
