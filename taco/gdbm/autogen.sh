#! /bin/sh
for libtoolize in glibtoolize libtoolize libtoolize15 libtoolize13 ; do
        LIBTOOLIZE=`which $libtoolize 2>/dev/null | grep -v 'no'`
        if test "$LIBTOOLIZE" ; then
		aclocal_includes=`dirname $LIBTOOLIZE | sed -e 's%bin%share/aclocal%'`
		break;
	fi
done

for autoconf in autoconf autoconf261 autoconf260 autoconf259 autoconf253 ; do
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

LIBTOOLIZE="$libtoolize --force --copy --automake"
ACLOCAL="aclocal${am_postfix} -I $aclocal_includes"
AUTOHEADER="autoheader${ac_postfix}"
AUTOMAKE="$automake -a -c --foreign"
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
    *2.5[2-79]|*2.6[0-9])
	;;
    *)
	echo "This autoconf version is not supported by gdbm."
	echo "gdbm only supports autoconf 2.5[2-79] and 2.6X."
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
if test ! -d config ; then 
	mkdir config ; 
else 
	rm -rf config/* ;  
fi

# Prepare the use of libtool
if ( $LIBTOOLIZE --version ) < /dev/null > /dev/null 2>&1 ; then
	echo "Preparing the use of libtool ..."
	$LIBTOOLIZE
	echo "done."
else
	echo "libtoolize not found -- aborting"
	exit
fi

# Generate the Makefiles and configure files
if ( $ACLOCAL --version ) < /dev/null > /dev/null 2>&1; then
	echo "Building macros..."
	$ACLOCAL
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

echo
echo 'run "./configure ; make"'
echo
