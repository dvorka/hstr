# Maintainer: Christian Wieden <wiedenchr at gmail dot com>
# Contributor: Ricardo Band <me at xengi dot de>
# Contributor: Filip Szymański <fszymanski at, fedoraproject.org>
# Contributor: Busindre <busilezas at busindre.com>

pkgname=hstr
pkgver=2.2
pkgrel=1
pkgdesc="A command line utility that brings improved BASH command completion from the history. It aims to make completion easier and more efficient than Ctrl-r."
arch=('any')
url="https://github.com/dvorka/hstr"
license=('Apache')
makedepends=('autoconf' 'automake' 'ncurses' 'readline')
source=(${pkgname}-${pkgver}.tar.gz::https://github.com/dvorka/${pkgname}/archive/${pkgver}.tar.gz)
sha256sums=('886531ca9d8d1a5a93a3db96e9fc64154a03f6de428c74c722b41057bceca266')

prepare() {
    cd "${pkgname}-${pkgver}"
    sed -i -e "s#<ncursesw/curses.h>#<curses.h>#g" src/include/hstr_curses.h
    sed -i -e "s#<ncursesw/curses.h>#<curses.h>#g" src/include/hstr.h
}

build() {
    cd "${pkgname}-${pkgver}/build/tarball"
    ./tarball-automake.sh
    cd ../..
    ./configure --prefix=/usr
    make
}

package() {
    cd "${pkgname}-${pkgver}"
    make DESTDIR="${pkgdir}/" install
    install -D -p -m 644 LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
    install -D -p -m 644 Changelog "${pkgdir}/usr/share/doc/${pkgname}/Changelog"
    install -D -p -m 644 README.md "${pkgdir}/usr/share/doc/${pkgname}/README.md"
}
