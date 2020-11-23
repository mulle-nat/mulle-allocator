#! /bin/sh

curl -L -O 'https://raw.githubusercontent.com/mulle-sde/mulle-sde/release/bin/installer-all' && \
chmod 755 installer-all && \
SDE_PROJECTS="mulle-objc-developer;latest" ./installer-all ~ no
