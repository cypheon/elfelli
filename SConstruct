# -*- Python -*-

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

env = Environment(toolpath=['tools'], tools=['default', 'gettext'])

opts = Options('elfelli.conf')
opts.Add(BoolOption('debug', 'Set to build debug version', 0))
opts.Add(BoolOption('profiling', 'Set to enable profiling', 0))
opts.Add(('ccflags', 'Additional flags that are passed to the C and C++ compilers', ''))
opts.Add(('prefix', 'Directory to install elfelli under', '/usr/local'))
opts.Add(('destdir', 'Everything installed will go in this directory', ''))
opts.Update(env)
opts.Save('elfelli.conf', env)

conf = env.Configure(custom_tests =
                 {'CheckPkgConfig': CheckPkgConfig,
                  'PkgConfig': PkgConfig})

if not conf.CheckPkgConfig('0.15'):
        Exit(1)
if not conf.PkgConfig('gtkmm-2.4', '2.8'):
        Exit(1)

env.AppendUnique(CCFLAGS=['-Wall'])

ccflags = env['ccflags'].split(' ')

if env['profiling']:
	env.AppendUnique(CPPDEFINES=['PROFILING'])

if env['debug']:
        ccflags = filter(lambda x: not x.startswith('-O'), ccflags)
	env.AppendUnique(CCFLAGS=['-g', '-O0'], CPPDEFINES=['DEBUG'])
else:
        if not filter(lambda x: x.startswith('-O'), ccflags):
                env.AppendUnique(CCFLAGS=['-O3'])
        env.AppendUnique(CPPDEFINES=['NDEBUG'])

env.AppendUnique(CCFLAGS=ccflags)

paths = {"bindir": env['prefix'] + '/bin',
         "datadir": env['prefix'] + '/share/elfelli',
         "localedir": env['prefix'] + '/share/locale'}
env.Dictionary().update(paths)

env.AppendUnique(CPPDEFINES=[('DATADIR', DefQuote(env['datadir'])),
                             ('LOCALEDIR', DefQuote(env['localedir']))])

env.Alias("install", map(lambda path: env['destdir']+path,paths.values()))

Help("""
scons        Build the program.
scons -c     Clean build directories.
scons -h     Show this help.

Options:""" + opts.GenerateHelpText(env))

Export('env')
SConscript(['src/SConscript', 'data/SConscript', 'po/SConscript'])
