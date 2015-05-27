# Maintainer: Christian Wieden <wiedenchr at gmail dot com
# Contributer: Ricardo Band <me at xengi dot de>
pkgname=hh
pkgver=1.18
pkgrel=1
pkgdesc="A command line utility that brings improved BASH command completion from the history. It aims to make completion easier and more efficient than Ctrl-r."
arch=('any')
url="https://github.com/dvorka/hstr"
license=('Apache')
makedepends=('readline' 'ncurses')
_pkgver=$(echo "$pkgver" | sed 's/.[0-9]*$//')
source=(https://github.com/dvorka/hstr/releases/download/$pkgver/$pkgname-$pkgver-src.tgz)
md5sums=('46336873fb23e7b59b54655f05b221d5')

build() {
    cd "$srcdir/hstr"
    sed -i -e "s#<ncursesw/curses.h>#<curses.h>#g" src/include/hstr_curses.h
    sed -i -e "s#<ncursesw/curses.h>#<curses.h>#g" src/hstr.c
    ./configure --prefix=/usr
    make
}

package() {
    cd "$srcdir/hstr"
    make DESTDIR="$pkgdir/" install
}
