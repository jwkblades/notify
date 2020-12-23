Name: notify
Version: 0.0.1
Release: 1%{?dist}
Summary: Scriptable desktop notifications for Linux

License: ASL 2.0
URL: https://github.com/jwkblades/notify
Source0: http://192.168.0.116/%{name}-%{version}.tgz

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
* Tue Dec 22 2020 James Blades <jwkblades+git@gmail.com> - 0.0.1-1
- Initial RPM build
