[ './ext', '../ext', './lib', '../lib' ].each { |l| $: << l }
require 'rubygems'
require 'fastxml'
require 'hpricot'
require 'open-uri'
require 'benchmark'
require 'REXML/document'
require 'xml/libxml'

test_path = './test_data/hasno_feed.xml'
#fraw = open('http://hasno.info/feed/atom.xml') { |f| f.readlines }
fraw = open( test_path ) { |f| f.readlines }
ds = fraw.join('')
xpath_xpr = "/feed/entry"

doc = hpd = rxd = lxd = nil
fxn = hpn = rxn = lxn = nil
fxs = hps = rxs = lxs = nil

Benchmark.bm(15) do |x|
  x.report("fastxml.new") { doc = FastXml::Doc.new( ds ) }
  x.report("fastxml.to_s") { fxs = doc.to_s }
  x.report("fastxml.search") { fxn = doc.search( xpath_xpr ) }
  puts ""
  x.report("hpricot.new") { hpd = Hpricot( ds ) }
  x.report("hpricot.to_s") { hps = hpd.to_s }
  x.report("hpricot.search") { hpn = hpd.search( xpath_xpr ) }
  puts ""
  x.report("libxml.new") { lxd = XML::Document.file( test_path ) }
  x.report("libxml.to_s") { lxs = lxd.to_s }
  x.report("libxml.search") { lxn = lxd.find( xpath_xpr )}
  puts ""
  x.report("REXML.new") { rxd = REXML::Document.new( ds ) }
  x.report("REXML.to_s") { rxs = rxd.to_s }
  x.report("REXML.xpath") { rxn = REXML::XPath.match( rxd, xpath_xpr ) }
end

puts "\noutput matching\n"
puts "fastxml vs libxml: %s\n" % (fxs && lxs && (fxs == lxs))

puts "\nxpath expression: #{xpath_xpr}\n"
puts "fastxml nodes: %d" % fxn.length
puts " libxml nodes: %d" % lxn.length
puts "hpricot nodes: %d" % hpn.length
puts "  REXML nodes: %d" % rxn.length
