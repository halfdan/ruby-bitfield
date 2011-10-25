# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

# Set the name of the extension
extension_name = 'bitfield'

# The destination
dir_config(extension_name)

# Require used libraries
have_library("stdc++")

# Do the work
create_makefile(extension_name)
