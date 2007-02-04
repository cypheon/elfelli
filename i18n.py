#!/usr/bin/env python

"""This is the i18n helper script for Elfelli

Usage:
./i18n.py update                Update po and pot files.
./i18n.py init <locale> ...     Initialise translations.
"""

import os, sys
from glob import glob

def find_locales():
    po_files = glob('po/*.po')
    locales = [file[3:-3] for file in po_files]
    return locales

def update_pot():
    print 'Updating po/elfelli.pot...'
    os.system('xgettext -o po/elfelli.pot -kN_ -k_ --c++ src/*.cpp src/*.h')

def update_po_files(locales):
    print 'Updating po files...'
    for lang in locales:
        os.system('msgmerge --update po/%s.po po/elfelli.pot' % lang)

def init_translation(locale):
    print 'Initialising %s locale...' % locale
    os.system('msginit -i po/elfelli.pot -o po/%s.po' % locale)

if __name__ == '__main__':
    locales = find_locales()
    if (len(sys.argv) >= 2) and (sys.argv[1] == 'update'):
        update_pot()
        update_po_files(locales)

    elif (len(sys.argv) >= 3) and (sys.argv[1] == 'init'):
        update_pot()
        for locale in sys.argv[2:]:
            if not (locale in locales):
                init_translation(locale)
            else:
                print "A '%s' translation already exists!" % locale
            locales = find_locales()

    else:
        print __doc__
