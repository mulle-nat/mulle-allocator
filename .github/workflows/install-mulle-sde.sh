#! /bin/sh

OTHER_PROJECTS="mulle-c/mulle-c-developer;latest
mulle-sde/mulle-test;latest"
SDE_PROJECTS="mulle-sde-developer;latest" 

export SDE_PROJECTS
export OTHER_PROJECTS

curl -L -O 'https://raw.githubusercontent.com/mulle-sde/mulle-sde/release/bin/installer-all' && \
chmod 755 installer-all && \
./installer-all ~ no
