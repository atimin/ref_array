# encoding: utf-8

require 'rubygems'
require 'bundler'

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end


require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new(:spec)

require "rake/extensiontask"

Rake::ExtensionTask.new do |ext|
  ext.name = 'ref_array'          
  ext.ext_dir = 'ext/ref_array' 
  ext.lib_dir = 'lib/'             
end

task :default => [:compile, :spec]
