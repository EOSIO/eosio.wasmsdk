#! /bin/bash

PREFIX="usr"
SPREFIX=${PREFIX}
SUBPREFIX="opt/${PROJECT}/${VERSION}"
SSUBPREFIX="opt\/${PROJECT}\/${VERSION}"
RELEASE="${VERSION_SUFFIX}"

# default release to "1" if there is no suffix
if [[ -z $RELEASE ]]; then
  RELEASE="1"
fi

NAME="${PROJECT}-${VERSION_NO_SUFFIX}-${RELEASE}"

export PREFIX
export SUBPREFIX
export SPREFIX
export SSUBPREFIX

. ./generate_tarball.sh ${NAME}.tar.gz

RPMBUILD=`realpath ~/rpmbuild/BUILDROOT/${NAME}.x86_64`
mkdir -p ${RPMBUILD} || exit 1
FILES=$(tar -xvzf ${NAME}.tar.gz -C ${RPMBUILD})
PFILES=""
for f in ${FILES[@]}; do
  if [ -f ${RPMBUILD}/${f} ]; then
    PFILES="${PFILES}/${f}\n"
  fi
done
echo -e ${PFILES} &> ~/rpmbuild/BUILD/filenames.txt

mkdir -p ${PROJECT} || exit 1
echo -e "Name: ${PROJECT} 
Version: ${VERSION_NO_SUFFIX}
License: MIT
Vendor: ${VENDOR} 
Source: ${URL} 
URL: ${URL} 
Packager: ${VENDOR} <${EMAIL}>
Summary: ${DESC}
Release: ${RELEASE}
%description
${DESC}
%files -f filenames.txt" &> ${PROJECT}.spec

rpmbuild -bb ${PROJECT}.spec || exit 1
BUILDSTATUS=$?
mv ~/rpmbuild/RPMS/x86_64 ./ || exit 1
rm -r ${PROJECT} ~/rpmbuild/BUILD/filenames.txt ${PROJECT}.spec || exit 1

exit $BUILDSTATUS
