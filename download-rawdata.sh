#!/bin/sh
OUTPUTDIR=rawdata
BASEURL=http://wwwa2.kph.uni-mainz.de/download/deltaspin/rawdata
wget -P $OUTPUTDIR -e robots=off -np -nd -nc -r -A".root" $BASEURL
cd $OUTPUTDIR
sha256sum -c SHA256SUM || exit 1
echo "============================"
echo "Download and verify finished"
echo "Have fun analyzing :)"
