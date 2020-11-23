#! /bin/sh

curl -L -O 'https://raw.githubusercontent.com/mulle-sde/mulle-sde/release/bin/installer-all' && \
chmod 755 installer-all && \
OTHER_PROJECTS="mulle-c/mulle-c-developer;latest" \
SDE_PROJECTS="mulle-sde-developer;latest" ./installer-all ~ no

curl -L 'https://github.com/mulle-sde/mulle-test/archive/latest.tar.gz' \
 | tar xfz - && cd 'mulle-test-latest' && ./bin/installer ~