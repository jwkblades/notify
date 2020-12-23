#!/bin/bash

set -e
export HOME="/root"

rpmdev-setuptree
if [[ ! -d /notify-0.0.1 ]]; then
    cp -r /notify/ /notify-0.0.1
fi
tar -czvf /root/rpmbuild/SOURCES/notify-0.0.1.tgz /notify-0.0.1

cp pkg/fedora/notify.spec /root/rpmbuild/SPECS/
rpmbuild -ba /root/rpmbuild/SPECS/notify.spec

rpmlint /root/rpmbuild/SPECS/notify.spec /root/rpmbuild/SRPMS/notify-*.rpm
rpmlint /root/rpmbuild/SPECS/notify.spec /root/rpmbuild/RPMS/*/notify-*.rpm

mock --verbose /root/rpmbuild/SRPMS/notify-0.0.1-1.fc32.src.rpm

mkdir -p build
for pkg in $(find /root/rpmbuild -name "*.rpm"); do
    cp "${pkg}" build/
done
