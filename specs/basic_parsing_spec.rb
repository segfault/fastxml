# $Id$
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Doc, " when created" do
  before(:all) do
    @data_raw = open( "./test_data/hasno_feed.xml" )
    @data_ary = @data_raw.readlines
    @data_str = @data_ary.join('')
  end

  before do
    @data_raw.rewind if @data_raw
  end

  after(:all) do
    @data_raw.close if @data_raw
  end


  it 'should parse string input' do
    @data_str.should_not be_nil
    doc = FastXml::Doc.new( @data_str )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end

  it 'should parse array input' do
    @data_ary.should_not be_nil
    doc = FastXml::Doc.new( @data_ary )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end

end
