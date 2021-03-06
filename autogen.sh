#!/bin/sh

[ -d autom4te.cache ] && rm -rf autom4te.cache

for libtoolize in glibtoolize libtoolize libtoolize15 libtoolize13 ; do
        LIBTOOLIZE=`which $libtoolize 2>/dev/null | grep -v 'no'`
        if test "$LIBTOOLIZE" ; then
		aclocal_includes=`dirname $LIBTOOLIZE | sed -e 's%bin%share/aclocal%'`
                break;
        fi
done
for autoconf in autoconf autoconf262 autoconf261 autoconf260 autoconf259 autoconf253 ; do
	AUTOCONF=`which $autoconf 2>/dev/null | grep -v '^no'`
	if test "$AUTOCONF" ; then
		ac_postfix=`basename $AUTOCONF | sed -e "s/autoconf//"`
		break;
	fi
done

for automake in automake automake19 automake18 automake17 ; do
	AUTOMAKE=`which $automake 2>/dev/null | grep -v '^no'`
	if test "$AUTOMAKE" ; then
		am_postfix=`basename $AUTOMAKE | sed -e "s/automake//"`
		break
	fi
done

LIBTOOLIZE="$libtoolize --force --copy --automake "
ACLOCAL="aclocal${am_postfix} -I m4 -I log4cpp -I log4cpp/m4 -I $aclocal_includes" 
AUTOHEADER="autoheader${ac_postfix}"
AUTOMAKE="$automake -a -c"
AUTOCONF="$autoconf"

touch AUTHORS ChangeLog NEWS README
if test ! -d config ; then mkdir config ; else rm -rf config/* ;  fi


# Discover what version of autoconf we are using.
autoconfversion=`$AUTOCONF --version | head -n 1`
automakeversion=`$AUTOMAKE --version | head -n 1`
libtoolversion=`$LIBTOOLIZE --version | head -n 1`

echo "Using $autoconfversion"
echo "Using $automakeversion"
echo "Using $libtoolversion"

case $autoconfversion in
    *2.5[9]|*2.6[0-9])
	;;
    *)
	echo "This autoconf version is not supported by taco."
	echo "taco only supports autoconf 2.5[9] 2.6X."
	exit
	;;
esac

case $automakeversion in
    *1.[7-9]*|*1.1[0-9]*)
        ;;
    *)
        echo "This automake version is not supported by taco."
        echo "taco only supports automake 1.[7-9].* and 1.1[0-9]."
        echo "You may download it from ftp://ftp.gnu.org/gnu/automake"
        exit
        ;;
esac

case $libtoolversion in
    *1.[45]*|*2.[024]*)
        ;;
    *)
        echo "This libtool version is not supported by taco."
        echo "taco only supports libtool 1.[45].*. and 2.2*"
        echo "You may download it from ftp://ftp.gnu.org/gnu/libtool"
        exit
        ;;
esac

echo -n "Locating GNU m4... "
GNUM4=
for prog in $M4 gm4 gnum4 m4; do
	# continue if $prog generates error (e.g. does not exist)
	( $prog --version ) < /dev/null > /dev/null 2>&1
	if test $? -ne 0 ; then continue; fi

	# /dev/null input prevents a hang of the script for some m4 compilers (e.g. on FreeBSD)
	case `$prog --version < /dev/null 2>&1` in
	*GNU*)	GNUM4=$prog
		break ;;
	esac
done
if test x$GNUM4 = x ; then
	echo "not found."
	exit
else
	echo `which $GNUM4`
fi

# 
# Prepare the use of libtool
if ( $LIBTOOLIZE --version ) < /dev/null > /dev/null 2>&1 ; then
	echo "Preparing the use of libtool ..."
	$LIBTOOLIZE
	if { test -r ltmain.sh ; } ; then mv ltmain.sh config/ ; fi 
	echo "done."
else
	echo "libtoolize not found -- aborting"
	exit
fi

# Generate the Makefiles and configure files
if ( $ACLOCAL --version ) < /dev/null > /dev/null 2>&1; then
	echo "Building macros..."
	$ACLOCAL -I log4cpp -I log4cpp/m4
	echo "done."
else
	echo "aclocal not found -- aborting"
	exit
fi

if ( $AUTOHEADER --version ) < /dev/null > /dev/null 2>&1; then
	echo "Building config header template..."
	$AUTOHEADER 
	echo "done."
else
	echo "autoheader not found -- aborting"
	exit
fi

if ( $AUTOMAKE --version ) < /dev/null > /dev/null 2>&1; then
	echo "Building Makefile templates..."
	$AUTOMAKE
	echo "done."
else
	echo "automake not found -- aborting"
	exit
fi

if ( $AUTOCONF --version ) < /dev/null > /dev/null 2>&1; then
	echo "Building configure..."
	$AUTOCONF
	echo "done."
else
	echo "autoconf not found -- aborting"
	exit
fi

(cd gdbm && sh autogen.sh)
(cd log4cpp && sh autogen.sh)

