#!/bin/sh
mkdir -p ~/bin/
cp -p c ~/bin/
pushd . >/dev/null
cd ~/bin/
if [ `uname` == 'Darwin' ]; then
CTRL="cmd"
else
CTRL="ctrl"
fi
for x in '' '+c' '+v' '+R' '+del' '+h'; do
    ln -fs c $CTRL$x
done
popd >/dev/null
