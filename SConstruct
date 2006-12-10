# -*- Python -*-

import tools.Gettext

def DefQuote(str):
        return r'\"%s\"' % str

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

env = Environment(BUILDERS={'MO': tools.Gettext.GettextMOBuilder})

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
	env.AppendUnique(CPPDEFINES='PROFILING')

if env['debug']:
	env.AppendUnique(CXXFLAGS=['-g', '-O0'], CPPDEFINES='DEBUG')
else:
	env.AppendUnique(CXXFLAGS=['-O3'], CPPDEFINES='NDEBUG')

paths = {"bindir": env['prefix'] + '/bin',
         "datadir": env['prefix'] + '/share/elfelli',
         "localedir": env['prefix'] + '/share/locale'}
env.Dictionary().update(paths)

env.AppendUnique(CPPDEFINES=[('DATADIR', DefQuote(env['datadir'])),
                             ('LOCALEDIR', DefQuote(env['localedir']))])

env.Alias("install", paths.values())

Help("""
scons        Build the program.
scons -c     Clean build directories.
scons -h     Show this help.

Options:""" + opts.GenerateHelpText(env))

Export('env')
SConscript(['src/SConscript', 'data/SConscript', 'po/SConscript'])
