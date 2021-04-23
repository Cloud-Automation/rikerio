#!/bin/bash

GROUP=rikerio

if ! [ $(getent group $GROUP) ]; then
  groupadd $GROUP
fi


mkdir -p /etc/rikerio
chown root:rikerio /etc/rikerio

mkdir -p /var/lib/rikerio
mkdir -p /var/run/rikerio

chown root:rikerio /var/lib/rikerio
chown root:rikerio /var/run/rikerio

chown root:rikerio @CMAKE_INSTALL_PREFIX@/include/rikerio.h
chown root:rikerio @CMAKE_INSTALL_PREFIX@/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
chmod 750 @CMAKE_INSTALL_PREFIX@/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
source /etc/bash_completion.d/rio-completion.bash
