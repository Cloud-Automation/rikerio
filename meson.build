project('rikerio-server', 'cpp')
cc = meson.get_compiler('cpp')

version_conf_data = configuration_data()
version_conf_data.set('VERSION_MAJOR', '2')
version_conf_data.set('VERSION_MINOR', '3')
version_conf_data.set('VERSION_PATCH', '0')
configure_file(input : 'include/version.h.in',
               output : 'version.h',
               configuration : version_conf_data)

rikerio_conf_data = configuration_data()
rikerio_conf_data.set('VERSION_MAJOR', '3')
rikerio_conf_data.set('VERSION_MINOR', '0')
configure_file(input : 'include/rikerio.h.in',
               output : 'rikerio.h',
               configuration : rikerio_conf_data)


libdl_dep = cc.find_library('dl', required : false)

rikerio_profile_add_sources = [ 'src/server.cpp' ]
rikerio_link_add_sources = [ 'src/add-link.cpp' ]
rikerio_link_list_sources = [ 'src/list-link.cpp' ]
rikerio_link_rm_sources = [ 'src/rm-link.cpp' ]
rikerio_lib_sources = [ 'src/rikerio.cpp' ]
rikerio_include_directories = include_directories('include', '.')

rikerio_sl = library('rikerio',
  sources: [ rikerio_lib_sources ],
  include_directories: rikerio_include_directories,
  version: '3.0.0',
  soversion: '3.0')

rikerio_dep = declare_dependency(
  include_directories : rikerio_include_directories,
  link_with : rikerio_sl,
  version : meson.project_version())


executable('rio-link-add',
  sources: [ rikerio_link_add_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-link-ls',
  sources: [ rikerio_link_list_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-link-rm',
  sources: [ rikerio_link_rm_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-server',
  sources: [ rikerio_profile_add_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep, libdl_dep ])

executable('rio-test',
  sources: [ 'src/test.cpp' ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep, libdl_dep ])

