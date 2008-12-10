#!/bin/sh
# postinst script for fake
#
# see: dh_installdeb(1)

set -e

# summary of how this script can be called:
#        * <postinst> `configure' <most-recently-configured-version>
#        * <old-postinst> `abort-upgrade' <new version>
#        * <conflictor's-postinst> `abort-remove' `in-favour' <package>
#          <new-version>
#        * <postinst> `abort-remove'
#        * <deconfigured's-postinst> `abort-deconfigure' `in-favour'
#          <failed-install-package> <version> `removing'
#          <conflicting-package> <version>
# for details, see http://www.debian.org/doc/debian-policy/ or
# the debian-policy package


case "$1" in
    configure)
    ;;

    abort-upgrade|abort-remove|abort-deconfigure)
    ;;

    *)
        echo "postinst called with unknown argument \`$1'" >&2
        exit 1
    ;;
esac


echo "Creating Desktop Entries"
######################################### -- Create the desktop files
echo "[Desktop Entry]
Encoding=UTF-8
Type=Application

Exec=tspike --networksrc 0
Icon=/usr/share/soma/tspikes/icon.svg

Name=TSpikes" > Soma-tspikes.desktop

echo "[Desktop Entry]
Encoding=UTF-8

Icon=/usr/share/soma/soma/soma-main.svg
Type=Directory
Name=Soma" > Soma-Directory.directory

######################################### -- Install the desktop files
xdg-desktop-menu install Soma-Directory.directory Soma-tspikes.desktop

######################################### -- Create the desktop files

rm Soma-Directory.directory
rm Soma-tspikes.desktop
echo "Entries Created! Cleaning up..."

<<<<<<< .mine

=======
>>>>>>> .r2942
exit 0


