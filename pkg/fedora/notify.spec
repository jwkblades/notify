Name: notify
Version: {{VERSION_NUMBER}}
Release: {{VERSION_BUILD}}%{?dist}
Summary: Scriptable desktop notifications for Linux

License: ASL 2.0
URL: https://github.com/jwkblades/notify
Source0: https://github.com/jwkblades/notify/archive/%{name}-%{version}-{{VERSION_BUILD}}.tgz

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
./test --gtest_filter="-Integration.*"

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
