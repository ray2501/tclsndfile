%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}

Name:          tclsndfile
Summary:       Tcl bindings for libsndfile
Version:       1.2
Release:       0
License:       LGPL-2.1-or-later
Group:         Development/Libraries/Tcl
Source:        %{name}-%{version}.tar.gz
URL:           https://github.com/ray2501/tclsndfile
BuildRequires: autoconf
BuildRequires: make
BuildRequires: tcl-devel >= 8.4
BuildRequires: libsndfile-devel
Requires:      tcl >= 8.4
Requires:      libsndfile
BuildRoot:     %{buildroot}

%description
Tcl bindings for libsndfile.

%prep
%setup -q -n %{name}-%{version}

%build
./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{tcl_archdir}/%{name}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%{tcl_archdir}
