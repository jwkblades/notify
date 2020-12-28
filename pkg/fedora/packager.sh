#!/bin/bash

set -e

export HOME="/root"
export VERSION_FULL="$(./bin/version)"
export VERSION_NUMBER="$(echo -n "${VERSION_FULL}" | cut -d- -f1)"
export VERSION_BUILD="$(echo -n "${VERSION_FULL}" | cut -d- -f2)"
export VERSION_HASH="$(echo -n "${VERSION_FULL}" | cut -d- -f3)"

rpmdev-setuptree
if [[ ! -d /notify-${VERSION_NUMBER} ]]; then
    cp -r /notify/ /notify-${VERSION_NUMBER}
    sync /notify-${VERSION_NUMBER}
fi
tar -czvf /root/rpmbuild/SOURCES/notify-${VERSION_NUMBER}-${VERSION_BUILD}.tgz /notify-${VERSION_NUMBER}

specFile="/root/rpmbuild/SPECS/notify.spec"
cp pkg/fedora/notify.spec ${specFile}
sed -i "s/{{VERSION_NUMBER}}/${VERSION_NUMBER}/g" ${specFile}
sed -i "s/{{VERSION_BUILD}}/${VERSION_BUILD}/g" ${specFile}
rpmbuild -ba ${specFile}

rpmlint ${specFile} /root/rpmbuild/SRPMS/notify-*.rpm
rpmlint ${specFile} /root/rpmbuild/RPMS/*/notify-*.rpm

mock --verbose /root/rpmbuild/SRPMS/notify-${VERSION_NUMBER}-${VERSION_BUILD}.fc32.src.rpm

mkdir -p build
for pkg in $(find /root/rpmbuild -name "*.rpm"); do
    cp "${pkg}" build/
done

mkdir -p pkg/fedora/archive/
cp "${specFile}" "pkg/fedora/archive/notify-${VERSION_FULL}.spec"
