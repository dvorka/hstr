#
# Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# RPM spec of HSTR built from the upstream tarball - see rpm-build.sh
# @HSTR_VERSION@ and @HSTR_CHANGELOG_DATE@ are replaced by rpm-build.sh

Name:           hstr
Version:        @HSTR_VERSION@
Release:        1%{?dist}
Summary:        Shell history suggest box for Bash and Zsh

License:        Apache-2.0
URL:            https://github.com/dvorka/hstr
Source0:        hstr-%{version}-tarball.tgz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  ncurses-devel
BuildRequires:  readline-devel
BuildRequires:  pkgconfig(bash-completion)

%description
HSTR (HiSToRy) is a command line utility that brings improved Bash and Zsh
command completion from the history. It aims to make completion easier and
more efficient than Ctrl-r.

%prep
# upstream tarball root directory is hstr/ and it already contains configure
%autosetup -n hstr

%build
%configure
%make_build

%install
%make_install

%files
%license LICENSE
%doc README.md CONFIGURATION.md INSTALLATION.md TIPS.md Changelog
%{_bindir}/hstr
%{_bindir}/hh
%{_mandir}/man1/hstr.1*
%{_datadir}/bash-completion/completions/hstr

%changelog
* @HSTR_CHANGELOG_DATE@ Martin Dvorak <martin.dvorak@mindforger.com> - @HSTR_VERSION@-1
- HSTR @HSTR_VERSION@ - see Changelog for details
