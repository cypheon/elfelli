# -*- Python -*-

import os

def CheckPkgConfig(context, min_version='0'):
        context.Message('Checking for pkgconfig... ')
        r = context.TryAction('pkg-config --atleast-pkgconfig-version %s' % min_version)
        context.Result(r[0])
        return r[0]

def PkgConfig(context, pkg, version, version_op='>='):
        context.Message('Checking for %s %s %s... ' % (pkg, version_op, version))

	context.env.ParseConfig('pkg-config --cflags --libs "%s %s %s"' % (pkg, version_op, version))
	context.Result(True)
	return True

env = Environment()

opts = Options('elfelli.conf')
opts.Add(BoolOption('debug', 'Set to build debug version', 1))
opts.Add(BoolOption('profiling', 'Set to enable profiling', 0))
opts.Add(('prefix', 'Directory to install elfelli under', '/usr/local'))
opts.Update(env)
opts.Save('elfelli.conf', env)

if COMMAND_LINE_TARGETS:
        targets = COMMAND_LINE_TARGETS
else:
        targets = DEFAULT_TARGETS

conf = env.Configure(custom_tests =
                 {'CheckPkgConfig': CheckPkgConfig,
                  'PkgConfig': PkgConfig})

if not conf.CheckPkgConfig('0.15'):
        Exit(1)

if not conf.PkgConfig('gtkmm-2.4', '2.8'):
        Exit(1)

if env['profiling']:
	env.AppendUnique(CPPDEFINES='PROFILING ')

if env['debug']:
	env.AppendUnique(CXXFLAGS=['-g', '-O0'], CPPDEFINES='DEBUG')
else:
	env.AppendUnique(CXXFLAGS=['-O3'], CPPDEFINES='NDEBUG')



paths = {
        "bindir": env['prefix']+'/bin',
        "datadir": env['prefix']+'/share/elfelli',
        "localedir": env['prefix']+'/share/locale'}

if 'install' in targets:
        try:
                for path in paths.values():
                        if not os.path.lexists(path):
                                os.makedirs(path, 0755)
                        elif not (os.path.isdir(path) and os.access(path, os.W_OK or os.X_OK)):
                                print 'Error: \'%s\' is not writable!' % path
                                Exit(1)
        except OSError,e:
                print 'Error %d: %s' % (e[0],e[1])
                Exit(1)
        os.system('./i18n.py install %s' % paths['localedir'])


env.AppendUnique(CPPFLAGS=r'-DDATADIR=\"%s\" '%paths['datadir'])
env.AppendUnique(CPPFLAGS=r'-DLOCALEDIR=\"%s\" '%paths['localedir'])
env.Alias("install", paths.values())

Help("""
scons        Build the program.
scons -c     Clean build directories.
scons -h     Show this help.

Options:""" + opts.GenerateHelpText(env))

Export('env paths targets')
SConscript(['src/SConscript', 'data/SConscript'])
