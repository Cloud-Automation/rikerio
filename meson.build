project('rikerio-server', 'c')
cc = meson.get_compiler('c')

version_conf_data = configuration_data()
version_conf_data.set('VERSION_MAJOR', '2')
version_conf_data.set('VERSION_MINOR', '2')
version_conf_data.set('VERSION_PATCH', '0')
configure_file(input : 'include/version.h.in',
               output : 'version.h',
               configuration : version_conf_data)

rikerio_conf_data = configuration_data()
rikerio_conf_data.set('VERSION_MAJOR', '2')
rikerio_conf_data.set('VERSION_MINOR', '2')
configure_file(input : 'include/rikerio.h.in',
               output : 'rikerio.h',
               configuration : rikerio_conf_data)


libdl_dep = cc.find_library('dl', required : false)

rikerio_profile_add_sources = [ 'src/server.c' ]
rikerio_alias_add_sources = [ 'src/add-alias.c' ]
rikerio_alias_list_sources = [ 'src/list-alias.c' ]
rikerio_alias_rm_sources = [ 'src/rm-alias.c' ]
rikerio_lib_sources = [ 'src/rikerio.c' ]
rikerio_include_directories = include_directories('include', '.')

rikerio_sl = library('rikerio',
  sources: [ rikerio_lib_sources ],
  include_directories: rikerio_include_directories,
  version: '2.2.0',
  soversion: '2.2')

rikerio_dep = declare_dependency(
  include_directories : rikerio_include_directories,
  link_with : rikerio_sl,
  version : meson.project_version())

executable('rio-alias-add',
  sources: [ rikerio_alias_add_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-alias-ls',
  sources: [ rikerio_alias_list_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-alias-rm',
  sources: [ rikerio_alias_rm_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep ])

executable('rio-profile-create',
  sources: [ rikerio_profile_add_sources ],
  include_directories: rikerio_include_directories,
  dependencies: [ rikerio_dep, libdl_dep ])


