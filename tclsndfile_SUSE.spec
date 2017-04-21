%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}

Name:          tclsndfile
Summary:       Tcl bindings for libsndfile
Version:       0.8
Release:       2
License:       LGPL v2.1
Group:         Development/Libraries/Tcl
Source:        https://github.com/ray2501/tclsndfile/tclsndfile_0.8.zip
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
%setup -q -n %{name}

%build
./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{directory}/%{_lib}/tcl/%{name}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%{directory}/%{_lib}/tcl
