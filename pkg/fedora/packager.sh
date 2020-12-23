#!/bin/bash

rpmdev-setuptree
curl http://192.168.0.116/notify-0.0.1.tgz --output /root/rpmbuild/SOURCES/notify-0.0.1.tgz

cp pkg/fedora/notify.spec /root/rpmbuild/SPECS/
rpmbuild -ba /root/rpmbuild/SPECS/notify.spec

rpmlint /root/rpmbuild/SPECS/notify.spec /root/rpmbuild/SRPMS/notify-*.rpm
rpmlint /root/rpmbuild/SPECS/notify.spec /root/rpmbuild/RPMS/*/notify-*.rpm

mock --verbose /root/rpmbuild/SRPMS/notify-0.0.1-1.fc32.src.rpm

mkdir -p build
for pkg in $(find /root/rpmbuild -name "*.rpm"); do
    cp "${pkg}" build/
done
