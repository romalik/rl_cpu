#!/sh

/tar x < bin.tar
/tar x < scripts.tar

exec /bin/sh scripts/postinst.sh
