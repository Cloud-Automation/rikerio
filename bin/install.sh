#!/bin/bash

groupadd rikerio
chown root:rikerio @CMAKE_INSTALL_PREFIX@/include/rikerio.h
chown root:rikerio @CMAKE_INSTALL_PREFIX@/bin/{rio,rikerio-server}
chmod 750 @CMAKE_INSTALL_PREFIX@/bin/{rio,rikerio-server}
source /etc/bash_completion.d/rio-completion.bash
