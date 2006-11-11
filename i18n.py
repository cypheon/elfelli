#!/usr/bin/env python

import os, sys
from glob import glob

usage = """This is the i18n helper script for Elfelli

Usage:
./i18n.py [update]              Update po files, generate mo files.
./i18n.py install /path/locale  Install mo files in this path.
./i18n.py init <locale> ...     Initialise translations.
"""

def find_locales():
    po_files = glob('po/*.po')
    locales = [file[3:-3] for file in po_files]
    return locales

def make_mo(locale):
    print 'Generating binary %s message catalog...' % locale
    if not os.path.isdir('po/locale/%s/LC_MESSAGES' % locale):
        os.makedirs('po/locale/%s/LC_MESSAGES' % locale, 0755)
    os.system('msgfmt -o po/locale/%s/LC_MESSAGES/elfelli.mo po/%s.po' % (locale,locale))

def update_pot():
    print 'Updating po/elfelli.pot...'
    os.system('xgettext -o po/elfelli.pot -kN_ -k_ --c++ src/*.cpp src/*.h')

def update_po_files(locales):
    print 'Updating po files...'
    for lang in locales:
        os.system('msgmerge --update po/%s.po po/elfelli.pot' % lang)

def compile_po_files(locales):
    for locale in locales:
        make_mo(locale)

def install_mo_files(locales, dest):
    for locale in locales:
        o = ''
        if os.geteuid() == 0:
            o = '-oroot -groot '
        os.system('install -v -m644 %s -D po/locale/%s/LC_MESSAGES/elfelli.mo %s/%s/LC_MESSAGES/elfelli.mo' % (o, locale, dest, locale))

def init_translation(locale):
    print 'Initialising %s locale...' % locale
    os.system('msginit -i po/elfelli.pot -o po/%s.po' % locale)

if __name__ == '__main__':
    locales = find_locales()
    if len(sys.argv) == 1 or sys.argv[1] == 'update':
        update_pot()
        update_po_files(locales)
        compile_po_files(locales)

    elif (sys.argv[1] == 'install') and (len(sys.argv) == 3):
        install_mo_files(locales, sys.argv[2]);

    elif (sys.argv[1] == 'init') and (len(sys.argv) >= 3):
        update_pot()
        for locale in sys.argv[2:]:
            if not (locale in locales):
                init_translation(locale)
            else:
                print "A '%s' translation already exists!" % locale
            locales = find_locales()

    else:
        print usage
