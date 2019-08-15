#!/bin/bash
set -eo pipefail
cd $( dirname "${BASH_SOURCE[0]}" )/.. # Ensure we're in the repo root and not inside of scripts
. ./.cicd/.helpers

echo "Pull request branch: $TRAVIS_PULL_REQUEST_BRANCH"
echo "Branch: $TRAVIS_BRANCH"
echo "Pull request slug: $TRAVIS_PULL_REQUEST_SLUG"
echo "Repo slug: $TRAVIS_REPO_SLUG"

if [[ "$(uname)" == Darwin ]]; then
    echo 'Detected Darwin, building natively.'
    [[ -d eosio.cdt ]] && cd eosio.cdt
    [[ ! -d build ]] && mkdir build
    cd build
    echo '$ cmake ..'
    cmake ..
    echo "$ make -j$MAKE_PROC_LIMIT"
    travis_wait 180 make -j$MAKE_PROC_LIMIT
    travis_wait 30 ctest -j$MAKE_PROC_LIMIT -L unit_tests -V -T Test
else # linux
    echo 'Detected Linux, building in Docker.'
    execute docker run --rm -v $(pwd):/workdir -v /usr/lib/ccache -v $HOME/.ccache:/opt/.ccache -e MAKE_PROC_LIMIT -e CCACHE_DIR=/opt/.ccache ${FULL_TAG}
fi