pkgname=mpllvm
pkgrel=1
pkgver=1.0
pkgdesc="Metaprogramming facilities for LLVM"
arch=('i686' 'x86_64')
url='https://github.com/arcanis/trivia.mpllvm/'
license=('BSD')
depends=('llvm')
makedependes=('git')

build() {

    install -d $pkgdir/usr/include
    cp -dPr --no-preserve=ownership $startdir/includes $pkgdir/usr/include/mpllvm

}
