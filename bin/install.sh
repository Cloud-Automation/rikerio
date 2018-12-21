#!/bin/bash

PREFIX=${1%/}

groupadd rikerio
chown root:rikerio ${PREFIX}/include/rikerio.h
chown root:rikerio ${PREFIX}/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
chmod 750 ${PREFIX}/bin/{rio,rio-alias-rm,rio-alias-inspect,rio-alias-add}
