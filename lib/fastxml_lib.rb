# FastXml Ruby-Side Includes
#  Ruby code that's defines some FastXml functionality.
# This is included into the FastXml class structure when it's
# initially loaded.
module FastXml
  VERSION = "0.1.92" 
  module Include #:nodoc:
  end
end

module FastXml::Common #:nodoc: all
  def children_of_type(type)
    self.search( "//#{type}" )
  end
  
  def each_child(&blk)
    self.children.each { |chld| yield chld }
  end
  
  def /(xpath)
    self.search( "/#{xpath.to_s}" )
  end
  
  def at(xpath)
    nodes = self.search( xpath )
    return nil unless nodes && nodes.length > 0
    nodes[0]
  end
  
  alias :to_s :display
end

module FastXml::Include::Doc #:nodoc:
  include FastXml::Common
  
  def doc?
    true
  end
  
  def doctype?
    nil
  end
  
  def forgiving?
    (@forgiving ||= false)
  end
  
  def  validate?
    (@validate_dtd ||= false)
  end
  
  def xpath
    "/"
  end
end

module FastXml::Include::Node #:nodoc:
  include FastXml::Common
  def doc?
    false
  end
end

module FastXml::Include::NodeList #:nodoc:
  def [](idx)
    self.entry(idx)
  end
  
  def first
    self.entry(0)
  end
  
  def last
    self.entry(-1)
  end
  
  def empty?
    return (length == 0)
  end
  
  def at(tgt)
    return self.entry( tgt.to_i ) if tgt =~ /^\d+$/
    ret = []
    each { |nd| ret << (nd/tgt).to_ary }
    ret.flatten!
  end
end

