import SCons.Builder

Builder = SCons.Builder.Builder

GettextMOBuilder = Builder(action='msgfmt -o $TARGET $SOURCE',
                           suffix='.mo', src_suffix='.po')
