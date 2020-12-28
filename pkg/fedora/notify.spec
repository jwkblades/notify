Name: notify
Version: {{VERSION_NUMBER}}
Release: {{VERSION_BUILD}}%{?dist}
Summary: Scriptable desktop notifications for Linux

License: ASL 2.0
URL: https://github.com/jwkblades/notify
Source0: https://github.com/jwkblades/notify/archive/%{name}-%{version}-{{VERSION_BUILD}}.tgz

Requires: bash
Requires: util-linux

BuildRoot: %{_topdir}/BUILDROOT/
BuildRequires: bash
BuildRequires: binutils
BuildRequires: clang
BuildRequires: compiler-rt
BuildRequires: findutils
BuildRequires: gmock-devel
BuildRequires: gtest-devel
BuildRequires: gtk3-devel
BuildRequires: jq
BuildRequires: libnotify-devel
BuildRequires: make
BuildRequires: curl

%description
Scriptable desktop notifications for Linux

%global debug_package %{nil}

%prep
%autosetup

%build
export FLAVOR=""
make notify
strip --strip-all notify

%check
export FLAVOR=""
make test
./test

%install
mkdir -p %{buildroot}%{_bindir}
install -m 0755 notify %{buildroot}%{_bindir}

%clean
rm -rf notify %{buildroot}

%files
%{_bindir}/notify

%doc README.md
%license LICENSE

%changelog
* Mon Dec 28 2020 James Blades <jwkblades+git@gmail.com> - 1.0.0-1
- Allow notify to run in headless mode.
- Post notification options to wall
- Listen to signals to select each of the options

* Tue Dec 22 2020 James Blades <jwkblades+git@gmail.com> - 0.0.1-1
- Initial RPM build
