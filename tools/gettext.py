import SCons.Builder

def generate(env, **kw):
    Builder = SCons.Builder.Builder

    env['MSGFMT'] = env.Detect('msgfmt') or 'msgfmt'

    GettextMOBuilder = Builder(action=env['MSGFMT'] + ' -o $TARGET $SOURCE',
                               suffix='.mo', src_suffix='.po')
    env['BUILDERS']['MO'] = GettextMOBuilder

def exists(env):
    return env.Detect('msgfmt')
