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
  

end