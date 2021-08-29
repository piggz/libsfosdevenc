Name:       libsfosdevenc

Summary:    Library used by Sailfish OS Device Encryption
Version:    0.1
Release:    1
Group:      Qt/Qt
License:    GPLv2
URL:        https://github.com/sailfishos-open/libsfosdevenc
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(libcryptsetup)
BuildRequires:  cmake

%description
%summary

%prep
%setup -q -n %{name}-%{version}

%build

%cmake . 
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

%files
%defattr(-,root,root,-)
%{_libdir}
%{_includedir}
