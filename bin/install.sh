#!/bin/bash

groupadd rikerio
chown root:rikerio @CMAKE_INSTALL_PREFIX@/include/rikerio.h
chown root:rikerio @CMAKE_INSTALL_PREFIX@/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
chmod 750 @CMAKE_INSTALL_PREFIX@/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
source /etc/bash_completion.d/rio-completion.bash
