# $Id$
require 'rubygems'
require 'rake'
require 'rake/rdoctask'
require 'rake/packagetask'
require 'rake/gempackagetask'
require 'rake/contrib/sshpublisher'
require 'spec/rake/spectask'
require 'rake/clean'
require 'rbconfig'
require 'rubyforge'

$: << './lib'
$: << './ext'
require 'fastxml_lib'
full_name = "FastXml"
short_name = full_name.downcase

extension = "fastxml"
ext = "ext"
ext_so = "#{ext}/#{extension}.#{Config::CONFIG['DLEXT']}"
ext_files = FileList[
  "#{ext}/*.c",
  "#{ext}/*.h",
  "#{ext}/extconf.rb",
  "#{ext}/Makefile",
  "lib"
]


CLEAN.include ["#{ext}/Makefile", "#{ext}/.o", "#{ext_so}", '*.gem']
             
# Many of these tasks were garnered from zenspider's Hoe
# just forced to work my way

desc 'Default: run unit tests.'
task :default => ["#{extension}", :spec]

spec = Gem::Specification.new do |s| 
  s.name = short_name
  s.full_name
  s.version = FastXml::VERSION
  s.author = "Mark Guzman"
  s.email = "segfault@hasno.info"
  s.homepage = "http://rubyforge.org/fastxml"
  s.platform = Gem::Platform::RUBY
  s.summary = "Fast Xml Library"
  s.rubyforge_project = short_name
  s.description = "A simple ruby interface to libxml (with hpricot-like syntax)"
  s.files = FileList["{bin,lib,ext,test_data}/**/*"].to_a
  s.require_path = "lib"
  s.autorequire = short_name
  s.test_files = FileList["{specs}/**/*spec.rb"].to_a
  s.has_rdoc = true
  s.extra_rdoc_files = %w[README LICENSE]
  s.add_dependency("rspec", ">= 1.0.3")
  s.add_dependency("rake", ">= #{RAKEVERSION}")
  s.add_dependency("rubyforge", ">= #{::RubyForge::VERSION}")
end

desc "Builds just the #{extension} extension"
task extension.to_sym => ["#{ext}/Makefile", ext_so ]

file "#{ext}/Makefile" => ["#{ext}/extconf.rb"] do
  Dir.chdir(ext) do ruby "extconf.rb" end
end

file ext_so => ext_files do
  Dir.chdir(ext) do
    sh(PLATFORM =~ /win32/ ? 'nmake' : 'make')
  end
  #mkdir_p ARCHLIB
  #cp ext_so, ARCHLIB
end

desc "Task for cruise Control"
task :cruise => ["clean", "#{extension}", "spec"] do
  out = ENV['CC_BUILD_ARTIFACTS']
  system "mv coverage #{out}" if out
end

Spec::Rake::SpecTask.new do |t|
  t.rcov = true
  t.spec_files = FileList[ 'specs/*spec.rb' ]
end

Rake::RDocTask.new do |rd|
  rd.main = "README"
  rd.rdoc_files.include("README", "LICENSE", "ext/*.c", "lib/**/*.rb")
  rd.title = "%s (%s) Documentation" % [ full_name, spec.version ]
  rd.rdoc_dir = 'doc'
end


Rake::GemPackageTask.new(spec) do |pkg| 
  pkg.need_tar = true 
end 

desc 'Clean up all the extras'
task :clean => [ :clobber_rdoc, :clobber_package ] do
  %w[*.gem ri coverage*].each do |pattern|
    files = Dir[pattern]
    rm_rf files unless files.empty?
  end
end

desc 'Publish RDoc to RubyForge'
task :publish_docs => [:clean, :rdoc] do
  config = YAML.load(File.read(File.expand_path("~/.rubyforge/user-config.yml")))
  host = "#{config["username"]}@rubyforge.org"
  remote_dir = "/var/www/gforge-projects/#{spec.rubyforge_project}"
  local_dir = 'doc'
  sh %{rsync -av --delete #{local_dir}/ #{host}:#{remote_dir}}
end

desc 'Package and upload the release to rubyforge.'
task :release => [:clean, :package] do |t|
  v = ENV["VERSION"] or abort "Must supply VERSION=x.y.z"
  abort "Versions don't match '#{v}' vs '#{spec.version}'" if v != spec.version.to_s
  pkg = "pkg/#{spec.name}-#{spec.version}"

  if $DEBUG then
    puts "release_id = rf.add_release #{spec.rubyforge_project.inspect}, #{spec.name.inspect}, #{version.inspect}, \"#{pkg}.tgz\""
    puts "rf.add_file #{spec.rubyforge_project.inspect}, #{spec.name.inspect}, release_id, \"#{pkg}.gem\""
  end

  rf = RubyForge.new
  puts "Logging in"
  rf.login

  changes = open("NOTES").readlines.join("") if File.exists?("NOTES")
  c = rf.userconfig
  c["release_notes"] = spec.description if spec.description
  c["release_changes"] = changes if changes
  c["preformatted"] = true

  files = ["#{pkg}.tgz", "#{pkg}.gem"].compact

  puts "Releasing #{spec.name} v. #{spec.version}"
  rf.add_release spec.rubyforge_project, spec.name, spec.version.to_s, *files
end

desc 'Install the package as a gem'
task :install_gem => [:clean, :package] do
  sh "sudo gem install pkg/*.gem"
end

desc 'Run Benchmarks'
namespace :bm do
  desc 'simple speedtest (atom rss)'
  task :speed => [ "#{extension}" ] do
    sh 'ruby ./benchmarks/speedtest.rb'
  end

  desc 'large file test (1MB)'
  task :largefile => [ "#{extension}" ] do
    sh 'ruby ./benchmarks/largefile.rb'
  end

  desc 'unicode file'
  task :unicode => [ "#{extension}" ] do
    sh 'ruby ./benchmarks/unicode.rb'
  end
end
